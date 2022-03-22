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

using Position_t = Point3D<int>;
using Direction_t = aoc::Direction_t<Position_t::Value_t>;

///////////////////////////////////////////////////////////////////////////////

class Beacon
{
public:
	Beacon(Position_t position)
		: _position{ position }
	{}

	const Position_t& position() const { return _position; }
	void set_position(const Position_t& new_position)
	{
		_position = new_position;
	}

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

class BeaconCloudRotator
{
	struct RotatedBeacons
	{
		Beacons_t beacons;
		Quaternion_t rotation;
	};

public:

	BeaconCloudRotator(const Beacons_t& beacons)
		: _beacons{ beacons }
	{}

	std::vector<RotatedBeacons> get_rotations()
	{
		auto out = std::vector<RotatedBeacons>{};

		for (const auto& rotation : quaternion::cubic_rotations()) {
			out.push_back(rotate_all_beacons(rotation));
		}

		return out;
	}

	RotatedBeacons rotate_all_beacons(const Quaternion_t& rotation)
	{
		auto out = RotatedBeacons{};
		out.rotation = rotation;

		out.beacons.reserve(_beacons.size());
		std::transform(_beacons.begin(), _beacons.end(), std::back_inserter(out.beacons), [&rotation](auto&& b) -> Beacon {
			return { rotate_beacon(b, rotation) };
			});

		return out;
	}

	static Beacon rotate_beacon(const Beacon& b, const Quaternion_t& rotation)
	{
		return rotate(b.position(), rotation);
	}

private:

	const Beacons_t& _beacons;
};

///////////////////////////////////////////////////////////////////////////////

class BeaconCloudRegistrator
{
	using Line_t = Line3d<int>;

public:

	BeaconCloudRegistrator(const Beacons_t& reference, const Beacons_t& sample, size_t overlap_threshold)
		: _overlap_threshold{ overlap_threshold }
		, _reference{ reference }
		, _sample{ sample }
	{}

	std::optional<std::pair<Direction_t, uint32_t>> find_offset_and_score()
	{
		return _find_offset_with_threshold(_create_all_offsets());
	}

private:

	std::vector<Line_t> _create_all_offsets() const
	{
		auto lines = std::vector<Line_t>{};
		lines.reserve(_reference.size() * _sample.size());

		for (const auto& ref_beacon : _reference) {
			for (const auto& sample_beacon : _sample) {
				lines.emplace_back(ref_beacon.position(), sample_beacon.position());
			}
		}

		return lines;
	}

	std::optional<std::pair<Direction_t, uint32_t>> _find_offset_with_threshold(std::vector<Line_t> point_offsets) const
	{
		const auto best_offset_and_score =
			_find_best_scoring_offset(
				_drop_overlaps_with_unmatched_points(
					_drop_single_point_groups(
						_group_offsets_by_direction(std::move(point_offsets)))));

		if (!best_offset_and_score || best_offset_and_score->second < _overlap_threshold) {
			return {};
		}

		return { std::move(best_offset_and_score) };
	}

