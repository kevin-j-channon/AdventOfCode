#pragma once

///////////////////////////////////////////////////////////////////////////////

#include <stdexcept>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

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

private:
	std::vector<Value_t> _data;

public:
	using Size_t = decltype(_data.size());

	Table(Size_t row_size, Size_t col_size)
		: _data(row_size * col_size)
		, _row_size{row_size}
		, _col_size{col_size}
	{}

	Size_t row_size() const { return _row_size; }
	Size_t col_size() const { return _col_size; }

	Value_t& operator()(Size_t row, Size_t col)
	{
		return _data[row * _row_size + col];
	}

	const Value_t& operator()(Size_t row, Size_t col) const
	{
		return const_cast<Table&>(*this)(row, col);
	}

private:
	Size_t _row_size;
	Size_t _col_size;
};

///////////////////////////////////////////////////////////////////////////////

struct Exception : public std::runtime_error
{
	Exception(const std::string& msg) : std::runtime_error{ msg } {}
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
