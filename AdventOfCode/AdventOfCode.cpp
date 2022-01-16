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
#include "PaperFolder.hpp"
#include "Polymerizer.hpp"
#include "CavernPathFinder.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

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

		// Day 12
		{
			Logger::WriteMessage("Day 12:\n");

			// Part 1
			{
				std::ifstream data_file(DATA_DIR / "Day12_input.txt");
				Assert::IsTrue(data_file.is_open());

				auto caves = aoc::navigation::CaveLoader::load(data_file);
				auto routes = aoc::navigation::CaveRoutes{ caves };
				auto route_count = std::accumulate(routes.begin(), routes.end(), uint32_t{ 0 }, [](auto curr, auto _) { return ++curr; });

				Logger::WriteMessage(std::format("\tNumber of routes: {}\n", route_count).c_str());
			}

			// Part 2
			{
#ifndef _DEBUG
				std::ifstream data_file(DATA_DIR / "Day12_input.txt");
				Assert::IsTrue(data_file.is_open());

				auto caves = aoc::navigation::CaveLoader::load(data_file);
				auto routes = aoc::navigation::CaveRoutes{ caves };
				auto route_count = aoc::navigation::CaveRevisitor{ caves }.routes().size();

				Logger::WriteMessage(std::format("\tNumber of routes with single double-visits: {}\n", route_count).c_str());
#else
				Logger::WriteMessage("\tDay 12 part 2 is prohibitively slow in DEBUG. Run in RELEASE to run this example\n");
#endif
			}
		}

		// Day 13
		{
			Logger::WriteMessage("Day 13:\n");

			// Part 1
			{
				std::ifstream data_file(DATA_DIR / "Day13_input.txt");
				Assert::IsTrue(data_file.is_open());

				auto paper = std::move(aoc::Paper{}.load(data_file));
				auto folds = aoc::FoldSequence{}.load(data_file);

				auto mark_count = aoc::PaperFolder::apply_fold(std::move(paper), folds.front()).mark_count();

				Logger::WriteMessage(std::format("\tNumber of marks after first fold: {}\n", mark_count).c_str());
			}

			// Part 2
			{
				std::ifstream data_file(DATA_DIR / "Day13_input.txt");
				Assert::IsTrue(data_file.is_open());

				auto paper = std::move(aoc::Paper{}.load(data_file));
				auto folds = aoc::FoldSequence{}.load(data_file);

				const auto code = aoc::PaperReader<6, 5>::decode(aoc::PaperFolder{ std::move(paper) }.apply(folds).as_matrix());

				Logger::WriteMessage(std::format("\tThe secret code is: {}\n", code).c_str());
			}
		}

		// Day 14
		{
			Logger::WriteMessage("Day 14:\n");

			// Part 1
			{
				std::ifstream data_file(DATA_DIR / "Day14_input.txt");
				Assert::IsTrue(data_file.is_open());

				auto polymer = aoc::polymer::Polymer::from_stream(data_file);
				const auto rules = aoc::polymer::InsertionRuleLoader::from_stream(data_file);

				const auto score = polymer.polymerize(10, rules).score();

				Logger::WriteMessage(std::format("\tPolymerization score after 10 cycles: {}\n", score).c_str());
			}

			// Part 2
			{
				std::ifstream data_file(DATA_DIR / "Day14_input.txt");
				Assert::IsTrue(data_file.is_open());

				auto polymer = aoc::polymer::Polymer::from_stream(data_file);
				const auto rules = aoc::polymer::InsertionRuleLoader::from_stream(data_file);

				const auto score = polymer.polymerize(40, rules).score();

				Logger::WriteMessage(std::format("\tPolymerization score after 40 cycles: {}\n", score).c_str());
			}
		}

		// Day 15
		{
			Logger::WriteMessage("Day 15:\n");

			// Part 1
			{
				std::ifstream data_file(DATA_DIR / "Day15_input.txt");
				Assert::IsTrue(data_file.is_open());

				const auto cavern = aoc::navigation::Cavern{ data_file };
				const auto score = aoc::navigation::CavernPathFinder{}.plot_course(cavern.risk_grid()).score();

				Logger::WriteMessage(std::format("\tOptimal path score: {}\n", score).c_str());
			}

			// Part 2
			{
				
			}
		}
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

	TEST_METHOD(FullInputWorks)
	{
		std::ifstream data_file(DATA_DIR / "Day11_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto flashes = aoc::DumboOctopusModel<10>{}.load(data_file).step(100);

		Assert::AreEqual(1617, flashes);
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
	TEST_METHOD(Part1)
	{
		std::ifstream data_file(DATA_DIR / "Day12_input.txt");
		Assert::IsTrue(data_file.is_open());

		auto caves = aoc::navigation::CaveLoader::load(data_file);
		auto routes = aoc::navigation::CaveRoutes{ caves };
		auto route_count = std::accumulate(routes.begin(), routes.end(), uint32_t{ 0 }, [](auto curr, auto _) { return ++curr; });

		Assert::AreEqual(uint32_t{ 5212 }, route_count);
	}

	TEST_METHOD(Part2)
	{
	#ifndef _DEBUG
		std::ifstream data_file(DATA_DIR / "Day12_input.txt");
		Assert::IsTrue(data_file.is_open());

		auto caves = aoc::navigation::CaveLoader::load(data_file);
		auto routes = aoc::navigation::CaveRoutes{ caves };
		auto route_count = aoc::navigation::CaveRevisitor{ caves }.routes().size();

		Assert::AreEqual(size_t{ 134862 }, route_count);
	#else
		Logger::WriteMessage("\tDay 12 part 2 is prohibitively slow in DEBUG. Run in RELEASE to run this example\n");
	#endif
	}
};
}

