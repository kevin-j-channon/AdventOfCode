#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"
#include "CavernPathFinder.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

namespace test_cavern_path_finder
{
TEST_CLASS(TestCavern)
{
public:
	TEST_METHOD(LoadFromStream)
	{
		constexpr auto data_str =
			"1163751742\n"
			"1381373672\n"
			"2136511328\n"
			"3694931569\n"
			"7463417111\n"
			"1319128137\n"
			"1359912421\n"
			"3125421639\n"
			"1293138521\n"
			"2311944581"
			;

		std::stringstream data(data_str);

		const auto cavern = aoc::navigation::Cavern::from_stream(data);

		const auto& risks = cavern.risk_grid();
		Assert::AreEqual(aoc::navigation::Cavern::Size_t{ 10 }, risks.n_rows);
		Assert::AreEqual(aoc::navigation::Cavern::Size_t{ 10 }, risks.n_cols);

		constexpr auto expected = std::array{
			std::array{1,1,6,3,7,5,1,7,4,2},
			std::array{1,3,8,1,3,7,3,6,7,2},
			std::array{2,1,3,6,5,1,1,3,2,8},
			std::array{3,6,9,4,9,3,1,5,6,9},
			std::array{7,4,6,3,4,1,7,1,1,1},
			std::array{1,3,1,9,1,2,8,1,3,7},
			std::array{1,3,5,9,9,1,2,4,2,1},
			std::array{3,1,2,5,4,2,1,6,3,9},
			std::array{1,2,9,3,1,3,8,5,2,1},
			std::array{2,3,1,1,9,4,4,5,8,1}
		};

		for (auto r = 0u; r < 10; ++r) {
			for (auto c = 0u; c < 10; ++c) {
				Assert::AreEqual(expected[r][c], risks.at(r, c), std::format(L"Mis-match at row {}, col {}", r, c).c_str());
			}
		}
	}
};
}
