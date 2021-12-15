#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "Common.hpp"
#include "StringOperations.hpp"

#include <cstdint>
#include <vector>
#include <istream>
#include <numeric>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{
class CrabSorter
{
public:
	CrabSorter load(std::istream& stream) try
	{
		auto str = std::string{};
		std::getline(stream, str);

		const auto position_strings = split(str, ',');

		_positions.resize(position_strings.size());

		std::transform(position_strings.begin(), position_strings.end(), _positions.begin(), [](const auto& s) {
			return string_to<uint32_t>(s);
			});

		return *this;
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

	auto positions() const { return _positions; }

	std::pair<size_t, uint32_t> best_position_and_cost()
	{
		const auto max_position = *std::max_element(_positions.begin(), _positions.end());

		auto out = std::make_pair(std::numeric_limits<uint32_t>::max(), size_t{ 0 });

		for (size_t position = 0; position <= max_position; ++position) {
			const auto cost = _position_cost(position);
			if (cost < out.second) {
				out.first = position;
				out.second = cost;
			}
		}

		return out;
	}

private:

	uint32_t _position_cost(size_t position) const
	{
		return std::accumulate(_positions.begin(), _positions.end(), uint32_t{ 0 },
			[position](auto curr, const auto& next) {
				return static_cast<uint32_t>(curr + std::abs(static_cast<int>(next - position)));
			});
	}

	std::vector<uint32_t> _positions;
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
