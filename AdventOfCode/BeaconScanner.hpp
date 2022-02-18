#pragma once

#include "Common.hpp"

namespace aoc
{
namespace navigation
{

class Scanner
{
	using Position_t = Point3D<int>;
public:

	using Id_t = uint32_t;

	Scanner(Id_t id)
		: _id{ id }
	{}

	static Scanner from_stream(std::istream& is)
	{
		auto out = Scanner{_id_from_stream(is)};

		return out;
	}

	const Id_t& id() const { return _id; }

private:

	static Id_t _id_from_stream(std::istream& is)
	{
		auto line = std::string{};
		std::getline(is, line);

		return _id_from_string(line);
	}

	static Id_t _id_from_string(const std::string& s)
	{
		auto parts = split(s, ' ');
		return string_to<Id_t>(parts[2]);
	}

	Id_t _id;
	std::vector<Position_t> _beacon_positions;
};
}
}