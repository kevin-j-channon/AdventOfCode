#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "StringOperations.hpp"

#include <istream>
#include <string>
#include <vector>
#include <cstdint>
#include <optional>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

namespace bingo
{

template<typename Value_T>
class FileBasedNumberDrawer
{
public:
	using Value_t = Value_T;

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
		std::transform(value_strings.begin(), value_strings.end(), _values.begin(), [](auto s) { return static_cast<Value_t>(std::stol(s)); });
	}
};

class Board
{
public:

	struct Cell
	{
		uint8_t value;
		bool is_marked{ false };
	};

	Board(uint8_t size)
		: _numbers{ size, size }
	{}

	Board& load(std::istream& stream)
	{
		for (auto row = 0; row < _numbers.rows() && stream.good(); ++row) {
			if (!stream.good())
			{
				throw Exception("Invalid bingo board size board");
			}

			_load_row(stream, _numbers.row_begin(row));
		}

		return *this;
	}

	bool mark(uint8_t number)
	{
		auto cell = _find(number);
		if (!cell)
			return false;

		cell->is_marked = true;

		return true;
	}

private:

	const Cell* _find(uint8_t number) const
	{
		const auto it = std::find_if(_numbers.begin(), _numbers.end(), [number](auto n) {
			return number == n.value;
			});
		if (_numbers.end() == it)
			return nullptr;

		return &(*it);
	}

	Cell* _find(uint8_t number) { return const_cast<Board*>(this)->find(number); }

	static Cell _string_to_cell(const std::string& str)
	{
		const auto value = std::stol(str);
		if (value > std::numeric_limits<uint8_t>::max())
			throw Exception("Invalid board value");

		return Cell{ static_cast<uint8_t>(value), false };
	}

	void _load_row(std::istream& stream, Table<Cell>::RowIterator_t row)
	{
		auto line = std::string{};
		std::getline(stream, line);

		const auto value_strings = split(line, ' ', SplitBehaviour::drop_empty);
		if (value_strings.size() != _numbers.cols()) {
			throw Exception("Invalid bingo board size board");
		}

		std::transform(value_strings.begin(), value_strings.end(), row, _string_to_cell);
	}

	Table<Cell> _numbers;
};

class Player
{
public:
	Player(Board& board)
		: _board{ board }
	{}

	bool check_for_win(uint8_t number)
	{
		auto maked_a_number = _board.mark(number);
		if (!maked_a_number)
			return false;

		return _board.state() == Board::State_t::win;
	}

private:

	bool _check_board_for_win() const
	{
		return false;
	}

	Board _board;
};

template<typename NumberDrawer_T>
class Game
{
	using Boards_t = std::vector<Board>;
	using Players_t = std::vector<Player>;
public:

	Game()
	{}

	Game&& load(std::istream& stream)
	{
		_load_drawer(stream);
		_load_boards(stream);

		return std::move(*this);
	}

	Game&& play()
	{
		const auto winning_number = std::find_if(_drawer.begin(), _drawer.end(), [this](auto number) {
			return true;
			});

		return std::move(*this);
	}

private:

	void _load_drawer(std::istream& stream)
	{
		_drawer.load(stream);
	}

	void _load_boards(std::istream& stream)
	{
		while (_skip_blank_line(stream))
		{
			_load_board(stream);
		}
	}

	void _load_board(std::istream& stream)
	{
		auto board = Board{ 5 };
		board.load(stream);
		_boards.push_back(std::move(board));
	}

	bool _skip_blank_line(std::istream& stream)
	{
		auto line = std::string{};
		getline(stream, line);

		if (!line.empty())
			throw Exception(std::format("Line that should have been blank actually contained \"{}\"", line));

		return stream.eof();
	}

	NumberDrawer_T _drawer;
	Boards_t _boards;
	Players_t _players;
};
}

///////////////////////////////////////////////////////////////////////////////

class EntertainmentSystems
{
public:

};

///////////////////////////////////////////////////////////////////////////////

}	// namespace: aoc

///////////////////////////////////////////////////////////////////////////////
