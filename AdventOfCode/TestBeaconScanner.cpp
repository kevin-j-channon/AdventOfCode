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
};
}
