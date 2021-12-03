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

		Vec2d(Value_T x, Value_T y)
			: x{x}
			, y{y}
		{}

		Vec2d(const Vec2d&) = default;
		Vec2d& operator=(const Vec2d&) = default;

		Vec2d(Vec2d&&) = default;
		Vec2d& operator=(Vec2d&&) = default;
	};

	template<typename Value_T>
	Vec2d<Value_T> operator+(const Vec2d<Value_T>& v1, const Vec2d<Value_T>& v2)
	{
		return Vec2d<Value_T>{ v1.x + v2.x, v1.y + v2.y };
	}

	class Direction : public Vec2d<int>
	{
	public:
		struct Exception : public std::runtime_error
		{
			Exception(const std::string& msg) : std::runtime_error{ msg } {}
		};

		Direction(int x, int y)
			: Vec2d<int>{ x, y }
		{}

	};


	static Direction DirectionFromText(const std::string& txt)
	{
		const auto parts = split(strip(txt), ' ');
		if (parts.size() != 2)
		{
			throw Direction::Exception(std::format("Invalid direction string: {}", txt));
		}

		try
		{
			if (parts[0] == "forward")
			{
				return Direction{ std::stoi(parts[1]), 0 };
			}
			else if (parts[0] == "up")
			{
				return Direction{ 0, -std::stoi(parts[1]) };
			}
			else if (parts[0] == "down")
			{
				return Direction{ 0, std::stoi(parts[1]) };
			}
		} catch (const std::invalid_argument&)
		{
			throw Direction::Exception(std::format("Invalid direction string: {} {}", parts[0], parts[1]));
		}

		throw Direction::Exception(std::format("Invalid direction string: {} {}", parts[0], parts[1]));
	}
}
