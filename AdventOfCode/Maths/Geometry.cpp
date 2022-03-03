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

}	// namesppace: aoc

///////////////////////////////////////////////////////////////////////////////
