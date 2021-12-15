#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "Common.hpp"
#include "StringOperations.hpp"

#include <istream>
#include <array>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{
class DigitData;
}

///////////////////////////////////////////////////////////////////////////////

std::istream& operator>>(std::istream& is, aoc::DigitData& dd);

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

class DigitData
{
public:

	static constexpr auto output_digit_count = 4;
	static constexpr auto reference_digit_count = 10;

	using OutputDigits_t = std::array<std::string, output_digit_count>;
	using ReferenceDigits_t = std::array<std::string, reference_digit_count>;

	DigitData& load(std::istream& is)
	{
		is >> *this;
		return *this;
	}

	const OutputDigits_t& output_value_strings() const { return _output_value_strings; }
	OutputDigits_t& output_value_strings() { return _output_value_strings; }

	const ReferenceDigits_t& reference_value_strings() const { return _reference_value_strings; }
	ReferenceDigits_t& reference_value_strings() { return _reference_value_strings; }

private:
	OutputDigits_t _output_value_strings;
	ReferenceDigits_t _reference_value_strings;
};

///////////////////////////////////////////////////////////////////////////////

class DigitAnalyser
{
public:
	DigitAnalyser& load(std::istream& is)
	{
		using Iter_t = std::istream_iterator<DigitData>;

		while (is.good()) {
			auto dd = DigitData{};
			is >> dd;
			_data.push_back(std::move(dd));
		}

		return *this;
	}

	uint32_t count_1478()
	{
		return std::accumulate(_data.begin(), _data.end(), uint32_t{ 0 }, [this](auto&& curr, const auto& digit_data) {
			return curr + _recognised_digits_count(digit_data);
			});
	}

private:

	static uint32_t _recognised_digits_count(const DigitData& digit_data)
	{
		return static_cast<uint32_t>(std::count_if(digit_data.output_value_strings().begin(), digit_data.output_value_strings().end(), _is_1478));
	}

	static bool _is_1478(const std::string& s)
	{
		return s.length() == 2 || s.length() == 3 || s.length() == 4 || s.length() == 7;
	}

	std::vector<DigitData> _data;
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////

std::istream& operator>>(std::istream& is, aoc::DigitData& dd)
{
	using Iter_t = std::istream_iterator<std::string>;

	if (is.eof())
		return is;

	std::copy_n(Iter_t{ is }, aoc::DigitData::reference_digit_count, dd.reference_value_strings().begin());

	char ignore;
	is >> ignore;

	std::copy_n(Iter_t{ is }, aoc::DigitData::output_digit_count, dd.output_value_strings().begin());

	// Read the newline character.
	is.read(&ignore, 1);

	return is;
}

///////////////////////////////////////////////////////////////////////////////
