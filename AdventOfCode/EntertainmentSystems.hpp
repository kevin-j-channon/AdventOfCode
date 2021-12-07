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

class Bingo
{
public:

	using Draws_t = std::vector<uint8_t>;

	class Board
	{
		using ValueGrid_t = std::vector<std::vector<uint8_t>>;

	public:
		Board(uint8_t size)
			: _size{ size }
		{}

		void load(std::istream& stream)
		{
			for (auto row = 0; row < _size && stream.good(); ++row) {
				auto line = std::string{};
				std::getline(stream, line);
				const auto value_strings = split(line, ',');
				if (value_strings.size() != _size) {
					throw Exception("Invalid bingo board size board");
				}

				auto row_values = ValueGrid_t::value_type(_size, 0);
				std::transform(value_strings.begin(), value_strings.end(), row_values.begin(), [](auto s) { return static_cast<uint8_t>(std::stol(s)); });

				_numbers.push_back(std::move(row_values));
			}
		}
	private:
		uint8_t _size;
		ValueGrid_t _numbers;
	};

	using Boards_t = std::vector<Board>;

	void load(std::istream& stream)
	{
		auto line = std::string{};
		std::getline(stream, line);
		const auto draws_strings = split(line, ',');
		_draws.resize(draws_strings.size(), 0);
		std::transform(draws_strings.begin(), draws_strings.end(), _draws.begin(), [](auto s) { return static_cast<uint8_t>(std::stol(s)); });
	}

	const Draws_t& draws() const { return _draws; }
	const Boards_t& boards() const { return _boards; }

private:
	Draws_t _draws;
	Boards_t _boards;
};

///////////////////////////////////////////////////////////////////////////////

class EntertainmentSystems
{
public:

	Bingo bingo() const { return Bingo{}; }
};

///////////////////////////////////////////////////////////////////////////////

}	// namespace: aoc

///////////////////////////////////////////////////////////////////////////////
