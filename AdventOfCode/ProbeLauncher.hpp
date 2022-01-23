#pragma once

#include "Common.hpp"
#include "StringOperations.hpp"

namespace aoc
{
namespace science
{

class ProbeLauncher
{
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

		auto x_min_max = split(x_parts[1], '.', SplitBehaviour::drop_empty);
		if (x_min_max.size() != 2) {
			throw IOException("Invalid target area input: Invalid x range");
		}

		return *this;
	}

private:
	Rectangle<uint32_t> _target;
};

}
}
