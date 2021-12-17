#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "StringOperations.hpp"

#include <armadillo>

#include <stdexcept>
#include <string>
#include <vector>
#include <istream>
#include <cmath>

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

	bool operator==(const This_t& other) const
	{
		return x == other.x && y == other.y;
	}

	bool operator!=(const This_t& other) const
	{
		return !(*this == other);
	}


	bool operator<(const This_t& other) const
	{
		if (x != other.x)
			return x < other.x;

		if (y != other.y)
			return y < other.y;

		return false;
	}

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

	enum Orientation
	{
		horizontal = 0b001,
		vertical   = 0b010,
		diagonal   = 0b100,
	};

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

template<typename Value_T>
bool is_horizontal(const Line2d<Value_T>& line)
{
	return line.start.y == line.finish.y;
}

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
bool is_diagonal(const Line2d<Value_T>& line)
{
	return std::abs(static_cast<int>(line.finish.x) - static_cast<int>(line.start.x)) == std::abs(static_cast<int>(line.finish.y) - static_cast<int>(line.start.y));
}

///////////////////////////////////////////////////////////////////////////////

template<size_t ORIENTATION, typename Value_T>
std::vector<Vec2d<Value_T>> rasterize(const Line2d<Value_T>& line)
{
	auto out = std::vector<Vec2d<Value_T>>{};
	
	if constexpr (static_cast<bool>(ORIENTATION & Line2d<Value_T>::horizontal)) {
		if (is_vertical(line)) {
			const auto y_min = std::min(line.start.y, line.finish.y);
			const auto y_max = std::max(line.start.y, line.finish.y);

			auto out = std::vector<Vec2d<Value_T>>{ y_max - y_min + 1 };

			for (auto y = y_min; y <= y_max; ++y) {
				out[y - y_min] = Vec2d<Value_T>{ line.start.x, y };
			}

			return out;
		}
	}

	if constexpr (static_cast<bool>(ORIENTATION & Line2d<Value_T>::vertical)) {
		if (is_horizontal(line)) {
			const auto x_min = std::min(line.start.x, line.finish.x);
			const auto x_max = std::max(line.start.x, line.finish.x);

			auto out = std::vector<Vec2d<Value_T>>{x_max - x_min + 1};

			for (auto x = x_min; x <= x_max; ++x) {
				out[x - x_min] = Vec2d<Value_T>{ x, line.start.y };
			}

			return out;
		}
	}

	if constexpr (static_cast<bool>(ORIENTATION & Line2d<Value_T>::diagonal)) {
		if (is_diagonal(line)) {
			const auto [lower, upper] = line.start.x < line.finish.x ? std::make_pair(line.start, line.finish) : std::make_pair(line.finish, line.start);
			const auto y_increment = lower.y < upper.y ? 1 : -1;

			auto out = std::vector<Vec2d<Value_T>>{ upper.x - lower.x + 1 };

			for (auto point = lower; point.x <= upper.x; ++point.x, point.y += y_increment)
			{
				out[point.x - lower.x] = point;
			}

			return out;
		}
	}

	throw Exception("Only horizontal or vertical lines can be rasterized");
}

///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
struct Point3D
{
	using Value_t = Value_T;
	using This_t = Point3D<Value_t>;

	Point3D() : x{ 0 }, y{ 0 }, z{ 0 } {}

	Point3D(Value_t x_, Value_t y_, Value_t z_)
		: x{ std::move(x_) }
		, y{ std::move(y_) }
		, z{ std::move(z_) }
	{}

	Point3D(const This_t&) = default;
	This_t& operator=(const This_t&) = default;

	Point3D(This_t&&) = default;
	This_t& operator=(This_t&&) = default;

	Value_T x;
	Value_T y;
	Value_T z;
};

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
