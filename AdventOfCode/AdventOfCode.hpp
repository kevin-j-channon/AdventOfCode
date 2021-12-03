#pragma once

#include "StringOperations.hpp"

#include <numeric>
#include <array>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <format>

namespace aoc
{
	class Submarine
	{
	public:

		template<size_t WINDOW_SIZE, typename Iter_T>
		uint32_t GetDepthScore(Iter_T begin, Iter_T end)
		{
			auto current_idx = size_t{ 0 };

			auto window = std::array<typename Iter_T::value_type, WINDOW_SIZE>{};
			for (auto it = window.begin(); it < window.end(); ++it, ++current_idx) {
				*it = *begin++;
			}

			auto prev_score = std::accumulate(window.begin(), window.end(), uint32_t{ 0 });

			return std::accumulate(begin, end, 0, [&window, &prev_score, &current_idx](auto curr, auto next) {

				auto new_score = uint32_t{ prev_score - window[current_idx % window.size()] + next };

				auto out = new_score > prev_score ? ++curr : curr;

				prev_score = new_score;
				window[current_idx % window.size()] = next;
				++current_idx;

				return out;
				});
		}
	};

	template<typename Value_T>
	struct Vec2d
	{
		Value_T x{};
		Value_T y{};
	};

	template<typename Value_T>
	Vec2d<Value_T> operator+(const Vec2d<Value_T>& v1, const Vec2d<Value_T>& v2)
	{
		return Vec2d<Value_T>{ v1.x + v2.x, v1.y + v2.y };
	}

	struct Exception : public std::runtime_error
	{
		Exception(const std::string& msg) : std::runtime_error{ msg } {}
	};

	using Direction = Vec2d<int>;
}

#include <iostream>

namespace std
{
	istream& operator>>(istream& is, aoc::Direction& direction)
	{
		auto cmd = std::string{};

		direction = aoc::Direction{};

		is >> cmd;

		if (cmd.empty())
			return is;

		if (cmd == "forward")
		{
			is >> direction.x;
		}
		else if (cmd == "up")
		{
			is >> direction.y;
			direction.y *= -1;
		}
		else if (cmd == "down")
		{
			is >> direction.y;
		}
		else {
			throw aoc::Exception(std::format("Invalid direction string"));
		}

		if (is.fail())
			throw aoc::Exception(std::format("Invalid direction string"));

		return is;
	}
}
