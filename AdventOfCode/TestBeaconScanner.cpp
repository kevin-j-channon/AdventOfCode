#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "BeaconScanner.hpp"

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

		const auto scanner = Scanner::from_stream(data);
		Assert::AreEqual(Scanner::Id_t{ 0 }, scanner.id());
	}

	TEST_METHOD(MalformedIdLineRaisesException)
	{
		constexpr auto data_str =
			"--- scanner ---\n"
			"0,2\n"
			"4,1\n"
			"3,3\n";
		std::stringstream data{ data_str };

		Assert::ExpectException<aoc::IOException>([&]() { Scanner::from_stream(data); });
	}

	TEST_METHOD(InitialiseBeaconPositionsFromStream)
	{
		constexpr auto data_str =
			"--- scanner 0 ---\n"
			"0,2\n"
			"4,1\n"
			"3,3\n";
		std::stringstream data{ data_str };

		const auto scanner = Scanner::from_stream(data);
		Assert::AreEqual(size_t{ 3 }, scanner.beacons().size());

		{
			const auto& b = scanner.beacons().at(0);
			Assert::AreEqual(0, b.position().x);
			Assert::AreEqual(2, b.position().y);
			Assert::AreEqual(0, b.position().z);
		}

		{
			const auto& b = scanner.beacons().at(1);
			Assert::AreEqual(4, b.position().x);
			Assert::AreEqual(1, b.position().y);
			Assert::AreEqual(0, b.position().z);
		}

		{
			const auto& b = scanner.beacons().at(2);
			Assert::AreEqual(3, b.position().x);
			Assert::AreEqual(3, b.position().y);
			Assert::AreEqual(0, b.position().z);
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

		const auto scanners = read_all_scanners(data);
		Assert::AreEqual(size_t{ 2 }, scanners.size());

		auto point_values = std::array{
			std::array{
				std::array{ 0, 2 },
				std::array{ 4, 1 },
				std::array{ 3, 3 }
			},
			std::array{
				std::array{ -1, -1 },
				std::array{ -5,  0 },
				std::array{ -2,  1 }
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

				Assert::AreEqual(0, b.position().z);
			}
		}
	}
};
}
