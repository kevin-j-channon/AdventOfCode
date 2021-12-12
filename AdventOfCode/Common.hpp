#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "StringOperations.hpp"

#include <armadillo>

#include <stdexcept>
#include <string>
#include <vector>
#include <istream>

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
	using Value_t = Value_T;
	using This_t = Vec2d<Value_t>;

	Vec2d() : x{ 0 }, y{ 0 } {}

	Vec2d(Value_t x_, Value_t y_)
		: x{ std::move(x_) }
		, y{ std::move(y_) }
	{}

	Vec2d(const This_t&) = default;
	This_t& operator=(const This_t&) = default;

	Vec2d(This_t&&) = default;
	This_t& operator=(This_t&&) = default;

	Value_T x;
	Value_T y;
};

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
Vec2d<Value_T> operator+(const Vec2d<Value_T>& v1, const Vec2d<Value_T>& v2)
{
	return { v1.x + v2.x, v1.y + v2.y };
}

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
struct Line2d
{
	using Value_t = Value_T;
	using This_t = Line2d<Value_t>;

	Line2d() {}

	Line2d(Vec2d<Value_t> start_, Vec2d<Value_t> finish_)
		: start{std::move(start_)}
		, finish{std::move(finish_)}
	{}

	Line2d(const This_t&) = default;
	This_t& operator=(const This_t&) = default;

	Line2d(This_t&&) = default;
	This_t& operator=(This_t&&) = default;

	Vec2d<Value_T> start;
	Vec2d<Value_T> finish;

	This_t& from(std::istream& is)
	{
		is >> *this;

		return *this;
	}
};

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
bool is_vertical(const Line2d<Value_T>& line)
{
	return line.start.x == line.finish.x;
}

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////

namespace std
{

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
istream& operator>>(std::istream& is, aoc::Vec2d<Value_T>& vec) try
{
	if (is.eof())
		return is;

	auto str = std::string{};
	is >> str;

	auto x_and_y_str = split(str, ',');
	if (x_and_y_str.size() != 2) {
		is.setstate(std::ios::failbit);
		throw aoc::Exception("Failed to read Vec2d");
	}

	vec.x = string_to<Value_T>(x_and_y_str[0]);
	vec.y = string_to<Value_T>(x_and_y_str[1]);

	return is;
}
catch (std::invalid_argument&)
{
	is.setstate(std::ios::failbit);
	throw aoc::Exception("Failed to extract Line2d from stream");
}
catch (std::out_of_range&)
{
	is.setstate(std::ios::failbit);
	throw aoc::Exception("Failed to extract Line2d from stream");
}

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
istream& operator>>(istream& is, aoc::Line2d<Value_T>& line) try
{
	line = aoc::Line2d<Value_T>{{0, 0}, {0, 0}};

	if (is.eof())
		return is;

	is >> line.start;

	// Skip the arrow part
	char arrow_buffer[5] = { 0 };
	is.read(arrow_buffer, 4);
	if (strcmp(arrow_buffer, " -> ") != 0) {
		is.setstate(std::ios::failbit);
		throw aoc::Exception("Invalid arrow in Line2d serialization");
	}

	is >> line.finish;

	return is;
}
catch (std::invalid_argument&)
{
	is.setstate(std::ios::failbit);
	throw aoc::Exception("Failed to extract Line2d from stream");
}
catch (std::out_of_range&)
{
	is.setstate(std::ios::failbit);
	throw aoc::Exception("Failed to extract Line2d from stream");
}

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
