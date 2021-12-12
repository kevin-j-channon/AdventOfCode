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
istream& operator>>(istream& is, aoc::Line2d<Value_T>& line)
{
	line = aoc::Line2d<Value_T>{{0, 0}, {0, 0}};

	if (is.eof())
		return is;

	auto point_str = std::string{};
	is >> point_str;

	auto x_and_y_str = split(point_str, ',');
	if (x_and_y_str.size() != 2) {
		is.setstate(std::ios::failbit);
		throw aoc::Exception("Failed to read line start point");
	}

	auto start = aoc::Vec2d<Value_T>{ static_cast<Value_T>(std::stol(x_and_y_str[0])), static_cast<Value_T>(std::stol(x_and_y_str[1])) };

	// This isn't a point, but the arrow. We reuse this string though, instead of creating a pointless one.
	is >> point_str;
	if (point_str != "->") {
		is.setstate(std::ios::failbit);
		throw aoc::Exception(std::format("Invalid point seperator: {}", point_str));
	}

	// This is the second point.
	point_str.clear();
	is >> point_str;

	x_and_y_str = split(point_str, ',');
	if (x_and_y_str.size() != 2) {
		is.setstate(std::ios::failbit);
		throw aoc::Exception("Failed to read line start point");
	}

	auto finish = aoc::Vec2d<Value_T>{ string_to<uint32_t>(x_and_y_str[0]), string_to<uint32_t>(x_and_y_str[1]) };

	line.start = std::move(start);
	line.finish = std::move(finish);

	return is;
}

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
