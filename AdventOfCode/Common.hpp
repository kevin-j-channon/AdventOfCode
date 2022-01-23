#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "StringOperations.hpp"
#include "Exception.hpp"

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

template<typename Value_T>
struct Point2D
{
	using Value_t = Value_T;
	using This_t = Point2D<Value_t>;

	Point2D() : x{ 0 }, y{ 0 } {}

	constexpr Point2D(Value_t x_, Value_t y_)
		: x{ std::move(x_) }
		, y{ std::move(y_) }
	{}

	Point2D(const This_t&) = default;
	This_t& operator=(const This_t&) = default;

	Point2D(This_t&&) = default;
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

	Point2D& load(std::istream& is) try
	{
		if (is.eof())
			return *this;

		auto str = std::string{};
		is >> str;

		from_string(str);

		return *this;
	}
	catch (const Exception&)
	{
		is.setstate(std::ios::failbit);
		throw;
	}
	catch (const std::invalid_argument&)
	{
		is.setstate(std::ios::failbit);
		throw aoc::Exception("Failed to extract Point2D from stream");
	}
	catch (const std::out_of_range&)
	{
		is.setstate(std::ios::failbit);
		throw aoc::Exception("Failed to extract Point2D from stream");
	}

	Point2D& from_string(const std::string& str)
	{
		auto x_and_y_str = split(str, ',');
		if (x_and_y_str.size() != 2) {
			throw aoc::Exception("Failed to read Point2D");
		}

		this->x = string_to<Value_T>(x_and_y_str[0]);
		this->y = string_to<Value_T>(x_and_y_str[1]);

		return *this;
	}

