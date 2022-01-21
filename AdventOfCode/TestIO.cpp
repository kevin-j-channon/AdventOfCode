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
	case aoc::comms::BITS::PacketType::operation_sum: return L"operation 0";
	case aoc::comms::BITS::PacketType::operation_product: return L"operation 1";
	case aoc::comms::BITS::PacketType::operation_min: return L"operation 2";
	case aoc::comms::BITS::PacketType::operation_max: return L"operation 3";
	case aoc::comms::BITS::PacketType::literal_value: return L"literal value";
	case aoc::comms::BITS::PacketType::operation_greater: return L"operation 5";
	case aoc::comms::BITS::PacketType::operation_less: return L"operation 6";
	case aoc::comms::BITS::PacketType::operation_equal: return L"operation 7";
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

	TEST_METHOD(DecompressExample1)
	{
		std::stringstream hex_stream("D2FE28");

		aoc::comms::BITS::IStream bit_stream(hex_stream);

		constexpr auto expected_bits = "110100101111111000101000";
		const auto len = std::strlen(expected_bits);
		const auto mismatching_bit = std::mismatch(expected_bits, expected_bits + len, std::istreambuf_iterator<char>(bit_stream));
		Assert::IsTrue(mismatching_bit.first == expected_bits + len, std::format(L"Mismatching bit at {}", std::distance(expected_bits, mismatching_bit.first)).c_str());
	}

	TEST_METHOD(DecompressExample2)
	{
		std::stringstream hex_stream("38006F45291200");

		aoc::comms::BITS::IStream bit_stream(hex_stream);

		constexpr auto expected_bits = "00111000000000000110111101000101001010010001001000000000";
		const auto len = std::strlen(expected_bits);
		const auto mismatching_bit = std::mismatch(expected_bits, expected_bits + len, std::istreambuf_iterator<char>(bit_stream));
		Assert::IsTrue(mismatching_bit.first == expected_bits + len, std::format(L"Mismatching bit at {}", std::distance(expected_bits, mismatching_bit.first)).c_str());
	}

	TEST_METHOD(DecompressExample3)
	{
		std::stringstream hex_stream("EE00D40C823060");

		aoc::comms::BITS::IStream bit_stream(hex_stream);

		constexpr auto expected_bits = "11101110000000001101010000001100100000100011000001100000";
		const auto len = std::strlen(expected_bits);
		const auto mismatching_bit = std::mismatch(expected_bits, expected_bits + len, std::istreambuf_iterator<char>(bit_stream));
		Assert::IsTrue(mismatching_bit.first == expected_bits + len, std::format(L"Mismatching bit at {}", std::distance(expected_bits, mismatching_bit.first)).c_str());
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
		std::stringstream bits(   "1101001010101");

		const auto header = aoc::comms::BITS::Header{ bits };

		Assert::AreEqual(aoc::comms::BITS::PacketType::literal_value, header.type());
	}
};

TEST_CLASS(PacketDeserialisation)
{
public:
	TEST_METHOD(LiteralValue)
	{
		std::stringstream hex_data{"D2FE28"};
		aoc::comms::BITS::IStream bits{ hex_data };

		auto packet = aoc::comms::BITS::Packet{};
		bits >> packet;

		Assert::AreEqual(uint64_t{ 2021 }, packet.value());
	}
	TEST_METHOD(OperatorPacket1)
	{
		std::stringstream hex_data{ "38006F45291200" };
		aoc::comms::BITS::IStream bits{ hex_data };

		auto packet = aoc::comms::BITS::Packet{};
		bits >> packet;

	}
};

