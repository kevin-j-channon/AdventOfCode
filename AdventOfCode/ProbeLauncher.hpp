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

		auto x_min_max = split(x_parts[1], '.', SplitBehaviour::drop_empty);
		if (x_min_max.size() != 2) {
			throw IOException("Invalid target area input: Invalid x range");
		}

		auto top_left = Target_t::Point_t{};
		auto bottom_right = Target_t::Point_t{};

		try {
			top_left.x = string_to<decltype(top_left)::Value_t>(x_min_max[0]);
			bottom_right.x = string_to<decltype(top_left)::Value_t>(x_min_max[1]);
		}
		catch (const std::invalid_argument& e) {
			throw IOException(std::format("Invalid target area input: {}", e.what()));
		}
		catch (const std::out_of_range& e) {
			throw IOException(std::format("Invalid target area input: {}", e.what()));
		}

		return *this;
	}

private:
	Target_t _target;
};

}
}
