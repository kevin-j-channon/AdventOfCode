#pragma once

#include "Common.hpp"
#include "StringOperations.hpp"
#include "DiagnosticLog.hpp"
#include "BoatSystems.hpp"
#include "EntertainmentSystems.hpp"

#include <numeric>
#include <array>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

class Submarine
{
public:

	const BoatSystems& boat_systems() const { return _boat_systems; }
	const EntertainmentSystems& entertainment() const { return _entertainment; }

private:
	BoatSystems _boat_systems;
	EntertainmentSystems _entertainment;
};

///////////////////////////////////////////////////////////////////////////////

}	// namespace: aoc

///////////////////////////////////////////////////////////////////////////////
