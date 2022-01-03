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
	TEST_METHOD(AddPointFromStream)
	{
		std::stringstream ss("6,10");

		const auto mark_count = aoc::Paper{}.load(ss).mmark_count();

		Assert::AreEqual(size_t{ 1 }, mark_count);
	}
};
}