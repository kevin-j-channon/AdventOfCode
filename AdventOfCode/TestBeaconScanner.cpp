#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "BeaconScanner.hpp"

#include <cmath>

namespace test_beacon_scanner
{

using namespace aoc::navigation;

TEST_CLASS(InitialiseScannerReport)
{
public:
	TEST_METHOD(InitialiseIdFromStream)
	{
		constexpr auto data_str =
			"--- scanner 0 ---\n"
			"0,2\n"
			"4,1\n"
			"3,3\n";
		std::stringstream data{ data_str };

		const auto scanner = ScannerReport::from_stream(data);
		Assert::AreEqual(ScannerReport::Id_t{ 0 }, scanner.id());
	}

	TEST_METHOD(MalformedIdLineRaisesException)
	{
		constexpr auto data_str =
			"--- scanner ---\n"
			"0,2\n"
			"4,1\n"
			"3,3\n";
		std::stringstream data{ data_str };

		Assert::ExpectException<aoc::IOException>([&]() { ScannerReport::from_stream(data); });
	}

	TEST_METHOD(InitialiseBeaconPositionsFromStream)
	{
		constexpr auto data_str =
			"--- scanner 0 ---\n"
			"0,2\n"
			"4,1\n"
			"3,3\n";
		std::stringstream data{ data_str };

		const auto scanner = ScannerReport::from_stream(data);
		Assert::AreEqual(size_t{ 3 }, scanner.beacons().size());

		{
			const auto& b = scanner.beacons().at(0);
			Assert::AreEqual(0.0, b.position().x);
			Assert::AreEqual(2.0, b.position().y);
			Assert::AreEqual(0.0, b.position().z);
		}

		{
			const auto& b = scanner.beacons().at(1);
			Assert::AreEqual(4.0, b.position().x);
			Assert::AreEqual(1.0, b.position().y);
			Assert::AreEqual(0.0, b.position().z);
		}

		{
			const auto& b = scanner.beacons().at(2);
			Assert::AreEqual(3.0, b.position().x);
			Assert::AreEqual(3.0, b.position().y);
			Assert::AreEqual(0.0, b.position().z);
		}
	}
	TEST_METHOD(ReadingMultipleScannersFromAStream)
	{
		constexpr auto data_str =
			"--- scanner 0 ---\n"
			"0,2\n"
			"4,1\n"
			"3,3\n"
			"\n"
			"--- scanner 1 ---\n"
			"-1,-1\n"
			"-5,0\n"
			"-2,1"
			;
		std::stringstream data{ data_str };

		const auto scanners = read_scanner_report(data);
		Assert::AreEqual(size_t{ 2 }, scanners.size());

		auto point_values = std::array{
			std::array{
				std::array{ 0.0, 2.0 },
				std::array{ 4.0, 1.0 },
				std::array{ 3.0, 3.0 }
			},
			std::array{
				std::array{ -1.0, -1.0 },
				std::array{ -5.0,  0.0 },
				std::array{ -2.0,  1.0 }
			}
		};

		for (auto i = 0; i < scanners.size(); ++i) {
			const auto& scanner = scanners[i];
			Assert::AreEqual(size_t{ 3 }, scanner.beacons().size());

			const auto& values = point_values[i];
			for (auto j = 0; j < 3; ++j) {
				const auto& b = scanner.beacons().at(j);
				
				Assert::AreEqual(values[j][0], b.position().x);
				Assert::AreEqual(values[j][1], b.position().y);

				Assert::AreEqual(0.0, b.position().z);
			}
		}
	}
};

TEST_CLASS(TestMapper)
{
public:
	TEST_METHOD(FindTranslationOffset)
	{
		constexpr auto data_str =
			"--- scanner 0 ---\n"
			"0,2\n"
			"4,1\n"
			"3,3\n"
			"\n"
			"--- scanner 1 ---\n"
			"-1,-1\n"
			"-5,0\n"
			"-2,1"
			;
		std::stringstream data{ data_str };

		const auto reports = read_scanner_report(data);

		const auto offset_and_score = MappedSpace::find_translational_offset(reports.at(0).beacons(), reports.at(1).beacons());

		Assert::IsTrue(offset_and_score.has_value());

		const auto [offset, score] = *offset_and_score;
		Assert::AreEqual(-5.0, offset.x);
		Assert::AreEqual(-2.0, offset.y);
		Assert::AreEqual(uint32_t{ 3 }, score);
	}

	TEST_METHOD(RotateBeacon)
	{
		const auto beacons = Beacons_t{
			{{0.0, 0.0, 1.0}},
			{{0.0, 1.0, 0.0}},
			{{0.0, 1.0, 1.0}},
			{{1.0, 0.0, 0.0}},
			{{1.0, 0.0, 1.0}},
			{{1.0, 1.0, 0.0}},
			{{1.0, 1.0, 1.0}},
		};

		const auto rotation_axes = std::vector<aoc::Direction_t<Position_t::Value_t>>{
			{0.0, 0.0, 1.0},
			{0.0, 1.0, 0.0},
			{1.0, 0.0, 0.0},
			// {0.0, 1.0, 1.0},
			// {1.0, 0.0, 1.0},
			// {1.0, 1.0, 0.0},
			// {1.0, 1.0, 1.0},
		};

		const auto expected = std::vector<Beacons_t>{
			{{{0.,0.,1.}},{{-1.,0.,0.}},{{-1.,0.,1.}},{{0.,1.,0.}},{{0.,1.,1.}},{{-1.,1.,0.}},{{-1.,1.,1.}}},
			{{{1.,0.,0.}},{{0.,1.,0.}},{{1.,1.,0.}},{{0.,0.,-1.}},{{1.,0.,-1.}},{{0.,1.,-1.}},{{1.,1.,-1.}}},
			{{{0.,-1.,0.}},{{0.,0.,1.}},{{0.,-1.,1.}},{{1.,0.,0.}},{{1.,-1.,0.}},{{1.,0.,1.}},{{1.,-1.,1.}}}
		};

		auto expected_beacons = expected.begin();
		for (const auto& axis : rotation_axes) {
			auto expected_beacon = expected_beacons->begin();
			
			for (const auto& b : beacons) {
				const auto rotated = BeaconCloudRotator::rotate_beacon(b, aoc::quaternion::from_axis_and_angle(axis, std::numbers::pi / 2));

				Assert::AreEqual((int)expected_beacon->position().x, (int)rotated.position().x);
				Assert::AreEqual((int)expected_beacon->position().y, (int)rotated.position().y);
				Assert::AreEqual((int)expected_beacon->position().z, (int)rotated.position().z);

				++expected_beacon;
			}
			
			++expected_beacons;
		}
	}
};
}
