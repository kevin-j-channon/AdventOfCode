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
	using Point_t = Point2D<Value_t>;
	using This_t = Line2d<Value_t>;

	enum Orientation
	{
		horizontal = 0b001,
		vertical   = 0b010,
		diagonal   = 0b100,
	};

	Line2d() {}

	Line2d(Point_t start_, Point_t finish_)
		: start{std::move(start_)}
		, finish{std::move(finish_)}
	{}

	Line2d(const This_t&) = default;
	This_t& operator=(const This_t&) = default;

	Line2d(This_t&&) = default;
	This_t& operator=(This_t&&) = default;

	Point_t start;
	Point_t finish;

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

	Rectangle(Point_t top_left, Point_t bottom_right)
		: _top_left{std::move(top_left)}
		, _bottom_right{std::move(bottom_right)}
	{}

	Rectangle(const Rectangle&) = default;
	Rectangle& operator=(const Rectangle&) = default;

	Rectangle(Rectangle&&) = default;
	Rectangle& operator=(Rectangle&&) = default;

	const Point_t& top_left() const { return _top_left; }
	const Point_t& bottom_right() const { return _bottom_right; }

	bool contains(const Point_t& p) const
	{
		if (p.x < _top_left.x) {
			return false;
		}

		if (p.x > _bottom_right.x) {
			return false;
		}

		if (p.y > _top_left.y) {
			return false;
		}

		if (p.y < _bottom_right.y) {
			return false;
		}

		return true;
	}

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

	bool operator<(const This_t& other) const
	{
		if (x != other.x)
			return x < other.x;

		if (y != other.y)
			return y < other.y;

		if (z != other.z)
			return z < other.z;

		return false;
	}

	Point3D& load(std::istream& is) try
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

	Point3D& from_string(const std::string& str)
	{
		auto x_y_z_str = split(str, ',');
		if (x_y_z_str.size() != 3) {
			throw aoc::Exception("Failed to read Point2D");
		}

		this->x = string_to<XValue_t>(x_y_z_str[0]);
		this->y = string_to<YValue_t>(x_y_z_str[1]);
		this->z = string_to<ZValue_t>(x_y_z_str[2]);

		return *this;
	}

	XValue_t x;
	YValue_t y;
	ZValue_t z;
};

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
struct Line3d
{
	using Value_t = Value_T;
	using Point_t = Point3D<Value_t>;
	using This_t = Line3d<Value_t>;

	Line3d() {}

	Line3d(Point_t start_, Point_t finish_)
		: start{ std::move(start_) }
		, finish{ std::move(finish_) }
	{}

	Line3d(const This_t&) = default;
	This_t& operator=(const This_t&) = default;

	Line3d(This_t&&) = default;
	This_t& operator=(This_t&&) = default;

	Point_t start;
	Point_t finish;

	This_t& from(std::istream& is)
	{
		is >> *this;

		return *this;
	}
};

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
class ValueRange
{
public:
	using Value_t = Value_T;
	using This_t = ValueRange<Value_t>;

	struct ConstIterator
	{
		using iterator_category = std::random_access_iterator_tag;
		using value_type = Value_t;
		using difference_type = std::make_signed_t<value_type>;
		using pointer = const value_type*;
		using reference = const value_type&;

		explicit ConstIterator(value_type begin)
			: _current{ begin }
		{}

		auto operator<=>(const ConstIterator&) const = default;

		reference operator*() const { return _current; }

		ConstIterator& operator++()
		{
			++_current;
			return *this;
		}

		ConstIterator operator++(int)
		{
			ConstIterator out = *this;
			++(*this);
			return out;
		}

		ConstIterator& operator--()
		{
			--_current;
			return *this;
		}

		ConstIterator operator--(int)
		{
			ConstIterator out = *this;
			--(*this);
			return out;
		}

		ConstIterator& operator+=(difference_type increment)
		{
			_current += increment;
			return *this;
		}

		ConstIterator& operator-=(difference_type increment)
		{
			_current -= increment;
			return *this;
		}

		friend ConstIterator operator+(ConstIterator it_1, difference_type increment)
		{
			it_1 += increment;
			return it_1;
		}

		friend ConstIterator operator-(ConstIterator it_1, difference_type decrement)
		{
			it_1 -= decrement;
			return it_1;
		}

		friend difference_type operator-(const ConstIterator& it_1, const ConstIterator& it_2)
		{
			return it_1._current - it_2._current;
		}

	private:
		value_type _current;
	};

	ValueRange() : _min{ 0 }, _max{ 0 } {}
	
	constexpr ValueRange(Value_t max_)
		: _min{ 0 }
		, _max{ std::move(max_) }
	{
		if (_min > _max) {
			throw InvalidArgException("Range min is greater than max");
		}
	}

	constexpr ValueRange(Value_t min_, Value_t max_)
		: _min{ std::move(min_) }
		, _max{ std::move(max_) }
	{
		if (_min > _max) {
			throw InvalidArgException("Range min is greater than max");
		}
	}

	ValueRange(const This_t&) = default;
	This_t& operator=(const This_t&) = default;

	ValueRange(This_t&&) = default;
	This_t& operator=(This_t&&) = default;

