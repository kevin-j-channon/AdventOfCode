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
	using RowIterator_t = decltype(_data.begin());
	using ConstRowIterator_t = decltype(_data.cbegin());

	Table(Size_t row_size, Size_t col_size)
		: _data(row_size * col_size)
		, _row_size{row_size}
		, _col_size{col_size}
	{}

	Size_t row_size() const { return _row_size; }
	Size_t col_size() const { return _col_size; }

	ConstRowIterator_t row_begin(Size_t row) const { return std::next(_data.begin(), row * _row_size); }
	RowIterator_t row_begin(Size_t row) { return std::next(_data.begin(), row * _row_size); }

	ConstRowIterator_t row_end(Size_t row) const { return std::next(_data.begin(), (row + 1) * _row_size); }
	RowIterator_t row_end(Size_t row) { return std::next(_data.begin(), (row + 1) * _row_size); }

	Value_t& operator()(Size_t row, Size_t col)
	{
		return _data[row * _row_size + col];
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
		if (row >= _row_size)
			throw Exception("Row out of range");

		if (col >= _col_size)
			throw Exception("Col out of range");
	}

	Size_t _row_size;
	Size_t _col_size;
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
