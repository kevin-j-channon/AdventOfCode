#pragma once

///////////////////////////////////////////////////////////////////////////////

#include <stdexcept>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

struct Exception : public std::runtime_error
{
	Exception(const std::string& msg) : std::runtime_error{ msg } {}
};

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
struct Vec2d
{
	Value_T x{};
	Value_T y{};
};

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
Vec2d<Value_T> operator+(const Vec2d<Value_T>& v1, const Vec2d<Value_T>& v2)
{
	return { v1.x + v2.x, v1.y + v2.y };
}

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
class Table
{
public:
	using Value_t = Value_T;
	using Container_t = std::vector<Value_t>;

private:
	Container_t _data;

public:
	using Size_t = decltype(_data.size());
	using Iterator_t = decltype(_data.begin());
	using ConstIterator_t = decltype(_data.cbegin());
	using RowIterator_t = decltype(_data.begin());
	using ConstRowIterator_t = decltype(_data.cbegin());

	Table(Size_t rows, Size_t cols)
		: _data(rows* cols, Value_t{})
		, _rows{rows}
		, _cols{cols}
	{}

	Size_t rows() const { return _rows; }
	Size_t cols() const { return _cols; }

	ConstIterator_t begin() const { return _data.begin(); }
	Iterator_t begin() { return _data.begin(); }

	ConstIterator_t end() const { return _data.end(); }
	Iterator_t end() { return _data.end(); }

	ConstRowIterator_t row_begin(Size_t row) const { return std::next(_data.begin(), row * _cols); }
	RowIterator_t row_begin(Size_t row) { return std::next(_data.begin(), row * _cols); }

	ConstRowIterator_t row_end(Size_t row) const { return std::next(_data.begin(), (row + 1) * _cols); }
	RowIterator_t row_end(Size_t row) { return std::next(_data.begin(), (row + 1) * _cols); }

	Value_t& operator()(Size_t row, Size_t col)
	{
		return _data[row * _cols + col];
	}

	const Value_t& operator()(Size_t row, Size_t col) const
	{
		return const_cast<Table&>(*this)(row, col);
	}

	const Value_t& at(Size_t row, Size_t col) const
	{
		_validate_element(row, col);

		return this->operator()(row, col);
	}

	void set(Size_t row, Size_t col, Value_t val)
	{
		_validate_element(row, col);

		this->operator()(row, col) = val;
	}

private:
	void _validate_element(Size_t row, Size_t col) const
	{
		if (row >= _rows)
			throw Exception("Row out of range");

		if (col >= _cols)
			throw Exception("Col out of range");
	}

	Size_t _rows;
	Size_t _cols;
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
