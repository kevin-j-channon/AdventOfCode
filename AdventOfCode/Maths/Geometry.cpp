#include "Geometry.hpp"

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{
namespace quaternion
{

///////////////////////////////////////////////////////////////////////////////

Quaternion_t from_axis_and_angle(const RotationAxis_t& axis, RotationAngle_t angle)
{
	return boost::qvm::rot_quat(boost::qvm::vec<double, 3>{ {axis.x, axis.y, axis.z}}, angle);
}

///////////////////////////////////////////////////////////////////////////////

std::pair<RotationAxis_t, RotationAngle_t> to_axis_and_angle(const Quaternion_t& q)
{
	const auto angle = 2 * std::acos(q.a[0]);
	const auto sin_half_angle = std::sin(0.5 * angle);

	return { {q.a[1] / sin_half_angle, q.a[2] / sin_half_angle, q.a[3] / sin_half_angle}, angle };
}

///////////////////////////////////////////////////////////////////////////////

Quaternion_t from_point(const Point3D<double>& p)
{
	return { { 0.0, p.x, p.y, p.z } };
}

///////////////////////////////////////////////////////////////////////////////

}	// namespace: quaternion

///////////////////////////////////////////////////////////////////////////////

Point3D<double> rotate(const Point3D<double>& p, const Quaternion_t& q)
{
	const auto rotated = (q * quaternion::from_point(p)) * quaternion::conjugate(q);
	return { rotated.a[1], rotated.a[2], rotated.a[3] };
}

///////////////////////////////////////////////////////////////////////////////

Point3D<double> rotate(const Point3D<double>& p, const Direction_t<double>& axis, double angle)
{
	return rotate(p, quaternion::from_axis_and_angle(axis, angle));
}

///////////////////////////////////////////////////////////////////////////////

Point3D<int> rotate(const Point3D<int>& p, const Quaternion_t& q)
{
	const auto p_double = Point3D<double>{ static_cast<double>(p.x), static_cast<double>(p.y) , static_cast<double>(p.z) };
	const auto rotated_point = rotate(p_double, q);
	return { std::lround(rotated_point.x), std::lround(rotated_point.y), std::lround(rotated_point.z) };
}

///////////////////////////////////////////////////////////////////////////////

}	// namesppace: aoc

///////////////////////////////////////////////////////////////////////////////
