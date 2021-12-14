#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "StringOperations.hpp"
#include "Common.hpp"

#include <cstdint>
#include <istream>
#include <string>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

class Lanternfish
{
public:
	explicit Lanternfish(uint32_t days_until_spawning)
		: _days_until_spawning{ days_until_spawning }
	{}

	Lanternfish()
		: _days_until_spawning{ 8 }
	{}

	uint32_t days_until_spawning() const { return _days_until_spawning; }

private:
	uint32_t _days_until_spawning;
};

///////////////////////////////////////////////////////////////////////////////

class LanternfishShoal
{
public:

	using Size_t = size_t;

	LanternfishShoal& load(std::istream& stream)
	{
		try
		{
			auto str = std::string{};
			std::getline(stream, str);

			const auto spawning_times = split(str, ',');
			_fish.resize(spawning_times.size());

			std::transform(spawning_times.begin(), spawning_times.end(), _fish.begin(), [](const auto& s) {
				return Lanternfish{ string_to<uint32_t>(s) };
				});
		}
		catch (const std::invalid_argument&)
		{
			stream.setstate(std::ios::failbit);
			throw Exception("Failed to read Lanternfish shoal from stream");
		}

		return *this;
	}

	Size_t size() const { return _fish.size(); }

private:
	std::vector<Lanternfish> _fish;
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
