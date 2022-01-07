#pragma once

#include "Common.hpp"

namespace  aoc
{
class Polymer
{
public:
	using Size_t = size_t;

	Polymer() = default;

	Polymer(Polymer&&) = default;
	Polymer& operator=(Polymer&&) = default;

	Polymer(const Polymer&) = delete;
	Polymer& operator=(const Polymer&) = delete;

	static Polymer from_stream(std::istream& is)
	{
		auto seed_str = std::string{};
		std::getline(is, seed_str);

		return from_string(seed_str);
	}

	static Polymer from_string(const std::string& str)
	{
		if (str.empty()) {
			return Polymer{};
		}

		auto out = Polymer{};

		out._sequence.assign(str.begin(), str.end());

		return out;
	}

	Size_t length() const { return _sequence.size(); }

private:

	std::vector<char> _sequence;
};
}