	Value_T x;
	Value_T y;
};

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
Point2D<Value_T> operator+(const Point2D<Value_T>& v1, const Point2D<Value_T>& v2)
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

	Line2d(Point2D<Value_t> start_, Point2D<Value_t> finish_)
		: start{std::move(start_)}
		, finish{std::move(finish_)}
	{}

	Line2d(const This_t&) = default;
	This_t& operator=(const This_t&) = default;

	Line2d(This_t&&) = default;
	This_t& operator=(This_t&&) = default;

	Point2D<Value_T> start;
	Point2D<Value_T> finish;

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
std::vector<Point2D<Value_T>> rasterize(const Line2d<Value_T>& line)
{
	auto out = std::vector<Point2D<Value_T>>{};
	
	if constexpr (static_cast<bool>(ORIENTATION & Line2d<Value_T>::horizontal)) {
		if (is_vertical(line)) {
			const auto y_min = std::min(line.start.y, line.finish.y);
			const auto y_max = std::max(line.start.y, line.finish.y);

			auto out = std::vector<Point2D<Value_T>>{ y_max - y_min + 1 };

			for (auto y = y_min; y <= y_max; ++y) {
				out[y - y_min] = Point2D<Value_T>{ line.start.x, y };
			}

			return out;
		}
	}

	if constexpr (static_cast<bool>(ORIENTATION & Line2d<Value_T>::vertical)) {
		if (is_horizontal(line)) {
			const auto x_min = std::min(line.start.x, line.finish.x);
			const auto x_max = std::max(line.start.x, line.finish.x);

			auto out = std::vector<Point2D<Value_T>>{x_max - x_min + 1};

			for (auto x = x_min; x <= x_max; ++x) {
				out[x - x_min] = Point2D<Value_T>{ x, line.start.y };
			}

			return out;
		}
	}

	if constexpr (static_cast<bool>(ORIENTATION & Line2d<Value_T>::diagonal)) {
		if (is_diagonal(line)) {
			const auto [lower, upper] = line.start.x < line.finish.x ? std::make_pair(line.start, line.finish) : std::make_pair(line.finish, line.start);
			const auto y_increment = lower.y < upper.y ? 1 : -1;

			auto out = std::vector<Point2D<Value_T>>{ upper.x - lower.x + 1 };

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

template<typename Value_T>
class Rectangle
{
public:
	using Value_t = Value_T;
	using Point_t = Point2D<Value_t>;

	Rectangle()
		: _top_left{0, 0}
		, _bottom_right{0, 0}
	{}

	Rectangle(const Rectangle&) = default;
	Rectangle& operator=(const Rectangle&) = default;

	Rectangle(Rectangle&&) = default;
	Rectangle& operator=(Rectangle&&) = default;

	const Point_t& top_left() const { return _top_left; }
	const Point_t& bottom_right() const { return _bottom_right; }

private:
	Point_t _top_left;
	Point_t _bottom_right;
};

///////////////////////////////////////////////////////////////////////////////

template<typename XValue_T, typename YValue_T = XValue_T, typename ZValue_T = XValue_T>
struct Point3D
{
	using XValue_t = XValue_T;
	using YValue_t = YValue_T;
	using ZValue_t = ZValue_T;
	using This_t = Point3D<XValue_t, YValue_t, ZValue_t>;

	Point3D() : x{ 0 }, y{ 0 }, z{ 0 } {}

	Point3D(XValue_t x_, YValue_t y_, ZValue_t z_)
		: x{ std::move(x_) }
		, y{ std::move(y_) }
		, z{ std::move(z_) }
	{}

	Point3D(const This_t&) = default;
	This_t& operator=(const This_t&) = default;

	Point3D(This_t&&) = default;
	This_t& operator=(This_t&&) = default;

	bool operator==(const This_t & other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	bool operator!=(const This_t & other) const
	{
		return !(*this == other);
	}

	XValue_t x;
	YValue_t y;
	ZValue_t z;
};

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
struct ValueRange
{
	using Value_t = Value_T;
	using This_t = ValueRange<Value_t>;

	ValueRange() : min{ 0 }, max{ 0 } {}

	constexpr ValueRange(Value_t x_, Value_t y_)
		: min{ std::move(x_) }
		, max{ std::move(y_) }
	{}

	ValueRange(const This_t&) = default;
	This_t& operator=(const This_t&) = default;

	ValueRange(This_t&&) = default;
	This_t& operator=(This_t&&) = default;

	bool operator==(const This_t& other) const
	{
		return min == other.x && max == other.y;
	}

	bool operator!=(const This_t& other) const
	{
		return !(*this == other);
	}


	bool operator<(const This_t& other) const
	{
		if (min != other.x)
			return min < other.x;

		if (max != other.y)
			return max < other.y;

		return false;
	}

	ValueRange& load(std::istream& is) try
	{
		if (is.eof())
			return *this;

		auto str = std::string{};
		is >> str;

		from_string(str);

		return *this;
	}
	catch (const Exception&)
	{
		is.setstate(std::ios::failbit);
		throw;
	}
	catch (const std::invalid_argument&)
	{
		is.setstate(std::ios::failbit);
		throw aoc::Exception("Failed to extract ValueRange from stream");
	}
	catch (const std::out_of_range&)
	{
		is.setstate(std::ios::failbit);
		throw aoc::Exception("Failed to extract ValueRange from stream");
	}

	ValueRange& from_string(const std::string& str)
	{
		auto x_and_y_str = split(str, '.', SplitBehaviour::drop_empty);
		if (x_and_y_str.size() != 2) {
			throw aoc::Exception("Failed to read ValueRange");
		}

		this->min = string_to<Value_T>(x_and_y_str[0]);
		this->max = string_to<Value_T>(x_and_y_str[1]);

		return *this;
	}

	Value_T min;
	Value_T max;
};

///////////////////////////////////////////////////////////////////////////////

template<size_t BASE, size_t EXPONENT>
struct Exp
{
	static constexpr size_t value = BASE * Exp<BASE, EXPONENT - 1>::value;
};

template<size_t BASE>
struct Exp<BASE, 0>
{
	static constexpr size_t value = 1;
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////

namespace std
{

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
inline istream& operator>>(std::istream& is, aoc::Point2D<Value_T>& p)
{
	p.load(is);
	return is;
}

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
inline istream& operator>>(istream& is, aoc::Line2d<Value_T>& line) try
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
