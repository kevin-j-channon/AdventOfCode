#pragma once

#include "Common.hpp"
#include "StringOperations.hpp"

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{
namespace science
{

///////////////////////////////////////////////////////////////////////////////

using Position_t = Point2D<int32_t>;
using Velocity_t = Point2D<int32_t>;

///////////////////////////////////////////////////////////////////////////////

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
	using Trajectory_t = std::vector<Position_t>;
	
	Ballistics(Arena_t arena)
		: _arena{ std::move(arena) }
	{}

	Trajectory_t trajectory(Position_t p, Velocity_t v) const
	{
		if (!_arena.contains(p)) {
			throw OutOfRangeException("Origin is out of range for trajectory calculation");
		}

		auto out = Trajectory_t{};

		do {
			out.push_back(p);
			std::tie(p, v) = _next_position(std::move(p), std::move(v));
		} while (_arena.contains(p));

		return std::move(out);
	}

private:

	static std::pair<Position_t, Velocity_t> _next_position(Position_t p, Velocity_t v)
	{
		_update_x(p, v);
		_update_y(p, v);

		return {std::move(p), std::move(v)};
	}

	static void _update_x(Position_t& p, Velocity_t& v)
	{
		p.x += v.x;

		if (v.x > 0) {
			--v.x;
		}
		else if (v.x < 0) {
			++v.x;
		}
	}

	static void _update_y(Position_t& p, Velocity_t& v)
	{
		p.y += v.y;
		--v.y;
	}

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

	static std::vector<Velocity_t> find_launch_velocities(const Target& target)
	{
		const auto calculator = Ballistics{ _get_arena(target) };
	
		auto out = std::vector<Velocity_t>{};

		const auto [x_velocity_range, y_velocity_range] = _calculate_velocity_ranges(target);
		for (auto v_x : x_velocity_range) {
			for (auto v_y : y_velocity_range) {
				const auto trajectory = calculator.trajectory({ 0, 0 }, { v_x, v_y });
				if (_trajectory_intersects_target(trajectory, target) ) {
					out.emplace_back(v_x, v_y);
				}
			}
		}

		return std::move(out);
	}

private:

	static bool _trajectory_intersects_target(const Ballistics::Trajectory_t& trajectory, const Target& target)
	{
		return trajectory.rend() != std::find_if(trajectory.rbegin(), trajectory.rend(),
			[&target](const auto& p) {
				return target.area().contains(p);
			});
	}

	static Ballistics::Arena_t _get_arena(const Target& target)
	{
		const auto [x_position_range, y_position_range] = _calculate_arena_size(target);

		return {
			{x_position_range.min(), y_position_range.max()},
			{x_position_range.max(), y_position_range.min()}
		};
	}

	static std::pair<ValueRange<int32_t>, ValueRange<int32_t>> _calculate_arena_size(const Target& target)
	{
		return {
			{0, target.area().bottom_right().x},
			{target.area().bottom_right().y, _position_y_max(target)}
		};
	}

	static std::pair<ValueRange<int32_t>, ValueRange<int32_t>> _calculate_velocity_ranges(const Target& target)
	{
		return {
			{ _velocity_x_min(target), _velocity_x_max(target) },
			{ _velocity_y_min(target), _velocity_y_max(target) }
		};
	}

	/*
	x_1 = x_0 + v_0
	x_2 = x_1 + (v_0 - 1)		=  x_0 + v_0 + (v_0 - 1)
	x_3 = x_2 + (v_0 - 2)		=  x_0 + v_0 + (v_0 - 1) + (v_0 - 2)
	
	...

	x_N = x_0 + v_0 + (v_0 - 1) + (v_0 - 2) + ... + (v_0 - N + 1), x_0 == 0
	    = Nv_0 - \Sum_{i=1}^{N-1}{v_0 - i}
	    = Nv_0 - (N - 1)v_0 + \Sum_{i=1}^{N-1}{i}
	    = v_0 + \frac{N(N - 1)}{2}

	=>     2x_N = 2v_0 + N(N - 1), N == v_0 at max x
	   2x_{max} = 2v_0 + v_0(v_0 - 1)
	            = 2v_0 + v_0^2 - v_0
				= v_0^2 + v_0
              0 = v_0^2 + v_0 - 2x

			 v_0 = \frac{-1 +/- \sqrt{1 + 8x}}{2}
	*/
	static int32_t _velocity_x_min(const Target& target)
	{
		return static_cast<int32_t>(std::ceil(0.5 * (std::sqrt(1 + 8 * target.area().top_left().x) - 1)));
	}

	static int32_t _velocity_x_max(const Target& target)
	{
		return target.area().bottom_right().x;
	}

	static int32_t _velocity_y_min(const Target& target)
	{
		return target.area().bottom_right().y;
	}

	static int32_t _velocity_y_max(const Target& target)
	{
		return 1 - target.area().bottom_right().y;
	}

	static int32_t _position_y_max(const Target& target)
	{
		return (-target.area().bottom_right().y) * ((-target.area().bottom_right().y) - 1) / 2;
	}
};

///////////////////////////////////////////////////////////////////////////////

}
}
