#pragma once

#include "StringOperations.hpp"

#include <numeric>
#include <array>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <format>
#include <istream>
#include <bitset>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
struct Vec2d
{
	Value_T x{};
	Value_T y{};
};

///////////////////////////////////////////////////////////////////////////////

struct Exception : public std::runtime_error
{
	Exception(const std::string& msg) : std::runtime_error{ msg } {}
};

///////////////////////////////////////////////////////////////////////////////

using Direction = Vec2d<int>;

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
Vec2d<Value_T> operator+(const Vec2d<Value_T>& v1, const Vec2d<Value_T>& v2)
{
	return { v1.x + v2.x, v1.y + v2.y };
}

///////////////////////////////////////////////////////////////////////////////

struct Aiming
{
	int x{};
	int aim{};
	int depth{};

	Direction to_direction() const
	{
		return { x, depth };
	}

	Aiming operator+(const Direction& d) const
	{
		const auto new_aim = this->aim + d.y;
		return {
			this->x + d.x,
			new_aim,
			this->depth + d.x * new_aim
		};
	}
};

///////////////////////////////////////////////////////////////////////////////

struct PowerParams
{
	static constexpr uint32_t bit_count = 12;

	using Bits_t = std::array<bool, bit_count>;
	Bits_t bits;

	uint32_t gamma_rate() const
	{
		return std::accumulate(bits.begin(), bits.end(), uint32_t{ 0 }, [](auto curr, auto next) {
			curr <<= 1;
			if (next)
				curr |= 1;

			return curr;
			});
	}

	uint32_t epsilon_rate() const
	{
		return std::accumulate(bits.begin(), bits.end(), uint32_t{ 0 }, [](auto curr, auto next) {
			curr <<= 1;
			if (!next)
				curr |= 1;

			return curr;
			});
	}
};

///////////////////////////////////////////////////////////////////////////////

class DiagnosticLog
{
public:
	static constexpr uint32_t entry_size = 12;
	using Entry_t = std::array<bool, entry_size>;

private:
	std::vector<Entry_t> entries;

public:

	using Iterator_t = decltype(entries.begin());

	void load(std::istream& is) try
	{
		entries.clear();

		using Iter_t = std::istream_iterator<std::string>;
		std::transform(Iter_t(is), Iter_t(), std::back_inserter(entries), createEntryFromString);
	}
	catch (const Exception&)
	{
		is.setstate(std::ios::failbit);
		entries.clear();

		throw;
	}

private:
	static Entry_t createEntryFromString(const std::string& str)
	{
		if (str.length() != entry_size)
			throw Exception(std::format("Invalid log entry: {}", str));

		auto entry = Entry_t{};
		std::transform(str.begin(), str.end(), entry.begin(), [](auto c) { return c == '1' ? true : false; });
		return entry;
	}
};

///////////////////////////////////////////////////////////////////////////////

class Submarine
{
public:

	template<size_t WINDOW_SIZE, typename Iter_T>
	uint32_t depth_score(Iter_T begin, Iter_T end)
	{
		auto current_idx = size_t{ 0 };

		auto window = std::array<typename Iter_T::value_type, WINDOW_SIZE>{};
		for (auto it = window.begin(); it < window.end(); ++it, ++current_idx) {
			*it = *begin++;
		}

		auto prev_score = std::accumulate(window.begin(), window.end(), uint32_t{ 0 });

		return std::accumulate(begin, end, 0, [&window, &prev_score, &current_idx](auto curr, auto next) {

			auto new_score = uint32_t{ prev_score - window[current_idx % window.size()] + next };

			auto out = new_score > prev_score ? ++curr : curr;

			prev_score = new_score;
			window[current_idx % window.size()] = next;
			++current_idx;

			return out;
			});
	}

	template<typename Iter_T>
	Direction net_direction(Iter_T begin, Iter_T end)
	{
		return std::accumulate(begin, end, Direction{});
	}

	template<typename Iter_T>
	Direction net_aiming(Iter_T begin, Iter_T end)
	{
		return std::accumulate<>(begin, end, Aiming{}).to_direction();
	}

	template<typename Iter_T>
	PowerParams evaluate_power_params(Iter_T begin, Iter_T end)
	{
		auto bit_counts = std::accumulate(begin, end, std::array<int, aoc::PowerParams::bit_count>{},
			[](auto&& curr, auto&& pp) {
				std::transform(curr.begin(), curr.end(), pp.bits.begin(), curr.begin(),
					[](auto count, auto bit) {
						return count + (bit ? 1 : -1);
					});

				return curr;
			});

		auto out = PowerParams{};
		std::transform(bit_counts.begin(), bit_counts.end(), out.bits.begin(), [](auto count) {
			return count >= 0 ? true : false;
			});

		return out;
	}
};

///////////////////////////////////////////////////////////////////////////////

}	// namespace: aoc

///////////////////////////////////////////////////////////////////////////////

namespace std
{

///////////////////////////////////////////////////////////////////////////////

istream& operator>>(istream& is, aoc::Direction& direction)
{
	auto cmd = std::string{};

	direction = aoc::Direction{ 0, 0 };

	is >> cmd;

	if (cmd.empty())
		return is;

	if (cmd == "forward")
	{
		is >> direction.x;
	}
	else if (cmd == "up")
	{
		is >> direction.y;
		direction.y *= -1;
	}
	else if (cmd == "down")
	{
		is >> direction.y;
	}
	else {
		throw aoc::Exception(std::format("Invalid direction string"));
	}

	if (is.fail())
		throw aoc::Exception(std::format("Invalid direction string"));

	return is;
}

///////////////////////////////////////////////////////////////////////////////

istream& operator>>(istream& is, aoc::PowerParams& pp)
{
	pp = aoc::PowerParams{};

	auto param_str = std::string{};
	is >> param_str;

	if (param_str.length() != aoc::PowerParams::bit_count) {
		is.setstate(std::ios::failbit);
		return is;
	}

	std::transform(param_str.begin(), param_str.end(), pp.bits.begin(), [](auto c) { return c == '1' ? true : false; });

	return is;
}

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
