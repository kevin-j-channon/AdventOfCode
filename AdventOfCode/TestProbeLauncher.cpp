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

	TEST_METHOD(InputWithoutTwoRectanglePointsThrows)
	{
		std::stringstream data("target area: x=244..303");
		Assert::ExpectException<aoc::IOException>([&data]() { aoc::science::ProbeLauncher{}.read_target(data); });
	}

	TEST_METHOD(InputWithoutValidXPartThrows)
	{
		std::stringstream data("target area: x244..303, y=-91..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { aoc::science::ProbeLauncher{}.read_target(data); });
	}

	TEST_METHOD(InputWithInvalidDimensionLabelThrows)
	{
		std::stringstream data("target area: z=244..303, y=-91..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { aoc::science::ProbeLauncher{}.read_target(data); });
	}

	TEST_METHOD(InputWithoutValidXRangeThrows)
	{
		std::stringstream data("target area: x=244.., y=-91..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { aoc::science::ProbeLauncher{}.read_target(data); });
	}

	TEST_METHOD(InputWithNonNumericXMinThrows)
	{
		std::stringstream data("target area: x=ddd..303, y=-91..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { aoc::science::ProbeLauncher{}.read_target(data); });
	}

	TEST_METHOD(InputWithOutOfRangeXMinThrows)
	{
		std::stringstream data("target area: x=4294967296..303, y=-91..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { aoc::science::ProbeLauncher{}.read_target(data); });
	}

	TEST_METHOD(InputWithoutValidYPartThrows)
	{
		std::stringstream data("target area: x=244..303, y-91..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { aoc::science::ProbeLauncher{}.read_target(data); });
	}

	TEST_METHOD(InputWithoutValidYRangeThrows)
	{
		std::stringstream data("target area: x=244..303, y=-..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { aoc::science::ProbeLauncher{}.read_target(data); });
	}

	TEST_METHOD(InputWithNonNumericYMinThrows)
	{
		std::stringstream data("target area: x=ddd..303, y=abc..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { aoc::science::ProbeLauncher{}.read_target(data); });
	}

	TEST_METHOD(InputWithOutOfRangeYMaxThrows)
	{
		std::stringstream data("target area: x=244..303, y=-91..4294967296");
		Assert::ExpectException<aoc::IOException>([&data]() { aoc::science::ProbeLauncher{}.read_target(data); });
	}

	TEST_METHOD(RectangleHasTheExpectedValues)
	{
		std::stringstream data("target area: x=244..303, y=-91..-54");

		const auto launcher = aoc::science::ProbeLauncher{}.read_target(data);

		Assert::AreEqual(244, launcher.target().top_left().x);
		Assert::AreEqual(-54, launcher.target().top_left().y);
		Assert::AreEqual(303, launcher.target().bottom_right().x);
		Assert::AreEqual(-91, launcher.target().bottom_right().y);
	}
};
}
