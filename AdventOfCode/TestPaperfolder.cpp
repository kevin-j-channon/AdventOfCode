#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"
#include "PaperFolder.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

namespace test_paper_folder
{
TEST_CLASS(TestPaper)
{
public:
	TEST_METHOD(LoadPointFromStream)
	{
		std::stringstream ss("6,10");

		const auto mark_count = aoc::Paper{}.load(ss).mark_count();

		Assert::AreEqual(size_t{ 1 }, mark_count);
	}

	TEST_METHOD(LoadExamplePointsFromStream)
	{
		constexpr auto data_str =
			"6,10\n"
			"0,14\n"
			"9,10\n"
			"0,3\n"
			"10,4\n"
			"4,11\n"
			"6,0\n"
			"6,12\n"
			"4,1\n"
			"0,13\n"
			"10,12\n"
			"3,4\n"
			"3,0\n"
			"8,4\n"
			"1,10\n"
			"2,14\n"
			"8,10\n"
			"9,0\n"
			"\n";

		std::stringstream data(data_str);

		const auto mark_count = aoc::Paper{}.load(data).mark_count();

		Assert::AreEqual(size_t{ 18 }, mark_count);
	}

	TEST_METHOD(PaperContainsNoMarksOnConstruction)
	{
		const auto paper = aoc::Paper{};

		Assert::AreEqual(size_t{ 0 }, paper.mark_count());
		Assert::AreEqual(false, paper.read({ 1, 2 }));
	}

	TEST_METHOD(MarkCheckEraseCheck)
	{
		auto paper = aoc::Paper{};

		paper.mark({ 1, 2 });
		paper.mark({ 3, 4 });
		paper.mark({ 5, 6 });

		Assert::AreEqual(size_t{ 3 }, paper.mark_count());
		Assert::AreEqual(true, paper.read({ 1, 2 }));
		Assert::AreEqual(true, paper.read({ 3, 4 }));
		Assert::AreEqual(true, paper.read({ 5, 6 }));

		paper.erase({ 3, 4 });

		Assert::AreEqual(size_t{ 2 }, paper.mark_count());
		Assert::AreEqual(true, paper.read({ 1, 2 }));
		Assert::AreEqual(true, paper.read({ 5, 6 }));
	}

	TEST_METHOD(StdInserterInsertsMarksIntoPaper)
	{
		const auto marks = {
			aoc::Paper::Point_t{1, 2},
			aoc::Paper::Point_t{3, 4},
			aoc::Paper::Point_t{9, 10}
		};

		auto paper = aoc::Paper{};

		std::copy(marks.begin(), marks.end(), std::inserter(paper, paper.end()));

		Assert::AreEqual(size_t{ 3 }, paper.mark_count());
		Assert::AreEqual(true, paper.read({ 1, 2 }));
		Assert::AreEqual(true, paper.read({ 3, 4 }));
		Assert::AreEqual(true, paper.read({ 9, 10 }));
	}

	TEST_METHOD(AsMatrix)
	{
		auto paper = aoc::Paper{};

		paper.mark({ 1, 2 });
		paper.mark({ 3, 4 });
		paper.mark({ 5, 6 });

		const auto mat = paper.as_matrix();
		Assert::AreEqual(arma::uword{ 7 }, mat.n_cols, L"Incorrect column count");
		Assert::AreEqual(arma::uword{ 7 }, mat.n_rows, L"Incorrect row count");

		for (auto r = arma::uword{ 0 }; r < mat.n_rows; ++r) {
			for (auto c = arma::uword{ 0 }; c < mat.n_cols; ++c) {
				Assert::AreEqual(paper.read({ c, r }), (mat.at(r, c) != 0));
			}
		}
	}
};

TEST_CLASS(TestFoldSequence)
{
public:
	TEST_METHOD(LoadFolderFromStream)
	{
		constexpr auto data_str =
			"fold along y=7\n"
			"fold along x=5";

		std::stringstream data(data_str);

		auto folds = aoc::FoldSequence{}.load(data);
		Assert::AreEqual(size_t{ 2 }, folds.size());
	}
};

TEST_CLASS(TestPaperFolder)
{
public:
	TEST_METHOD(PaperFolderWorksForSingleXFold)
	{
		std::stringstream paper_data("6,10");
		auto paper = std::move(aoc::Paper{}.load(paper_data));

		std::stringstream fold_data("fold along x=4");
		auto folds = aoc::FoldSequence{}.load(fold_data);

		paper = aoc::PaperFolder{std::move(paper)}.apply(folds);

		Assert::AreEqual(size_t{ 1 }, paper.mark_count());
		Assert::IsTrue(paper.read({ 2, 10 }));
	}

	TEST_METHOD(PaperFolderWorksForSingleYFold)
	{
		std::stringstream paper_data("6,10");
		auto paper = std::move(aoc::Paper{}.load(paper_data));

		std::stringstream fold_data("fold along y=7");
		auto folds = aoc::FoldSequence{}.load(fold_data);

		paper = aoc::PaperFolder{ std::move(paper) }.apply(folds);

		Assert::AreEqual(size_t{ 1 }, paper.mark_count());
		Assert::IsTrue(paper.read({ 6, 4 }));
	}

	TEST_METHOD(PaperFolderWorksForXYFold)
	{
		std::stringstream paper_data("6,10");
		auto paper = std::move(aoc::Paper{}.load(paper_data));

		std::stringstream fold_data("fold along x=4\nfold along y=7");
		auto folds = aoc::FoldSequence{}.load(fold_data);

		paper = aoc::PaperFolder{ std::move(paper) }.apply(folds);

		Assert::AreEqual(size_t{ 1 }, paper.mark_count());
		Assert::IsTrue(paper.read({ 2, 4 }));
	}

	TEST_METHOD(SimpleExampleWorks)
	{
		constexpr auto data_str =
			"6,10\n"
			"0,14\n"
			"9,10\n"
			"0,3\n"
			"10,4\n"
			"4,11\n"
			"6,0\n"
			"6,12\n"
			"4,1\n"
			"0,13\n"
			"10,12\n"
			"3,4\n"
			"3,0\n"
			"8,4\n"
			"1,10\n"
			"2,14\n"
			"8,10\n"
			"9,0\n"
			"\n"
			"fold along y=7\n"
			"fold along x=5"
			;
		std::stringstream data(data_str);

		auto paper = std::move(aoc::Paper{}.load(data));
		auto folds = aoc::FoldSequence{}.load(data);

		paper = aoc::PaperFolder{ std::move(paper) }.apply(folds);

		Assert::IsTrue(paper.read({ 0, 0 }));
		Assert::IsTrue(paper.read({ 0, 1 }));
		Assert::IsTrue(paper.read({ 0, 2 }));
		Assert::IsTrue(paper.read({ 0, 3 }));
		Assert::IsTrue(paper.read({ 0, 4 }));
		Assert::IsTrue(paper.read({ 1, 0 }));
		Assert::IsFalse(paper.read({ 1, 1 }));
		Assert::IsFalse(paper.read({ 1, 2 }));
		Assert::IsFalse(paper.read({ 1, 3 }));
		Assert::IsTrue(paper.read({ 1, 4 }));
		Assert::IsTrue(paper.read({ 2, 0 }));
		Assert::IsFalse(paper.read({ 2, 1 }));
		Assert::IsFalse(paper.read({ 2, 2 }));
		Assert::IsFalse(paper.read({ 2, 3 }));
		Assert::IsTrue(paper.read({ 2, 4 }));
		Assert::IsTrue(paper.read({ 3, 0 }));
		Assert::IsFalse(paper.read({ 3, 1 }));
		Assert::IsFalse(paper.read({ 3, 2 }));
		Assert::IsFalse(paper.read({ 3, 3 }));
		Assert::IsTrue(paper.read({ 3, 4 }));
		Assert::IsTrue(paper.read({ 4, 0 }));
		Assert::IsTrue(paper.read({ 4, 1 }));
		Assert::IsTrue(paper.read({ 4, 2 }));
		Assert::IsTrue(paper.read({ 4, 3 }));
		Assert::IsTrue(paper.read({ 4, 4 }));
		Assert::IsFalse(paper.read({ 5, 0 }));
		Assert::IsFalse(paper.read({ 5, 1 }));
		Assert::IsFalse(paper.read({ 5, 2 }));
		Assert::IsFalse(paper.read({ 5, 3 }));
		Assert::IsFalse(paper.read({ 5, 4 }));
		Assert::IsFalse(paper.read({ 6, 0 }));
		Assert::IsFalse(paper.read({ 6, 1 }));
		Assert::IsFalse(paper.read({ 6, 2 }));
		Assert::IsFalse(paper.read({ 6, 3 }));
		Assert::IsFalse(paper.read({ 6, 4 }));

		Assert::AreEqual(size_t{ 16 }, paper.mark_count());
	}
};

TEST_CLASS(TestCodeReading)
{
public:

	auto create_test_letter(char letter) -> aoc::Paper::Matrix_t
	{
		const auto& char_map = aoc::char_maps[letter - 'A'];
		auto out = aoc::Paper::Matrix_t(6, 5).fill(0);

		for (auto r = 0; r < out.n_rows; ++r) {
			for (auto c = 0; c < out.n_cols; ++c) {
				out.at(r, c) = char_map[r][c];
			}
		}

		return out;
	}
	TEST_METHOD(CorrectLetterHasBiggestScore)
	{
		const auto letters = {
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
			'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
		};
		for (auto letter : letters) {
			const auto test_letter_map = create_test_letter(letter);
			Assert::AreEqual(letter, aoc::CharacterReader<6, 5>::decode(test_letter_map.submat(0,0,5,4)));
		}
	}
	
	TEST_METHOD(IterateLetterMatrices)
	{
		constexpr auto letter_width = 5;

		// Build a fake paper matrix that has each of the letters in it in order.
		auto paper_matrix = aoc::Paper::Matrix_t(6, 26 * letter_width);
		for (auto idx = 0; idx < 26; ++idx) {
			paper_matrix.submat(0, idx * letter_width, 5, letter_width * (idx + 1) - 1) = create_test_letter('A' + idx);
		}

		// Check that each of the letters can be read from the main thing.
		Assert::AreEqual("ABCDEFGHIJKLMNOPQRSTUVWXYZ"s, aoc::PaperReader<6, 5>::decode(paper_matrix));
	}
};
}