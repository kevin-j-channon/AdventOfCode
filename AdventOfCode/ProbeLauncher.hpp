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

		const auto x_parts = split(parts[0], '=');
		if (x_parts.size() != 2) {
			throw IOException("Invalid target area input: Invalid x parts");
		}

		if (strip(x_parts[0]) != "x") {
			throw IOException(std::format("Invalid target area input: Invalid dimension label '{}'", strip(x_parts[0])));
		}

		const auto x_range = ValueRange<typename Target_t::Point_t::Value_t>{}.from_stream(is);

		auto top_left = Target_t::Point_t{};
		auto bottom_right = Target_t::Point_t{};

		top_left.x = x_range.min();
		bottom_right.x = x_range.max();

		return *this;
	}

private:
	Target_t _target;
};

}
}
