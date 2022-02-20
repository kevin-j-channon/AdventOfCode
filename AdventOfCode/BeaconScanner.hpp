#pragma once

#include "Common.hpp"

namespace aoc
{
namespace navigation
{
using Position_t = Point3D<int>;

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

using Beacons_t = std::vector<Beacon>;

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

using Scanners_t = std::vector<Scanner>;

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
		
		const auto x = string_to<Position_t::XValue_t>(parts[0]);
		const auto y = string_to<Position_t::YValue_t>(parts[1]);

		auto z = Position_t::ZValue_t{ 0 };
		if (parts.size() == 3) {
			z = string_to<Position_t::ZValue_t>(parts[2]);
		}

		return { x, y, z };
	}

	Id_t _id;
	Beacons_t _beacons;
};

std::vector<ScannerReport> read_scanner_report(std::istream& is)
{
	auto out = std::vector<ScannerReport>{};

	while (!is.eof()) {
		out.push_back(ScannerReport::from_stream(is));
	}

	return out;
}

class MappedSpace
{
public:
	using Direction_t = Point2D<int>;

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

		const auto offset = find_translational_offset(out.beacons(), reports[1].beacons());

		return out;
	}

	static std::optional<Direction_t> find_translational_offset(const Beacons_t& reference, const Beacons_t& sample)
	{
		const auto threshold = std::min(sample.size(), size_t{ 12 });

		auto lines = std::vector<Line2d<int>>{};
		lines.reserve(reference.size() * sample.size());
		
		for (const auto& ref_beacon : reference) {
			for (const auto& sample_beacon : sample) {
				lines.emplace_back(
					Point2D<int>{ref_beacon.position().x, ref_beacon.position().y },
					Point2D<int>{ sample_beacon.position().x, sample_beacon.position().y }
				);
			}
		}

		auto size_score = [](const Line2d<int>& line) -> int {
			const auto X = line.finish.x - line.start.x;
			const auto Y = line.finish.y - line.start.y;

			return X * X + Y * Y;
		};

		// Collect lines that are the same size
		auto sized_lines = std::map<int, std::vector<Line2d<int>>>{};
		for (auto& line : lines) {
			const auto score = size_score(line);
			sized_lines[score].push_back(std::move(line));
		}

		auto sized_lines_2 = std::map<int, std::vector<Line2d<int>>>{};
		for (auto it = sized_lines.begin(); it != sized_lines.end(); ++it) {
			if (it->second.size() >= threshold) {
				sized_lines_2[it->first] = std::move(it->second);
			}
		}

		if (sized_lines_2.empty()) {
			return {};
		}

		// Find all the parallel lines
		auto direction = [](auto&& line) { return Direction_t{ line.finish.x - line.start.x , line.finish.y - line.start.y }; };

		auto parallel_groups = std::map<int, std::map<Direction_t, std::vector<Line2d<int>>>>{};
		for (auto& [size_score, lines ] : sized_lines_2)
		{
			for (auto& line : lines) {
				// All these lines are the same length, so if the diffs in their coords are equal, then they're parallel too.
				parallel_groups[size_score][direction(line)].push_back(std::move(line));
			}
		}


		auto parallel_groups_2 = std::map<int, std::map<Direction_t, std::vector<Line2d<int>>>>{};
		for (auto size_group = parallel_groups.begin(); size_group != parallel_groups.end(); ++size_group) {
			for (auto direction_group = size_group->second.begin(); direction_group != size_group->second.end(); ++direction_group) {
				if (direction_group->second.size() >= threshold) {
					parallel_groups_2[size_group->first][direction_group->first] = std::move(direction_group->second);
				}
			}
		}

		// If there is only one element left, then this is a potential region of overlap.
		if (parallel_groups_2.size() != 1 || parallel_groups_2.begin()->second.size() != 1) {
			return {};
		}

		return parallel_groups_2.begin()->second.begin()->first;
	}

private:
	Beacons_t _beacons;
	Scanners_t _scanners;
};

uint32_t calculate_number_of_beacons(const Scanners_t& scanners)
{
	return 0;
}

}
}