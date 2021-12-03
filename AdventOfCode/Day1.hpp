#pragma once

#include <numeric>
#include <array>
#include <algorithm>

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
}
