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
	const auto q_normalized = boost::qvm::normalized(q);

	const auto angle = 2 * std::acos(q_normalized.a[0]);
	const auto normalization_factor = std::sqrt(1 - q_normalized.a[0]* q_normalized.a[0]);
	if (normalization_factor < 1e-10) {
		return {{0.0, 0.0, 0.0}, angle};
	}

	return {{q_normalized.a[1] / normalization_factor, q_normalized.a[2] / normalization_factor, q_normalized.a[3] / normalization_factor}, angle};
}

///////////////////////////////////////////////////////////////////////////////

Quaternion_t from_point(const Point3D<double>& p)
{
	return {{ 0.0, p.x, p.y, p.z }};
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
