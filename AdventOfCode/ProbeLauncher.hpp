#pragma once

#include "Common.hpp"
#include "StringOperations.hpp"

namespace aoc
{
namespace science
{

class ProbeLauncher
{
	using Target_t = Rectangle<uint32_t>;
public:
	ProbeLauncher& read_target(std::istream& is)
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

		auto top_left = Target_t::Point_t{};
		auto bottom_right = Target_t::Point_t{};

		const auto x_range = _get_range_for_dimension(parts[0], "x");

		top_left.x = x_range.min();
		bottom_right.x = x_range.max();


		return *this;
	}

private:

	static ValueRange<typename Target_t::Point_t::Value_t> _get_range_for_dimension(const std::string& str, const std::string& dimension)
	{
		const auto parts = split(str, '=');
		if (parts.size() != 2) {
			throw IOException(std::format("Invalid target area input: Invalid {} parts", dimension));
		}

		if (strip(parts[0]) != dimension) {
			throw IOException(std::format("Invalid target area input: Invalid dimension label '{}'", strip(parts[0])));
		}

		return ValueRange<typename Target_t::Point_t::Value_t>{}.from_string(strip(str));
	}

	Target_t _target;
};

}
}
