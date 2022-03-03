#pragma once

#include "Common.hpp"
#include <Maths/Geometry.hpp>

#include <numbers>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{
namespace navigation
{

///////////////////////////////////////////////////////////////////////////////

using Position_t = Point3D<double>;
using Direction_t = aoc::Direction_t<Position_t::Value_t>;

///////////////////////////////////////////////////////////////////////////////

class Beacon
{
public:
	Beacon(Position_t position)
		: _position{ position }
	{}

	const Position_t& position() const { return _position; }

private:
	Position_t _position;
};

///////////////////////////////////////////////////////////////////////////////

using Beacons_t = std::vector<Beacon>;

///////////////////////////////////////////////////////////////////////////////

class Scanner
{
public:
	using Id_t = uint32_t;

	Scanner(Id_t id)
		: _id{ id }
	{}

private:
	Id_t _id;
};

///////////////////////////////////////////////////////////////////////////////

using Scanners_t = std::vector<Scanner>;

///////////////////////////////////////////////////////////////////////////////

class ScannerReport
{
public:

	using Id_t = Scanner::Id_t;

	ScannerReport(Id_t id)
		: _id{ id }
	{}

	static ScannerReport from_stream(std::istream& is)
	{
		auto out = ScannerReport{_id_from_stream(is)};
		
		out._beacons = _read_beacons(is);

		return out;
	}

	const Id_t& id() const { return _id; }
	const Beacons_t& beacons() const { return _beacons; }

private:

	static Id_t _id_from_stream(std::istream& is)
	{
		auto line = std::string{};
		std::getline(is, line);

		return _id_from_string(line);
	}

	static Id_t _id_from_string(const std::string& s)
	{
		auto parts = split(s, ' ');
		if (parts.size() != 4) {
			throw IOException{ std::format("Failed to read scanner ID from \"{}\": Invalid line format", s)};
		}

		return string_to<Id_t>(parts[2]);
	}

	static Beacons_t _read_beacons(std::istream& is)
	{
		auto out = Beacons_t{};

		auto line = std::string{};
		while (std::getline(is, line)) {
			if (line.empty()) {
				return out;
			}

			out.emplace_back(_read_position_from_string(line));
		}

		return out;
	}

	static Position_t _read_position_from_string(const std::string& s)
	{
		const auto parts = split(s, ',');
		
		const auto x = string_to<Position_t::Value_t>(parts[0]);
		const auto y = string_to<Position_t::Value_t>(parts[1]);

		auto z = Position_t::Value_t{ 0 };
		if (parts.size() == 3) {
			z = string_to<Position_t::Value_t>(parts[2]);
		}

		return { x, y, z };
	}

	Id_t _id;
	Beacons_t _beacons;
};

///////////////////////////////////////////////////////////////////////////////

std::vector<ScannerReport> read_scanner_report(std::istream& is)
{
	auto out = std::vector<ScannerReport>{};

	while (!is.eof()) {
		out.push_back(ScannerReport::from_stream(is));
	}

	return out;
}

///////////////////////////////////////////////////////////////////////////////

class MappedSpace
{
	using Line_t = Line3d<double>;

	struct RotatedBeacons
	{
		Beacons_t beacons;
		Quaternion_t rotation;
	};

public:

	const Beacons_t& beacons() const { return _beacons; }
	const Scanners_t& scanners() const { return _scanners; }

	static MappedSpace from_reports(const std::vector<ScannerReport>& reports)
	{
		auto out = MappedSpace{};
		if (reports.empty()) {
			return out;
		}

		// The first one is easy.
		out._scanners.emplace_back(reports[0].id());
		out._beacons = reports[0].beacons();

		out = std::accumulate(std::next(reports.begin()), reports.end(), std::move(out), [](auto&& current, const auto& report) -> MappedSpace {
			return _rotate_and_add_beacons(std::move(current), report);
			});

		const auto offset = find_translational_offset(out.beacons(), reports[1].beacons());

		return out;
	}

	static std::optional<std::pair<Direction_t, uint32_t>> find_translational_offset(const Beacons_t& reference, const Beacons_t& sample)
	{
		return _find_offset_with_threshold(
			_create_all_offsets(reference, sample),
			std::min(sample.size(), size_t{ 12 })
		);
	}

	static std::vector<RotatedBeacons> get_rotations(const Beacons_t& original_beacons)
	{
		static const auto rotation_axes = get_rotation_axes();

		auto out = std::vector<RotatedBeacons>{};
		out.push_back({ original_beacons, Quaternion_t{{1.0, 0.0, 0.0, 0.0}} });

		for (const auto& [axis, symetries] : rotation_axes) {
			const auto step = 2 * std::numbers::pi / (1 + symetries);

			for (const auto& i : ValueRange<int>(1, symetries)) {
				out.push_back(rotate_all_beacons(original_beacons, quaternion::from_axis_and_angle(axis, i * step)));
			}
		}

		return out;
	}

