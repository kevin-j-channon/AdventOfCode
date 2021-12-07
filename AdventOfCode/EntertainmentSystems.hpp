#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "StringOperations.hpp"

#include <istream>
#include <string>
#include <vector>
#include <cstdint>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

class Bingo
{
public:

	using Draws_t = std::vector<uint8_t>;

	void load(std::istream& stream)
	{
		auto line = std::string{};
		std::getline(stream, line);
		const auto draws_strings = split(line, ',');
		_draws.resize(draws_strings.size(), 0);
		std::transform(draws_strings.begin(), draws_strings.end(), _draws.begin(), [](auto s) { return static_cast<uint8_t>(std::stol(s)); });
	}

	const Draws_t& draws() const { return _draws; }

private:
	Draws_t _draws;
};

///////////////////////////////////////////////////////////////////////////////

class EntertainmentSystems
{
public:

	Bingo bingo() const { return Bingo{}; }
};

///////////////////////////////////////////////////////////////////////////////

}	// namespace: aoc

///////////////////////////////////////////////////////////////////////////////