namespace day_13
{

TEST_CLASS(TestDay13)
{
public:
	TEST_METHOD(Part1)
	{
		std::ifstream data_file(DATA_DIR / "Day13_input.txt");
		Assert::IsTrue(data_file.is_open());

		auto paper = std::move(aoc::Paper{}.load(data_file));
		auto folds = aoc::FoldSequence{}.load(data_file);
		auto mark_count = aoc::PaperFolder::apply_fold(std::move(paper), folds.front()).mark_count();
			
		Assert::AreEqual(size_t{ 655 }, mark_count);
	}

	TEST_METHOD(Part2)
	{
		std::ifstream data_file(DATA_DIR / "Day13_input.txt");
		Assert::IsTrue(data_file.is_open());

		auto paper = std::move(aoc::Paper{}.load(data_file));
		auto folds = aoc::FoldSequence{}.load(data_file);

		const auto code = aoc::PaperReader<6, 5>::decode(aoc::PaperFolder{ std::move(paper) }.apply(folds).as_matrix());

		Assert::AreEqual("JPZCUAUR"s, code);
	}
};

}

namespace day_14
{

TEST_CLASS(TestDay14)
{
public:
	TEST_METHOD(Part1)
	{
		std::ifstream data_file(DATA_DIR / "Day14_input.txt");
		Assert::IsTrue(data_file.is_open());

		auto polymer = aoc::polymer::Polymer::from_stream(data_file);
		const auto rules = aoc::polymer::InsertionRuleLoader::from_stream(data_file);

		const auto score = polymer.polymerize(10, rules).score();

		Assert::AreEqual(uint64_t{ 2549 }, score);
	}

	TEST_METHOD(Part2)
	{
		std::ifstream data_file(DATA_DIR / "Day14_input.txt");
		Assert::IsTrue(data_file.is_open());

		auto polymer = aoc::polymer::Polymer::from_stream(data_file);
		const auto rules = aoc::polymer::InsertionRuleLoader::from_stream(data_file);

		const auto score = polymer.polymerize(40, rules).score();

		Assert::AreEqual(uint64_t{ 2516901104210 }, score);
	}
};

}

