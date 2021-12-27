#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"
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
			Assert::AreEqual(static_cast<uint8_t>(*expected), (*cell).value);
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
