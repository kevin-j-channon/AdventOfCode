#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "StringOperations.hpp"

#include <istream>
#include <string>
#include <vector>
#include <cstdint>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

namespace bingo
{
class Draws
{
public:
	using Value_t = uint8_t;

private:
	std::vector<Value_t> _values;

public:

	using Iterator_t = decltype(_values.begin());
	using ConstIterator_t = decltype(_values.cbegin());
	using Size_t = decltype(_values.size());

	ConstIterator_t begin() const { return _values.begin(); }
	Iterator_t begin() { return _values.begin(); }

	ConstIterator_t end() const { return _values.end(); }
	Iterator_t end() { return _values.end(); }

	Size_t size() const { return _values.size(); }

	void load(std::istream& stream)
	{
		auto line = std::string{};
		std::getline(stream, line);
		
		const auto value_strings = split(line, ',');

		_values.resize(value_strings.size(), 0);
		std::transform(value_strings.begin(), value_strings.end(), _values.begin(), [](auto s) { return static_cast<uint8_t>(std::stol(s)); });
	}
};

class Board
{
	using ValueGrid_t = std::vector<std::vector<uint8_t>>;

public:

	struct Cell
	{
		uint8_t value;
		bool is_marked{ false };
	};

	Board(uint8_t size)
		: _size{ size }
	{}

	void load(std::istream& stream)
	{
		for (auto row = 0; row < _size && stream.good(); ++row) {
			auto line = std::string{};
			std::getline(stream, line);
			const auto value_strings = split(line, ' ', SplitBehaviour::drop_empty);
			if (value_strings.size() != _size) {
				throw Exception("Invalid bingo board size board");
			}

			std::transform(value_strings.begin(), value_strings.end(),.begin(), [](auto s) { return static_cast<uint8_t>(std::stol(s)); });

			_numbers.push_back(std::move(row_values));
		}
	}
private:
	uint8_t _size;
	Table< _numbers;
};

using Boards_t = std::vector<Board>;
};

///////////////////////////////////////////////////////////////////////////////

class EntertainmentSystems
{
public:

};

///////////////////////////////////////////////////////////////////////////////

}	// namespace: aoc

///////////////////////////////////////////////////////////////////////////////
