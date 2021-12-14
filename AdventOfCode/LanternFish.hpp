#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "StringOperations.hpp"
#include "Common.hpp"

#include <cstdint>
#include <istream>
#include <string>
#include <stdexcept>
#include <format>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

class Lanternfish
{
public:

	static const auto max_days_until_spawning = uint32_t{ 8 };
	explicit Lanternfish(uint32_t days_until_spawning)
		: _days_until_spawning{ days_until_spawning }
	{
		if (_days_until_spawning > max_days_until_spawning) {
			throw Exception(std::format("Time until spawning ({}) exceeds maximum ({})",
				_days_until_spawning, max_days_until_spawning).c_str());
		}
	}

	Lanternfish()
		: _days_until_spawning{ max_days_until_spawning }
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

	LanternfishShoal& load(std::istream& stream) try
	{
		_load(stream);
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

	Size_t size() const { return _fish.size(); }

private:

	void _load(std::istream& stream)
	{
		auto str = std::string{};
		std::getline(stream, str);

		const auto spawning_times = split(str, ',');
		_fish.resize(spawning_times.size());

		std::transform(spawning_times.begin(), spawning_times.end(), _fish.begin(), [](const auto& s) {
			return Lanternfish{ string_to<uint32_t>(s) };
			});
	}

	std::vector<Lanternfish> _fish;
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
