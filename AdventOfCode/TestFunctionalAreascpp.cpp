#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"
#include <Maths/Geometry.hpp>
#include "DiagnosticLog.hpp"
#include "BoatSystems.hpp"
#include "AdventOfCode.hpp"

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

namespace string_operations
{
TEST_CLASS(SplitStringOnChar)
{
public:

	TEST_METHOD(splitStringWorksWhenDropEmptyOptionIsSet)
	{
		const auto str = "1 2 3"s;
		const auto split_str = split(str, ' ', SplitBehaviour::drop_empty);

		Assert::AreEqual(size_t{ 3 }, split_str.size());
	}

	TEST_METHOD(splitStringWorksWhenDropEmptyOptionIsNotSet)
	{
		const auto str = "1 2 3"s;
		const auto split_str = split(str, ' ');

		Assert::AreEqual(size_t{ 3 }, split_str.size());
	}

	TEST_METHOD(SplitStringDropsConsecutiveDelimitersWhenOptionIsSet)
	{
		const auto str = "1  2      3"s;
		const auto split_str = split(str, ' ', SplitBehaviour::drop_empty);

		Assert::AreEqual(size_t{ 3 }, split_str.size());
	}

	TEST_METHOD(SplitStringDropsConsecutiveDelimitersAtBeginning)
	{
		const auto str = "   1 2      3"s;
		const auto split_str = split(str, ' ', SplitBehaviour::drop_empty);

		Assert::AreEqual(size_t{ 3 }, split_str.size());
	}

	TEST_METHOD(SplitStringDropsConsecutiveDelimitersAtEnd)
	{
		const auto str = "1 2      3   "s;
		const auto split_str = split(str, ' ', SplitBehaviour::drop_empty);

		Assert::AreEqual(size_t{ 3 }, split_str.size());
	}

	TEST_METHOD(SplitStringEmptyResultWhenStringIsOnlyDelimiters)
	{
		const auto str = "   "s;
		const auto split_str = split(str, ' ', SplitBehaviour::drop_empty);

		Assert::IsTrue(split_str.empty());
	}

	TEST_METHOD(SplitStringNonEmptyResultWhenStringIsOnlyUndroppedDelimiters)
	{
		const auto str = "    "s;
		const auto split_str = split(str, ' ', SplitBehaviour::none);

		Assert::AreEqual(size_t{ 4 }, split_str.size());
	}
};

TEST_CLASS(SplitStringOnString)
{
public:
	TEST_METHOD(SplittingOnAnEmptyStringRaisesException)
	{
		Assert::ExpectException<aoc::InvalidArgException>([]() { split("1 2 3"s, ""s); });
	}
	
	TEST_METHOD(splitStringReturnsEmptyIfStringLengthIsLessThanToDelimiterLength)
	{
		const auto str = "1**2"s;
		const auto split_str = split(str, "*****"s, SplitBehaviour::drop_empty);

		Assert::AreEqual(size_t{ 1 }, split_str.size());
	}

	TEST_METHOD(splitStringWorksWhenDropEmptyOptionIsSet)
	{
		const auto str = "1****2**3"s;
		const auto split_str = split(str, "**"s, SplitBehaviour::drop_empty);

		Assert::AreEqual(size_t{ 3 }, split_str.size());
	}

	TEST_METHOD(splitStringWorksWhenDropEmptyOptionIsNotSet)
	{
		const auto str = "1<|>2<|>3<|>4"s;
		const auto split_str = split(str, "<|>"s);

		Assert::AreEqual(size_t{ 4 }, split_str.size());
	}

	TEST_METHOD(SplitStringDropsConsecutiveDelimitersWhenOptionIsSet)
	{
		const auto str = "1..2......3"s;
		const auto split_str = split(str, ".."s, SplitBehaviour::drop_empty);

		Assert::AreEqual(size_t{ 3 }, split_str.size());
	}

	TEST_METHOD(SplitStringDropsConsecutiveDelimitersAtBeginning)
	{
		const auto str = "_delim__delim__delim_1_delim_2_delim__delim__delim__delim__delim__delim_3"s;
		const auto split_str = split(str, "_delim_"s, SplitBehaviour::drop_empty);

		Assert::AreEqual(size_t{ 3 }, split_str.size());
	}

	TEST_METHOD(SplitStringDropsConsecutiveDelimitersAtEnd)
	{
		const auto str = "1xyz2xyzxyzxyzxyzxyzxyz3xyzxyzxyz"s;
		const auto split_str = split(str, "xyz"s, SplitBehaviour::drop_empty);

		Assert::AreEqual(size_t{ 3 }, split_str.size());
	}

	TEST_METHOD(SplitStringEmptyResultWhenStringIsOnlyDelimiters)
	{
		const auto str = "************"s;
		const auto split_str = split(str, "***"s, SplitBehaviour::drop_empty);

		Assert::IsTrue(split_str.empty());
	}

	TEST_METHOD(SplitStringNonEmptyResultWhenStringIsOnlyUndroppedDelimiters)
	{
		const auto str = "************"s;
		
		{
			const auto split_str = split(str, "*"s, SplitBehaviour::none);
			Assert::AreEqual(size_t{ 13 }, split_str.size());
		}

		{
			const auto split_str = split(str, "**"s, SplitBehaviour::none);
			Assert::AreEqual(size_t{ 7 }, split_str.size());
		}

		{
			const auto split_str = split(str, "***"s, SplitBehaviour::none);
			Assert::AreEqual(size_t{ 5 }, split_str.size());
		}

		{
			const auto split_str = split(str, "****"s, SplitBehaviour::none);
			Assert::AreEqual(size_t{ 4 }, split_str.size());
		}

		{
			const auto split_str = split(str, "******"s, SplitBehaviour::none);
			Assert::AreEqual(size_t{ 3 }, split_str.size());
		}

		{
			const auto split_str = split(str, "************"s, SplitBehaviour::none);
			Assert::AreEqual(size_t{ 2 }, split_str.size());
		}
	}
};
}