	bool operator==(const This_t& other) const
	{
		return _min == other.x && _max == other.y;
	}

	bool operator!=(const This_t& other) const
	{
		return !(*this == other);
	}

	ValueRange& from_stream(std::istream& is) try
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
		throw aoc::IOException("Failed to extract ValueRange from stream");
	}
	catch (const std::out_of_range&)
	{
		is.setstate(std::ios::failbit);
		throw aoc::IOException("Failed to extract ValueRange from stream");
	}

	ValueRange& from_string(const std::string& str)
	{
		using namespace std::string_literals;

		auto min_max_str = split(str, ".."s, SplitBehaviour::drop_empty);
		if (min_max_str.size() != 2) {
			throw aoc::IOException("Failed to read ValueRange");
		}

		try {
			const auto min = string_to<Value_T>(min_max_str[0]);
			const auto max = string_to<Value_T>(min_max_str[1]);

			if (min > max) {
				throw IOException("Range min is greater than max");
			}

			_min = min;
			_max = max;
		}
		catch (const std::out_of_range& e) {
			throw IOException(std::format("Failed to read ValueRange: {}", e.what()));
		}
		catch (const std::invalid_argument& e) {
			throw IOException(std::format("Failed to read ValueRange: {}", e.what()));
		}

		return *this;
	}

	const Value_t& min() const { return _min; }
	const Value_t& max() const { return _max; }

	auto begin() const { return ConstIterator{ _min }; }
	auto end() const { return ConstIterator{ _max + 1 }; }

private:
	Value_T _max;
	Value_T _min;
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

template<typename Container_T>
class PairwiseCombinationIterator
{
public:
	using iterator_category = std::input_iterator_tag;
	using value_type = std::pair<typename Container_T::const_iterator, typename Container_T::const_iterator>;
	using reference = const value_type&;
	using pointer = const value_type*;
	using difference_type = std::ptrdiff_t;

	PairwiseCombinationIterator(const Container_T& values)
		: _at_end{ values.begin() == values.end() }
		, _begin{ values.begin() }
		, _end{ values.end() }
		, _current{ values.begin(), values.begin() }
	{}

	PairwiseCombinationIterator()
		: _at_end{ true }
	{}

	PairwiseCombinationIterator(const PairwiseCombinationIterator& other)
		: _at_end{ other._at_end }
		, _begin{ other._begin }
		, _end{ other._end }
		, _current{ other._current }
	{}

	PairwiseCombinationIterator& operator=(const PairwiseCombinationIterator& other)
	{
		auto temp = other;
		this->swap(temp);

		return *this;
	}

	PairwiseCombinationIterator(PairwiseCombinationIterator&& other) = default;
	PairwiseCombinationIterator& operator=(PairwiseCombinationIterator&& other) = default;

	bool operator==(const PairwiseCombinationIterator& other) const
	{
		if (_at_end && other._at_end) {
			return true;
		}

		if (_at_end != other._at_end) {
			return false;
		}

		if (_current != other._current) {
			return false;
		}

		return true;
	}

	bool operator!=(const PairwiseCombinationIterator& other) const
	{
		return !(*this == other);
	}

	reference operator*() const
	{
		return _current;
	}

	pointer operator->() const
	{
		return &_current;
	}

	PairwiseCombinationIterator& operator++()
	{
		if (_at_end) {
			throw OutOfRangeException("");
		}

		++_current.second;
		if (_current.second == _end) {
			_current.second = _begin;
			++_current.first;
		}

		if (_current.first == _end) {
			_at_end = true;
		}

		return *this;
	}

	PairwiseCombinationIterator operator++(int)
	{
		auto temp = PairwiseCombinationIterator{ *this };
		++(*this);
		return temp;
	}

	void swap(PairwiseCombinationIterator& other)
	{
		std::swap(_at_end, other._at_end);
		std::swap(_begin, other._begin);
		std::swap(_end, other._end);
		std::swap(_current, other._current);
	}

private:
	bool _at_end;
	typename Container_T::const_iterator _begin, _end;
	value_type _current;
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
inline istream& operator>>(std::istream& is, aoc::Point3D<Value_T>& p)
{
	p.load(is);
	return is;
}

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
inline istream& operator>>(std::istream& is, aoc::ValueRange<Value_T>& range)
{
	range.from_stream(is);
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

template<typename Value_T>
inline istream& operator>>(istream& is, aoc::Line3d<Value_T>& line) try
{
	line = aoc::Line3d<Value_T>{ {0, 0, 0}, {0, 0, 0} };

	if (is.eof())
		return is;

	is >> line.start;

	// Skip the arrow part
	char arrow_buffer[5] = { 0 };
	is.read(arrow_buffer, 4);
	if (strcmp(arrow_buffer, " -> ") != 0) {
		is.setstate(std::ios::failbit);
		throw aoc::Exception("Invalid arrow in Line3d serialization");
	}

	is >> line.finish;

	return is;
}
catch (std::invalid_argument&)
{
	is.setstate(std::ios::failbit);
	throw aoc::Exception("Failed to extract Line3d from stream");
}
catch (std::out_of_range&)
{
	is.setstate(std::ios::failbit);
	throw aoc::Exception("Failed to extract Line3d from stream");
}

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
