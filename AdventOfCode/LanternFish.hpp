#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "StringOperations.hpp"
#include "Common.hpp"

#include <cstdint>
#include <istream>
#include <string>
#include <stdexcept>
#include <format>
#include <chrono>
#include <array>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

class Lanternfish
{
public:

	static const auto max_days_until_spawning = uint32_t{ 8 };
	static const auto days_until_spawning_reset_value = uint32_t{ 6 };

	explicit Lanternfish(uint32_t days_until_spawning)
		: _spawn_now{ false }
		, _days_until_spawning{ days_until_spawning }
	{
		if (_days_until_spawning > max_days_until_spawning) {
			throw Exception(std::format("Time until spawning ({}) exceeds maximum ({})",
				_days_until_spawning, max_days_until_spawning).c_str());
		}
	}

	Lanternfish()
		: _spawn_now{false}
		, _days_until_spawning { max_days_until_spawning }
	{}

	uint32_t days_until_spawning() const { return _days_until_spawning; }

	void decrement_time_to_spawning()
	{
		if (_days_until_spawning == 0) {
			_days_until_spawning = days_until_spawning_reset_value;
			_spawn_now = true;
		}
		else {
			_spawn_now = false;
			--_days_until_spawning;
		}
	}

	bool spawn_now() const { return _spawn_now; }

private:
	bool _spawn_now;
	uint32_t _days_until_spawning;
};

///////////////////////////////////////////////////////////////////////////////

class LanternfishShoal
{
	std::vector<Lanternfish> _fish;

public:

	using Size_t = size_t;
	using Iterator_t = decltype(_fish.begin());
	using ConstIterator_t = decltype(_fish.cbegin());

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

	Iterator_t begin() { return _fish.begin(); }
	ConstIterator_t begin() const { return _fish.begin(); }

	Iterator_t end() { return _fish.end(); }
	ConstIterator_t end() const { return _fish.end(); }

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
};

///////////////////////////////////////////////////////////////////////////////

class LanternfishShoalModel
{
public:
	LanternfishShoalModel(LanternfishShoal& shoal)
	{
		std::fill(_fish_counts.begin(), _fish_counts.end(), 0);
		std::for_each(shoal.begin(), shoal.end(), [this](const auto& fish) {
			_fish_counts[fish.days_until_spawning()]++;
			});
	}

	LanternfishShoalModel& run_for(std::chrono::days run_time)
	{
		while (run_time-- > std::chrono::days{ 0 }) {
			_step();
		}
		return *this;
	}

	LanternfishShoal::Size_t shoal_size() const { return std::accumulate(_fish_counts.begin(), _fish_counts.end(), LanternfishShoal::Size_t{0}); }

private:

	void _step()
	{
		const auto zero_day_fish_count = _fish_counts[0];
		std::copy(std::next(_fish_counts.begin()), _fish_counts.end(), _fish_counts.begin());
		_fish_counts.back() = zero_day_fish_count;
		_fish_counts[Lanternfish::days_until_spawning_reset_value] += zero_day_fish_count;
	}

	std::array<LanternfishShoal::Size_t, Lanternfish::max_days_until_spawning + 1> _fish_counts;
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
