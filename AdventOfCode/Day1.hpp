#pragma once

#include <ranges>

namespace aoc
{
	class DepthAssessor
	{
	public:

		template<typename Iter_T>
		uint32_t GetDepthScore(Iter_T begin, Iter_T end)
		{
			auto score = uint32_t{0};
			auto val = *begin;
			
			std::advance(begin, 1);

			for (; begin != end; std::advance(begin, 1)) {
				if (*begin > val) {
					++score;
				}

				val = *begin;
			}

			return score;
		}
	};
}
