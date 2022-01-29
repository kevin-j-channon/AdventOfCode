#pragma once

#include "StringOperations.hpp"
#include "Exception.hpp"

namespace aoc
{
namespace snailfish
{

class Value
{
public:
	Value()
		: _left{ 0 }
		, _right{ 0 }
	{}

	Value(uint32_t left, uint32_t right) 
		: _left{ left }
		, _right{ right }
	{}

	auto operator<=>(const Value&) const = default;

	Value& from_stream(std::istream& is)
	{
		_move_to_start_of_values(is);
		const auto details = _extract_values_as_string(is);
		_update_from_comma_sep_value_pair(details);

		return *this;
	}

	std::wstring as_wstring() const
	{
		return std::format(L"[{},{}]", _left, _right);
	}

private:

	static void _move_to_start_of_values(std::istream& is)
	{
		auto openning_bracket = char{};
		is.read(&openning_bracket, 1);
		if (is.fail()) {
			throw IOException("Failed to read snailfish value: Empty stream");
		}

		if (openning_bracket != '[') {
			throw IOException("Failed to read snailfish value: No start brakcet");
		}
	}

	static std::string _extract_values_as_string(std::istream& is)
	{
		std::stringstream details;
		const auto it = std::find_if(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(),
			[&details](auto c) -> bool {
				if (c != ']') {
					details << c;
					return false;
				}

				return true;
			});

		if (it == std::istreambuf_iterator<char>()) {
			throw IOException("Failed to read snailfish value from stream: No end bracket");
		}

		return details.str();
	}

	void _update_from_comma_sep_value_pair(const std::string& str)
	{
		const auto parts = split(str, ',');
		if (parts.size() != 2) {
			throw IOException("Failed to read snailfish value from stream: invalid number of elements");
		}

		_left = string_to<uint32_t>(parts[0]);
		_right = string_to<uint32_t>(parts[1]);
	}

	uint32_t _left;
	uint32_t _right;
};

}
}
