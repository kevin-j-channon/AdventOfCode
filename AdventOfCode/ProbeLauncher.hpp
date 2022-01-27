#pragma once

#include "Common.hpp"
#include "StringOperations.hpp"

namespace aoc
{
namespace science
{
class Target
{
	using Area_t = Rectangle<int32_t>;
public:
	Target& from_stream(std::istream& is)
	{
		const auto [x_range_str, y_range_str] = _get_x_and_y_range_strings(is);

		auto top_left = Area_t::Point_t{};
		auto bottom_right = Area_t::Point_t{};

		const auto x_range = _get_range_for_dimension(x_range_str, "x");
		const auto y_range = _get_range_for_dimension(y_range_str, "y");

		_area = Area_t{ { x_range.min(), y_range.max() }, { x_range.max(), y_range.min() } };

		return *this;
	}

	const Area_t& area() const { return _area; }

private:

	static std::pair<std::string, std::string> _get_x_and_y_range_strings(std::istream& is)
	{
		auto line = std::string{};
		std::getline(is, line);

		auto parts = split(line, ':');
		if (parts.size() != 2) {
			throw IOException("Invalid target area input");
		}

		parts = split(parts[1], ',');
		if (parts.size() != 2) {
			throw IOException("Invalid target area input: two points required to define a rectangle");
		}

		return { std::move(parts[0]), std::move(parts[1]) };
	}

	static ValueRange<typename Area_t::Point_t::Value_t> _get_range_for_dimension(const std::string& str, const std::string& dimension)
	{
		const auto parts = split(str, '=');
		if (parts.size() != 2) {
			throw IOException(std::format("Invalid target area input: Invalid {} parts", dimension));
		}

		if (strip(parts[0]) != dimension) {
			throw IOException(std::format("Invalid target area input: Invalid dimension label '{}'", strip(parts[0])));
		}

		return ValueRange<typename Area_t::Point_t::Value_t>{}.from_string(strip(parts[1]));
	}

	Area_t _area;
};

///////////////////////////////////////////////////////////////////////////////

class Ballistics
{
public:
	using Arena_t = Rectangle<int32_t>;
	using Position_t = Point2D<int32_t>;
	using Velocity_t = Point2D<int32_t>;
	using Trajectory_t = std::vector<Position_t>;
	
	Ballistics(Arena_t arena)
		: _arena{ std::move(arena) }
	{}

	Trajectory_t trajectory(const Position_t& origin, const Velocity_t& launch_velocity) const
	{
		if (!_arena.contains(origin)) {
			throw OutOfRangeException("Origin is out of range for trajectory calculation");
		}

		auto out = Trajectory_t{{origin}};



		return std::move(out);
	}

private:
	Arena_t _arena;
};

///////////////////////////////////////////////////////////////////////////////

class ProbeLauncher
{
public:
	
	static uint32_t max_y(const Target& target)
	{
		return (-target.area().bottom_right().y) * ((-target.area().bottom_right().y) - 1) / 2;
	}
};

///////////////////////////////////////////////////////////////////////////////

}
}
