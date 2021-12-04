#include "AdventOfCode.hpp"

#include "CppUnitTest.h"

#include <ranges>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <format>
#include <string>
#include <filesystem>
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::string_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

namespace AdventOfCode
{
	TEST_CLASS(TestDay1)
	{
	public:

		TEST_METHOD(CountNetIncreaseFromVector)
		{
			auto measurements = std::vector<uint32_t>{ 1, 2, 3, 2, 1, 2, 3 };

			auto depth_score = aoc::Submarine().GetDepthScore<1>(measurements.begin(), measurements.end());

			Assert::AreEqual(uint32_t{ 4 }, depth_score);
		}

		TEST_METHOD(DepthScoreVectorWithVariousJumps)
		{
			auto measurements = std::vector<uint32_t>{ 1, 22, 3, 200, 1000, 2, 3, 4, 100 };

			auto depth_score = aoc::Submarine().GetDepthScore<1>(measurements.begin(), measurements.end());

			Assert::AreEqual(uint32_t{ 6 }, depth_score);
		}

		TEST_METHOD(ExampleValues)
		{
			auto measurements = std::vector<uint32_t>{ 199, 200, 208, 210, 200, 207, 240, 269, 260, 263 };

			auto depth_score = aoc::Submarine().GetDepthScore<1>(measurements.begin(), measurements.end());

			Assert::AreEqual(uint32_t{ 7 }, depth_score);
		}

		TEST_METHOD(CountNetIncreaseFromFile)
		{
			std::ifstream data_file(DATA_DIR / "Day1_input.txt");
			Assert::IsTrue(data_file.is_open());

			auto depth_score = aoc::Submarine().GetDepthScore<1>(std::istream_iterator<uint32_t>(data_file), std::istream_iterator<uint32_t>());

			Assert::AreEqual(uint32_t{ 1502 }, depth_score);
		}
		TEST_METHOD(RollingWindow3ExampleValues)
		{
			auto measurements = std::vector<uint32_t>{ 199, 200, 208, 210, 200, 207, 240, 269, 260, 263 };

			auto depth_score = aoc::Submarine().GetDepthScore<3>(measurements.begin(), measurements.end());

			Assert::AreEqual(uint32_t{ 5 }, depth_score);
		}

		TEST_METHOD(RollingWindow3FromFile)
		{
			std::ifstream data_file(DATA_DIR / "Day1_input.txt");
			Assert::IsTrue(data_file.is_open());

			auto depth_score = aoc::Submarine().GetDepthScore<3>(std::istream_iterator<uint32_t>(data_file), std::istream_iterator<uint32_t>());

			Assert::AreEqual(uint32_t{ 1538 }, depth_score);
		}
	};

	TEST_CLASS(TestDay2)
	{
	public:

		TEST_METHOD(VectorsCanBeAdded)
		{
			const auto v1 = aoc::Vec2d<int>{ 1, 2 };
			const auto v2 = aoc::Vec2d<int>{ 3, 4 };

			const auto v3 = v1 + v2;

			Assert::AreEqual(4, v3.x);
			Assert::AreEqual(6, v3.y);
		}

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
			const auto net_direction = aoc::Submarine().GetNetDirection(StreamIter_t(ss), StreamIter_t());

			Assert::AreEqual(15, net_direction.x);
			Assert::AreEqual( 6, net_direction.y);
		}

		TEST_METHOD(ParseDirectionsFromFile)
		{
			std::ifstream data_file(DATA_DIR / "Day2_input.txt");
			Assert::IsTrue(data_file.is_open());

			using StreamIter_t = std::istream_iterator<aoc::Direction>;
			const auto net_direction = aoc::Submarine().GetNetDirection(StreamIter_t(data_file), StreamIter_t());

			Assert::AreEqual(1895, net_direction.x);
			Assert::AreEqual(894, net_direction.y);
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
			const auto net_aim = aoc::Submarine().GetNetAiming(StreamIter_t(ss), StreamIter_t());

			Assert::AreEqual(15, net_aim.x);
			Assert::AreEqual(60, net_aim.y);
		}
	};

	TEST_CLASS(TestDay3)
	{
	public:
		TEST_METHOD(ReadPowerParamsFromStream)
		{
			std::stringstream ss("111011110101");
			auto pp = aoc::PowerParameters{};

			ss >> pp;

			auto a1 = std::array<bool, aoc::PowerParameters::bit_count>{ true, true, true, false, true, true, true, true, false, true, false, true  };
			for (auto i = 0; i < aoc::PowerParameters::bit_count; ++i)
			{
				Assert::AreEqual(a1[i], pp.bits[i]);
			}
		}

		TEST_METHOD(ReadMultiplePowerParamsFromStream)
		{
			std::stringstream ss("111011110101\n011000111010");
			auto v = std::vector<aoc::PowerParameters>{};

			using Iter_t = std::istream_iterator<aoc::PowerParameters>;

			std::copy(Iter_t(ss), Iter_t(), std::back_inserter(v));

			Assert::AreEqual(size_t{ 2 }, v.size());

			auto a1 = std::array<bool, aoc::PowerParameters::bit_count>{ true, true, true, false, true, true, true, true, false, true, false, true  };
			for (auto i = 0; i < aoc::PowerParameters::bit_count; ++i)
			{
				Assert::AreEqual(a1[i], v[0].bits[i]);
			}

			auto a2 = std::array<bool, aoc::PowerParameters::bit_count>{ false, true, true, false, false, false, true, true, true, false, true, false };
			for (auto i = 0; i < aoc::PowerParameters::bit_count; ++i)
			{
				Assert::AreEqual(a2[i], v[1].bits[i]);
			}
		}

		TEST_METHOD(AccumulatePowerParamsWorks)
		{

		}
	};
}
