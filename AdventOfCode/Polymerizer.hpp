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
		out._seed_length = out._sequence.size();

		return out;
	}

	Size_t length() const { return _sequence.size(); }

	template<size_t N>
	Polymer& polymerize(const InsertionRuleTable_t& rules)
	{
		_redistribute<N>();
		for (auto cycle = 0; cycle < N; ++cycle) {
			_single_polymerization_cycle<N>(cycle, rules);
		}
	}

private:

	template<size_t N>
	size_t _calculate_step_size(size_t cycle) const
	{
		return std::pow(2, N - cycle - 1) * _seed_length;
	}

	template<size_t N>
	void _redistribute()
	{
		const auto original_size = _sequence.size();
		_sequence.resize(std::pow(2.0, N) * (_sequence.size() - 1) - 1, 0);
		
		auto it = _sequence.rbegin();
		for (auto ch = std::reverse_iterator<decltype(_sequence.begin())>(std::next(_sequence.begin(), original_size));
			 ch != _sequence.rend();
			 ++ch, it += _calculate_step_size(0, cycles)) {
			*it = *ch;
		}
	}

	template<size_t N>
	void _single_polymerization_cycle(size_t cycle, const InsertionRuleTable_t& rules)
	{
		const auto step = _calculate_step_size<N>(cycle + 1, )

		auto left_char = _sequence.begin();
		auto right_char = std::next(_sequence.begin(), step);

		for (;; left_char += step, right_char += step) {

			*std::next(left_char, step / 2) = rules.at({ *left_char, *right_char });

			if (right_char == std::prev(_sequence.end())) {
				break;
			}
		}
	}

	size_t _seed_length{ 0 };
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
