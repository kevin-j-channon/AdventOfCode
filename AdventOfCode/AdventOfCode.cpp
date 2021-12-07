#include "Common.hpp"
#include "DiagnosticLog.hpp"
#include "BoatSystems.hpp"
#include "AdventOfCode.hpp"

#include "CppUnitTest.h"

#include <vector>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <format>
#include <string>
#include <filesystem>
#include <algorithm>

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

			auto depth_score = aoc::Submarine().boat_systems().depth_score<1>(measurements.begin(), measurements.end());

			Assert::AreEqual(uint32_t{ 4 }, depth_score);
		}

		TEST_METHOD(DepthScoreVectorWithVariousJumps)
		{
			auto measurements = std::vector<uint32_t>{ 1, 22, 3, 200, 1000, 2, 3, 4, 100 };

			auto depth_score = aoc::Submarine().boat_systems().depth_score<1>(measurements.begin(), measurements.end());

			Assert::AreEqual(uint32_t{ 6 }, depth_score);
		}

		TEST_METHOD(ExampleValues)
		{
			auto measurements = std::vector<uint32_t>{ 199, 200, 208, 210, 200, 207, 240, 269, 260, 263 };

			auto depth_score = aoc::Submarine().boat_systems().depth_score<1>(measurements.begin(), measurements.end());

			Assert::AreEqual(uint32_t{ 7 }, depth_score);
		}

		TEST_METHOD(CountNetIncreaseFromFile)
		{
			std::ifstream data_file(DATA_DIR / "Day1_input.txt");
			Assert::IsTrue(data_file.is_open());

			auto depth_score = aoc::Submarine().boat_systems().depth_score<1>(std::istream_iterator<uint32_t>(data_file), std::istream_iterator<uint32_t>());

			Assert::AreEqual(uint32_t{ 1502 }, depth_score);
		}
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

	TEST_CLASS(TestDay2)
	{
	public:

		TEST_METHOD(VectorsCanBeAdded)
		{
			const auto v1 = aoc::Vec2d<int>{ 1, 2 };
			const auto v2 = aoc::Vec2d<int>{ 3, 4 };

			const auto v3 = v1 + v2;

			Assert::AreEqual(4, v3.x);
			Assert::AreEqual(6, v3.y);
		}

		TEST_METHOD(InvalidInputDirectionThrows)
		{
			Assert::ExpectException<aoc::Exception>([]() {
				std::stringstream ss("wibble 5"s);
				auto d = aoc::Direction{};
				ss >> d;
				});
		}

		TEST_METHOD(InvalidInputNumberThrows)
		{
			Assert::ExpectException<aoc::Exception>([]() {
				std::stringstream ss("forward xx"s);
				auto d = aoc::Direction{};
				ss >> d; });
		}

		TEST_METHOD(CreateDirectionFromInputRow_forward)
		{
			std::stringstream ss("forward 9");
			auto direction = aoc::Direction{};

			ss >> direction;

			Assert::AreEqual(9, direction.x);
			Assert::AreEqual(0, direction.y);
		}

		TEST_METHOD(CreateDirectionFromInputRow_up)
		{
			std::stringstream ss("up 5"s);
			auto direction = aoc::Direction{};

			ss >> direction;

			Assert::AreEqual(0, direction.x);
			Assert::AreEqual(-5, direction.y);
		}

		TEST_METHOD(CreateDirectionFromInputRow_down)
		{
			std::stringstream ss("down 30"s);
			auto direction = aoc::Direction{};

			ss >> direction;

			Assert::AreEqual(0, direction.x);
			Assert::AreEqual(30, direction.y);
		}

		TEST_METHOD(ParseDirectionsFromStream)
		{
			std::stringstream ss("forward 3\ndown 3\nforward 12\ndown 5\nup 2");

			using StreamIter_t = std::istream_iterator<aoc::Direction>;
			const auto net_direction = aoc::Submarine().boat_systems().net_direction(StreamIter_t(ss), StreamIter_t());

			Assert::AreEqual(15, net_direction.x);
			Assert::AreEqual( 6, net_direction.y);
		}

		TEST_METHOD(ParseDirectionsFromFile)
		{
			std::ifstream data_file(DATA_DIR / "Day2_input.txt");
			Assert::IsTrue(data_file.is_open());

			using StreamIter_t = std::istream_iterator<aoc::Direction>;
			const auto net_direction = aoc::Submarine().boat_systems().net_direction(StreamIter_t(data_file), StreamIter_t());

			Assert::AreEqual(1895, net_direction.x);
			Assert::AreEqual(894, net_direction.y);
		}

		TEST_METHOD(DirectionsCanBeAddedAimings)
		{
			auto aiming = aoc::Aiming{};
			
			aiming = aiming + aoc::Direction{ 5, 0 };

			Assert::AreEqual(5, aiming.x);
			Assert::AreEqual(0, aiming.aim);
			Assert::AreEqual(0, aiming.depth);

			aiming = aiming + aoc::Direction{ 0, 5 };

			Assert::AreEqual(5, aiming.x);
			Assert::AreEqual(5, aiming.aim);
			Assert::AreEqual(0, aiming.depth);

			aiming = aiming + aoc::Direction{ 8, 0 };

			Assert::AreEqual(13, aiming.x);
			Assert::AreEqual(5, aiming.aim);
			Assert::AreEqual(40, aiming.depth);

			aiming = aiming + aoc::Direction{ 0, -3 };

			Assert::AreEqual(13, aiming.x);
			Assert::AreEqual(2, aiming.aim);
			Assert::AreEqual(40, aiming.depth);

			aiming = aiming + aoc::Direction{ 0, 8 };

			Assert::AreEqual(13, aiming.x);
			Assert::AreEqual(10, aiming.aim);
			Assert::AreEqual(40, aiming.depth);

			aiming = aiming + aoc::Direction{ 2, 0 };

			Assert::AreEqual(15, aiming.x);
			Assert::AreEqual(10, aiming.aim);
			Assert::AreEqual(60, aiming.depth);
		}

		TEST_METHOD(ConvertingAimingToDirectionWorks)
		{
			const auto d = aoc::Aiming{ 1, 2, 3 }.to_direction();

			Assert::AreEqual(1, d.x);
			Assert::AreEqual(3, d.y);
		}

		TEST_METHOD(AimingFromStreamWorks)
		{
			std::stringstream ss("forward 5\ndown 5\nforward 8\nup 3\ndown 8\nforward 2");

			using StreamIter_t = std::istream_iterator<aoc::Direction>;
			const auto net_aim = aoc::Submarine().boat_systems().net_aiming(StreamIter_t(ss), StreamIter_t());

			Assert::AreEqual(15, net_aim.x);
			Assert::AreEqual(60, net_aim.y);
		}
	};

	TEST_CLASS(TestDay3)
	{
	public:
		TEST_METHOD(ReadLogEntriedFromStream)
		{
			std::stringstream ss("111011110101");
			auto log_entry = aoc::DiagnosticLog::Entry_t{};

			ss >> log_entry;

			auto a1 = aoc::DiagnosticLog::Entry_t{ 1,1,1,0,1,1,1,1,0,1,0,1  };
			for (auto i = 0; i < a1.size(); ++i)
			{
				Assert::AreEqual(a1[i], log_entry[i]);
			}
		}

		TEST_METHOD(ReadMultiplePowerParamsFromStream)
		{
			std::stringstream ss("111011110101\n011000111010");
			auto v = std::vector<aoc::DiagnosticLog::Entry_t>{};

			using Iter_t = std::istream_iterator<aoc::DiagnosticLog::Entry_t>;

			std::copy(Iter_t(ss), Iter_t(), std::back_inserter(v));

			Assert::AreEqual(size_t{ 2 }, v.size());

			const auto entry_1 = aoc::DiagnosticLog::Entry_t{ 1,1,1,0,1,1,1,1,0,1,0,1  };
			for (auto i = 0; i < entry_1.size(); ++i)
			{
				Assert::AreEqual(entry_1[i], v[0][i]);
			}

			const auto Entry_2 = aoc::DiagnosticLog::Entry_t{ 0,1,1,0,0,0,1,1,1,0,1,0 };
			for (auto i = 0; i < Entry_2.size(); ++i)
			{
				Assert::AreEqual(Entry_2[i], v[1][i]);
			}
		}

		TEST_METHOD(DiagnosticLogEntryAsWorks)
		{
			Assert::AreEqual(uint32_t{ 0b111000110010 },
				aoc::DiagnosticLog::entry_as<uint32_t>({ 1,1,1,0,0,0,1,1,0,0,1,0 }));
		}

		TEST_METHOD(DiagnosticLogFlippedEntryAsWorks)
		{
			Assert::AreEqual(uint32_t{ 0b000111001101 },
				aoc::DiagnosticLog::flipped_entry_as<uint32_t>({ 1,1,1,0,0,0,1,1,0,0,1,0 }));
		}

		TEST_METHOD(CalculatePowerParamsFromFile)
		{
			std::ifstream data_file(DATA_DIR / "Day3_input.txt");
			Assert::IsTrue(data_file.is_open());

			auto log = aoc::DiagnosticLog{data_file};

			const auto power_consumption = aoc::Submarine().boat_systems().power_consumption(log);

			Logger::WriteMessage(std::format("Power: {}", power_consumption).c_str());
			Assert::AreEqual(uint32_t{ 693486 }, power_consumption);
		}

		TEST_METHOD(BadDiagnosticLogEntryCausesException)
		{
			// First entry has too many bits in it.
			std::stringstream ss("1110111101011\n011000111010\n100000010010");
			auto log = aoc::DiagnosticLog{};

			Assert::ExpectException<aoc::Exception>([&]() { log.load(ss); });
		}

		TEST_METHOD(BadDiagnosticLogEntrySetsStreamFailbit)
		{
			// First entry has too many bits in it.
			std::stringstream ss("1110111101011\n011000111010\n100000010010");
			auto log = aoc::DiagnosticLog{};

			try { log.load(ss); }
			catch (const aoc::Exception&) {}

			Assert::IsTrue(ss.fail());
		}

		TEST_METHOD(DiagnosticLogBeginAndEndAreCorrect)
		{
			std::stringstream ss("111011110101\n011000111010\n100000010010");
			auto log = aoc::DiagnosticLog{ss};

			Assert::AreEqual(ptrdiff_t{ 3 }, std::distance(log.begin(), log.end()));
		}

		TEST_METHOD(InvalidCharacterInLogCausesException)
		{
			// Second entry has an invalid character in it.
			std::stringstream ss("111011110101\n011000121010\n100000010010");
			auto log = aoc::DiagnosticLog{};

			Assert::ExpectException<aoc::Exception>([&](){ log.load(ss); });
		}

		TEST_METHOD(DiagnosticLogMostFrequentBitsWorks)
		{
			std::stringstream ss("111011110101\n"
				                 "011000111010\n"
				                 "100000010010");

			const auto log = aoc::DiagnosticLog{ ss };

			const auto most_frequent_bits = log.get_most_frequent_bits();
			const auto expected = aoc::DiagnosticLog::Entry_t{ 1,1,1,0,0,0,1,1,0,0,1,0 };

			Assert::IsTrue(std::equal(expected.begin(), expected.end(), most_frequent_bits.begin()));
		}

		TEST_METHOD(DiagnosticLogLeastFrequentBitsWorks)
		{
			std::stringstream ss("111011110101\n"
				"011000111010\n"
				"100000010010");

			const auto log = aoc::DiagnosticLog{ ss };

			const auto least_frequent_bits = log.get_least_frequent_bits();
			const auto expected = aoc::DiagnosticLog::Entry_t{ 0,0,0,1,1,1,0,0,1,1,0,1 };

			Assert::IsTrue(std::equal(expected.begin(), expected.end(), least_frequent_bits.begin()));
		}

		TEST_METHOD(LifeSupportFilterBitsWorks)
		{
			const auto target_entry = aoc::DiagnosticLog::Entry_t{ 0,1,1,0,1,0,0,1,0,1,1,0 };

			constexpr auto log_lines =
				"111011110101\n"
				"011000111010\n"
				"100000010010";

			std::stringstream ss(log_lines);
			const auto best_match = aoc::LifeSupport(aoc::DiagnosticLog{ ss }).filter_using_most_frequent_bits();

			Assert::AreEqual(aoc::DiagnosticLog::entry_as<uint32_t>({ 1,1,1,0,1,1,1,1,0,1,0,1 }), best_match);
		}

		TEST_METHOD(FilterBitsForMostCommonOnExampleData)
		{
			constexpr auto log_lines =
				"000000000100\n"
				"000000011110\n"
				"000000010110\n"
				"000000010111\n"
				"000000010101\n"
				"000000001111\n"
				"000000000111\n"
				"000000011100\n"
				"000000010000\n"
				"000000011001\n"
				"000000000010\n"
				"000000001010";

			std::stringstream ss(log_lines);
			const auto log = aoc::DiagnosticLog{ ss };

			const auto best_match = aoc::LifeSupport(log).filter_using_most_frequent_bits();
			Assert::AreEqual(uint32_t{ 0b10111 }, best_match);
		}

		TEST_METHOD(FilterBitsForLeastCommonOnExampleData)
		{
			constexpr auto log_lines =
				"001000000000\n"
				"111100000000\n"
				"101100000000\n"
				"101110000000\n"
				"101010000000\n"
				"011110000000\n"
				"001110000000\n"
				"111000000000\n"
				"100000000000\n"
				"110010000000\n"
				"000100000000\n"
				"010100000000";

			std::stringstream ss(log_lines);
			const auto log = aoc::DiagnosticLog{ ss };

			const auto best_match = aoc::LifeSupport(log).filter_using_least_frequent_bits();
			Assert::AreEqual(uint32_t{ 0b10100000000 }, best_match);
		}

		TEST_METHOD(LifeSupportRatingIsCalculatedFromFile)
		{
			std::ifstream data_file(DATA_DIR / "Day3_input.txt");
			Assert::IsTrue(data_file.is_open());

			auto log = aoc::DiagnosticLog{ data_file };

			Assert::AreEqual(uint32_t{ 3379326 }, aoc::Submarine().boat_systems().life_support_rating(log));
		}
	};

	TEST_CLASS(TestDay4)
	{
	public:

		TEST_METHOD(LoadExampleBingoGameDraws)
		{
			std::stringstream ss{ "7, 4, 9, 5\n" };

			auto game = aoc::Bingo{};

			game.load(ss);

			const auto expected_draws = { 7, 4, 9, 5 };
			Assert::IsTrue(std::equal(expected_draws.begin(), expected_draws.end(), game.draws().begin()));
		}

		TEST_METHOD(LoadExampleBingoGameBoards)
		{
			constexpr auto board_str =
				"22 13 17 11  0\n"
				"8  2 23  4 24\n"
				"21  9 14 16  7\n"
				"6 10  3 18  5\n"
				"1 12 20 15 19\n"
				"\n";

			std::stringstream ss{ board_str };

			auto board = aoc::Bingo::Board{};

			board.load(ss);
		}
	};
}
