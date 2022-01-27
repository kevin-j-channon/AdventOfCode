#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "ProbeLauncher.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

namespace test_probe_launcher
{
using namespace aoc::science;

TEST_CLASS(TestInitialization)
{
public:
	TEST_METHOD(InputWithoutColonThrows)
	{
		std::stringstream data("target area x=244..303, y=-91..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { Target{}.from_stream(data); });
	}

	TEST_METHOD(InputWithoutTwoRectanglePointsThrows)
	{
		std::stringstream data("target area: x=244..303");
		Assert::ExpectException<aoc::IOException>([&data]() { Target{}.from_stream(data); });
	}

	TEST_METHOD(InputWithoutValidXPartThrows)
	{
		std::stringstream data("target area: x244..303, y=-91..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { Target{}.from_stream(data); });
	}

	TEST_METHOD(InputWithInvalidDimensionLabelThrows)
	{
		std::stringstream data("target area: z=244..303, y=-91..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { Target{}.from_stream(data); });
	}

	TEST_METHOD(InputWithoutValidXRangeThrows)
	{
		std::stringstream data("target area: x=244.., y=-91..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { Target{}.from_stream(data); });
	}

	TEST_METHOD(InputWithNonNumericXMinThrows)
	{
		std::stringstream data("target area: x=ddd..303, y=-91..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { Target{}.from_stream(data); });
	}

	TEST_METHOD(InputWithOutOfRangeXMinThrows)
	{
		std::stringstream data("target area: x=4294967296..303, y=-91..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { Target{}.from_stream(data); });
	}

	TEST_METHOD(InputWithoutValidYPartThrows)
	{
		std::stringstream data("target area: x=244..303, y-91..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { Target{}.from_stream(data); });
	}

	TEST_METHOD(InputWithoutValidYRangeThrows)
	{
		std::stringstream data("target area: x=244..303, y=-..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { Target{}.from_stream(data); });
	}

	TEST_METHOD(InputWithNonNumericYMinThrows)
	{
		std::stringstream data("target area: x=ddd..303, y=abc..-54");
		Assert::ExpectException<aoc::IOException>([&data]() { Target{}.from_stream(data); });
	}

	TEST_METHOD(InputWithOutOfRangeYMaxThrows)
	{
		std::stringstream data("target area: x=244..303, y=-91..4294967296");
		Assert::ExpectException<aoc::IOException>([&data]() { Target{}.from_stream(data); });
	}

	TEST_METHOD(RectangleHasTheExpectedValues)
	{
		std::stringstream data("target area: x=244..303, y=-91..-54");

		const auto target = Target{}.from_stream(data);

		Assert::AreEqual(244, target.area().top_left().x);
		Assert::AreEqual(-54, target.area().top_left().y);
		Assert::AreEqual(303, target.area().bottom_right().x);
		Assert::AreEqual(-91, target.area().bottom_right().y);
	}
};

TEST_CLASS(TestBalistics)
{
public:

	TEST_METHOD(OutOfRangeOriginCausesException)
	{
		Assert::ExpectException<aoc::OutOfRangeException>([]() {
			Ballistics{ Ballistics::Arena_t{{0, 10}, {20, -10}} }.trajectory({ -10, 0 }, { 1, 1 });
			});
	}

	TEST_METHOD(TrajectoryWithPositiveXAndYVelocity)
	{
		const auto trajectory = Ballistics{ Ballistics::Arena_t{{0, 10}, {20, -10}} }.trajectory({ 0, 0 }, { 2, 2 });
		const auto expected_trajectory = std::array{
			Ballistics::Position_t{0, 0},
			Ballistics::Position_t{2, 2},
			Ballistics::Position_t{3, 3},
			Ballistics::Position_t{3, 3},
			Ballistics::Position_t{3, 2},
			Ballistics::Position_t{3, 0},
			Ballistics::Position_t{3,-3},
			Ballistics::Position_t{3,-7}
		};

		Assert::AreEqual(expected_trajectory.size(), trajectory.size());
		for (auto i : aoc::ValueRange<size_t>(trajectory.size() - 1)) {
			Assert::IsTrue(expected_trajectory[i] == trajectory[i]);
		}
	}

};
}