	static std::optional<std::pair<Direction_t, uint32_t>> _find_best_scoring_offset(std::map<Direction_t, std::vector<Line_t>> parallel_groups)
	{
		if (parallel_groups.empty()) {
			return {};
		}

		const auto direction_with_max_number_of_lines = std::max_element(parallel_groups.begin(), parallel_groups.end(), [](const auto& x1, const auto& x2) {
			return x1.second.size() < x2.second.size();
			});

		return { { direction_with_max_number_of_lines->first, static_cast<uint32_t>(direction_with_max_number_of_lines->second.size()) } };
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

	static std::map<Direction_t, std::vector<Line_t>> _drop_single_point_groups(std::map<Direction_t, std::vector<Line_t>> groups)
	{
		if (groups.empty()) {
			return {};
		}

		while (true) {
			auto to_erase = std::find_if(groups.begin(), groups.end(), [](auto&& g) { return g.second.size() < 2;  });
			if (groups.end() == to_erase) {
				break;
			}

			groups.erase(to_erase);
		}

		return std::move(groups);
	}

	std::map<Direction_t, std::vector<Line_t>> _drop_overlaps_with_unmatched_points(std::map<Direction_t, std::vector<Line_t>> groups) const
	{
		if (groups.empty()) {
			return {};
		}

		const auto groups_to_drop = _find_groups_to_drop_due_to_unmatched_points(groups);
		std::for_each(groups_to_drop.begin(), groups_to_drop.end(), [&groups](auto&& g) { groups.erase(g); });

		return std::move(groups);
	}

	std::vector<Direction_t> _find_groups_to_drop_due_to_unmatched_points(const std::map<Direction_t, std::vector<Line_t>>& groups) const
	{
		auto out = make_vector<Direction_t>(Capacity{ groups.size() });

		for (const auto& [direction, lines] : groups) {
			if (_group_has_unmatched_points(direction, lines, groups)) {
				out.push_back(direction);
			}
		}

		return out;
	}

	bool _group_has_unmatched_points(const Direction_t& direction, const std::vector<Line_t>& lines, const std::map<Direction_t, std::vector<Line_t>>& all_groups) const
	{
		if (lines.size() < 2) {
			// There can be no sensible enclosing volume in this case.
			return false;
		}

		return _has_unmatched_points_in_overlap_region(direction, lines, all_groups);
	}

	bool _has_unmatched_points_in_overlap_region(const Direction_t& direction, const std::vector<Line_t>& lines, const std::map<Direction_t, std::vector<Line_t>>& all_groups) const
	{

		// Sample must contain only the line end points in the overlap region.
		const auto sample_overlap_region = _determine_enclosing_volume_of_terminal_points(lines, [](const Line_t& l) -> const Point3D<int>&{ return l.finish; });
		const auto sample_overlap_region_has_points_not_on_a_line_end = !range_to<std::vector<Position_t>>(_sample
			| std::views::transform([](auto&& beacon) { return beacon.position(); })
			| std::views::filter([&sample_overlap_region](auto&& point) { return sample_overlap_region.contains(point); })
			| std::views::filter([&lines](auto&& point) {
				return std::find_if(lines.begin(), lines.end(), [&point](auto&& line) {
					return line.finish == point;
					}) == lines.end();
				})).empty();

		if (sample_overlap_region_has_points_not_on_a_line_end) {
			return true;
		}

		// Reference must contain only the line start points in the overlap region.
		const auto reference_overlap_region = _determine_enclosing_volume_of_terminal_points(lines, [](const Line_t& l) -> const Point3D<int>&{ return l.start; });
		const auto reference_overlap_region_has_points_not_on_a_line_start = !range_to<std::vector<Position_t>>(_reference
			| std::views::transform([](auto&& beacon) { return beacon.position(); })
			| std::views::filter([&reference_overlap_region](auto&& point) { return reference_overlap_region.contains(point); })
			| std::views::filter([&lines](auto&& point) {
				return std::find_if(lines.begin(), lines.end(), [&point](auto&& line) {
					return line.start == point;
					}) == lines.end();
				})).empty();

		if (reference_overlap_region_has_points_not_on_a_line_start) {
			return true;
		}

		return false;
	}

	template<typename LineEndExtractor_T>
	static Cubiod<int> _determine_enclosing_volume_of_terminal_points(const std::vector<Line_t>& lines, LineEndExtractor_T line_end)
	{
		if (lines.empty()) {
			return { {0,0,0}, {0,0,0} };
		}

		const auto [x_min, x_max] = std::ranges::minmax_element(lines, [&line_end](auto&& line_1, auto&& line_2) {
			return line_end(line_1).x < line_end(line_2).x;
			});

		const auto [y_min, y_max] = std::ranges::minmax_element(lines, [&line_end](auto&& line_1, auto&& line_2) {
			return line_end(line_1).y < line_end(line_2).y;
			});

		const auto [z_min, z_max] = std::ranges::minmax_element(lines, [&line_end](auto&& line_1, auto&& line_2) {
			return line_end(line_1).z < line_end(line_2).z;
			});

		return { {line_end(*x_min).x, line_end(*y_min).y, line_end(*z_min).z}, {line_end(*x_max).x, line_end(*y_max).y, line_end(*z_max).z}};
	}

	size_t _overlap_threshold;
	const Beacons_t& _reference;
	const Beacons_t& _sample;
};

///////////////////////////////////////////////////////////////////////////////

class MappedSpace
{
	using Line_t = Line3d<int>;

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

		const auto sets_of_rotated_beacons = BeaconCloudRotator{ report.beacons() }.get_rotations();
		for (const auto& rotated_beacons : sets_of_rotated_beacons) {
			const auto offset_and_score = BeaconCloudRegistrator{ map.beacons(), rotated_beacons.beacons, 12 }.find_offset_and_score();
			if (!offset_and_score) {
				continue;
			}
			
			results.emplace_back(rotated_beacons.rotation, offset_and_score->first, offset_and_score->second);
		}

		return std::move(map);
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