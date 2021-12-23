#pragma once

#include <armadillo>

#include <istream>
#include <string>
#include <cstdint>
#include <iterator>
#include <algorithm>

namespace aoc
{
template<arma::uword GRID_SIZE>
class DumboOctopusModel
{
	arma::Mat<uint32_t> _octopus;

	static uint32_t _char_to_digit(char c)
	{
		return static_cast<uint32_t>(c - '0');
	}

	template<typename Fn_T>
	void _apply_to_grid(Fn_T fn)
	{
		for (auto r = 0; r < GRID_SIZE; ++r) {
			for (auto c = 0; c < GRID_SIZE; ++c) {
				_octopus.at(r, c) = fn(r, c);
			}
		}
	}

public:

	DumboOctopusModel()
		: _octopus( GRID_SIZE, GRID_SIZE )
	{}

	DumboOctopusModel(const std::vector<std::vector<uint32_t>>& initial_state)
		: _octopus(GRID_SIZE, GRID_SIZE)
	{
		_apply_to_grid([&initial_state](auto r, auto c) {return initial_state[r][c]; });
	}

	DumboOctopusModel(const DumboOctopusModel&) = default;
	DumboOctopusModel& operator=(const DumboOctopusModel&) = default;

	DumboOctopusModel(DumboOctopusModel&&) = default;
	DumboOctopusModel& operator=(DumboOctopusModel&&) = default;

	const arma::Mat<uint32_t>& state() const { return _octopus; }

	DumboOctopusModel& load(std::istream& is)
	{
		auto digit_matrix = std::vector<std::string>(GRID_SIZE);
		std::generate_n(digit_matrix.begin(), GRID_SIZE, [&is]() {
				auto line = std::string{};
				std::getline(is, line);
				return line;
			});

		_apply_to_grid([&digit_matrix](auto r, auto c) {return _char_to_digit(digit_matrix[r][c]); });

		return *this;
	}

	DumboOctopusModel& step()
	{
		_apply_to_grid([this](auto r, auto c) { return _octopus.at(r, c) + 1; });

		return *this;
	}
};
}