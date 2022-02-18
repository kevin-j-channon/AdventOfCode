#pragma once

#include "Common.hpp"

namespace aoc
{
namespace navigation
{
using Position_t = Point3D<int>;

class Beacon
{
public:
	Beacon(Position_t position)
		: _position{ position }
	{}

	const Position_t& position() const { return _position; }

private:
	Position_t _position;
};

class Scanner
{
public:

	using Id_t = uint32_t;
	using Beacons_t = std::vector<Beacon>;

	Scanner(Id_t id)
		: _id{ id }
	{}

	static Scanner from_stream(std::istream& is)
	{
		auto out = Scanner{_id_from_stream(is)};
		
		out._beacons = _read_beacons(is);

		return out;
	}

	const Id_t& id() const { return _id; }
	const Beacons_t& beacons() const { return _beacons; }

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
		if (parts.size() != 4) {
			throw IOException{ std::format("Failed to read scanner ID from \"{}\": Invalid line format", s)};
		}

		return string_to<Id_t>(parts[2]);
	}

	static Beacons_t _read_beacons(std::istream& is)
	{
		auto out = Beacons_t{};

		auto line = std::string{};
		while (std::getline(is, line)) {
			if (line.empty()) {
				return out;
			}

			const auto parts = split(line, ',');
			const auto x = string_to<Position_t::XValue_t>(parts[0]);
			const auto y = string_to<Position_t::YValue_t>(parts[1]);
			auto z = Position_t::ZValue_t{ 0 };
			if (parts.size() == 3) {
				z = string_to<Position_t::ZValue_t>(parts[2]);
			}

			out.emplace_back(Position_t{ x, y, z });
		}

		return out;
	}

	Id_t _id;
	Beacons_t _beacons;
};
}
}