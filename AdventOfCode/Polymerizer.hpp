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
	std::string as_string() const { return std::string(_sequence.begin(), _sequence.end()); }

	template<size_t N>
	Polymer& polymerize(const InsertionRuleTable_t& rules)
	{
		_redistribute<N>();
		_recursive_polymerize<1, N>(rules);

		return *this;
	}

private:

	template<size_t CYCLE, size_t TOTAL_CYCLES>
	static constexpr size_t _step_size()
	{
		return Exp<2, TOTAL_CYCLES - CYCLE>::value;
	}

	template<size_t N>
	void _redistribute()
	{
		const auto original_size = _sequence.size();
		_sequence.resize(Exp<2, N>::value * (original_size - 1) + 1, 0);
		
		auto it = _sequence.rbegin();
		const auto terminate = std::prev(_sequence.rend());
		auto ch = std::reverse_iterator<decltype(_sequence.begin())>(std::next(_sequence.begin(), original_size));
		do {
			std::swap(*it, *ch);

			++ch;
			std::advance(it, _step_size<0, N>());
		} while (it != terminate);
	}

	template<size_t CYCLE, size_t TOTAL_CYCLES>
	void _recursive_polymerize(const InsertionRuleTable_t& rules)
	{
		constexpr auto step = _step_size<CYCLE - 1, TOTAL_CYCLES>();

		auto left_char = _sequence.begin();
		auto right_char = std::next(_sequence.begin(), step);
		auto insert_char = std::next(_sequence.begin(), step / 2);
		const auto terminate = std::prev(_sequence.end());

		while (true) {
			*insert_char = rules.at({ *left_char, *right_char });

			left_char = right_char;

			if (right_char == terminate) {
				break;
			}

			right_char += step;
			insert_char += step;
		}

		if constexpr (CYCLE < TOTAL_CYCLES) {
			_recursive_polymerize<CYCLE + 1, TOTAL_CYCLES>(rules);
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

class Scorer
{
public:
	Scorer(const Polymer& p)
		: _polymer{p.as_string()}
	{}

	uint32_t calculate() const
	{
		const auto counts = _count_monomers();

		auto compare_counts = [](const auto& c1, const auto& c2) { return c1.second < c2.second; };

		const auto min_count = std::min_element(counts.begin(), counts.end(), compare_counts)->second;
		const auto max_count = std::max_element(counts.begin(), counts.end(), compare_counts)->second;

		return static_cast<uint32_t>(max_count - min_count);
	}

private:

	std::map<Monomer_t, uint32_t> _count_monomers() const
	{
		return std::accumulate(_polymer.begin(), _polymer.end(), std::map<Monomer_t, uint32_t>{}, [](auto&& counts, const auto& monomer) {
			counts[monomer]++;
			return std::move(counts);
			});
	}

	const std::string _polymer;
};

}
}
