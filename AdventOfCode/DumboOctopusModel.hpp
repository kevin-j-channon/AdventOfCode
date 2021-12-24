#pragma once

#include <armadillo>

#include <istream>
#include <string>
#include <cstdint>
#include <iterator>
#include <algorithm>
#include <sstream>

namespace aoc
{

void print_grid(const arma::Mat<int>& grid)
{
	std::stringstream ss;
	grid.print(ss);
	Logger::WriteMessage(std::format("{}\n", ss.str()).c_str());
}

template<arma::uword GRID_SIZE>
class DumboOctopusModel
{
	arma::Mat<int> _octopus;
	arma::Mat<int> _flash_grid;
	arma::Mat<int> _flash_mask;

	static int _char_to_digit(char c)
	{
		return static_cast<int>(c - '0');
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

	int _single_pass_flash()
	{
		auto flashes = int{ 0 };

		for (auto r = 1; r < GRID_SIZE + 1; ++r) {
			for (auto c = 1; c < GRID_SIZE + 1; ++c) {
				if (_flash_grid.at(r, c) > flash_threshold) {
					_flash_grid.at(r, c) = std::numeric_limits<int>::min();

					_flash_grid.submat(r - 1, c - 1, r + 1, c + 1) += _flash_mask;

					++flashes;
				}
			}
		}

		return flashes;
	}

	int _process_all_flashes()
	{
		auto flashes = int{ 0 };
		_flash_grid.submat(1, 1, GRID_SIZE, GRID_SIZE) = _octopus;

		while (true) {
			const auto flashes_this_pass = _single_pass_flash();
			if (flashes_this_pass == 0) {
				break;
			}

			flashes += flashes_this_pass;
		}

		return flashes;
	}

	void _reset_octopus_that_flashed()
	{
		for (auto r = 1; r < GRID_SIZE + 1; ++r) {
			for (auto c = 1; c < GRID_SIZE + 1; ++c) {
				if (_flash_grid.at(r, c) < 0) {
					_flash_grid.at(r, c) = reset_energy_value;
				}
			}
		}
	}

public:

	static constexpr int flash_threshold = 9;
	static constexpr int reset_energy_value = 0;

	DumboOctopusModel()
		: _octopus( GRID_SIZE, GRID_SIZE )
		, _flash_grid(GRID_SIZE + 2, GRID_SIZE + 2)
		, _flash_mask(3, 3)
	{
		_flash_grid.fill(0);

		_flash_mask.fill(1);
		_flash_mask.at(1, 1) = 0;
	}

	template<typename Container_T>
	DumboOctopusModel(const Container_T& initial_state)
		: _octopus(GRID_SIZE, GRID_SIZE)
		, _flash_grid(GRID_SIZE + 2, GRID_SIZE + 2)
		, _flash_mask(3, 3)
	{
		_apply_to_grid([&initial_state](auto r, auto c) {return initial_state[r][c]; });

		_flash_grid.fill(0);

		_flash_mask.fill(1);
		_flash_mask.at(1, 1) = 0;
	}

	DumboOctopusModel(const DumboOctopusModel&) = default;
	DumboOctopusModel& operator=(const DumboOctopusModel&) = default;

	DumboOctopusModel(DumboOctopusModel&&) = default;
	DumboOctopusModel& operator=(DumboOctopusModel&&) = default;

	const arma::Mat<int>& state() const { return _octopus; }

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

	DumboOctopusModel& increment()
	{
		_apply_to_grid([this](auto r, auto c) { return _octopus.at(r, c) + 1; });

		return *this;
	}

	int flash()
	{
		const auto flashes = _process_all_flashes();

		_reset_octopus_that_flashed();

		_octopus = _flash_grid.submat(1, 1, GRID_SIZE, GRID_SIZE);

		return flashes;
	}

	int step(uint32_t number_of_steps)
	{
		auto flashes = int{ 0 };

		for (auto _ = uint32_t{ 0 }; _ < number_of_steps; ++_) {
			flashes += this->increment().flash();
		}

		return flashes;
	}
};
}