namespace day_15
{

TEST_CLASS(TestDay15)
{
public:
	TEST_METHOD(Part1)
	{
		std::ifstream data_file(DATA_DIR / "Day15_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto cavern = aoc::navigation::Cavern{ data_file };
		const auto score = aoc::navigation::CavernPathFinder{}.plot_course(cavern.risk_grid()).score();

		Assert::AreEqual(size_t{ 811 }, score);
	}

	TEST_METHOD(Part2)
	{
		std::ifstream data_file(DATA_DIR / "Day15_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto score = aoc::navigation::CavernPathFinder{}.plot_course(aoc::navigation::Cavern{ data_file }.expand(5).risk_grid()).score();

		Assert::AreEqual(size_t{ 3012 }, score);
	}

	TEST_METHOD(Part2_mike)
	{
		std::ifstream data_file(DATA_DIR / "Day15_input_mike.txt");
		Assert::IsTrue(data_file.is_open());

		const auto score = aoc::navigation::CavernPathFinder{}.plot_course(aoc::navigation::Cavern{ data_file }.expand(5).risk_grid()).score();

		Assert::AreEqual(size_t{ 2835 }, score);
	}

	TEST_METHOD(Part2_compareExpanded)
	{
		std::ifstream data_file(DATA_DIR / "Day15_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto expanded_risks = aoc::navigation::Cavern{ data_file }.expand(5).risk_grid();

		std::ifstream ref_data_file(DATA_DIR / "Day15_expanded_cave_mike.txt");
		Assert::IsTrue(ref_data_file.is_open());

		const auto reference_risks = aoc::navigation::Cavern{ ref_data_file }.risk_grid();

		Assert::AreEqual(reference_risks.n_rows, expanded_risks.n_rows);
		Assert::AreEqual(reference_risks.n_cols, expanded_risks.n_cols);

		for (auto r = 0u; r < expanded_risks.n_rows; ++r) {
			for (auto c = 0u; c < expanded_risks.n_cols; ++c) {
				Assert::AreEqual(reference_risks.at(r, c), expanded_risks.at(r, c), std::format(L"Mismatch at ({}, {})", r, c).c_str());
			}
		}
	}

	TEST_METHOD(ExpandedGraphIsCorrect)
	{
		std::ifstream data_file(DATA_DIR / "Day15_input.txt");
		Assert::IsTrue(data_file.is_open());

		const auto risks = aoc::navigation::Cavern{ data_file }.expand(5).risk_grid();
		auto path_finder = aoc::navigation::CavernPathFinder{};
		const auto graph = path_finder.build_graph(risks);

		Assert::AreEqual(risks.n_elem, boost::num_vertices(graph));

		for (auto [vertex, end] = boost::vertices(graph); vertex != end; ++vertex) {
			const auto location = path_finder.cavern_location_from_vertex(*vertex);

			// Check location to the right, if this is not in the rightmost column
			if ( location.x < risks.n_cols - 1) {
				const auto dest = path_finder.vertex_from_cavern_location({ location.x + 1, location.y});
				const auto edge_descriptor = boost::edge(*vertex, dest, graph);
				const auto weight = boost::get(boost::edge_weight, graph, edge_descriptor.first);

				Assert::AreEqual(risks.at(location.y, location.x + 1), weight,
					std::format(L"Weight mismatch on edge ({},{}) --{}-> ({}, {})",
						location.x, location.y, weight, location.x + 1, location.y).c_str());
			}

			// Check location underneath, if this is not in the bottom row
			if (location.y < risks.n_rows - 1) {
				const auto dest = path_finder.vertex_from_cavern_location({ location.x, location.y + 1 });
				const auto edge_descriptor = boost::edge(*vertex, dest, graph);
				const auto weight = boost::get(boost::edge_weight, graph, edge_descriptor.first);

				Assert::AreEqual(risks.at(location.y + 1, location.x), weight,
					std::format(L"Weight mismatch on edge ({},{}) --{}-> ({}, {})",
						location.x, location.y, weight, location.x, location.y + 1).c_str());
			}
		}
	}
};

}