	static RotatedBeacons rotate_all_beacons(const Beacons_t& original, const Quaternion_t& rotation)
	{
		auto out = RotatedBeacons{};
		out.rotation = rotation;

		out.beacons.reserve(original.size());
		std::transform(original.begin(), original.end(), std::back_inserter(out.beacons), [&rotation](auto&& b) -> Beacon {
			return { rotate_beacon(b, rotation) };
			});

		return out;
	}

	static Beacon rotate_beacon(const Beacon& b, const Quaternion_t& rotation)
	{
		return { rotate(b.position(), rotation) };
	}

	static std::vector<std::pair<Direction_t, uint32_t>> get_rotation_axes()
	{
		auto out = std::vector<std::pair<Direction_t, uint32_t>>{};

		_add_plane_rotations(out);
		_add_edge_rotations(out);
		_add_vertex_rotations(out);

		return out;
	}

private:

	static MappedSpace _rotate_and_add_beacons(MappedSpace map, const ScannerReport& report)
	{
		auto results = std::vector<std::tuple<
			Quaternion_t,	// Rotation
			Direction_t,	// Offset
			uint32_t		// Score
			>>{};

		const auto sets_of_rotated_beacons = get_rotations(report.beacons());
		for (const auto& rotated_beacons : sets_of_rotated_beacons) {
			const auto offset_and_score = find_translational_offset(map.beacons(), rotated_beacons.beacons);
			if (!offset_and_score) {
				continue;
			}
			
			results.emplace_back(rotated_beacons.rotation, offset_and_score->first, offset_and_score->second);
		}

		return std::move(map);
	}


	static void _add_plane_rotations(std::vector<std::pair<Direction_t, uint32_t>>& rotation_axes)
	{
		// TODO: All these functions should actually return quaternions for each explicit rotation.
		rotation_axes.emplace_back(Direction_t{ 1, 0, 0}, 3);
		rotation_axes.emplace_back(Direction_t{ 0, 1, 0}, 3);
		rotation_axes.emplace_back(Direction_t{ 0, 0, 1}, 3);
	}

	static void _add_edge_rotations(std::vector<std::pair<Direction_t, uint32_t>>& rotation_axes)
	{
		rotation_axes.emplace_back(Direction_t{ 1, 1, 0}, 1);
		rotation_axes.emplace_back(Direction_t{ 1, 0, 1}, 1);
		rotation_axes.emplace_back(Direction_t{ 0, 1, 1}, 1);
		rotation_axes.emplace_back(Direction_t{ 1, -1, 0}, 1);
		rotation_axes.emplace_back(Direction_t{ 1, 0, -1}, 1);
		rotation_axes.emplace_back(Direction_t{ 0, 1, -1}, 1);
	}

	static void _add_vertex_rotations(std::vector<std::pair<Direction_t, uint32_t>>& rotation_axes)
	{
		rotation_axes.emplace_back(Direction_t{ 1, 1, 1}, 2);
		rotation_axes.emplace_back(Direction_t{ 1, 1, -1}, 2);
		rotation_axes.emplace_back(Direction_t{ 1, -1, 1}, 2);
		rotation_axes.emplace_back(Direction_t{-1, 1, 1}, 2);
	}

	static std::vector<Line_t> _create_all_offsets(const Beacons_t& reference, const Beacons_t& sample)
	{
		auto lines = std::vector<Line_t>{};
		lines.reserve(reference.size() * sample.size());

		for (const auto& ref_beacon : reference) {
			for (const auto& sample_beacon : sample) {
				lines.emplace_back(ref_beacon.position(), sample_beacon.position());
			}
		}

		return lines;
	}

	static std::optional<std::pair<Direction_t, uint32_t>> _find_offset_with_threshold(std::vector<Line_t> point_offsets, size_t threshold)
	{
		const auto best_offset_and_score = _find_best_scoring_offset(std::move(point_offsets));
		if (best_offset_and_score.second < threshold) {
			return {};
		}
		
		return {std::move(best_offset_and_score)};
	}

	static std::pair<Direction_t, uint32_t> _find_best_scoring_offset(std::vector<Line_t> point_offsets)
	{
		const auto parallel_groups = _group_offsets_by_direction(std::move(point_offsets));

		const auto direction_with_max_number_of_lines = std::max_element(parallel_groups.begin(), parallel_groups.end(), [](const auto& x1, const auto& x2) {
			return x1.second.size() < x2.second.size();
			});

		return { direction_with_max_number_of_lines->first, direction_with_max_number_of_lines->second.size() };
	}

	static std::map<Direction_t, std::vector<Line_t>> _group_offsets_by_direction(std::vector<Line_t> offsets)
	{
		auto direction = [](const Line_t& line) { return Direction_t{ line.finish.x - line.start.x , line.finish.y - line.start.y, line.finish.z - line.start.z }; };

		auto parallel_groups = std::map<Direction_t, std::vector<Line_t>>{};
		for (auto& offset : offsets) {
			parallel_groups[direction(offset)].push_back(std::move(offset));
		}

		return parallel_groups;
	}

	Beacons_t _beacons;
	Scanners_t _scanners;
};

///////////////////////////////////////////////////////////////////////////////

uint32_t calculate_number_of_beacons(const Scanners_t& scanners)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

}
}