#pragma once

#include "Common.hpp"
#include "StringOperations.hpp"

///////////////////////////////////////////////////////////////////////////////

namespace  aoc
{
namespace polymer
{

///////////////////////////////////////////////////////////////////////////////

using Monomer_t = char;
using Dimer_t = std::array<char, 2>;
using InsertionRuleTable_t = std::map<Dimer_t, Monomer_t>;
using InsertionRule_t = InsertionRuleTable_t::value_type;

///////////////////////////////////////////////////////////////////////////////

class Polymer
{
public:
	using Size_t = size_t;
	using Abundances_t = std::map<Monomer_t, uint64_t>;

	Polymer() = default;

	Polymer(Polymer&&) = default;
	Polymer& operator=(Polymer&&) = default;

	Polymer(const Polymer&) = delete;
	Polymer& operator=(const Polymer&) = delete;

	template<typename Iter_T>
	Polymer(Iter_T begin, Iter_T end)
	{
		if (begin == end) {
			throw InvalidArgException("Initialising Polymer with an empty string");
		}

		_build_abundances(begin, end);
	}

	Polymer(const std::string& str) : Polymer{ str.begin(), str.end() } {}

	static Polymer from_stream(std::istream& is)
	{
		auto line = std::string{};
		std::getline(is, line);

		return Polymer{ line };
	}

	Polymer& polymerize(uint32_t cycles, const InsertionRuleTable_t& rules)
	{
		for (auto cycle = 0u; cycle < cycles; ++cycle) {
			_do_single_polymerization(rules);
		}

		return *this;
	}

	const Abundances_t& abundances() const
	{
		return _monomer_abundances;
	}

	uint64_t score() const
	{
		auto compare_counts = [](const auto& c1, const auto& c2) { return c1.second < c2.second; };

		const auto min_count = std::min_element(_monomer_abundances.begin(), _monomer_abundances.end(), compare_counts)->second;
		const auto max_count = std::max_element(_monomer_abundances.begin(), _monomer_abundances.end(), compare_counts)->second;

		return static_cast<uint64_t>(max_count - min_count);
	}

private:

	using DimerAbundances_t = std::map<Dimer_t, uint64_t>;

	template<typename Iter_T>
	void _build_abundances(Iter_T begin, Iter_T end)
	{
		_clear_abundances();
		_add_abundances_from_sequence(begin, end);
	}

	void _clear_abundances()
	{
		_monomer_abundances.clear();
		_dimer_abundances.clear();
		_updated_dimer_abundances.clear();
	}

	template<typename Iter_T>
	void _add_abundances_from_sequence(Iter_T begin, Iter_T end)
	{
		++_monomer_abundances[*begin];

		for (auto c1 = begin, c2 = std::next(begin); c2 != end; ++c1, ++c2) {
			++_monomer_abundances[*c2];
			++_dimer_abundances[{*c1, * c2}];
		}
	}

	void _do_single_polymerization(const InsertionRuleTable_t& rules)
	{
		_updated_dimer_abundances.clear();

		for (const auto& [dimer, abundance] : _dimer_abundances) {
			_apply_polymerization_rules_to_dimer(dimer, rules.at(dimer), abundance);
		}

		std::swap(_dimer_abundances, _updated_dimer_abundances);
	}

	void _apply_polymerization_rules_to_dimer(const Dimer_t& dimer, const Monomer_t& monomer_to_insert, uint64_t abundance)
	{
		_monomer_abundances[monomer_to_insert] += abundance;

		_updated_dimer_abundances[{dimer[0], monomer_to_insert}] += abundance;
		_updated_dimer_abundances[{monomer_to_insert, dimer[1]}] += abundance;
	}

	DimerAbundances_t _dimer_abundances;
	DimerAbundances_t _updated_dimer_abundances;

	Abundances_t _monomer_abundances;
};

///////////////////////////////////////////////////////////////////////////////

class InsertionRuleLoader
{
public:

	static InsertionRuleTable_t from_stream(std::istream& is)
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

///////////////////////////////////////////////////////////////////////////////

}
}

///////////////////////////////////////////////////////////////////////////////
