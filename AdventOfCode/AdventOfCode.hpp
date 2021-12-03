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

			auto prev_score = std::accumulate(window.begin(), window.end(), uint32_t{0});

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

	template<typename Value_T>
	struct Direction : public Vec2d<Value_T>
	{
		struct Exception : public std::runtime_error
		{
			Exception(const std::string& msg) : std::runtime_error{ msg } {}
		};

		static Direction FromText(const std::string& txt)
		{
			const auto parts = split(strip(txt), ' ');
			if (parts.size() != 2)
			{
				throw Exception(std::format("Invalid direction string: {}", txt));
			}

			if (parts[0] == "forward")
			{
				return Direction{ std::stoi(parts[1]), 0 };
			}

			throw Exception(std::format("Invalid direction string: {} {}", parts[0], parts[1]));
		}
	};


}
