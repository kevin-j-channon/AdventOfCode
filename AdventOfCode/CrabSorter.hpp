#pragma once

#include <cstdint>
#include <vector>
#include <istream>

namespace aoc
{
namespace crab_sorting
{
std::vector<uint32_t> load(std::istream& stream) try
{
	auto str = std::string{};
	std::getline(stream, str);

	const auto position_strings = split(str, ',');
	
	auto positions = std::vector<uint32_t>(position_strings.size(), 0);

	std::transform(position_strings.begin(), position_strings.end(), positions.begin(), [](const auto& s) {
		return string_to<uint32_t>(s);
		});

	return positions;
}
catch (const std::invalid_argument&)
{
	stream.setstate(std::ios::failbit);
	throw Exception("Failed to read Lanternfish shoal from stream: Non-numeric character");
}
catch (const std::out_of_range&)
{
	stream.setstate(std::ios::failbit);
	throw Exception("Failed to read Lanternfish shoal from stream: Value out-of-range");
}
catch (const Exception&)
{
	stream.setstate(std::ios::failbit);
	throw Exception("Failed to read Lanternfish shoal from stream: Value out-of-range");
}

}

}
