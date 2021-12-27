#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "Common.hpp"
#include "StringOperations.hpp"

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{
class DigitData;
}

///////////////////////////////////////////////////////////////////////////////

inline std::istream& operator>>(std::istream& is, aoc::DigitData& dd);

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

	const ReferenceDigits_t& reference_value_strings() const { return _ref_values_str; }
	ReferenceDigits_t& reference_value_strings() { return _ref_values_str; }

	uint32_t decode()
	{
		_determine_reference_digits();
		return _decode_value();
	}

private:

	char _string_to_bits(const std::string& str) const
	{
		return std::accumulate(str.begin(), str.end(), char{ 0 }, [this](auto&& curr, const auto& next) { return curr | _segment_bits[next - 'a']; });
	}

	template<size_t LENGTH>
	static bool _has_length(const std::string& str)
	{
		return str.length() == LENGTH;
	}

	void _determine_reference_digits()
	{
		// Find easy digits
		_ref_digits[1] = _string_to_bits(*std::find_if(_ref_values_str.begin(), _ref_values_str.end(), _has_length<2>));
		_ref_digits[4] = _string_to_bits(*std::find_if(_ref_values_str.begin(), _ref_values_str.end(), _has_length<4>));
		_ref_digits[7] = _string_to_bits(*std::find_if(_ref_values_str.begin(), _ref_values_str.end(), _has_length<3>));
		_ref_digits[8] = _string_to_bits(*std::find_if(_ref_values_str.begin(), _ref_values_str.end(), _has_length<7>));

		auto two_three_five_bits = std::array<char, 3>{};
		auto it_235 = two_three_five_bits.begin();
		{
			for (auto it_ref = _ref_values_str.begin(); it_ref != _ref_values_str.end(); ++it_ref) {
				if (it_ref->length() == 5) {
					*it_235++ = _string_to_bits(*it_ref);
					if (it_235 == two_three_five_bits.end()) {
						break;
					}
				}
			}
		}

		auto zero_six_nine_bits = std::array<char, 3>{};
		{
			auto it_069 = zero_six_nine_bits.begin();
			for (auto it_ref = _ref_values_str.begin(); it_ref != _ref_values_str.end(); ++it_ref) {
				if (it_ref->length() == 6) {
					*it_069++ = _string_to_bits(*it_ref);
					if (it_069 == zero_six_nine_bits.end()) {
						break;
					}
				}
			}
		}

		// SIX will not be equal to ONE when ANDed with ONE.
		_ref_digits[6] = *std::find_if(zero_six_nine_bits.begin(), zero_six_nine_bits.end(), [this](auto bits) {
			return !(_ref_digits[1] == (_ref_digits[1] & bits));
			});


		// NINE will have no non-zero bits if XORd with EIGHT and then ANDed with FOUR (ZERO will have a single non-zero bit in the d position)
		_ref_digits[9] = *std::find_if(zero_six_nine_bits.begin(), zero_six_nine_bits.end(), [this](auto bits) {
			if (bits == _ref_digits[6]) {
				return false;
			}

			return 0 == ((bits ^ _ref_digits[8]) & _ref_digits[4]);
			});

		// ZERO is the one in the zero_six_nine set that hasn't been found yet.
		_ref_digits[0] = *std::find_if(zero_six_nine_bits.begin(), zero_six_nine_bits.end(), [this](auto bits) {
			return !((bits == _ref_digits[6]) || (bits == _ref_digits[9]));
			});

		// THREE is equal to ONE when ANDed with ONE.
		_ref_digits[3] = *std::find_if(two_three_five_bits.begin(), two_three_five_bits.end(), [this](const auto& bits) {
			return _ref_digits[1] == (_ref_digits[1] & bits);
			});

		// FIVE is equal to NINE when ORd with THREE
		_ref_digits[5] = *std::find_if(two_three_five_bits.begin(), two_three_five_bits.end(), [this](const auto& bits) {
			return _ref_digits[9] == (_ref_digits[3] | bits);
			});

		// TWO is the one in the two_three_five set that hasn't been found yet.
		_ref_digits[2] = *std::find_if(two_three_five_bits.begin(), two_three_five_bits.end(), [this](auto bits) {
			return !((bits == _ref_digits[3]) || (bits == _ref_digits[5]));
			});
	}

	uint32_t _decode_value() const
	{
		auto output_digits_bits = std::array<uint8_t, output_digit_count>{};
		std::transform(_output_value_strings.begin(), _output_value_strings.end(), output_digits_bits.begin(), [this](const auto& str) {return _string_to_bits(str); });

		auto decoded_digits = std::array<uint8_t, output_digit_count>{};
		std::transform(output_digits_bits.begin(), output_digits_bits.end(), decoded_digits.begin(), [this](auto bits) {
			return static_cast<uint8_t>(std::distance(_ref_digits.begin(), std::find(_ref_digits.begin(), _ref_digits.end(), bits)));
			});

		return 1000 * decoded_digits[0] + 100 * decoded_digits[1] + 10 * decoded_digits[2] + decoded_digits[3];
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
	static constexpr std::array<char, 7> _segment_bits =
	{
		000000001,	// a
		0b0000010,	// b
		0b0000100,	// c
		0b0001000,	// d
		0b0010000,	// e
		0b0100000,	// f
		0b1000000,	// g
	};

	OutputDigits_t _output_value_strings;
	ReferenceDigits_t _ref_values_str;

	std::array<char, reference_digit_count> _ref_digits;
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

	uint32_t decode_and_sum()
	{
		return std::accumulate(_data.begin(), _data.end(), uint32_t{ 0 }, [this](auto curr, auto& data) {
			return curr + data.decode();
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
