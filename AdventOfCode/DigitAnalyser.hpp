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

	void determine_reference_digits()
	{
		std::for_each(_data.begin(), _data.end(), _determine_reference_digits_for);
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

	void _determine_reference_digits_for(const DigitData& data)
	{
		// Find easy digits
		const auto& ref_strings = data.reference_value_strings();

		const auto str_1 = std::find_if(ref_strings.begin(), ref_strings.end(), [](const auto& s) { return s.length() == 2; });
		const auto str_4 = std::find_if(ref_strings.begin(), ref_strings.end(), [](const auto& s) { return s.length() == 4; });
		const auto str_7 = std::find_if(ref_strings.begin(), ref_strings.end(), [](const auto& s) { return s.length() == 3; });
		const auto str_8 = std::find_if(ref_strings.begin(), ref_strings.end(), [](const auto& s) { return s.length() == 7; });

		auto digit_mapping = std::array<char, 10>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

		auto string_to_bits = [](auto&& curr, const auto& next) { return curr & (next - 'a'); };

		digit_mapping[1] = std::accumulate(str_1->begin(), str_1->end(), '\0', string_to_bits);
		digit_mapping[4] = std::accumulate(str_4->begin(), str_4->end(), '\0', string_to_bits);
		digit_mapping[7] = std::accumulate(str_7->begin(), str_7->end(), '\0', string_to_bits);
		digit_mapping[8] = std::accumulate(str_8->begin(), str_8->end(), '\0', string_to_bits);

		auto two_five_three_strings = std::array<std::string, 3>{};
		for ()
		two_five_three_strings.erase(std::remove_if(two_five_three_strings.begin(), two_five_three_strings.end(), [](const auto& str) {
			}), two_five_three_strings.end());

		auto segment_mapping = std::array<char, 7>{ 0, 0, 0, 0, 0, 0, 0 };
		segment_mapping[0] = digit_mapping[1] ^ digit_mapping[7];
		segment_mapping[3] = digit_mapping[4] & digit_mapping[2] & digit_mapping[5];


	}

	/*
		 aaaa 
		c    b
		c    b
		 dddd
		f    e
		f    e
		 gggg
	*/
	static constexpr std::array<char, 10> _digit_bits =
	{
		0b1110111,	// 0
		0b0010010,	// 1
		0b0101011,	// 2
		0b1011011,	// 3
		0b0011110,	// 4
		0b1011101,	// 5
		0b1111101,	// 6
		0b0010011,	// 7
		0b1111111,	// 8
		0b1011111	// 9
	};

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
