#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"
#include "Lanternfish.hpp"
#include "CrabSorter.hpp"
#include "DumboOctopusModel.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

namespace modelling
{
TEST_CLASS(LanternFish)
{
public:
	TEST_METHOD(FishFromIntHasCorrectLife)
	{
		Assert::AreEqual(uint32_t{ 3 }, aoc::Lanternfish{ 3 }.days_until_spawning());
	}

	TEST_METHOD(ShoalOfFishCanBeReadFromStream)
	{
		std::stringstream data("3,4,3,1,2");
		const auto shoal = aoc::LanternfishShoal{}.load(data);

		Assert::AreEqual(aoc::LanternfishShoal::Size_t{ 5 }, shoal.size());
	}

	TEST_METHOD(ReadingShoalFromMalformedStringRaisesAocException)
	{
		std::stringstream data("3,4,X,1,2");
		Assert::ExpectException<aoc::Exception>([&data]() { aoc::LanternfishShoal{}.load(data); });
		Assert::IsTrue(data.fail());
	}

	TEST_METHOD(ReadingShoalFromStringWithOutOfRangeValueRaisesAocException)
	{
		std::stringstream data("3,4,5,-3,2");
		Assert::ExpectException<aoc::Exception>([&data]() { aoc::LanternfishShoal{}.load(data); });
		Assert::IsTrue(data.fail());
	}

	TEST_METHOD(ExampleShoalIsCorrectAfter18Days)
	{
		std::stringstream data("3,4,3,1,2");
		auto shoal = aoc::LanternfishShoal{}.load(data);

		const auto number_of_fish = aoc::LanternfishShoalModel{ shoal }.run_for(std::chrono::days(18)).shoal_size();

		Assert::AreEqual(aoc::LanternfishShoal::Size_t{ 26 }, number_of_fish);
	}

	TEST_METHOD(DecrementingTimeToSpawningBelowZeroResetsTime)
	{
		auto fish = aoc::Lanternfish{ 0 };
		fish.decrement_time_to_spawning();
		Assert::AreEqual(aoc::Lanternfish::days_until_spawning_reset_value, fish.days_until_spawning());
	}
};

TEST_CLASS(CrabSorting)
{
public:
	TEST_METHOD(LoadCrabPositionsFromStream)
	{
		std::stringstream data("16,1,2,0,4,2,7,1,2,14");

		const auto sorter = aoc::CrabSorter{}.load(data);

		const auto expected_positions = { 16, 1, 2, 0, 4, 2, 7, 1, 2, 14 };
		Assert::IsTrue(std::equal(expected_positions.begin(), expected_positions.end(), sorter.positions().begin()));
	}

	TEST_METHOD(CalculateBestPosition)
	{
		std::stringstream data("16,1,2,0,4,2,7,1,2,14");

		const auto [best_position, cost] = aoc::CrabSorter{}.load(data).best_position_and_cost([](uint32_t distance) { return distance; });

		Assert::AreEqual(size_t{ 2 }, best_position);
		Assert::AreEqual(uint32_t{ 37 }, cost);
	}
};

TEST_CLASS(TestDumboOctopusModel)
{
public:
	TEST_METHOD(LoadInitialStateFromStream)
	{
		constexpr auto data_str =
			"5483143223\n"
			"2745854711\n"
			"5264556173\n"
			"6141336146\n"
			"6357385478\n"
			"4167524645\n"
			"2176841721\n"
			"6882881134\n"
			"4846848554\n"
			"5283751526";

		std::stringstream data(data_str);

		const auto model = aoc::DumboOctopusModel<10>{}.load(data);

		const auto expected = std::vector<std::vector<int>>{
			{5,4,8,3,1,4,3,2,2,3},
			{2,7,4,5,8,5,4,7,1,1},
			{5,2,6,4,5,5,6,1,7,3},
			{6,1,4,1,3,3,6,1,4,6},
			{6,3,5,7,3,8,5,4,7,8},
			{4,1,6,7,5,2,4,6,4,5},
			{2,1,7,6,8,4,1,7,2,1},
			{6,8,8,2,8,8,1,1,3,4},
			{4,8,4,6,8,4,8,5,5,4},
			{5,2,8,3,7,5,1,5,2,6}
		};

		for (auto r = 0; r < 10; ++r) {
			for (auto c = 0; c < 10; ++c) {
				Assert::AreEqual(expected[r][c], model.state().at(r, c));
			}
		}
	}

	TEST_METHOD(ConstructFromVectorOfValues)
	{
		const auto expected = std::vector<std::vector<int>>{
			{5,4,8,3,1,4,3,2,2,3},
			{2,7,4,5,8,5,4,7,1,1},
			{5,2,6,4,5,5,6,1,7,3},
			{6,1,4,1,3,3,6,1,4,6},
			{6,3,5,7,3,8,5,4,7,8},
			{4,1,6,7,5,2,4,6,4,5},
			{2,1,7,6,8,4,1,7,2,1},
			{6,8,8,2,8,8,1,1,3,4},
			{4,8,4,6,8,4,8,5,5,4},
			{5,2,8,3,7,5,1,5,2,6}
		};

		const auto model = aoc::DumboOctopusModel<10>(expected);

		for (auto r = 0; r < 10; ++r) {
			for (auto c = 0; c < 10; ++c) {
				Assert::AreEqual(expected[r][c], model.state().at(r, c));
			}
		}
	}

	TEST_METHOD(IncrementMovesModelToTheExpectedState)
	{
		const auto expected = std::vector<std::vector<int>>{
			{5,4,8,3,1,4,3,2,2,3},
			{2,7,4,5,8,5,4,7,1,1},
			{5,2,6,4,5,5,6,1,7,3},
			{6,1,4,1,3,3,6,1,4,6},
			{6,3,5,7,3,8,5,4,7,8},
			{4,1,6,7,5,2,4,6,4,5},
			{2,1,7,6,8,4,1,7,2,1},
			{6,8,8,2,8,8,1,1,3,4},
			{4,8,4,6,8,4,8,5,5,4},
			{5,2,8,3,7,5,1,5,2,6}
		};

		const auto state = aoc::DumboOctopusModel<10>(expected).increment().state();

		for (auto r = 0; r < 10; ++r) {
			for (auto c = 0; c < 10; ++c) {
				Assert::AreEqual(expected[r][c] + 1, state.at(r, c));
			}
		}
	}

	TEST_METHOD(SingleFlashStepWorks) {
		const auto initial_grid = std::vector<std::vector<int>>{
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 9, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
		};

		auto model = aoc::DumboOctopusModel<5>(initial_grid);
		const auto flashes = model.increment().flash();

		Assert::AreEqual(1, flashes);

		const auto expected_grid = std::vector<std::vector<int>>{
			{1, 1, 1, 1, 1},
			{1, 2, 2, 2, 1},
			{1, 2, 0, 2, 1},
			{1, 2, 2, 2, 1},
			{1, 1, 1, 1, 1},
		};

		const auto& final_grid = model.state();

		for (auto r = 0; r < 5; ++r) {
			for (auto c = 0; c < 5; ++c) {
				Assert::AreEqual(expected_grid[r][c], final_grid.at(r, c), std::format(L"Failed to match r = {}, c = {}", r, c).c_str());
			}
		}
	}

	TEST_METHOD(TwoNonInteractingFlashesWork)
	{
		const auto initial_grid = std::vector<std::vector<int>>{
			{0, 0, 0, 0, 0, 0},
			{0, 9, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 9, 0},
			{0, 0, 0, 0, 0, 0}
		};

		auto model = aoc::DumboOctopusModel<6>(initial_grid);
		const auto flashes = model.increment().flash();

		Assert::AreEqual(2, flashes);

		const auto expected_grid = std::vector<std::vector<int>>{
			{2, 2, 2, 1, 1, 1},
			{2, 0, 2, 1, 1, 1},
			{2, 2, 2, 1, 1, 1},
			{1, 1, 1, 2, 2, 2},
			{1, 1, 1, 2, 0, 2},
			{1, 1, 1, 2, 2, 2}
		};

		const auto& final_grid = model.state();

		for (auto r = 0; r < 5; ++r) {
			for (auto c = 0; c < 5; ++c) {
				Assert::AreEqual(expected_grid[r][c], final_grid.at(r, c), std::format(L"Failed to match r = {}, c = {}", r, c).c_str());
			}
		}
	}

	TEST_METHOD(CornerFlashesWork)
	{
		const auto initial_grid = std::vector<std::vector<int>>{
			{9, 0, 0, 0, 9},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{9, 0, 0, 0, 9}
		};

		auto model = aoc::DumboOctopusModel<5>(initial_grid);
		const auto flashes = model.increment().flash();

		Assert::AreEqual(4, flashes);

		const auto expected_grid = std::vector<std::vector<int>>{
			{0, 2, 1, 2, 0},
			{2, 2, 1, 2, 2},
			{1, 1, 1, 1, 1},
			{2, 2, 1, 2, 2},
			{0, 2, 1, 2, 0}
		};

		const auto& final_grid = model.state();

		for (auto r = 0; r < 5; ++r) {
			for (auto c = 0; c < 5; ++c) {
				Assert::AreEqual(expected_grid[r][c], final_grid.at(r, c), std::format(L"Failed to match r = {}, c = {}", r, c).c_str());
			}
		}
	}

	TEST_METHOD(TwoInteractingFlashesWork)
	{
		const auto initial_grid = std::vector<std::vector<int>>{
			{0, 0, 0, 0, 0},
			{0, 9, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 9, 0},
			{0, 0, 0, 0, 0}
		};

		auto model = aoc::DumboOctopusModel<5>(initial_grid);
		const auto flashes = model.increment().flash();

		Assert::AreEqual(2, flashes);

		const auto expected_grid = std::vector<std::vector<int>>{
			{2, 2, 2, 1, 1},
			{2, 0, 2, 1, 1},
			{2, 2, 3, 2, 2},
			{1, 1, 2, 0, 2},
			{1, 1, 2, 2, 2}
		};

		const auto& final_grid = model.state();

		for (auto r = 0; r < 5; ++r) {
			for (auto c = 0; c < 5; ++c) {
				Assert::AreEqual(expected_grid[r][c], final_grid.at(r, c), std::format(L"Failed to match r = {}, c = {}", r, c).c_str());
			}
		}
	}

	TEST_METHOD(SecondaryFlashWorks)
	{
		const auto initial_grid = std::vector<std::vector<int>>{
			{0, 0, 0, 0, 0},
			{0, 9, 8, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0}
		};

		auto model = aoc::DumboOctopusModel<5>(initial_grid);
		const auto flashes = model.increment().flash();

		Assert::AreEqual(2, flashes);

		const auto expected_grid = std::vector<std::vector<int>>{
			{2, 3, 3, 2, 1},
			{2, 0, 0, 2, 1},
			{2, 3, 3, 2, 1},
			{1, 1, 1, 1, 1},
			{1, 1, 1, 1, 1}
		};

		const auto& final_grid = model.state();

		for (auto r = 0; r < 5; ++r) {
			for (auto c = 0; c < 5; ++c) {
				Assert::AreEqual(expected_grid[r][c], final_grid.at(r, c), std::format(L"Failed to match r = {}, c = {}", r, c).c_str());
			}
		}
	}

	TEST_METHOD(LimitOfsecondaryFlashesWorks)
	{
		const auto initial_grid = std::vector<std::vector<int>>{
			{1, 1, 1, 1, 1},
			{1, 9, 9, 9, 1},
			{1, 9, 1, 9, 1},
			{1, 9, 9, 9, 1},
			{1, 1, 1, 1, 1}
		};

		auto model = aoc::DumboOctopusModel<5>(initial_grid);
		const auto flashes = model.increment().flash();

		Assert::AreEqual(9, flashes);

		const auto expected_grid = std::vector<std::vector<int>>{
			{3, 4, 5, 4, 3},
			{4, 0, 0, 0, 4},
			{5, 0, 0, 0, 5},
			{4, 0, 0, 0, 4},
			{3, 4, 5, 4, 3}
		};

		const auto& final_grid = model.state();

		for (auto r = 0; r < 5; ++r) {
			for (auto c = 0; c < 5; ++c) {
				Assert::AreEqual(expected_grid[r][c], final_grid.at(r, c), std::format(L"Failed to match r = {}, c = {}", r, c).c_str());
			}
		}
	}

	TEST_METHOD(SteppingTheExampleWorks_2Steps)
	{
		constexpr auto grid_size = 10;
		constexpr auto initial_state = std::array<std::array<int, grid_size>, grid_size>{
			std::array<int, grid_size>{5, 4, 8, 3, 1, 4, 3, 2, 2, 3},
				std::array<int, grid_size>{2, 7, 4, 5, 8, 5, 4, 7, 1, 1},
				std::array<int, grid_size>{5, 2, 6, 4, 5, 5, 6, 1, 7, 3},
				std::array<int, grid_size>{6, 1, 4, 1, 3, 3, 6, 1, 4, 6},
				std::array<int, grid_size>{6, 3, 5, 7, 3, 8, 5, 4, 7, 8},
				std::array<int, grid_size>{4, 1, 6, 7, 5, 2, 4, 6, 4, 5},
				std::array<int, grid_size>{2, 1, 7, 6, 8, 4, 1, 7, 2, 1},
				std::array<int, grid_size>{6, 8, 8, 2, 8, 8, 1, 1, 3, 4},
				std::array<int, grid_size>{4, 8, 4, 6, 8, 4, 8, 5, 5, 4},
				std::array<int, grid_size>{5, 2, 8, 3, 7, 5, 1, 5, 2, 6}
		};

		auto model = aoc::DumboOctopusModel<grid_size>{ initial_state };

		const auto flashes = model.step(2);

		Assert::AreEqual(35, flashes);

		constexpr auto expected_state = std::array<std::array<int, grid_size>, grid_size>{
			std::array<int, grid_size>{8, 8, 0, 7, 4, 7, 6, 5, 5, 5},
				std::array<int, grid_size>{5, 0, 8, 9, 0, 8, 7, 0, 5, 4},
				std::array<int, grid_size>{8, 5, 9, 7, 8, 8, 9, 6, 0, 8},
				std::array<int, grid_size>{8, 4, 8, 5, 7, 6, 9, 6, 0, 0},
				std::array<int, grid_size>{8, 7, 0, 0, 9, 0, 8, 8, 0, 0},
				std::array<int, grid_size>{6, 6, 0, 0, 0, 8, 8, 9, 8, 9},
				std::array<int, grid_size>{6, 8, 0, 0, 0, 0, 5, 9, 4, 3},
				std::array<int, grid_size>{0, 0, 0, 0, 0, 0, 7, 4, 5, 6},
				std::array<int, grid_size>{9, 0, 0, 0, 0, 0, 0, 8, 7, 6},
				std::array<int, grid_size>{8, 7, 0, 0, 0, 0, 6, 8, 4, 8}
		};

		for (auto r = 0; r < grid_size; ++r) {
			for (auto c = 0; c < grid_size; ++c) {
				Assert::AreEqual(expected_state[r][c], model.state().at(r, c), std::format(L"Failed to match r = {}, c = {}", r, c).c_str());
			}
		}
	}

	TEST_METHOD(SteppingTheExampleWorks_10Steps)
	{
		constexpr auto grid_size = 10;
		constexpr auto initial_state = std::array<std::array<int, grid_size>, grid_size>{
			std::array<int, grid_size>{5, 4, 8, 3, 1, 4, 3, 2, 2, 3},
				std::array<int, grid_size>{2, 7, 4, 5, 8, 5, 4, 7, 1, 1},
				std::array<int, grid_size>{5, 2, 6, 4, 5, 5, 6, 1, 7, 3},
				std::array<int, grid_size>{6, 1, 4, 1, 3, 3, 6, 1, 4, 6},
				std::array<int, grid_size>{6, 3, 5, 7, 3, 8, 5, 4, 7, 8},
				std::array<int, grid_size>{4, 1, 6, 7, 5, 2, 4, 6, 4, 5},
				std::array<int, grid_size>{2, 1, 7, 6, 8, 4, 1, 7, 2, 1},
				std::array<int, grid_size>{6, 8, 8, 2, 8, 8, 1, 1, 3, 4},
				std::array<int, grid_size>{4, 8, 4, 6, 8, 4, 8, 5, 5, 4},
				std::array<int, grid_size>{5, 2, 8, 3, 7, 5, 1, 5, 2, 6}
		};

		auto model = aoc::DumboOctopusModel<grid_size>{ initial_state };

		const auto flashes = model.step(10);

		Assert::AreEqual(204, flashes);

		constexpr auto expected_state = std::array<std::array<int, grid_size>, grid_size>{
			std::array<int, grid_size>{0, 4, 8, 1, 1, 1, 2, 9, 7, 6},
				std::array<int, grid_size>{0, 0, 3, 1, 1, 1, 2, 0, 0, 9},
				std::array<int, grid_size>{0, 0, 4, 1, 1, 1, 2, 5, 0, 4},
				std::array<int, grid_size>{0, 0, 8, 1, 1, 1, 1, 4, 0, 6},
				std::array<int, grid_size>{0, 0, 9, 9, 1, 1, 1, 3, 0, 6},
				std::array<int, grid_size>{0, 0, 9, 3, 5, 1, 1, 2, 3, 3},
				std::array<int, grid_size>{0, 4, 4, 2, 3, 6, 1, 1, 3, 0},
				std::array<int, grid_size>{5, 5, 3, 2, 2, 5, 2, 3, 5, 0},
				std::array<int, grid_size>{0, 5, 3, 2, 2, 5, 0, 6, 0, 0},
				std::array<int, grid_size>{0, 0, 3, 2, 2, 4, 0, 0, 0, 0}
		};

		for (auto r = 0; r < grid_size; ++r) {
			for (auto c = 0; c < grid_size; ++c) {
				Assert::AreEqual(expected_state[r][c], model.state().at(r, c), std::format(L"Failed to match r = {}, c = {}", r, c).c_str());
			}
		}
	}

	TEST_METHOD(SteppingTheExampleWorks_100Steps)
	{
		constexpr auto grid_size = 10;
		constexpr auto initial_state = std::array<std::array<int, grid_size>, grid_size>{
			std::array<int, grid_size>{5, 4, 8, 3, 1, 4, 3, 2, 2, 3},
				std::array<int, grid_size>{2, 7, 4, 5, 8, 5, 4, 7, 1, 1},
				std::array<int, grid_size>{5, 2, 6, 4, 5, 5, 6, 1, 7, 3},
				std::array<int, grid_size>{6, 1, 4, 1, 3, 3, 6, 1, 4, 6},
				std::array<int, grid_size>{6, 3, 5, 7, 3, 8, 5, 4, 7, 8},
				std::array<int, grid_size>{4, 1, 6, 7, 5, 2, 4, 6, 4, 5},
				std::array<int, grid_size>{2, 1, 7, 6, 8, 4, 1, 7, 2, 1},
				std::array<int, grid_size>{6, 8, 8, 2, 8, 8, 1, 1, 3, 4},
				std::array<int, grid_size>{4, 8, 4, 6, 8, 4, 8, 5, 5, 4},
				std::array<int, grid_size>{5, 2, 8, 3, 7, 5, 1, 5, 2, 6}
		};

		auto model = aoc::DumboOctopusModel<grid_size>{ initial_state };

		const auto flashes = model.step(100);

		Assert::AreEqual(1656, flashes);

		constexpr auto expected_state = std::array<std::array<int, grid_size>, grid_size>{
			std::array<int, grid_size>{0, 3, 9, 7, 6, 6, 6, 8, 6, 6},
				std::array<int, grid_size>{0, 7, 4, 9, 7, 6, 6, 9, 1, 8},
				std::array<int, grid_size>{0, 0, 5, 3, 9, 7, 6, 9, 3, 3},
				std::array<int, grid_size>{0, 0, 0, 4, 2, 9, 7, 8, 2, 2},
				std::array<int, grid_size>{0, 0, 0, 4, 2, 2, 9, 8, 9, 2},
				std::array<int, grid_size>{0, 0, 5, 3, 2, 2, 2, 8, 7, 7},
				std::array<int, grid_size>{0, 5, 3, 2, 2, 2, 2, 9, 6, 6},
				std::array<int, grid_size>{9, 3, 2, 2, 2, 2, 8, 9, 6, 6},
				std::array<int, grid_size>{7, 9, 2, 2, 2, 8, 6, 8, 6, 6},
				std::array<int, grid_size>{6, 7, 8, 9, 9, 9, 8, 7, 6, 6}
		};

		for (auto r = 0; r < grid_size; ++r) {
			for (auto c = 0; c < grid_size; ++c) {
				Assert::AreEqual(expected_state[r][c], model.state().at(r, c), std::format(L"Failed to match r = {}, c = {}", r, c).c_str());
			}
		}
	}

	TEST_METHOD(FirstSynchronisedFlashIsStep195)
	{
		constexpr auto grid_size = 10;
		constexpr auto initial_state = std::array<std::array<int, grid_size>, grid_size>{
			std::array<int, grid_size>{5, 4, 8, 3, 1, 4, 3, 2, 2, 3},
				std::array<int, grid_size>{2, 7, 4, 5, 8, 5, 4, 7, 1, 1},
				std::array<int, grid_size>{5, 2, 6, 4, 5, 5, 6, 1, 7, 3},
				std::array<int, grid_size>{6, 1, 4, 1, 3, 3, 6, 1, 4, 6},
				std::array<int, grid_size>{6, 3, 5, 7, 3, 8, 5, 4, 7, 8},
				std::array<int, grid_size>{4, 1, 6, 7, 5, 2, 4, 6, 4, 5},
				std::array<int, grid_size>{2, 1, 7, 6, 8, 4, 1, 7, 2, 1},
				std::array<int, grid_size>{6, 8, 8, 2, 8, 8, 1, 1, 3, 4},
				std::array<int, grid_size>{4, 8, 4, 6, 8, 4, 8, 5, 5, 4},
				std::array<int, grid_size>{5, 2, 8, 3, 7, 5, 1, 5, 2, 6}
		};

		auto model = aoc::DumboOctopusModel<grid_size>{ initial_state };

		const auto first_sync_step = model.find_first_sync_step();

		Assert::AreEqual(195, first_sync_step);
	}
};

}
