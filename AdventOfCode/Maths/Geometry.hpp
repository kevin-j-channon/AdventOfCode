#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "StringOperations.hpp"
#include "Exception.hpp"

#include <boost/qvm.hpp>
#include <boost/qvm/quat_operations.hpp>

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
		vertical = 0b010,
		diagonal = 0b100,
	};

	Line2d() {}

	Line2d(Point_t start_, Point_t finish_)
		: start{ std::move(start_) }
		, finish{ std::move(finish_) }
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

template<typename Value_T>
class Rectangle
{
public:
	using Value_t = Value_T;
	using Point_t = Point2D<Value_t>;

	Rectangle()
		: _top_left{ 0, 0 }
		, _bottom_right{ 0, 0 }
	{}

	Rectangle(Point_t top_left, Point_t bottom_right)
		: _top_left{ std::move(top_left) }
		, _bottom_right{ std::move(bottom_right) }
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

	bool operator==(const This_t& other) const
	{
		return x == other.x && y == other.y && z == other.z;
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

		this->x = string_to<Value_t>(x_y_z_str[0]);
		this->y = string_to<Value_t>(x_y_z_str[1]);
		this->z = string_to<Value_t>(x_y_z_str[2]);

		return *this;
	}

	Value_t x;
	Value_t y;
	Value_t z;
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

			auto out = std::vector<Point2D<Value_T>>{ x_max - x_min + 1 };

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
using Direction_t = Point3D<Value_T>;

using RotationAxis_t = Direction_t<double>;
using RotationAngle_t = double;

using Quaternion_t = boost::qvm::quat<double>;

namespace quaternion
{

Quaternion_t from_axis_and_angle(const RotationAxis_t& axis, RotationAngle_t angle);

template<typename Value_T>
	requires std::is_integral_v<Value_T>
Quaternion_t from_axis_and_angle(const Direction_t<Value_T>& axis, RotationAngle_t angle)
{
	return from_axis_and_angle(RotationAxis_t{ static_cast<double>(axis.x), static_cast<double>(axis.y) , static_cast<double>(axis.z) }, angle);
}

std::pair<RotationAxis_t, RotationAngle_t> to_axis_and_angle(const Quaternion_t& q);

inline Quaternion_t from_point(const Point3D<double>& p);

constexpr std::array<Quaternion_t, 24> cubic_rotations()
{
	constexpr auto a = 1.0 / std::numbers::sqrt2;
	return {
		// Identity rotation
		Quaternion_t{{ 1.0, 0.0, 0.0, 0.0 }},	// 1, 0, 0 -> 0

		// Plane rotations.
		// NOTE: For these elements, the non-zero values happen to be normalized without further manipulation.
		Quaternion_t{{   a,   a, 0.0, 0.0 }},	// 1, 0, 0 -> 1 * pi / 2
		Quaternion_t{{ 0.0, 1.0, 0.0, 0.0 }},	// 1, 0, 0 -> 2 * pi / 2
		Quaternion_t{{  -a,   a, 0.0, 0.0 }},	// 1, 0, 0 -> 3 * pi / 2
		Quaternion_t{{   a, 0.0,   a, 0.0 }},	// 0, 1, 0 -> 1 * pi / 2
		Quaternion_t{{ 0.0, 0.0, 1.0, 0.0 }},	// 0, 1, 0 -> 2 * pi / 2
		Quaternion_t{{  -a, 0.0,   a, 0.0 }},	// 0, 1, 0 -> 3 * pi / 2
		Quaternion_t{{   a, 0.0, 0.0,   a }},	// 0, 0, 1 -> 1 * pi / 2
		Quaternion_t{{ 0.0, 0.0, 0.0, 1.0 }},	// 0, 0, 1 -> 2 * pi / 2
		Quaternion_t{{  -a, 0.0, 0.0,   a }},	// 0, 0, 1 -> 3 * pi / 2

		// Edge rotations.
		// NOTE: The non-zero components of these elements are actually 1.0, but to make them unit rotations, the quaternion
		// needs to be normalized. When you do this, the normalization factor is \frac{1}{\sqrt{2}}, so that's what ends up
		// appearing in the values below.
		Quaternion_t{{ 0.0, 0.0,   a,   a }},	// 0, 1, 1 -> pi
		Quaternion_t{{ 0.0,   a, 0.0,    a}},	// 1, 0, 1 -> pi
		Quaternion_t{{ 0.0,   a,   a, 0.0 }},	// 1, 1, 0 -> pi
		Quaternion_t{{ 0.0,   a,  -a,   a }},	// 0, 1, 1 -> pi
		Quaternion_t{{ 0.0,   a, 0.0,  -a }},	// 0, 1, 1 -> pi
		Quaternion_t{{ 0.0, 0.0,   a,  -a }},	// 0, 1, 1 -> pi

		// Vertex rotations.
		// NOTE: the vector values in these elements should be \frac{\sqrt{3}}{2}, however, when you normalise the vector part
		// the normmalization factor is \sqrt{\frac{x^2 + y^2 + z^2}{1 - w^2}}, which resolves to \sqrt{3}; so the vector part
		// becomes simply 1/2.
		Quaternion_t{{ 0.5, 0.5, 0.5, 0.5 }},	// 1, 1, 1 -> 2 * pi / 3
		Quaternion_t{{-0.5, 0.5, 0.5, 0.5 }},	// 1, 1, 1 -> 4 * pi / 3
		Quaternion_t{{ 0.5, 0.5, 0.5,-0.5 }},	// 1, 1,-1 -> 2 * pi / 3
		Quaternion_t{{-0.5, 0.5, 0.5,-0.5 }},	// 1, 1,-1 -> 4 * pi / 3
		Quaternion_t{{ 0.5, 0.5,-0.5, 0.5 }},	// 1,-1, 1 -> 2 * pi / 3
		Quaternion_t{{-0.5, 0.5,-0.5, 0.5 }},	// 1,-1, 1 -> 4 * pi / 3
		Quaternion_t{{ 0.5,-0.5, 0.5, 0.5 }},	//-1, 1, 1 -> 2 * pi / 3
		Quaternion_t{{-0.5,-0.5, 0.5, 0.5 }},	//-1, 1, 1 -> 4 * pi / 3
	};
}

using boost::qvm::conjugate;

}

Point3D<double> rotate(const Point3D<double>& p, const Quaternion_t& q);

Point3D<double> rotate(const Point3D<double>& p, const Direction_t<double>& axis, double angle);

Point3D<int> rotate(const Point3D<int>& p, const Quaternion_t& q);

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
inline istream& operator>>(istream& is, aoc::Line2d<Value_T>& line) try
{
	line = aoc::Line2d<Value_T>{ {0, 0}, {0, 0} };

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
