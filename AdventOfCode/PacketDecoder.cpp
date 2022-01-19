#include "PacketDecoder.hpp"

#include "Common.hpp"

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{
namespace comms
{
namespace BITS
{

///////////////////////////////////////////////////////////////////////////////

aoc::comms::BITS::OperatorPacket::~OperatorPacket()
{
	// This is here, even though it's empty, so that we can forward declare Packet to but held by this class as a unique_ptr.
}

///////////////////////////////////////////////////////////////////////////////

std::streamsize aoc::comms::BITS::OperatorPacket::from_stream(std::istream& is)
{
	auto length_type = char{};
	
	is >> length_type;
	auto bits_consumed = 1;

	if (length_type == '0') {
		const auto total_length = read_bit_value<15>(is);
		while (bits_consumed < total_length) {
			auto child_packet = std::make_unique<Packet>();
			bits_consumed += child_packet->from_stream(is);
			_child_packets.push_back(std::move(child_packet));
		}

		return bits_consumed;
	}
	else {
		// TODO: add a recursive method to keep track of the total number of bits consumed.
		while (bits_consumed < total_length) {
			auto child_packet = std::make_unique<Packet>();
			bits_consumed += child_packet->from_stream(is);
			_child_packets.push_back(std::move(child_packet));
		}

		return bits_consumed;
	}
}

///////////////////////////////////////////////////////////////////////////////

}
}
}

///////////////////////////////////////////////////////////////////////////////

std::istream& operator>>(std::istream& is, aoc::comms::BITS::LiteralValuePacket& packet)
{
	packet.from_stream(is);
	return is;
}

///////////////////////////////////////////////////////////////////////////////

std::istream& operator>>(std::istream& is, aoc::comms::BITS::OperatorPacket& packet)
{
	packet.from_stream(is);
	return is;
}

///////////////////////////////////////////////////////////////////////////////

namespace
{
template<typename Impl_T>
Impl_T extract_packet_impl(std::istream& is, uint8_t version)
{
	auto packet_impl = Impl_T{ version };
	is >> packet_impl;
	return std::move(packet_impl);
}
}

///////////////////////////////////////////////////////////////////////////////

std::istream& operator>>(std::istream& is, aoc::comms::BITS::Packet& packet)
{
	using namespace aoc::comms;

	const auto header = BITS::Header{ is };

	switch (header.type()) {
	case aoc::comms::BITS::PacketType::operation_0:
	case aoc::comms::BITS::PacketType::operation_1:
	case aoc::comms::BITS::PacketType::operation_2:
	case aoc::comms::BITS::PacketType::operation_3:
	case aoc::comms::BITS::PacketType::operation_5:
	case aoc::comms::BITS::PacketType::operation_6:
	case aoc::comms::BITS::PacketType::operation_7: {
		packet = extract_packet_impl<BITS::OperatorPacket>(is, header.version());
		break;
	}
	case aoc::comms::BITS::PacketType::literal_value: {
		packet = extract_packet_impl<BITS::LiteralValuePacket>(is, header.version());
		break;
	}
	default:
		throw aoc::IOException("Invalid packet type");
	}

	return is;
}

///////////////////////////////////////////////////////////////////////////////
