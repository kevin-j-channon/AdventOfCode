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
	template<size_t WINDOW_SIZE>
	class DepthAssessor
	{
	public:

		template<typename Iter_T>
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
		auto magnitude = std::string{};

		is >> cmd;
		is >> magnitude;

		if (cmd.empty())
			return is;

		cout << "direction string: " << cmd << ", " <<  magnitude << endl;

		try
		{
			if (cmd == "forward")
			{
				direction = { std::stoi(magnitude), 0 };
			}
			else if (cmd == "up")
			{
				direction = {0, -std::stoi(magnitude)};
			}
			else if (cmd == "down")
			{
				direction = { 0, std::stoi(magnitude) };
			}
			else {
				throw aoc::Exception(std::format("Invalid direction string: {} {}", cmd, magnitude));
			}
		}
		catch (const std::invalid_argument&)
		{
			throw aoc::Exception(std::format("Invalid direction string: {} {}", cmd, magnitude));
		}

		return is;
	}
}
