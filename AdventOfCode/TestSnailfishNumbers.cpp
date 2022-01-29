#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "SnailfishNumbers.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

template<>
std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<aoc::snailfish::Value>(const aoc::snailfish::Value& value)
{
	return value.as_wstring();
}

namespace test_snailfish_numbers
{

TEST_CLASS(TestInitialisation)
{
public:
	TEST_METHOD(FromStreamSucceedsForSimplestGoodStream)
	{
		std::stringstream data{ "[1,2]" };
		const auto n1 = aoc::snailfish::Value{}.from_stream(data);
		const auto n2 = aoc::snailfish::Value{ 1, 2 };

		Assert::AreEqual(n2, n1);
	}

	TEST_METHOD(FailsIfStreamIsEmpty)
	{
		std::stringstream data{ "" };
		Assert::ExpectException<aoc::IOException>([&data]() {aoc::snailfish::Value{}.from_stream(data); });
	}
};

}