#include "Day1.hpp"

#include "CppUnitTest.h"

#include <ranges>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <format>

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

		TEST_METHOD(CountNetIncreaseFromFile)
		{
			std::ifstream data_file("Data/Day1_input.txt");

			auto depth_increase = aoc::DepthAssessor().GetDepthChange(std::ranges::subrange(std::istream_iterator<uint32_t>(data_file), std::istream_iterator<uint32_t>()));

			Logger::WriteMessage(std::format("Depth increase = {}", depth_increase).c_str());
		}
	};
}
