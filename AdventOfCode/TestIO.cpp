#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.hpp"
#include "PacketDecoder.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

const auto DATA_DIR = std::filesystem::path(R"(..\..\AdventOfCode\Data)"s);


template<>
std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<aoc::comms::BITS::PacketType>(const aoc::comms::BITS::PacketType& type)
{
	switch (type)
	{
	case aoc::comms::BITS::PacketType::operation_0: return L"operation 0";
	case aoc::comms::BITS::PacketType::operation_1: return L"operation 1";
	case aoc::comms::BITS::PacketType::operation_2: return L"operation 2";
	case aoc::comms::BITS::PacketType::operation_3: return L"operation 3";
	case aoc::comms::BITS::PacketType::literal_value: return L"literal value";
	case aoc::comms::BITS::PacketType::operation_5: return L"operation 5";
	case aoc::comms::BITS::PacketType::operation_6: return L"operation 6";
	case aoc::comms::BITS::PacketType::operation_7: return L"operation 7";
	default:
		return L"UNKNOWN";
	}
}

namespace test_packet_decoder
{
TEST_CLASS(BitsStream)
{
public:
	TEST_METHOD(StreamDecompressesBits)
	{
		std::stringstream hex_stream("0F1E2D3C4B5A6978");

		aoc::comms::BITS::IStream bit_stream(hex_stream);

		constexpr auto expected_bits = "0000111100011110001011010011110001001011010110100110100101111000";
		const auto mismatching_bit = std::mismatch(expected_bits, expected_bits + 64, std::istreambuf_iterator<char>(bit_stream));
		Assert::IsTrue(mismatching_bit.first == expected_bits + 64, std::format(L"Mismatching bit at {}", std::distance(expected_bits, mismatching_bit.first)).c_str());
	}
};

TEST_CLASS(HeaderDeserialization)
{
public:
	TEST_METHOD(PacketHeaderHasVersion)
	{
		// The actual header stops here V
		std::stringstream bits(   "1101001010101");

		const auto header = aoc::comms::BITS::Header{ bits };

		Assert::AreEqual(uint8_t{ 6 }, header.version());
	}
	TEST_METHOD(PacketHeaderHasOperation)
	{
		// The actual header stops here V
		std::stringstream bits("1101001010101");

		const auto header = aoc::comms::BITS::Header{ bits };

		Assert::AreEqual(aoc::comms::BITS::PacketType::literal_value, header.type());
	}
};
}
