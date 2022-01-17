#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"
#include "PacketDecoder.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);

namespace test_packet_decoder
{
TEST_CLASS(BitsStream)
{
public:
	TEST_METHOD(StreamDecompressesBits)
	{
		std::stringstream hex_stream("0F1E2D3C4B5A6978");

		aoc::comms::BitsInputStream bit_stream(hex_stream);

		constexpr auto expected_bits = "0000111100011110001011010011110001001011010110100110100101111000";
		const auto mismatching_bit = std::mismatch(expected_bits, expected_bits + 64, std::istreambuf_iterator<char>(bit_stream));
		Assert::IsTrue(mismatching_bit.first == expected_bits + 64, std::format(L"Mismatching bit at {}", std::distance(expected_bits, mismatching_bit.first)).c_str());
	}
};
}
