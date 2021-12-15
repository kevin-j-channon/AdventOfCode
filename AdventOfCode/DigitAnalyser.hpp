#pragma once

#include "Common.hpp"
#include "StringOperations.hpp"

#include <istream>
#include <vector>
#include <string>

namespace aoc
{
class DigitData
{
public:
	DigitData& load(std::istream& stream)
	{
		auto line = std::string{};
		std::getline(stream, line);

		const auto reference_and_output_values = split(line, '|');

		_reference_value_strings = split(reference_and_output_values[0], ' ', SplitBehaviour::drop_empty);
		_output_value_strings = split(reference_and_output_values[1], ' ', SplitBehaviour::drop_empty);

		return *this;
	}

	const std::vector<std::string>& output_value_strings() const { return _output_value_strings; }

	const std::vector<std::string>& reference_value_strings() const { return _reference_value_strings; }

private:
	std::vector<std::string> _output_value_strings;
	std::vector<std::string> _reference_value_strings;
};
}