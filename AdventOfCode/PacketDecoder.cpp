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

namespace
{

///////////////////////////////////////////////////////////////////////////////

template<typename> static constexpr auto always_false_v = false;

///////////////////////////////////////////////////////////////////////////////

struct ValueExtractor
{
	uint64_t operator()(const LiteralValuePacket& literal_value) const
	{
		return literal_value.value();
	}

	uint64_t operator()(const OperatorPacket& op_packet) const
	{
		return 0;
	}
};

///////////////////////////////////////////////////////////////////////////////

struct StreamDeserialiser
{
	std::istream& _is;

	StreamDeserialiser(std::istream& is) : _is{ is } {}

	std::streamsize operator()(LiteralValuePacket& literal_packet)
	{
		return literal_packet.from_stream(_is);
	}

	std::streamsize operator()(OperatorPacket& op_packet)
	{
		return op_packet.from_stream(_is);
	}
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////

std::streambuf::int_type Streambuf::underflow()
{
	if (_hex_char_buf->sgetn(&_hex_char, 1) != 1) {
		return traits_type::eof();
	}

	_update_bit_buffer(_hex_char);

	setg(_bit_buffer->data(), _bit_buffer->data(), std::next(_bit_buffer->data(), _bit_buffer->size()));

	return traits_type::to_int_type(*gptr());
}

///////////////////////////////////////////////////////////////////////////////

int Streambuf::sync()
{
	auto result = this->overflow(traits_type::eof());
	_hex_char_buf->pubsync();
	return traits_type::eq_int_type(result, traits_type::eof()) ? -1 : 0;
}

///////////////////////////////////////////////////////////////////////////////

void Streambuf::_update_bit_buffer(char hex_char)
{
	static StaticMap<char, std::array<char, 4>, 16> hex_to_bits
	{
		{ '0', std::array{ '0','0','0','0' } },
		{ '1', std::array{ '0','0','0','1' } },
		{ '2', std::array{ '0','0','1','0' } },
		{ '3', std::array{ '0','0','1','1' } },
		{ '4', std::array{ '0','1','0','0' } },
		{ '5', std::array{ '0','1','0','1' } },
		{ '6', std::array{ '0','1','1','0' } },
		{ '7', std::array{ '0','1','1','1' } },
		{ '8', std::array{ '1','0','0','0' } },
		{ '9', std::array{ '1','0','0','1' } },
		{ 'A', std::array{ '1','0','1','0' } },
		{ 'B', std::array{ '1','0','1','1' } },
		{ 'C', std::array{ '1','1','0','0' } },
		{ 'D', std::array{ '1','1','0','1' } },
		{ 'E', std::array{ '1','1','1','0' } },
		{ 'F', std::array{ '1','1','1','1' } },
	};

	_bit_buffer = &hex_to_bits.at(hex_char);
}

///////////////////////////////////////////////////////////////////////////////

std::streamsize LiteralValuePacket::from_stream(std::istream& is)
{
	_reset();
	const auto items_consumed = _read_bit_groups(is);
	return items_consumed; // _consume_padding_bits_from_stream(is, items_consumed);
}

///////////////////////////////////////////////////////////////////////////////

uint64_t LiteralValuePacket::value() const
{
	if (_cached_value) {
		return *_cached_value;
	}

	_update_cached_value();

	return *_cached_value;
}

///////////////////////////////////////////////////////////////////////////////

void LiteralValuePacket::_reset()
{
	_bits.clear();
	_cached_value.reset();
}

///////////////////////////////////////////////////////////////////////////////

std::streamsize LiteralValuePacket::_read_bit_groups(std::istream& is)
{
	std::streamsize items_consumed = 0;

	while (true) {
		const auto bits_from_stream = aoc::comms::BITS::extract_bits<5>(is);
		items_consumed += 5;

		_bits.insert(_bits.end(), std::next(bits_from_stream.begin()), bits_from_stream.end());

		if (bits_from_stream[0] == '0') {
			break;
		}
	}

	return items_consumed;
}

///////////////////////////////////////////////////////////////////////////////

std::streamsize LiteralValuePacket::_consume_padding_bits_from_stream(std::istream& is, std::streamsize items_consumed)
{
	const auto addition_items_to_consume = items_consumed % 4;
	is.seekg(is.tellg() + addition_items_to_consume);

	return items_consumed + addition_items_to_consume;
}

///////////////////////////////////////////////////////////////////////////////

void LiteralValuePacket::_update_cached_value() const
{
	_cached_value = bit_chars_to_value<uint64_t>(_bits);
}

///////////////////////////////////////////////////////////////////////////////

aoc::comms::BITS::OperatorPacket::~OperatorPacket()
{
	// This is here, even though it's empty, so that we can forward declare Packet to be held by this class as a unique_ptr.
}

///////////////////////////////////////////////////////////////////////////////

std::streamsize aoc::comms::BITS::OperatorPacket::from_stream(std::istream& is)
{
	auto length_type = char{};

	is >> length_type;
	auto bits_consumed = std::streamsize{ 1 };
	
	switch (static_cast<LengthType>(length_type)) {
	case LengthType::toal_length: {
		const auto total_length = read_bit_value<15>(is);
		bits_consumed += 15;

		const auto bits_consumed_in_preamble = bits_consumed;

		while ((bits_consumed - bits_consumed_in_preamble) < total_length) {
			bits_consumed += _deserialize_and_add_subpackets(is);
		}
	} break;
	case LengthType::number_of_packets: {
		const auto number_of_packets = read_bit_value<11>(is);
		bits_consumed += 11;

		for (auto pkt_idx = 0; pkt_idx < number_of_packets; ++pkt_idx) {
			bits_consumed += _deserialize_and_add_subpackets(is);
		}
	} break;
	default:
		throw IOException("Invalid length type");
	}

	return bits_consumed;
}

///////////////////////////////////////////////////////////////////////////////

std::vector<const Packet*> OperatorPacket::children() const
{
	auto out = std::vector<const Packet*>(_child_packets.size(), nullptr);
	std::transform(_child_packets.begin(), _child_packets.end(), out.begin(), [](auto&& pkt) { return pkt.get(); });
	return out;
}

///////////////////////////////////////////////////////////////////////////////

std::streamsize OperatorPacket::_deserialize_and_add_subpackets(std::istream& is)
{
	auto child_packet = std::make_unique<Packet>();
	const auto bits_consumed = child_packet->from_stream(is);
	_child_packets.push_back(std::move(child_packet));

	return bits_consumed;
}

///////////////////////////////////////////////////////////////////////////////

std::streamsize Packet::from_stream(std::istream& is)
{
	const auto header = BITS::Header{ is };

	switch (header.type()) {
	case aoc::comms::BITS::PacketType::operation_0:
	case aoc::comms::BITS::PacketType::operation_1:
	case aoc::comms::BITS::PacketType::operation_2:
	case aoc::comms::BITS::PacketType::operation_3:
	case aoc::comms::BITS::PacketType::operation_5:
	case aoc::comms::BITS::PacketType::operation_6:
	case aoc::comms::BITS::PacketType::operation_7: {
		*this = OperatorPacket{ header.version() };
		break;
	}
	case aoc::comms::BITS::PacketType::literal_value: {
		*this = LiteralValuePacket{ header.version() };
		break;
	}
	default:
		throw aoc::IOException("Invalid packet type");
	}

	// The header is always 6 bits, so we add those now too.
	return 6 + std::visit(StreamDeserialiser{ is }, *this);
}

///////////////////////////////////////////////////////////////////////////////

uint64_t Packet::value() const
{
	return std::visit(ValueExtractor(), *this);
}

///////////////////////////////////////////////////////////////////////////////

uint8_t Packet::version() const
{
	return std::visit([](auto&& arg) -> uint8_t {
		return arg.version();
		}, *this);
}

///////////////////////////////////////////////////////////////////////////////

std::vector<const Packet*> Packet::children() const
{
	return std::visit([](auto&& arg) -> std::vector<const Packet*> {
		using Arg_t = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<Arg_t, LiteralValuePacket>) {
			return std::vector<const Packet*>();
		}
		else if constexpr (std::is_same_v<Arg_t, OperatorPacket>) {
			return arg.children();
		}
		else {
			static_assert(always_false_v<Arg_t>, "Unhandled variant type");
		}
		}, *this);
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
