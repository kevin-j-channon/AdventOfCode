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

class DiagnosticLog
{
public:
	static constexpr uint32_t entry_size = 12;
	using Entry_t = std::array<bool, entry_size>;

private:
	std::vector<Entry_t> entries;

public:

	using Iterator_t = decltype(entries.begin());
	using ConstIterator_t = decltype(entries.cbegin());
	using Size_t = decltype(entries.size());

	DiagnosticLog(std::istream& is)
	{
		load(is);
	}

	DiagnosticLog() {}


	void load(std::istream& is) try
	{
		entries.clear();

		using Iter_t = std::istream_iterator<Entry_t>;
		std::copy(Iter_t(is), Iter_t(), std::back_inserter(entries));
	}
	catch (const Exception&)
	{
		entries.clear();

		throw;
	}

	ConstIterator_t begin() const { return entries.begin(); }
	Iterator_t begin() { return entries.begin(); }

	ConstIterator_t end() const { return entries.end(); }
	Iterator_t end() { return entries.end(); }

	Size_t size() const { return entries.size(); }

	Entry_t get_most_frequent_bits() const
	{
		return get_most_frequent_bits(begin(), end());
	}

	template<typename LogEntryIter_T>
	static Entry_t get_most_frequent_bits(LogEntryIter_T begin, LogEntryIter_T end)
	{
		auto bit_counts = get_bit_counts(begin, end);
		return create_most_common_bits(std::move(bit_counts));
	}

	template<typename Out_T>
	static Out_T entry_as(const Entry_t& entry)
	{
		return std::accumulate(entry.begin(), entry.end(), Out_T{ 0 }, [](auto curr, auto next) {
			curr <<= 1;
			if (next)
				curr |= 1;

			return curr;
			});
	}

	template<typename Out_T>
	static Out_T flipped_entry_as(const Entry_t& entry)
	{
		return std::accumulate(entry.begin(), entry.end(), Out_T{ 0 }, [](auto curr, auto next) {
			curr <<= 1;
			if (!next)
				curr |= 1;

			return curr;
			});
	}

private:
	template<typename LogEntryIter_T>
	static std::array<int, aoc::DiagnosticLog::entry_size> get_bit_counts(LogEntryIter_T begin, LogEntryIter_T end)
	{
		return std::accumulate(begin, end, std::array<int, aoc::DiagnosticLog::entry_size>{},
			[](auto&& curr, auto&& entry) {
				std::transform(curr.begin(), curr.end(), entry.begin(), curr.begin(),
					[](auto count, auto bit) {
						return count + (bit ? 1 : -1);
					});

				return curr;
			});
	}

	static Entry_t create_most_common_bits(std::array<int, aoc::DiagnosticLog::entry_size>&& counts)
	{
		auto out = Entry_t{};
		std::transform(counts.begin(), counts.end(), out.begin(), [](auto count) {
			return count >= 0 ? true : false;
			});

		return out;
	}
};

///////////////////////////////////////////////////////////////////////////////

struct PowerParams
{
	using Bits_t = DiagnosticLog::Entry_t;
	Bits_t bits;

	uint32_t gamma_rate() const
	{
		return DiagnosticLog::entry_as<uint32_t>(bits);
	}

	uint32_t epsilon_rate() const
	{
		return DiagnosticLog::flipped_entry_as<uint32_t>(bits);
	}

	template<typename LogLineIter_T>
	static uint32_t power_consumption(LogLineIter_T begin, LogLineIter_T end)
	{
		auto most_frequent_bits = DiagnosticLog::get_most_frequent_bits(begin, end);
		auto power_params = PowerParams{most_frequent_bits};

		return power_params.gamma_rate() * power_params.epsilon_rate();
	}
};

///////////////////////////////////////////////////////////////////////////////

struct LifeSupportParams
{
	using Bits_t = std::array<bool, DiagnosticLog::entry_size>;
	Bits_t bits;

	uint32_t oxygen_gen_rating(const DiagnosticLog& log) const
	{
		const auto mfb = log.get_most_frequent_bits();
		auto scores = std::vector<uint32_t>(log.size(), 0);
		std::transform(log.begin(), log.end(), scores.begin(),
			[&mfb](const auto& entry) {
				auto i = uint32_t{ 0 };
				while (entry[i] == mfb[i]) ++i;
				return i;
			});

		const auto oxy_gen_element = std::next(log.begin(), std::distance(scores.begin(), std::max_element(scores.begin(), scores.end())));

	}
};

///////////////////////////////////////////////////////////////////////////////

class Submarine
{
public:

	template<size_t WINDOW_SIZE, typename LogLineIter_T>
	uint32_t depth_score(LogLineIter_T begin, LogLineIter_T end)
	{
		auto current_idx = size_t{ 0 };

		auto window = std::array<typename LogLineIter_T::value_type, WINDOW_SIZE>{};
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

	template<typename LogLineIter_T>
	Direction net_direction(LogLineIter_T begin, LogLineIter_T end)
	{
		return std::accumulate(begin, end, Direction{});
	}

	template<typename LogLineIter_T>
	Direction net_aiming(LogLineIter_T begin, LogLineIter_T end)
	{
		return std::accumulate(begin, end, Aiming{}).to_direction();
	}

	template<typename LogLineIter_T>
	uint32_t power_consumption(LogLineIter_T begin, LogLineIter_T end)
	{
		return PowerParams::power_consumption(begin, end);
	}

	uint32_t life_support_rating(const DiagnosticLog& log)
	{
		return 0;
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

istream& operator>>(istream& is, aoc::DiagnosticLog::Entry_t& entry)
{
	entry = aoc::DiagnosticLog::Entry_t{};

	auto param_str = std::string{};
	is >> param_str;

	if (param_str.empty())
	{
		is.setstate(std::ios::failbit);
		return is;
	}

	if (param_str.length() != aoc::DiagnosticLog::entry_size) {
		is.setstate(std::ios::failbit);
		throw aoc::Exception(std::format("Invalid log line: {}", param_str));
	}

	try {
		std::transform(param_str.begin(), param_str.end(), entry.begin(),
			[](auto c) {
				switch (c)
				{
				case '0': return false;
				case '1': return true;
				default:
					throw aoc::Exception(std::format("Invalid character in log line: {}", c));
				}
			});
	}
	catch (const aoc::Exception&)
	{
		is.setstate(std::ios::failbit);
		throw;
	}

	return is;
}

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