TEST_CLASS(OperatorPacket)
{
public:
	TEST_METHOD(TotalLengthChildPacketsAreCorrect)
	{
		std::stringstream hex_data{ "38006F45291200" };
		aoc::comms::BITS::IStream bits{ hex_data };

		auto packet = aoc::comms::BITS::Packet{};
		bits >> packet;

		const auto packets = packet.children();
		Assert::AreEqual(size_t{ 2 }, packets.size());

		Assert::AreEqual(uint8_t{ 6 }, packets[0]->version());
		Assert::AreEqual(uint64_t{ 10 }, packets[0]->value());

		Assert::AreEqual(uint8_t{ 2 }, packets[1]->version());
		Assert::AreEqual(uint64_t{ 20 }, packets[1]->value());
	}

	TEST_METHOD(TotalNumberChildPacketsAreCorrect)
	{
		std::stringstream hex_data{ "EE00D40C823060" };
		aoc::comms::BITS::IStream bits{ hex_data };

		auto packet = aoc::comms::BITS::Packet{};
		bits >> packet;

		const auto packets = packet.children();
		Assert::AreEqual(size_t{ 3 }, packets.size());

		Assert::AreEqual(uint8_t{ 2 }, packets[0]->version());
		Assert::AreEqual(uint64_t{ 1 }, packets[0]->value());

		Assert::AreEqual(uint8_t{ 4 }, packets[1]->version());
		Assert::AreEqual(uint64_t{ 2 }, packets[1]->value());

		Assert::AreEqual(uint8_t{ 1 }, packets[2]->version());
		Assert::AreEqual(uint64_t{ 3 }, packets[2]->value());
	}

	TEST_METHOD(SumPacketsHaveTheCorrectValue)
	{
		std::stringstream hex_data{ "C200B40A82" };
		aoc::comms::BITS::IStream bits{ hex_data };

		auto packet = aoc::comms::BITS::Packet{};
		bits >> packet;

		const auto child_packets = packet.children();
		Assert::AreEqual(size_t{ 2 }, child_packets.size());

		Assert::AreEqual(uint64_t{ 1 }, child_packets[0]->value());
		Assert::AreEqual(uint64_t{ 2 }, child_packets[1]->value());

		Assert::AreEqual(uint64_t{ 3 }, packet.value());
	}

	TEST_METHOD(ProductPacketsHaveTheCorrectValue)
	{
		std::stringstream hex_data{ "04005AC33890" };
		aoc::comms::BITS::IStream bits{ hex_data };

		auto packet = aoc::comms::BITS::Packet{};
		bits >> packet;

		const auto child_packets = packet.children();
		Assert::AreEqual(size_t{ 2 }, child_packets.size());

		Assert::AreEqual(uint64_t{ 6 }, child_packets[0]->value());
		Assert::AreEqual(uint64_t{ 9 }, child_packets[1]->value());

		Assert::AreEqual(uint64_t{ 54 }, packet.value());
	}

	TEST_METHOD(MinValuePacketsHaveTheCorrectValue)
	{
		std::stringstream hex_data{ "880086C3E88112" };
		aoc::comms::BITS::IStream bits{ hex_data };

		auto packet = aoc::comms::BITS::Packet{};
		bits >> packet;

		const auto child_packets = packet.children();
		Assert::AreEqual(size_t{ 3 }, child_packets.size());

		Assert::AreEqual(uint64_t{ 7 }, child_packets[0]->value());
		Assert::AreEqual(uint64_t{ 8 }, child_packets[1]->value());
		Assert::AreEqual(uint64_t{ 9 }, child_packets[2]->value());

		Assert::AreEqual(uint64_t{ 7 }, packet.value());
	}

	TEST_METHOD(MaxValuePacketsHaveTheCorrectValue)
	{
		std::stringstream hex_data{ "CE00C43D881120" };
		aoc::comms::BITS::IStream bits{ hex_data };

		auto packet = aoc::comms::BITS::Packet{};
		bits >> packet;

		const auto child_packets = packet.children();
		Assert::AreEqual(size_t{ 3 }, child_packets.size());

		Assert::AreEqual(uint64_t{ 7 }, child_packets[0]->value());
		Assert::AreEqual(uint64_t{ 8 }, child_packets[1]->value());
		Assert::AreEqual(uint64_t{ 9 }, child_packets[2]->value());

		Assert::AreEqual(uint64_t{ 9 }, packet.value());
	}
};

TEST_CLASS(PacketEnumerator)
{
public:
	TEST_METHOD(SumVersions1)
	{
		std::stringstream hex_data{ "8A004A801A8002F478" };
		aoc::comms::BITS::IStream bits{ hex_data };

		auto packet = aoc::comms::BITS::Packet{};
		bits >> packet;

		const auto version_sum = aoc::comms::BITS::PacketEnumerator{ packet }.reduce([](auto& current, auto&& pkt) {
				return current + pkt.version();
			}, uint32_t{ 0 });

		Assert::AreEqual(uint32_t{ 16 }, version_sum);
	}

	TEST_METHOD(SumVersions2)
	{
		std::stringstream hex_data{ "620080001611562C8802118E34" };
		aoc::comms::BITS::IStream bits{ hex_data };

		auto packet = aoc::comms::BITS::Packet{};
		bits >> packet;

		const auto version_sum = aoc::comms::BITS::PacketEnumerator{ packet }.reduce([](auto& current, auto&& pkt) {
			return current + pkt.version();
			}, uint32_t{ 0 });

		Assert::AreEqual(uint32_t{ 12 }, version_sum);
	}

	TEST_METHOD(SumVersions3)
	{
		std::stringstream hex_data{ "C0015000016115A2E0802F182340" };
		aoc::comms::BITS::IStream bits{ hex_data };

		auto packet = aoc::comms::BITS::Packet{};
		bits >> packet;

		const auto version_sum = aoc::comms::BITS::PacketEnumerator{ packet }.reduce([](auto& current, auto&& pkt) {
			return current + pkt.version();
			}, uint32_t{ 0 });

		Assert::AreEqual(uint32_t{ 23 }, version_sum);
	}

	TEST_METHOD(SumVersions4)
	{
		std::stringstream hex_data{ "A0016C880162017C3686B18A3D4780" };
		aoc::comms::BITS::IStream bits{ hex_data };

		auto packet = aoc::comms::BITS::Packet{};
		bits >> packet;

		const auto version_sum = aoc::comms::BITS::PacketEnumerator{ packet }.reduce([](auto& current, auto&& pkt) {
			return current + pkt.version();
			}, uint32_t{ 0 });

		Assert::AreEqual(uint32_t{ 31 }, version_sum);
	}
};
}
