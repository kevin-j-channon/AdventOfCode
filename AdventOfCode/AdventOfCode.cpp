#include "Day1.hpp"

#include "CppUnitTest.h"

#include <ranges>
#include <vector>
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AdventOfCode
{
	TEST_CLASS(TestDay1)
	{
	public:
		
		TEST_METHOD(CountNetIncreaseFromVector)
		{
			auto measurements = std::vector<uint32_t>{ 1, 2, 3, 2, 1, 2, 3 };

			auto depth_increase = aoc::DepthAssessor().GetDepthChange(measurements);

			Assert::AreEqual(uint32_t{ 2 }, depth_increase);
		}
	};
}
