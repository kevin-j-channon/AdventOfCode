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

	enum class State_t
	{
		no_win,
		win
	};

	struct Cell
	{
		uint8_t value{ 0 };
		bool is_marked{ false };
	};

	Board(uint8_t size)
		: _numbers{ size, size }
	{}

	Board(const Board&) = default;
	Board& operator=(const Board&) = default;

	Board& load(std::istream& stream)
	{
		for (auto row = 0; row < _numbers.row_count() && stream.good(); ++row) {
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

	State_t state() const
	{
		if (_have_row_win())
			return State_t::win;

		return State_t::no_win;
	}

private:

	bool _have_row_win() const
	{
		for (auto row = 0; row < _numbers.row_count(); ++row)
		{
			if (_is_winning_row(row))
				return true;
		}

		return false;
	}
	
	bool _is_winning_row(uint8_t row) const
	{
		return std::all_of(_numbers.row_begin(row), _numbers.row_end(row),
			[](const auto& cell) {
				return cell.is_marked;
			});
	}

	const Cell* _find(uint8_t number) const
	{
		const auto it = std::find_if(_numbers.begin(), _numbers.end(), [number](auto n) {
			return number == n.value;
			});
		if (_numbers.end() == it)
			return nullptr;

		return &(*it);
	}

	Cell* _find(uint8_t number) { return const_cast<Cell*>(const_cast<const Board*>(this)->_find(number)); }

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
		if (value_strings.size() != _numbers.col_count()) {
			throw Exception("Invalid bingo board size board");
		}

		std::transform(value_strings.begin(), value_strings.end(), row, _string_to_cell);
	}

	Table<Cell> _numbers;
};

class Player
{
public:
	Player()
		: _board{ nullptr }
	{}

	Player(const Player&) = default;
	Player& operator=(const Player&) = default;

	Player& assign_board(Board& board)
	{
		_board = &board;

		return *this;
	}

	Board::State_t play_number(uint8_t number)
	{
		if (!_board)
			throw Exception("Trying to play a Bingo without a board");

		auto maked_a_number = _board->mark(number);
		if (!maked_a_number)
			return Board::State_t::no_win;

		return _board->state();
	}

	const Board* board() const { return _board; }

private:

	bool _check_board_for_win() const
	{
		return false;
	}

	Board* _board;
};

template<typename NumberDrawer_T>
class Game
{
	using Boards_t = std::vector<Board>;
	using Players_t = std::vector<Player>;
public:

	struct WinData
	{
		uint8_t number;
		Board board;
	};

	Game& load(std::istream& stream)
	{
		_load_drawer(stream);
		_load_boards(stream);
		
		_assign_boards_to_players();

		return *this;
	}

	Game& play()
	{
		const auto winning_number = std::find_if(_drawer.begin(), _drawer.end(), [this](auto number) {
			_winning_player = std::find_if(_players.begin(), _players.end(), [number](auto& player) {
				return player.play_number(number) == Board::State_t::win;
				});

			return _players.end() != _winning_player;
			});

		if (winning_number != _drawer.end())
			_winning_number = *winning_number;

		return *this;
	}

	std::optional<WinData> get_winner() const
	{
		if (!_winning_number)
			return std::nullopt;

		return std::optional<WinData>(WinData{ *_winning_number, *_winning_player->board()});
	}

private:

	void _assign_boards_to_players()
	{
		_players.resize(_boards.size());
		std::transform(_boards.begin(), _boards.end(), _players.begin(), [](auto& board) {return Player().assign_board(board); });
	}

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

		return !stream.eof();
	}

	NumberDrawer_T _drawer;
	Boards_t _boards;
	Players_t _players;
	Players_t::const_iterator _winning_player;
	std::optional<uint8_t> _winning_number;
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
