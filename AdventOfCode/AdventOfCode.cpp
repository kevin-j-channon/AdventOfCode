#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"
#include "DiagnosticLog.hpp"
#include "BoatSystems.hpp"
#include "AdventOfCode.hpp"
#include "Lanternfish.hpp"
#include "CrabSorter.hpp"
#include "DigitAnalyser.hpp"
#include "SyntaxChecker.hpp"
#include "DumboOctopusModel.hpp"
#include "CaveNavigator.hpp"

#include <vector>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <format>
#include <string>
#include <filesystem>
#include <algorithm>
#include <chrono>

using namespace std::string_literals;
using namespace std::chrono_literals;

template<>
std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<aoc::bingo::Board::State_t>(const aoc::bingo::Board::State_t& state)
{
	switch (state)
	{
	case aoc::bingo::Board::State_t::no_win: return L"no win";
	case aoc::bingo::Board::State_t::win: return L"win";
	default:
		return L"UNKNOWN";
	}
}


const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

namespace program
{
TEST_CLASS(Story)
{
public:
	TEST_METHOD(_)
	{
		using namespace aoc;

		auto sub = Submarine{};

		// Day 1
		{
			Logger::WriteMessage("Day 1:\n");

			using Iter_t = std::istream_iterator<uint32_t>;
			{
				// Part 1
				std::ifstream data_file(DATA_DIR / "Day1_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto depth_score_1 = sub.boat_systems().depth_score<1>(Iter_t{ data_file }, Iter_t{});
				Logger::WriteMessage(std::format("\tCalculated depth score with window-size of 1: {}\n", depth_score_1).c_str());
			}

			// Part 2
			{
				std::ifstream data_file(DATA_DIR / "Day1_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto depth_score_3 = sub.boat_systems().depth_score<3>(Iter_t{ data_file }, Iter_t{});
				Logger::WriteMessage(std::format("\tCalculated depth score with window-size of 3: {}\n", depth_score_3).c_str());
			}
		}

		// Day 2
		{
			Logger::WriteMessage("Day 2:\n");

			using Iter_t = std::istream_iterator<aoc::Direction>;

			// Part 1
			{
				std::ifstream data_file(DATA_DIR / "Day2_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto net_direction = sub.boat_systems().net_direction(Iter_t{ data_file }, Iter_t{});
				Logger::WriteMessage(std::format("\tNet direction: ({}, {})\n", net_direction.x, net_direction.y).c_str());
			}

			// Part 2
			{
				std::ifstream data_file(DATA_DIR / "Day2_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto net_aiming = sub.boat_systems().net_aiming(Iter_t{ data_file }, Iter_t{});
				Logger::WriteMessage(std::format("\tNet aiming: ({}, {})\n", net_aiming.x, net_aiming.y).c_str());
			}
		}

		// Day 3
		{
			Logger::WriteMessage("Day 3:\n");

			// Part 1
			{
				std::ifstream data_file(DATA_DIR / "Day3_input.txt");
				Assert::IsTrue(data_file.is_open());

				auto log = aoc::DiagnosticLog{ data_file };

				const auto power_consumption = aoc::Submarine().boat_systems().power_consumption(log);

				Logger::WriteMessage(std::format("\tPower: {}\n", power_consumption).c_str());
			}

			// Part 2
			{
				std::ifstream data_file(DATA_DIR / "Day3_input.txt");
				Assert::IsTrue(data_file.is_open());

				auto log = aoc::DiagnosticLog{ data_file };

				Logger::WriteMessage(std::format("\tLife support rating: {}\n",
					aoc::Submarine().boat_systems().life_support_rating(log)).c_str());
			}
		}

		// Day 4
		{
			Logger::WriteMessage("Day 4:\n");

			// Part 1
			{
				std::ifstream data_file(DATA_DIR / "Day4_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto game_score = aoc::Submarine().entertainment().bingo_game()
					.load(data_file)
					.play_to_win()
					.score();

				Logger::WriteMessage(std::format("\tWinning bingo score: {}\n", *game_score).c_str());
			}

			// Part 2
			{
				std::ifstream data_file(DATA_DIR / "Day4_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto game_score = aoc::Submarine().entertainment().bingo_game()
					.load(data_file)
					.play_to_lose()
					.score();

				Logger::WriteMessage(std::format("\tLosing bingo score: {}\n", *game_score).c_str());
			}
		}

		// Day 5
		{
			Logger::WriteMessage("Day 5:\n");

			// Part 1
			{
				std::ifstream data_file(DATA_DIR / "Day5_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto vent_score = aoc::Submarine()
					.boat_systems()
					.detect_vents<aoc::VentAnalyzer::horizontal | aoc::VentAnalyzer::vertical>(data_file);

				Logger::WriteMessage(std::format("\tHorizontal|vertical vent score: {}\n", vent_score).c_str());
			}

			// Part 2
			{
				std::ifstream data_file(DATA_DIR / "Day5_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto vent_score = aoc::Submarine()
					.boat_systems()
					.detect_vents<aoc::VentAnalyzer::horizontal | aoc::VentAnalyzer::vertical | aoc::VentAnalyzer::diagonal>(data_file);

				Logger::WriteMessage(std::format("\tHorizontal|vertical|diagonal vent score: {}\n", vent_score).c_str());
			}
		}

		// Day 6
		{
			Logger::WriteMessage("Day 6:\n");

			// Part 1
			{
				std::ifstream data_file(DATA_DIR / "Day6_input.txt");
				Assert::IsTrue(data_file.is_open());

				auto shoal = aoc::LanternfishShoal{}.load(data_file);

				const auto number_of_fish = aoc::LanternfishShoalModel{ shoal }.run_for(std::chrono::days(80)).shoal_size();

				Logger::WriteMessage(std::format("\tShoal size after 80 days: {}\n", number_of_fish).c_str());
			}

			// Part 2
			{
				std::ifstream data_file(DATA_DIR / "Day6_input.txt");
				Assert::IsTrue(data_file.is_open());

				auto shoal = aoc::LanternfishShoal{}.load(data_file);

				const auto number_of_fish = aoc::LanternfishShoalModel{ shoal }.run_for(std::chrono::days(256)).shoal_size();

				Logger::WriteMessage(std::format("\tShoal size after 256 days: {}\n", number_of_fish).c_str());
			}
		}

		// Day 7
		{
			Logger::WriteMessage("Day 7:\n");

			// Part 1
			{
				std::ifstream data_file(DATA_DIR / "Day7_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto [best_position, cost] = aoc::CrabSorter{}.load(data_file).best_position_and_cost([](uint32_t distance) { return distance; });

				Logger::WriteMessage(std::format("\tBest position: {}, cost: {}\n", best_position, cost).c_str());
			}

			// Part 2
			{
				std::ifstream data_file(DATA_DIR / "Day7_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto [best_position, cost] = aoc::CrabSorter{}
					.load(data_file)
					.best_position_and_cost([](uint32_t distance) {
					return (distance * (1 + distance)) / 2;
						});

				Logger::WriteMessage(std::format("\tBest position: {}, cost: {}\n", best_position, cost).c_str());
			}
		}

		// Day 8
		{
			Logger::WriteMessage("Day 8:\n");

			// Part 1
			{
				std::ifstream data_file(DATA_DIR / "Day8_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto score = aoc::DigitAnalyser{}.load(data_file).count_1478();

				Logger::WriteMessage(std::format("\tNumber of 1, 4, 7 & 8 digits: {}\n", score).c_str());
			}

			// Part 2
			{
				std::ifstream data_file(DATA_DIR / "Day8_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto score = aoc::DigitAnalyser{}.load(data_file).decode_and_sum();

				Logger::WriteMessage(std::format("\tTotal score: {}\n", score).c_str());
			}
		}

		// Day 9
		{
			Logger::WriteMessage("Day 9:\n");

			// Part 1
			{
				std::ifstream data_file(DATA_DIR / "Day9_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto risk = aoc::Submarine().boat_systems().lava_tube_smoke_risk(data_file);

				Logger::WriteMessage(std::format("\tLava tube smoke risk: {}\n", risk).c_str());
			}

			// Part 2
			{
				// Life''s too short! Come back to this one at some point...
			}
		}

		// Day 10
		{
			Logger::WriteMessage("Day 10:\n");

			// Part 1
			{
				std::ifstream data_file(DATA_DIR / "Day10_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto score = aoc::SyntaxChecker{}.score_lines(data_file).syntax_error_score();

				Logger::WriteMessage(std::format("\tSyntax error score: {}\n", score).c_str());
			}

			// Part 2
			{
				std::ifstream data_file(DATA_DIR / "Day10_input.txt");
				Assert::IsTrue(data_file.is_open());


				auto checker = aoc::SyntaxChecker{};
				const auto score = checker.score_lines(data_file).incomplete_line_score();

				Logger::WriteMessage(std::format("\tIncompete lines score: {}\n", score).c_str());
			}
		}

		// Day 11
		{
			Logger::WriteMessage("Day 11:\n");

			// Part 1
			{
				std::ifstream data_file(DATA_DIR / "Day11_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto flashes = aoc::DumboOctopusModel<10>{}.load(data_file).step(100);

				Logger::WriteMessage(std::format("\tFlashes after 100 steps: {}\n", flashes).c_str());
			}

			// Part 2
			{
				std::ifstream data_file(DATA_DIR / "Day11_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto first_sync_step = aoc::DumboOctopusModel<10>{}.load(data_file).find_first_sync_step();

				Logger::WriteMessage(std::format("\tFirst synchronised flash at step: {}\n", first_sync_step).c_str());
			}
		}
	}
};
}


namespace entertainment
{
TEST_CLASS(BingoGame)
{
public:

	TEST_METHOD(LoadExampleDraws)
	{
		std::stringstream ss{ "7, 4, 9, 5\n" };

		auto draws = aoc::bingo::FileBasedNumberDrawer<uint8_t>{};

		draws.load(ss);

		const auto expected_draws = { 7, 4, 9, 5 };
		Assert::IsTrue(std::equal(expected_draws.begin(), expected_draws.end(), draws.begin()));
	}

	TEST_METHOD(BoardIdReturnsCorrectId)
	{
		Assert::AreEqual(aoc::bingo::Board::Id_t{ 10 }, aoc::bingo::Board{ 10, 3 }.id());
	}

	TEST_METHOD(LoadBoard)
	{
		constexpr auto board_str =
			"22 13 17 11  0\n"
			" 8  2 23  4 24\n"
			"21  9 14 16  7\n"
			" 6 10  3 18  5\n"
			" 1 12 20 15 19\n"
			"\n";

		std::stringstream ss{ board_str };

		auto board = aoc::bingo::Board{ 0, 5 };

		board.load(ss);
	}

	TEST_METHOD(IterateBoardWorks)
	{
		constexpr auto board_str =
			"22 13 17 11  0\n"
			" 8  2 23  4 24\n"
			"21  9 14 16  7\n"
			" 6 10  3 18  5\n"
			" 1 12 20 15 19\n"
			"\n";

		std::stringstream ss{ board_str };

		auto board = aoc::bingo::Board{ 0, 5 };

		board.load(ss);

		const auto expected_values = { 22, 8, 21, 6, 1, 13, 2, 9, 10, 12, 17, 23, 14, 3, 20, 11, 4, 16, 18, 15, 0, 24, 7, 5, 19 };
		auto cell = board.begin();
		for (auto expected = expected_values.begin(); expected != expected_values.end(); ++expected, ++cell) {
			Assert::AreEqual(static_cast<uint8_t>( *expected ), (*cell).value);
		}
	}

	TEST_METHOD(LoadGame)
	{
		constexpr auto game_str =
			"7, 4, 9, 5, 11, 17, 23, 2, 0, 14, 21, 24, 10, 16, 13, 6, 15, 25, 12, 22, 18, 20, 8, 19, 3, 26, 1\n"
			"\n"
			"22 13 17 11  0\n"
			"8  2 23  4 24\n"
			"21  9 14 16  7\n"
			"6 10  3 18  5\n"
			"1 12 20 15 19\n"
			"\n"
			"3 15  0  2 22\n"
			"9 18 13 17  5\n"
			"19  8  7 25 23\n"
			"20 11 10 24  4\n"
			"14 21 16 12  6\n"
			"\n"
			"14 21 17 24  4\n"
			"10 16 15  9 19\n"
			"18  8 23 26 20\n"
			"22 11 13  6  5\n"
			"2  0 12  3  7";

		std::stringstream ss{ game_str };

		auto game = aoc::bingo::Game<aoc::bingo::FileBasedNumberDrawer<uint8_t>>{}.load(ss);
	}

	TEST_METHOD(PlayGame)
	{
		constexpr auto game_str =
			"7, 4, 9, 5, 11, 17, 23, 2, 0, 14, 21, 24, 10, 16, 13, 6, 15, 25, 12, 22, 18, 20, 8, 19, 3, 26, 1\n"
			"\n"
			"22 13 17 11  0\n"
			"8  2 23  4 24\n"
			"21  9 14 16  7\n"
			"6 10  3 18  5\n"
			"1 12 20 15 19\n"
			"\n"
			"3 15  0  2 22\n"
			"9 18 13 17  5\n"
			"19  8  7 25 23\n"
			"20 11 10 24  4\n"
			"14 21 16 12  6\n"
			"\n"
			"14 21 17 24  4\n"
			"10 16 15  9 19\n"
			"18  8 23 26 20\n"
			"22 11 13  6  5\n"
			"2  0 12  3  7";

		std::stringstream ss{ game_str };

		auto game = aoc::bingo::Game<aoc::bingo::FileBasedNumberDrawer<uint8_t>>{}.load(ss).play_to_win();
	}

	TEST_METHOD(PlayingWithoutABoardCausesAnException)
	{
		Assert::ExpectException<aoc::Exception>([]() {aoc::bingo::Player().play_number(10); });
	}

	TEST_METHOD(PlayerCanCheckBoardIfBoardHasWon)
	{
		constexpr auto board_str =
			"22 13 17\n"
			" 8  2 23\n"
			"21  9 14";

		std::stringstream ss{ board_str };

		auto board = aoc::bingo::Board{ 0, 3 }.load(ss);

		auto player = aoc::bingo::Player().assign_board(board);

		Assert::AreEqual(aoc::bingo::Board::State_t::no_win, player.play_number(22));
		Assert::AreEqual(aoc::bingo::Board::State_t::no_win, player.play_number(13));
		Assert::AreEqual(aoc::bingo::Board::State_t::win, player.play_number(17));
	}

	TEST_METHOD(GameDetectsRowWin)
	{

		constexpr auto game_str =
			"7, 4, 9, 5, 11, 17, 23, 2, 0, 14, 21, 24, 10, 16, 13, 6, 15, 25, 12, 22, 18, 20, 8, 19, 3, 26, 1\n"
			"\n"
			"22 13 17 11  0\n"
			"8  2 23  4 24\n"
			"21  9 14 16  7\n"
			"6 10  3 18  5\n"
			"1 12 20 15 19\n"
			"\n"
			"3 15  0  2 22\n"
			"9 18 13 17  5\n"
			"19  8  7 25 23\n"
			"20 11 10 24  4\n"
			"14 21 16 12  6\n"
			"\n"
			"14 21 17 24  4\n"
			"10 16 15  9 19\n"
			"18  8 23 26 20\n"
			"22 11 13  6  5\n"
			"2  0 12  3  7";

		std::stringstream ss{ game_str };

		auto winner = aoc::bingo::Game<aoc::bingo::FileBasedNumberDrawer<uint8_t>>{}
			.load(ss)
			.play_to_win()
			.get_winner();

		Assert::IsTrue(std::nullopt != winner);
		Assert::AreEqual(uint8_t{ 24 }, winner->number);
		Assert::AreEqual(aoc::bingo::Board::Id_t{ 2 }, winner->board.id());
	}

	TEST_METHOD(GameDetectsColumnWin)
	{

		constexpr auto game_str =
			"1, 2, 3, 4, 5, 6, 7\n"
			"\n"
			"22 13 17 11  0\n"
			" 8  2 23  4 24\n"
			"21  9 14 16  7\n"
			" 6 10  3 18  5\n"
			" 1 12 20 15 19\n"
			"\n"
			" 8  1  0 30 22\n"
			" 9  2 13 17 21\n"
			"19  3  7 25 23\n"
			"20  4 10 24 11\n"
			"14  5 16 12  6\n"
			"\n"
			"14 21 17 24  4\n"
			"10 16 15  9 19\n"
			"18  8 23 26 20\n"
			"22 11 13  6  5\n"
			" 2  0 12  3  7";

		std::stringstream ss{ game_str };

		auto winner = aoc::bingo::Game<aoc::bingo::FileBasedNumberDrawer<uint8_t>>{}
			.load(ss)
			.play_to_win()
			.get_winner();

		Assert::IsTrue(std::nullopt != winner);
		Assert::AreEqual(uint8_t{ 5 }, winner->number);
		Assert::AreEqual(aoc::bingo::Board::Id_t{ 1 }, winner->board.id());
	}
};
}

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
}

namespace day_1
{
TEST_CLASS(SingleValueDepthScores)
{
public:

	TEST_METHOD(CountNetIncreaseFromFile)
	{
		std::ifstream data_file(DATA_DIR / "Day1_input.txt");
		Assert::IsTrue(data_file.is_open());

		auto depth_score = aoc::Submarine().boat_systems().depth_score<1>(std::istream_iterator<uint32_t>(data_file), std::istream_iterator<uint32_t>());

		Assert::AreEqual(uint32_t{ 1502 }, depth_score);
	}
};

TEST_CLASS(TripleValueDepthScores)
{
public:
	TEST_METHOD(RollingWindow3ExampleValues)
	{
		auto measurements = std::vector<uint32_t>{ 199, 200, 208, 210, 200, 207, 240, 269, 260, 263 };

		auto depth_score = aoc::Submarine().boat_systems().depth_score<3>(measurements.begin(), measurements.end());

		Assert::AreEqual(uint32_t{ 5 }, depth_score);
	}

	TEST_METHOD(RollingWindow3FromFile)
	{
		std::ifstream data_file(DATA_DIR / "Day1_input.txt");
		Assert::IsTrue(data_file.is_open());

		auto depth_score = aoc::Submarine().boat_systems().depth_score<3>(std::istream_iterator<uint32_t>(data_file), std::istream_iterator<uint32_t>());

		Assert::AreEqual(uint32_t{ 1538 }, depth_score);
	}
};
}

namespace day_2
{

TEST_CLASS(TestDay2)
{
public:

	TEST_METHOD(ParseDirectionsFromFile)
	{
		std::ifstream data_file(DATA_DIR / "Day2_input.txt");
		Assert::IsTrue(data_file.is_open());

		using StreamIter_t = std::istream_iterator<aoc::Direction>;
		const auto net_direction = aoc::Submarine().boat_systems().net_direction(StreamIter_t(data_file), StreamIter_t());

		Assert::AreEqual(1895, net_direction.x);
		Assert::AreEqual(894, net_direction.y);
	}
};
}

namespace day_3
{
TEST_CLASS(TestDay3)
{
public:

	TEST_METHOD(CalculatePowerParamsFromFile)
	{
		std::ifstream data_file(DATA_DIR / "Day3_input.txt");
		Assert::IsTrue(data_file.is_open());

		auto log = aoc::DiagnosticLog{ data_file };

		const auto power_consumption = aoc::Submarine().boat_systems().power_consumption(log);

		Logger::WriteMessage(std::format("Power: {}", power_consumption).c_str());
		Assert::AreEqual(uint32_t{ 693486 }, power_consumption);
	}

	TEST_METHOD(LifeSupportRatingIsCalculatedFromFile)
	{
		std::ifstream data_file(DATA_DIR / "Day3_input.txt");
		Assert::IsTrue(data_file.is_open());

		auto log = aoc::DiagnosticLog{ data_file };

		Assert::AreEqual(uint32_t{ 3379326 }, aoc::Submarine().boat_systems().life_support_rating(log));
	}
};
}

namespace day_4
{
TEST_CLASS(TestDay4)
{
public:
	TEST_METHOD(ExampleGameScoreIsCorrect)
	{
		constexpr auto game_str =
			"7, 4, 9, 5, 11, 17, 23, 2, 0, 14, 21, 24, 10, 16, 13, 6, 15, 25, 12, 22, 18, 20, 8, 19, 3, 26, 1\n"
			"\n"
			"22 13 17 11  0\n"
			"8  2 23  4 24\n"
			"21  9 14 16  7\n"
			"6 10  3 18  5\n"
			"1 12 20 15 19\n"
			"\n"
			"3 15  0  2 22\n"
			"9 18 13 17  5\n"
			"19  8  7 25 23\n"
			"20 11 10 24  4\n"
			"14 21 16 12  6\n"
			"\n"
			"14 21 17 24  4\n"
			"10 16 15  9 19\n"
			"18  8 23 26 20\n"
			"22 11 13  6  5\n"
			"2  0 12  3  7";

		std::stringstream ss{ game_str };

		const auto game_score = aoc::bingo::Game<aoc::bingo::FileBasedNumberDrawer<uint8_t>>{}
			.load(ss)
			.play_to_win()
			.score();

		Assert::IsTrue(std::nullopt != game_score);
		Assert::AreEqual(uint32_t{ 4512 }, *game_score);
	}

	TEST_METHOD(FindWinningScoreForFullInput)
	{
		std::ifstream data_file(DATA_DIR / "Day4_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto game_score = aoc::Submarine().entertainment().bingo_game()
			.load(data_file)
			.play_to_win()
			.score();

		Assert::IsTrue(std::nullopt != game_score);
		Assert::AreEqual(uint32_t{ 2745 }, *game_score);
	}

	TEST_METHOD(PlayingToLoseOnExampleData)
	{

		constexpr auto game_str =
			"7, 4, 9, 5, 11, 17, 23, 2, 0, 14, 21, 24, 10, 16, 13, 6, 15, 25, 12, 22, 18, 20, 8, 19, 3, 26, 1\n"
			"\n"
			"22 13 17 11  0\n"
			" 8  2 23  4 24\n"
			"21  9 14 16  7\n"
			" 6 10  3 18  5\n"
			" 1 12 20 15 19\n"
			"\n"
			" 3 15  0  2 22\n"
			" 9 18 13 17  5\n"
			"19  8  7 25 23\n"
			"20 11 10 24  4\n"
			"14 21 16 12  6\n"
			"\n"
			"14 21 17 24  4\n"
			"10 16 15  9 19\n"
			"18  8 23 26 20\n"
			"22 11 13  6  5\n"
			" 2  0 12  3  7";

		std::stringstream ss{ game_str };

		auto winner = aoc::bingo::Game<aoc::bingo::FileBasedNumberDrawer<uint8_t>>{}
			.load(ss)
			.play_to_lose()
			.get_winner();

		Assert::IsTrue(std::nullopt != winner);
		Assert::AreEqual(uint8_t{ 13 }, winner->number);
		Assert::AreEqual(aoc::bingo::Board::Id_t{ 1 }, winner->board.id());
	}

	TEST_METHOD(FindLosingScoreForFullInput)
	{
		std::ifstream data_file(DATA_DIR / "Day4_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto game_score = aoc::Submarine().entertainment().bingo_game()
			.load(data_file)
			.play_to_lose()
			.score();

		Assert::IsTrue(std::nullopt != game_score);
		Assert::AreEqual(uint32_t{ 6594 }, *game_score);
	}
};
}

namespace day_5
{
TEST_CLASS(TestDay5)
{
public:
	TEST_METHOD(FindVentScoreForFullInput)
	{
		std::ifstream data_file(DATA_DIR / "Day5_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto vent_score = aoc::Submarine()
			.boat_systems()
			.detect_vents<aoc::VentAnalyzer::horizontal | aoc::VentAnalyzer::vertical>(data_file);

		Assert::AreEqual(uint32_t{ 6267 }, vent_score);
	}

	TEST_METHOD(FindVentScoreWithDiagonalsForFullInput)
	{
		std::ifstream data_file(DATA_DIR / "Day5_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto vent_score = aoc::Submarine()
			.boat_systems()
			.detect_vents<aoc::VentAnalyzer::horizontal | aoc::VentAnalyzer::vertical | aoc::VentAnalyzer::diagonal>(data_file);

		Assert::AreEqual(uint32_t{ 20196 }, vent_score);
	}
};
}

namespace day_6
{
TEST_CLASS(TestDay6)
{
public:
	TEST_METHOD(FindFishCountForFullInput_80Days)
	{
		std::ifstream data_file(DATA_DIR / "Day6_input.txt");
		Assert::IsTrue(data_file.is_open());

		auto shoal = aoc::LanternfishShoal{}.load(data_file);

		const auto number_of_fish = aoc::LanternfishShoalModel{ shoal }.run_for(std::chrono::days(80)).shoal_size();

		Assert::AreEqual(aoc::LanternfishShoal::Size_t{ 360268 }, number_of_fish);
	}

	TEST_METHOD(FindFishCountForFullInput_256Days)
	{
		std::ifstream data_file(DATA_DIR / "Day6_input.txt");
		Assert::IsTrue(data_file.is_open());

		auto shoal = aoc::LanternfishShoal{}.load(data_file);

		const auto number_of_fish = aoc::LanternfishShoalModel{ shoal }.run_for(std::chrono::days(256)).shoal_size();

		Assert::AreEqual(aoc::LanternfishShoal::Size_t{ 1632146183902 }, number_of_fish);
	}
};
}

namespace day_7
{
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

	TEST_METHOD(CalculateBestPositionForAllInputWithLinCost)
	{
		std::ifstream data_file(DATA_DIR / "Day7_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto [best_position, cost] = aoc::CrabSorter{}.load(data_file).best_position_and_cost([](uint32_t distance) { return distance; });

		Assert::AreEqual(size_t{ 330 }, best_position);
		Assert::AreEqual(uint32_t{ 329389 }, cost);
	}

	TEST_METHOD(CalculateBestPositionForAllInputWithQuadraticCost)
	{
		std::ifstream data_file(DATA_DIR / "Day7_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto [best_position, cost] = aoc::CrabSorter{}
			.load(data_file)
			.best_position_and_cost([](uint32_t distance) { 
				return (distance * ( 1 + distance)) / 2;
			});

		Assert::AreEqual(size_t{ 459 }, best_position);
		Assert::AreEqual(uint32_t{ 86397080 }, cost);
	}
};
}

namespace day_8
{
TEST_CLASS(TestDay8)
{
public:
	TEST_METHOD(LoadOutputValueStringFromStream)
	{
		constexpr auto data_string =
			"be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb | fdgacbe cefdb cefbgd gcbe";

		std::stringstream ss(data_string);
		const auto digit_data = aoc::DigitData{}.load(ss);

		const auto& output_vals = digit_data.output_value_strings();
		Assert::AreEqual(size_t{ 4 }, output_vals.size());
	}

	TEST_METHOD(LoadReferenceValueStringFromStream)
	{
		constexpr auto data_string =
			"be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb | fdgacbe cefdb cefbgd gcbe";

		std::stringstream ss(data_string);
		const auto digit_data = aoc::DigitData{}.load(ss);

		const auto& ref_vals = digit_data.reference_value_strings();
		Assert::AreEqual(size_t{ 10 }, ref_vals.size());
	}

	TEST_METHOD(ScoreIsCalculatedCorrectlyForSampleData)
	{
		constexpr auto data_string =
			"be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb | fdgacbe cefdb cefbgd gcbe\n"
			"edbfga begcd cbg gc gcadebf fbgde acbgfd abcde gfcbed gfec | fcgedb cgb dgebacf gc\n"
			"fgaebd cg bdaec gdafb agbcfd gdcbef bgcad gfac gcb cdgabef | cg cg fdcagb cbg\n"
			"fbegcd cbd adcefb dageb afcb bc aefdc ecdab fgdeca fcdbega | efabcd cedba gadfec cb\n"
			"aecbfdg fbg gf bafeg dbefa fcge gcbea fcaegb dgceab fcbdga | gecf egdcabf bgf bfgea\n"
			"fgeab ca afcebg bdacfeg cfaedg gcfdb baec bfadeg bafgc acf | gebdcfa ecba ca fadegcb\n"
			"dbcfg fgd bdegcaf fgec aegbdf ecdfab fbedc dacgb gdcebf gf | cefg dcbef fcge gbcadfe\n"
			"bdfegc cbegaf gecbf dfcage bdacg ed bedf ced adcbefg gebcd | ed bcgafe cdgba cbgef\n"
			"egadfb cdbfeg cegd fecab cgb gbdefca cg fgcdab egfdb bfceg | gbdfcae bgc cg cgb\n"
			"gcafb gcf dcaebfg ecagb gf abcdeg gaef cafbge fdbac fegbdc | fgae cfgab fg bagce";

		std::stringstream ss(data_string);
		const auto score = aoc::DigitAnalyser{}.load(ss).count_1478();

		Assert::AreEqual(uint32_t{ 26 }, score);
	}

	TEST_METHOD(ScoreIsCalculatedCorrectlyForFullInput)
	{
		std::ifstream data_file(DATA_DIR / "Day8_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto score = aoc::DigitAnalyser{}.load(data_file).count_1478();

		Assert::AreEqual(uint32_t{ 554 }, score);
	}

	TEST_METHOD(DecodeSingleLine)
	{
		constexpr auto data_string =
			"acedgfb cdfbe gcdfa fbcad dab cefabd cdfgeb eafb cagedb ab | cdfeb fcadb cdfeb cdbaf";

		std::stringstream ss(data_string);
		const auto score = aoc::DigitData{}.load(ss).decode();

		Assert::AreEqual(uint32_t{ 5353 }, score);
	}

	TEST_METHOD(DecodeAndSumSampleData)
	{
		constexpr auto data_string =
			"be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb | fdgacbe cefdb cefbgd gcbe\n"
			"edbfga begcd cbg gc gcadebf fbgde acbgfd abcde gfcbed gfec | fcgedb cgb dgebacf gc\n"
			"fgaebd cg bdaec gdafb agbcfd gdcbef bgcad gfac gcb cdgabef | cg cg fdcagb cbg\n"
			"fbegcd cbd adcefb dageb afcb bc aefdc ecdab fgdeca fcdbega | efabcd cedba gadfec cb\n"
			"aecbfdg fbg gf bafeg dbefa fcge gcbea fcaegb dgceab fcbdga | gecf egdcabf bgf bfgea\n"
			"fgeab ca afcebg bdacfeg cfaedg gcfdb baec bfadeg bafgc acf | gebdcfa ecba ca fadegcb\n"
			"dbcfg fgd bdegcaf fgec aegbdf ecdfab fbedc dacgb gdcebf gf | cefg dcbef fcge gbcadfe\n"
			"bdfegc cbegaf gecbf dfcage bdacg ed bedf ced adcbefg gebcd | ed bcgafe cdgba cbgef\n"
			"egadfb cdbfeg cegd fecab cgb gbdefca cg fgcdab egfdb bfceg | gbdfcae bgc cg cgb\n"
			"gcafb gcf dcaebfg ecagb gf abcdeg gaef cafbge fdbac fegbdc | fgae cfgab fg bagce";

		std::stringstream ss(data_string);
		const auto score = aoc::DigitAnalyser{}.load(ss).decode_and_sum();

		Assert::AreEqual(uint32_t{ 61229 }, score);
	}

	TEST_METHOD(DecodeAndSumFullData)
	{
		std::ifstream data_file(DATA_DIR / "Day8_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto score = aoc::DigitAnalyser{}.load(data_file).decode_and_sum();

		Assert::AreEqual(uint32_t{ 990964 }, score);
	}
};
}

namespace day_9
{
TEST_CLASS(TestDay9)
{
public:
	TEST_METHOD(AnalyseFullData)
	{
		std::ifstream data_file(DATA_DIR / "Day9_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto risk = aoc::Submarine().boat_systems().lava_tube_smoke_risk(data_file);

		Assert::AreEqual(uint32_t{ 545 }, risk);
	}
};
}

namespace day_10
{
TEST_CLASS(TestDay10)
{
public:
	TEST_METHOD(SimpleRoundBracketScoresZero)
	{
		Assert::AreEqual(uint64_t{ 0 }, aoc::SyntaxChecker::score_line("()"s).value);
	}

	TEST_METHOD(SimpleSquareBracketScoresZero)
	{
		Assert::AreEqual(uint64_t{ 0 }, aoc::SyntaxChecker::score_line("[]"s).value);
	}

	TEST_METHOD(SimpleBracesScoresZero)
	{
		Assert::AreEqual(uint64_t{ 0 }, aoc::SyntaxChecker::score_line("{}"s).value);
	}

	TEST_METHOD(SimpleAngleBracketScoresZero)
	{
		Assert::AreEqual(uint64_t{ 0 }, aoc::SyntaxChecker::score_line("<>"s).value);
	}

	TEST_METHOD(MixedBracketsAreHandled)
	{
		Assert::AreEqual(uint64_t{ 0 }, aoc::SyntaxChecker::score_line("<[](){<><<[]>>}>"s).value);
	}

	TEST_METHOD(MismatchRoundBracketScoresCorrectly)
	{
		Assert::AreEqual(uint64_t{ 3 }, aoc::SyntaxChecker::score_line("<[](){<><<[]>)}>"s).value);
	}

	TEST_METHOD(MismatchSquareBracketScoresCorrectly)
	{
		Assert::AreEqual(uint64_t{ 57 }, aoc::SyntaxChecker::score_line("<[](){<><<[]>>]>"s).value);
	}

	TEST_METHOD(MismatchBraceScoresCorrectly)
	{
		Assert::AreEqual(uint64_t{ 1197 }, aoc::SyntaxChecker::score_line("<[](){<><<[]>>}}"s).value);
	}

	TEST_METHOD(MismatchAngleBracketScoresCorrectly)
	{
		Assert::AreEqual(uint64_t{ 25137 }, aoc::SyntaxChecker::score_line("<[](){<><<[]>>>>"s).value);
	}

	TEST_METHOD(SyntaxScoringSampleInput)
	{
		const auto str =
			"[({(<(())[]>[[{[]{<()<>>\n"
			"[(()[<>])]({[<{<<[]>>(\n"
			"{([(<{}[<>[]}>{[]{[(<()>\n"
			"(((({<>}<{<{<>}{[]{[]{}\n"
			"[[<[([]))<([[{}[[()]]]\n"
			"[{[{({}]{}}([{[{{{}}([]\n"
			"{<[[]]>}<{[{[{[]{()[[[]\n"
			"[<(<(<(<{}))><([]([]()\n"
			"<{([([[(<>()){}]>(<<{{\n"
			"<{([{{}}[<[[[<>{}]]]>[]]";

		std::stringstream ss(str);

		Assert::AreEqual(uint64_t{ 26397 }, aoc::SyntaxChecker{}.score_lines(ss).syntax_error_score());
	}
	TEST_METHOD(AnalyseFullDataForSyntax)
	{
		std::ifstream data_file(DATA_DIR / "Day10_input.txt");
		Assert::IsTrue(data_file.is_open());

		Assert::AreEqual(uint64_t{ 374061 }, aoc::SyntaxChecker{}.score_lines(data_file).syntax_error_score());
	}

	TEST_METHOD(IncompleteLineScoringSampleInput)
	{
		const auto str =
			"[({(<(())[]>[[{[]{<()<>>\n"
			"[(()[<>])]({[<{<<[]>>(\n"
			"{([(<{}[<>[]}>{[]{[(<()>\n"
			"(((({<>}<{<{<>}{[]{[]{}\n"
			"[[<[([]))<([[{}[[()]]]\n"
			"[{[{({}]{}}([{[{{{}}([]\n"
			"{<[[]]>}<{[{[{[]{()[[[]\n"
			"[<(<(<(<{}))><([]([]()\n"
			"<{([([[(<>()){}]>(<<{{\n"
			"<{([{{}}[<[[[<>{}]]]>[]]";

		std::stringstream ss(str);
		
		auto checker = aoc::SyntaxChecker{};
		const auto score = checker.score_lines(ss).incomplete_line_score();

		Assert::AreEqual(uint64_t{ 288957 }, checker.incomplete_line_score());
	}

	TEST_METHOD(IncompleteLineScoresAreCorrectForSampleInput)
	{
		const auto str =
			"[({(<(())[]>[[{[]{<()<>>\n"
			"[(()[<>])]({[<{<<[]>>(\n"
			"{([(<{}[<>[]}>{[]{[(<()>\n"
			"(((({<>}<{<{<>}{[]{[]{}\n"
			"[[<[([]))<([[{}[[()]]]\n"
			"[{[{({}]{}}([{[{{{}}([]\n"
			"{<[[]]>}<{[{[{[]{()[[[]\n"
			"[<(<(<(<{}))><([]([]()\n"
			"<{([([[(<>()){}]>(<<{{\n"
			"<{([{{}}[<[[[<>{}]]]>[]]";

		std::stringstream ss(str);

		const auto scores = aoc::SyntaxChecker{}.score_lines(ss).incomplete_line_scores();
		const auto expected = { 294, 5566, 288957, 995444, 1480781 };

		Assert::AreEqual(size_t{ 5 }, scores.size());

		Assert::IsTrue(std::equal(expected.begin(), expected.end(), scores.begin()));
	}

	TEST_METHOD(AnalyseFullDataForIncompleteLines)
	{
		std::ifstream data_file(DATA_DIR / "Day10_input.txt");
		Assert::IsTrue(data_file.is_open());

		std::ifstream ref_file(DATA_DIR / "Day10_incompleteOpennings_2.txt");
		Assert::IsTrue(ref_file.is_open());


		auto checker = aoc::SyntaxChecker{};
		const auto score = checker.check_incomplete_lines(data_file, ref_file);
		Assert::AreEqual(true, score);

		checker.incomplete_line_score();
	}

	TEST_METHOD(AnalyseFullDataForIncompleteLineScore)
	{
		std::ifstream data_file(DATA_DIR / "Day10_input.txt");
		Assert::IsTrue(data_file.is_open());


		auto checker = aoc::SyntaxChecker{};
		const auto score = checker.score_lines(data_file).incomplete_line_score();
		Assert::AreEqual(uint64_t{ 2116639949 }, score);
	}
};
}

namespace day_11
{

TEST_CLASS(TestDay11)
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
			std::array<int, grid_size>{5,4,8,3,1,4,3,2,2,3},
			std::array<int, grid_size>{2,7,4,5,8,5,4,7,1,1},
			std::array<int, grid_size>{5,2,6,4,5,5,6,1,7,3},
			std::array<int, grid_size>{6,1,4,1,3,3,6,1,4,6},
			std::array<int, grid_size>{6,3,5,7,3,8,5,4,7,8},
			std::array<int, grid_size>{4,1,6,7,5,2,4,6,4,5},
			std::array<int, grid_size>{2,1,7,6,8,4,1,7,2,1},
			std::array<int, grid_size>{6,8,8,2,8,8,1,1,3,4},
			std::array<int, grid_size>{4,8,4,6,8,4,8,5,5,4},
			std::array<int, grid_size>{5,2,8,3,7,5,1,5,2,6}
		};

		auto model = aoc::DumboOctopusModel<grid_size>{ initial_state };

		const auto flashes = model.step(2);

		Assert::AreEqual(35, flashes);

		constexpr auto expected_state = std::array<std::array<int, grid_size>, grid_size>{
			std::array<int, grid_size>{8,8,0,7,4,7,6,5,5,5},
			std::array<int, grid_size>{5,0,8,9,0,8,7,0,5,4},
			std::array<int, grid_size>{8,5,9,7,8,8,9,6,0,8},
			std::array<int, grid_size>{8,4,8,5,7,6,9,6,0,0},
			std::array<int, grid_size>{8,7,0,0,9,0,8,8,0,0},
			std::array<int, grid_size>{6,6,0,0,0,8,8,9,8,9},
			std::array<int, grid_size>{6,8,0,0,0,0,5,9,4,3},
			std::array<int, grid_size>{0,0,0,0,0,0,7,4,5,6},
			std::array<int, grid_size>{9,0,0,0,0,0,0,8,7,6},
			std::array<int, grid_size>{8,7,0,0,0,0,6,8,4,8}
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
			std::array<int, grid_size>{5,4,8,3,1,4,3,2,2,3},
			std::array<int, grid_size>{2,7,4,5,8,5,4,7,1,1},
			std::array<int, grid_size>{5,2,6,4,5,5,6,1,7,3},
			std::array<int, grid_size>{6,1,4,1,3,3,6,1,4,6},
			std::array<int, grid_size>{6,3,5,7,3,8,5,4,7,8},
			std::array<int, grid_size>{4,1,6,7,5,2,4,6,4,5},
			std::array<int, grid_size>{2,1,7,6,8,4,1,7,2,1},
			std::array<int, grid_size>{6,8,8,2,8,8,1,1,3,4},
			std::array<int, grid_size>{4,8,4,6,8,4,8,5,5,4},
			std::array<int, grid_size>{5,2,8,3,7,5,1,5,2,6}
		};

		auto model = aoc::DumboOctopusModel<grid_size>{ initial_state };

		const auto flashes = model.step(10);

		Assert::AreEqual(204, flashes);

		constexpr auto expected_state = std::array<std::array<int, grid_size>, grid_size>{
			std::array<int, grid_size>{0,4,8,1,1,1,2,9,7,6},
			std::array<int, grid_size>{0,0,3,1,1,1,2,0,0,9},
			std::array<int, grid_size>{0,0,4,1,1,1,2,5,0,4},
			std::array<int, grid_size>{0,0,8,1,1,1,1,4,0,6},
			std::array<int, grid_size>{0,0,9,9,1,1,1,3,0,6},
			std::array<int, grid_size>{0,0,9,3,5,1,1,2,3,3},
			std::array<int, grid_size>{0,4,4,2,3,6,1,1,3,0},
			std::array<int, grid_size>{5,5,3,2,2,5,2,3,5,0},
			std::array<int, grid_size>{0,5,3,2,2,5,0,6,0,0},
			std::array<int, grid_size>{0,0,3,2,2,4,0,0,0,0}
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
			std::array<int, grid_size>{5,4,8,3,1,4,3,2,2,3},
			std::array<int, grid_size>{2,7,4,5,8,5,4,7,1,1},
			std::array<int, grid_size>{5,2,6,4,5,5,6,1,7,3},
			std::array<int, grid_size>{6,1,4,1,3,3,6,1,4,6},
			std::array<int, grid_size>{6,3,5,7,3,8,5,4,7,8},
			std::array<int, grid_size>{4,1,6,7,5,2,4,6,4,5},
			std::array<int, grid_size>{2,1,7,6,8,4,1,7,2,1},
			std::array<int, grid_size>{6,8,8,2,8,8,1,1,3,4},
			std::array<int, grid_size>{4,8,4,6,8,4,8,5,5,4},
			std::array<int, grid_size>{5,2,8,3,7,5,1,5,2,6}
		};

		auto model = aoc::DumboOctopusModel<grid_size>{ initial_state };

		const auto flashes = model.step(100);

		Assert::AreEqual(1656, flashes);

		constexpr auto expected_state = std::array<std::array<int, grid_size>, grid_size>{
			std::array<int, grid_size>{0,3,9,7,6,6,6,8,6,6},
			std::array<int, grid_size>{0,7,4,9,7,6,6,9,1,8},
			std::array<int, grid_size>{0,0,5,3,9,7,6,9,3,3},
			std::array<int, grid_size>{0,0,0,4,2,9,7,8,2,2},
			std::array<int, grid_size>{0,0,0,4,2,2,9,8,9,2},
			std::array<int, grid_size>{0,0,5,3,2,2,2,8,7,7},
			std::array<int, grid_size>{0,5,3,2,2,2,2,9,6,6},
			std::array<int, grid_size>{9,3,2,2,2,2,8,9,6,6},
			std::array<int, grid_size>{7,9,2,2,2,8,6,8,6,6},
			std::array<int, grid_size>{6,7,8,9,9,9,8,7,6,6}
		};

		for (auto r = 0; r < grid_size; ++r) {
			for (auto c = 0; c < grid_size; ++c) {
				Assert::AreEqual(expected_state[r][c], model.state().at(r, c), std::format(L"Failed to match r = {}, c = {}", r, c).c_str());
			}
		}
	}

	TEST_METHOD(FullInputWorks)
	{
		std::ifstream data_file(DATA_DIR / "Day11_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto flashes = aoc::DumboOctopusModel<10>{}.load(data_file).step(100);

		Assert::AreEqual(1617, flashes);
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

	TEST_METHOD(FindFirstSyncStepOnFullInput)
	{
		std::ifstream data_file(DATA_DIR / "Day11_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto first_sync_step = aoc::DumboOctopusModel<10>{}.load(data_file).find_first_sync_step();

		Assert::AreEqual(258, first_sync_step);
	}
};

}

namespace day_12
{
TEST_CLASS(TestDay12)
{
public:

	TEST_METHOD(CaveBuilderCanBuildTunnelsTrivial_1)
	{
		auto cave_map = aoc::CaveMap_t{};
		auto builder = aoc::CaveMapBuilder{ cave_map };
		
		auto tunnels = std::vector<aoc::Tunnel_t>{ {"start", "end"} };

		builder.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin()));

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 1 }, boost::num_edges(cave_map.graph()));
	}

	TEST_METHOD(CaveBuilderCanBuildTunnelsTrivial_2)
	{
		auto cave_map = aoc::CaveMap_t{};
		auto builder = aoc::CaveMapBuilder{ cave_map };

		auto tunnels = std::vector<aoc::Tunnel_t>{ {"end", "start"} };

		builder.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin()));

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 1 }, boost::num_edges(cave_map.graph()));
	}

	TEST_METHOD(CaveBuilderCanBuildTunnelsSingleIntermediate)
	{
		auto cave_map = aoc::CaveMap_t{};

		auto tunnels = std::vector<aoc::Tunnel_t>{ {"a", "start"}, {"a", "end"} };

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin()))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin()), tunnels.end());

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "a" }, cave_map["a"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 2 }, boost::num_edges(cave_map.graph()));
	}


	TEST_METHOD(CaveBuilderCanBuildTunnelsTwoIntermediates)
	{
		auto cave_map = aoc::CaveMap_t{};

		auto tunnels = std::vector<aoc::Tunnel_t>{
			{"a", "start"},
			{"start", "b"},
			{"a", "end"},
			{"b", "end"}
		};

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin(), tunnels.size() / 2))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin(), tunnels.size() / 2), tunnels.end());

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "a" }, cave_map["a"]);
		Assert::AreEqual(aoc::Cave_t{ "b" }, cave_map["b"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 4 }, boost::num_edges(cave_map.graph()));
	}


	TEST_METHOD(CaveBuilderCanBuildTunnelsFiveIntermediates)
	{
		auto cave_map = aoc::CaveMap_t{};

		auto tunnels = std::vector<aoc::Tunnel_t>{
			{"a", "start"},
			{"start", "b"},
			{"start", "c"},
			{"start", "d"},
			{"start", "e"},
			{"a", "end"},
			{"b", "end"},
			{"c", "end"},
			{"d", "end"},
			{"e", "end"}
		};

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin(), tunnels.size() / 2))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin(), tunnels.size() / 2), tunnels.end());

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "a" }, cave_map["a"]);
		Assert::AreEqual(aoc::Cave_t{ "b" }, cave_map["b"]);
		Assert::AreEqual(aoc::Cave_t{ "c" }, cave_map["c"]);
		Assert::AreEqual(aoc::Cave_t{ "d" }, cave_map["d"]);
		Assert::AreEqual(aoc::Cave_t{ "e" }, cave_map["e"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 10 }, boost::num_edges(cave_map.graph()));
	}


	TEST_METHOD(CaveBuilderCanBuildTunnelsTwoConnectedIntermediates)
	{
		auto cave_map = aoc::CaveMap_t{};

		auto tunnels = std::vector<aoc::Tunnel_t>{
			{"a", "start"},
			{"start", "b"},
			{"a", "b"},
			{"a", "end"},
			{"b", "end"}
		};

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin(), 2))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin(), 3), tunnels.end())
			.add_non_terminal_tunnels(std::next(tunnels.begin(), 2), std::next(tunnels.begin(), 3));

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "a" }, cave_map["a"]);
		Assert::AreEqual(aoc::Cave_t{ "b" }, cave_map["b"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 6 }, boost::num_edges(cave_map.graph()));
	}

	TEST_METHOD(CaveBuilderCanBuildExample)
	{
		auto tunnels = std::vector<aoc::Tunnel_t>{
			{"start", "A"},
			{"start", "b"},
			{"A", "c"},
			{"A", "b"},
			{"b", "d"},
			{"A", "end"},
			{"b", "end"}
		};

		auto cave_map = aoc::CaveMap_t{};

		aoc::CaveMapBuilder{ cave_map }
			.handle_terminal_cave<aoc::TerminalCaveType::start>(tunnels.begin(), std::next(tunnels.begin(), 2))
			.handle_terminal_cave<aoc::TerminalCaveType::end>(std::next(tunnels.begin(), 5), tunnels.end())
			.add_non_terminal_tunnels(std::next(tunnels.begin(), 2), std::next(tunnels.begin(), 5));

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "A" }, cave_map["A"]);
		Assert::AreEqual(aoc::Cave_t{ "b" }, cave_map["b"]);
		Assert::AreEqual(aoc::Cave_t{ "c" }, cave_map["c"]);
		Assert::AreEqual(aoc::Cave_t{ "d" }, cave_map["d"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 10 }, boost::num_edges(cave_map.graph()));
	}

	TEST_METHOD(TerminalCaveTypeIsCorrect)
	{
		Assert::IsTrue(aoc::TerminalCaveType::start == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{"start", "a"}));
		Assert::IsTrue(aoc::TerminalCaveType::start == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "a", "start" }));

		Assert::IsTrue(aoc::TerminalCaveType::end == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "end", "a" }));
		Assert::IsTrue(aoc::TerminalCaveType::end == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "a", "end" }));

		Assert::IsTrue((aoc::TerminalCaveType::end | aoc::TerminalCaveType::start) == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "start", "end" }));
		Assert::IsTrue((aoc::TerminalCaveType::end | aoc::TerminalCaveType::start) == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "end", "start" }));

		Assert::IsTrue(aoc::TerminalCaveType::none == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "b", "a" }));
		Assert::IsTrue(aoc::TerminalCaveType::none == aoc::TerminalCaveType::to_type(aoc::Tunnel_t{ "a", "b" }));
	}

	TEST_METHOD(PartitioningTunnelsWorks)
	{
		auto tunnels = std::vector<aoc::Tunnel_t>{
			{"pf", "pk"},
			{"ZQ", "iz"},
			{"iz", "NY"},
			{"ZQ", "end"},
			{"pf", "gx"},
			{"pk", "ZQ"},
			{"ZQ", "dc"},
			{"NY", "start"},
			{"NY", "pf"},
			{"NY", "gx"},
			{"ag", "ZQ"},
			{"pf", "start"},
			{"start", "gx"},
			{"BN", "ag"},
			{"iz", "pf"},
			{"ag", "FD"},
			{"pk", "NY"},
			{"gx", "pk"},
			{"end", "BN"},
			{"ag", "pf"},
			{"iz", "pk"},
			{"pk", "ag"},
			{"iz", "end"},
			{"iz", "BN"}
		};

		const auto partitions = aoc::CaveMapBuilder::partition_tunnels(tunnels);

		Assert::IsTrue(tunnels.begin() == partitions.starts.begin());
		Assert::IsTrue(std::next(tunnels.begin(), 3) == partitions.starts.end());

		for (const auto& t : partitions.starts) {
			Assert::IsTrue(aoc::TerminalCaveType::start == aoc::TerminalCaveType::to_type(t));
		}

		Assert::IsTrue(std::next(tunnels.begin(), 3) == partitions.ends.begin());
		Assert::IsTrue(std::next(tunnels.begin(), 6) == partitions.ends.end());

		for (const auto& t : partitions.ends) {
			Assert::IsTrue(aoc::TerminalCaveType::end == aoc::TerminalCaveType::to_type(t));
		}

		Assert::IsTrue(std::next(tunnels.begin(), 6) == partitions.others.begin());
		Assert::IsTrue(tunnels.end() == partitions.others.end());

		for (const auto& t : partitions.others) {
			Assert::IsTrue(aoc::TerminalCaveType::none == aoc::TerminalCaveType::to_type(t));
		}
	}

	TEST_METHOD(CavesCanBeLoadedFromStream)
	{
		constexpr auto data_str =
			"start-A\n"
			"start-b\n"
			"A-c\n"
			"A-b\n"
			"b-d\n"
			"A-end\n"
			"b-end"
			;

		std::stringstream data(data_str);

		const auto cave_map = aoc::CaveNavigator{}.load(data).cave_map();

		Assert::AreEqual(aoc::Cave_t{ "start" }, cave_map["start"]);
		Assert::AreEqual(aoc::Cave_t{ "A" }, cave_map["A"]);
		Assert::AreEqual(aoc::Cave_t{ "b" }, cave_map["b"]);
		Assert::AreEqual(aoc::Cave_t{ "c" }, cave_map["c"]);
		Assert::AreEqual(aoc::Cave_t{ "d" }, cave_map["d"]);
		Assert::AreEqual(aoc::Cave_t{ "end" }, cave_map["end"]);

		Assert::AreEqual(size_t{ 10 }, boost::num_edges(cave_map.graph()));
	}
};
}
