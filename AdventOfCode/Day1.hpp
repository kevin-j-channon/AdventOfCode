#pragma once

#include <ranges>

namespace aoc
{
	class DepthAssessor
	{
	public:

		template<typename Range_T>
		uint32_t GetDepthChange(const Range_T& data)
		{
			auto out = uint32_t{0};
			for (auto v1 = data.cbegin(), v2 = std::next(data.begin()); v2 != data.cend(); ++v1, ++v2) {
				out += *v2 - *v1;
			}

			return out;
		}
	};
}