namespace boat_systems
{
TEST_CLASS(DirectionAndAiming)
{
public:
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
		Assert::AreEqual(6, net_direction.y);
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

TEST_CLASS(DepthMeasurements)
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
};

TEST_CLASS(LifeSupportSystems)
{
public:

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
};

TEST_CLASS(VentAnalysis)
{
public:
	TEST_METHOD(VentAnalyserScoresExampleData)
	{
		constexpr auto data_str =
			"0,9 -> 5,9\n"
			"8,0 -> 0,8\n"
			"9,4 -> 3,4\n"
			"2,2 -> 2,1\n"
			"7,0 -> 7,4\n"
			"6,4 -> 2,0\n"
			"0,9 -> 2,9\n"
			"3,4 -> 1,4\n"
			"0,0 -> 8,8\n"
			"5,5 -> 8,2";

		std::stringstream data{ data_str };

		Assert::AreEqual(uint32_t{ 5 }, aoc::VentAnalyzer{ data }.score<aoc::VentAnalyzer::horizontal | aoc::VentAnalyzer::vertical>());
	}
	TEST_METHOD(VentAnalyserScoresExampleDataWithDiagonals)
	{
		constexpr auto data_str =
			"0,9 -> 5,9\n"
			"8,0 -> 0,8\n"
			"9,4 -> 3,4\n"
			"2,2 -> 2,1\n"
			"7,0 -> 7,4\n"
			"6,4 -> 2,0\n"
			"0,9 -> 2,9\n"
			"3,4 -> 1,4\n"
			"0,0 -> 8,8\n"
			"5,5 -> 8,2";

		std::stringstream data{ data_str };

		Assert::AreEqual(uint32_t{ 12 }, aoc::VentAnalyzer{ data }
		.score<aoc::VentAnalyzer::horizontal | aoc::VentAnalyzer::vertical | aoc::VentAnalyzer::diagonal>());
	}
};

TEST_CLASS(FloorHeightAnalyserTests)
{
public:
	TEST_METHOD(HeightMapCanBeLoadedFromStream)
	{
		constexpr auto data_str =
			"2199943210\n"
			"3987894921\n"
			"9856789892\n"
			"8767896789\n"
			"9899965678";

		std::stringstream data(data_str);

		const auto analyser = aoc::FloorHeightAnalyser<uint8_t, 1>{}.load(data);

		Assert::AreEqual(arma::uword{ 5 }, analyser.rows());
		Assert::AreEqual(arma::uword{ 10 }, analyser.cols());
	}

	TEST_METHOD(MinimaCanBeAppended)
	{
		auto minima = aoc::FloorHeightAnalyser<uint8_t, 1>::Minima{}.append({ {1,1,1}, {1,2,3}, {5,5,5} });
		Assert::AreEqual(std::ptrdiff_t{ 3 }, std::distance(minima.begin(), minima.end()));

		using Point_t = aoc::FloorHeightAnalyser<uint8_t, 1>::Minima::Point_t;

		const auto expected_1 = { Point_t{1,1,1}, Point_t{1,2,3}, Point_t{5,5,5} };
		Assert::IsTrue(std::equal(expected_1.begin(), expected_1.end(), minima.begin()));

		minima.append({ {10, 11, 12} });
		Assert::AreEqual(std::ptrdiff_t{ 4 }, std::distance(minima.begin(), minima.end()));

		const auto expected_2 = { Point_t{1,1,1}, Point_t{1,2,3}, Point_t{5,5,5}, Point_t{10, 11, 12} };
		Assert::IsTrue(std::equal(expected_2.begin(), expected_2.end(), minima.begin()));
	}

	TEST_METHOD(SampleIsCorrect)
	{
		constexpr auto data_str =
			"2199943210\n"
			"3987894921\n"
			"9856789892\n"
			"8767896789\n"
			"9899965678";

		std::stringstream data(data_str);

		const auto minima = aoc::FloorHeightAnalyser<uint8_t, 1>{}.load(data).find_minima();

		Assert::AreEqual(size_t{ 4 }, minima.size());
	}
};

}

namespace diagnostic_log
{
TEST_CLASS(DiagnosticLog)
{
public:

	TEST_METHOD(ReadLogEntriedFromStream)
	{
		std::stringstream ss("111011110101");
		auto log_entry = aoc::DiagnosticLog::Entry_t{};

		ss >> log_entry;

		auto a1 = aoc::DiagnosticLog::Entry_t{ 1,1,1,0,1,1,1,1,0,1,0,1 };
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

		const auto entry_1 = aoc::DiagnosticLog::Entry_t{ 1,1,1,0,1,1,1,1,0,1,0,1 };
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
		auto log = aoc::DiagnosticLog{ ss };

		Assert::AreEqual(ptrdiff_t{ 3 }, std::distance(log.begin(), log.end()));
	}

	TEST_METHOD(InvalidCharacterInLogCausesException)
	{
		// Second entry has an invalid character in it.
		std::stringstream ss("111011110101\n011000121010\n100000010010");
		auto log = aoc::DiagnosticLog{};

		Assert::ExpectException<aoc::Exception>([&]() { log.load(ss); });
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
};
}