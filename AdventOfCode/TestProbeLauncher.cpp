#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "ProbeLauncher.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

namespace test_probe_launcher
{
TEST_CLASS(TestInitialization)
{
public:
	TEST_METHOD(InputWithoutColonThrows)
	{
		std::stringstream data("target area x=244..303, y=-91..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { aoc::science::ProbeLauncher{}.read_target(data); });
	}
};
}
