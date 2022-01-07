#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"
#include "Polymerizer.hpp"

using namespace std::string_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

namespace test_polymerizer
{
TEST_CLASS(TestPolymer)
{
public:
	TEST_METHOD(LoadStreamFromStream)
	{
		std::stringstream data("NNCB");

		const auto polymer = aoc::Polymer::from_stream(data);

		Assert::AreEqual(size_t{ 4 }, polymer.length());
	}
};
}
