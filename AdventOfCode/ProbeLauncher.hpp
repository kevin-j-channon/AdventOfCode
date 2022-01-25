#pragma once

#include "Common.hpp"
#include "StringOperations.hpp"

namespace aoc
{
namespace science
{

class ProbeLauncher
{
	using Target_t = Rectangle<int32_t>;
public:
	ProbeLauncher& read_target(std::istream& is)
	{
		const auto [x_range_str, y_range_str] = _get_x_and_y_range_strings(is);

		auto top_left = Target_t::Point_t{};
		auto bottom_right = Target_t::Point_t{};

		const auto x_range = _get_range_for_dimension(x_range_str, "x");
		const auto y_range = _get_range_for_dimension(y_range_str, "y");

		_target = Target_t{ { x_range.min(), y_range.max() }, { x_range.max(), y_range.min() } };

		return *this;
	}

	const Target_t& target() const { return _target; }

	uint32_t max_y() const
	{
		return (-_target.bottom_right().y) * ((-_target.bottom_right().y) - 1) / 2;
	}

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

	static ValueRange<typename Target_t::Point_t::Value_t> _get_range_for_dimension(const std::string& str, const std::string& dimension)
	{
		const auto parts = split(str, '=');
		if (parts.size() != 2) {
			throw IOException(std::format("Invalid target area input: Invalid {} parts", dimension));
		}

		if (strip(parts[0]) != dimension) {
			throw IOException(std::format("Invalid target area input: Invalid dimension label '{}'", strip(parts[0])));
		}

		return ValueRange<typename Target_t::Point_t::Value_t>{}.from_string(strip(parts[1]));
	}

	Target_t _target;
};

}
}
