#pragma once

#include "Common.hpp"
#include "StringOperations.hpp"

#include <istream>
#include <vector>
#include <string>

namespace aoc
{
class DigitAnalyser
{
public:
	DigitAnalyser& load(std::istream& stream)
	{
		auto line = std::string{};
		std::getline(stream, line);

		const auto reference_and_output_values = split(line, '|');

		_output_value_strings = split(reference_and_output_values[1], ' ', SplitBehaviour::drop_empty);

		return *this;
	}

	const std::vector<std::string>& output_value_strings() const { return _output_value_strings; }

private:
	std::vector<std::string> _output_value_strings;
};
}