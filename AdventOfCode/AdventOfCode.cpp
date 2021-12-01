#include "Day1.hpp"

#include "CppUnitTest.h"

#include <ranges>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <format>
#include <string>
#include <filesystem>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::string_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

namespace AdventOfCode
{
	TEST_CLASS(TestDay1)
	{
	public:

		TEST_METHOD(CountNetIncreaseFromVector)
		{
			auto measurements = std::vector<uint32_t>{ 1, 2, 3, 2, 1, 2, 3 };

			auto depth_score = aoc::DepthAssessor().GetDepthScore(measurements.begin(), measurements.end());

			Assert::AreEqual(uint32_t{ 4 }, depth_score);
		}

		TEST_METHOD(DepthScoreVectorWithVariousJumps)
		{
			auto measurements = std::vector<uint32_t>{ 1, 22, 3, 200, 1000, 2, 3, 4, 100 };

			auto depth_score = aoc::DepthAssessor().GetDepthScore(measurements.begin(), measurements.end());

			Assert::AreEqual(uint32_t{ 6 }, depth_score);
		}

		TEST_METHOD(CountNetIncreaseFromFile)
		{
			std::ifstream data_file(DATA_DIR / "Day1_input.txt");
			Assert::IsTrue(data_file.is_open());

			auto depth_score = aoc::DepthAssessor().GetDepthScore(std::istream_iterator<uint32_t>(data_file), std::istream_iterator<uint32_t>());

			Assert::AreEqual(uint32_t{ 1466 }, depth_score);
		}
	};
}
