#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "Common.hpp"

#include <cstdint>
#include <vector>
#include <array>
#include <istream>
#include <numeric>
#include <algorithm>
#include <format>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

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
		return most_frequent_bits(begin(), end());
	}

	template<typename LogEntryIter_T>
	static Entry_t most_frequent_bits(LogEntryIter_T begin, LogEntryIter_T end)
	{
		auto bit_counts = bit_balance(begin, end);
		return create_most_common_bits(std::move(bit_counts));
	}

	Entry_t get_least_frequent_bits() const
	{
		return least_frequent_bits(begin(), end());
	}

	template<typename LogEntryIter_T>
	static Entry_t least_frequent_bits(LogEntryIter_T begin, LogEntryIter_T end)
	{
		auto bit_counts = bit_balance(begin, end);
		return create_least_common_bits(std::move(bit_counts));
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
	static std::array<int, aoc::DiagnosticLog::entry_size> bit_balance(LogEntryIter_T begin, LogEntryIter_T end)
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

	static Entry_t create_least_common_bits(std::array<int, aoc::DiagnosticLog::entry_size>&& counts)
	{
		auto out = Entry_t{};
		std::transform(counts.begin(), counts.end(), out.begin(), [](auto count) {
			return count >= 0 ? false : true;
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

inline istream & operator>>(istream & is, aoc::DiagnosticLog::Entry_t & entry)
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

}	// namespace: std

///////////////////////////////////////////////////////////////////////////////
