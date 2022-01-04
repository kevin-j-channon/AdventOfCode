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

		const auto mark_count = aoc::Paper{}.load(ss).mmark_count();

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

		const auto mark_count = aoc::Paper{}.load(data).mmark_count();

		Assert::AreEqual(size_t{ 18 }, mark_count);
	}

	TEST_METHOD(LoadFolderFromStream)
	{
		constexpr auto data_str =
			"fold along y=7\n"
			"fold along x=5";

		std::stringstream data(data_str);

		auto folds = aoc::FoldSequence{}.load(data);
		Assert::AreEqual(size_t{ 2 }, folds.size());
	}
	
	TEST_METHOD(PaperFolderWorksForSingleFold)
	{
		std::stringstream paper_data("6,10");
		auto paper = aoc::Paper{}.load(paper_data);

		std::stringstream fold_data("fold along x=4");
		auto folds = aoc::FoldSequence{}.load(fold_data);

		paper = aoc::PaperFolder{ std::move(paper) }.apply(folds);
	}
};
}