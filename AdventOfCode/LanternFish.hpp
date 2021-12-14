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

	void add_fish(std::vector<Lanternfish>&& new_fish)
	{
		_fish.insert(_fish.end(), new_fish.begin(), new_fish.end());
	}

	void log() const
	{
		for (auto& fish : _fish) {
			Logger::WriteMessage(std::format("{}, ", fish.days_until_spawning()).c_str());
		}

		Logger::WriteMessage("\n");
	}

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
		: _shoal{ shoal }
	{}

	LanternfishShoalModel& run_for(std::chrono::days run_time)
	{
		while (run_time-- > std::chrono::days{ 0 }) {
			_step();
		}
		return *this;
	}

	const LanternfishShoal& shoal() const { return _shoal; }

private:

	void _step()
	{
		_decremnent_time_until_spawning();
		_spawn_fish();
	}

	void _decremnent_time_until_spawning()
	{
		std::for_each(_shoal.begin(), _shoal.end(), [](auto& fish) {
			fish.decrement_time_to_spawning();
			});
	}

	void _spawn_fish()
	{
		const auto new_fish_count = std::count_if(_shoal.begin(), _shoal.end(), [](const auto& fish) {
			return fish.spawn_now();
			});

		_shoal.add_fish(std::vector<Lanternfish>( new_fish_count, Lanternfish{}));
	}

	LanternfishShoal& _shoal;
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
