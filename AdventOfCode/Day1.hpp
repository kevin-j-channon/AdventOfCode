#pragma once

#include <numeric>

namespace aoc
{
	class DepthAssessor
	{
	public:

		template<typename Iter_T>
		uint32_t GetDepthScore(Iter_T begin, Iter_T end)
		{
			auto prev = *begin++;

			return std::accumulate(begin, end, 0, [&prev](auto curr, auto next) {
				auto out = next > prev ? ++curr : curr;
				prev = next;
				return out;
				});
		}
	};
}
