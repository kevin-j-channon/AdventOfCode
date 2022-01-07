#pragma once

#include "Common.hpp"
#include "StringOperations.hpp"

namespace  aoc
{
namespace polymer
{

using Dimer_t = std::array<char, 2>;

using Monomer_t = char;
using InsertionRuleTable_t = std::map<Dimer_t, Monomer_t>;
using InsertionRule_t = InsertionRuleTable_t::value_type;

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

class InsertionRuleLoader
{
public:

	static InsertionRuleTable_t insertion_rule_table_from_stream(std::istream& is)
	{
		auto out = InsertionRuleTable_t{};

		auto line = std::string{};
		while (!is.eof()) {
			std::getline(is, line);
			if (line.empty()) {
				continue;
			}

			out.insert(_insertion_rule_from_string(line));
		}

		return out;
	}

private:

	static InsertionRule_t _insertion_rule_from_string(const std::string& str)
	{
		const auto parts = _split_and_validate_rule_string(str);
		
		return { { parts[0][0], parts[0][1] }, parts[2][0] };
	}

	static std::vector<std::string> _split_and_validate_rule_string(const std::string& str)
	{
		const auto parts = split(str, ' ', SplitBehaviour::drop_empty);
		if (parts.size() != 3) {
			throw Exception("Inalid insertion rule line");
		}

		if (parts.front().length() != 2) {
			throw Exception("Invalid dimer in insertion rule line");
		}

		if (parts.back().length() != 1) {
			throw Exception("Invalid monomer in insertion rule line");
		}

		return parts;
	}
};

}
}
