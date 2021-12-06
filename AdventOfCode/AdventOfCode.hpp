#pragma once

#include "StringOperations.hpp"

#include <numeric>
#include <array>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <format>
#include <istream>
#include <map>

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

	static Entry_t flip_entry(Entry_t entry)
	{
		std::transform(entry.begin(), entry.end(), entry.begin(), [](auto x) {return !x; });
		return entry;
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

class LifeSupport
{
public:
	LifeSupport(const DiagnosticLog& log) : _log(log) {}

	static uint32_t score_entry(const DiagnosticLog::Entry_t& entry, const DiagnosticLog::Entry_t& target)
	{
		return static_cast<uint32_t>(std::distance(entry.begin(), std::mismatch(entry.begin(), entry.end(), target.begin()).first));
	}

	uint32_t find_best_match_to(const DiagnosticLog::Entry_t& target) const
	{
		// Make a vector of scores: the number of bits at the start of each log line that match the most frequent bits.
		auto scores = std::vector<uint32_t>(_log.size(), 0);
		std::transform(_log.begin(), _log.end(), scores.begin(),
			[&target](const auto& entry) {
				return score_entry(entry, target);
			});

		// Count the number of occurances of each score
		auto score_counts = std::accumulate(scores.begin(), scores.end(), std::map<uint32_t, uint32_t>{},
			[](auto&& counts, auto&& score) {
				counts[score]++;
				return std::move(counts);
			});

		// find the lowest count for which the score is 1, which would be the first one that we would filter to.
		const auto target_score_and_count = std::find_if(score_counts.begin(), score_counts.end(), [](auto score_and_count) {
			return score_and_count.second == 1;
			});

		// Find the first log entry that has this score.
		const auto final_element = std::find_if(_log.begin(), _log.end(),
			[&target_score_and_count, &target](auto entry) {
				return score_entry(entry, target) == target_score_and_count->first;
			});
		
		return DiagnosticLog::entry_as<uint32_t>(*final_element);
	}

	uint32_t rating() const
	{
		auto most_frequent_bits = _log.get_most_frequent_bits();
		const auto o2_gen_rating = find_best_match_to(most_frequent_bits);
		const auto co2_scrubber_rating = find_best_match_to(DiagnosticLog::flip_entry(std::move(most_frequent_bits)));

		return o2_gen_rating * co2_scrubber_rating;
	}

private:
	const DiagnosticLog& _log;
};

///////////////////////////////////////////////////////////////////////////////

struct LogProcessor
{
	static uint32_t power_consumption(const DiagnosticLog& log)
	{
		const auto most_frequent_bits = log.get_most_frequent_bits();
		return DiagnosticLog::entry_as<uint32_t>(most_frequent_bits) * DiagnosticLog::flipped_entry_as<uint32_t>(most_frequent_bits);
	}

	static uint32_t life_support_rating(const DiagnosticLog& log)
	{
		return LifeSupport(log).rating();
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

	template<typename Iter_T>
	Direction net_direction(Iter_T begin, Iter_T end)
	{
		return std::accumulate(begin, end, Direction{});
	}

	template<typename Iter_T>
	Direction net_aiming(Iter_T begin, Iter_T end)
	{
		return std::accumulate(begin, end, Aiming{}).to_direction();
	}

	uint32_t power_consumption(const DiagnosticLog& log)
	{
		return LogProcessor::power_consumption(log);
	}

	uint32_t life_support_rating(const DiagnosticLog& log)
	{
		return LogProcessor::life_support_rating(log);
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
