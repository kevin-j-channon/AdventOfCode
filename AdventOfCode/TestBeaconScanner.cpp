#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "BeaconScanner.hpp"

namespace test_beacon_scanner
{

TEST_CLASS(InitialiseScannerReport)
{
public:
	TEST_METHOD(InitialiseFromStream)
	{
		using namespace aoc::navigation;

		constexpr auto data_str =
			"--- scanner 0 ---\n"
			"0,2\n"
			"4,1\n"
			"3,3\n";
		std::stringstream data{ data_str };

		const auto scanner = Scanner::from_stream(data);
		Assert::AreEqual(Scanner::Id_t{ 0 }, scanner.id());
	}
};
}
