#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "StaticMap.hpp"

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{
namespace comms
{
namespace BITS
{

///////////////////////////////////////////////////////////////////////////////

using HexStream_t = std::istream;

///////////////////////////////////////////////////////////////////////////////

constexpr auto bit_masks = std::array{
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000001 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000010 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000100 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00001000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00010000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00100000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'01000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'10000000 },

		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'00000001'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'00000010'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'00000100'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'00001000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'00010000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'00100000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'01000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000000'10000000'00000000 },

		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000001'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000010'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00000100'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00001000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00010000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'00100000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'01000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000000'10000000'00000000'00000000 },

		uint64_t{0b00000000'00000000'00000000'00000000'00000001'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000010'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00000100'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00001000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00010000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'00100000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'01000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000000'10000000'00000000'00000000'00000000 },

		uint64_t{0b00000000'00000000'00000000'00000001'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000010'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00000100'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00001000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00010000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'00100000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'01000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000000'10000000'00000000'00000000'00000000'00000000 },

		uint64_t{0b00000000'00000000'00000001'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000010'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00000100'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00001000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00010000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'00100000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'01000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000000'10000000'00000000'00000000'00000000'00000000'00000000 },

		uint64_t{0b00000000'00000001'00000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000010'00000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00000100'00000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00001000'00000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00010000'00000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'00100000'00000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'01000000'00000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000000'10000000'00000000'00000000'00000000'00000000'00000000'00000000 },

		uint64_t{0b00000001'00000000'00000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000010'00000000'00000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00000100'00000000'00000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00001000'00000000'00000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00010000'00000000'00000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b00100000'00000000'00000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b01000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000 },
		uint64_t{0b10000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000 }
};

template<typename Result_T, size_t SIZE>
std::enable_if_t<std::is_integral_v<Result_T> && SIZE <= 64, Result_T> bit_chars_to_value(const std::array<char, SIZE>& bits)
{
	auto out = Result_T{ 0 };

	for (size_t i = 0; i < SIZE; ++i) {
		if (bits[i] != '0') {
			out |= bit_masks[SIZE - i - 1];
		}
	}

	return out;
}

template<typename Result_T>
std::enable_if_t<std::is_integral_v<Result_T>, Result_T> bit_chars_to_value(const std::vector<char>& bits)
{
	if (bits.size() > 64) {
		throw OutOfRangeException("Bits vector is too large (>64)");
	}

	auto out = Result_T{ 0 };

	for (size_t i = 0; i < bits.size(); ++i) {
		if (bits[i] != '0') {
			out |= bit_masks[bits.size() - i - 1];
		}
	}

	return out;
}

///////////////////////////////////////////////////////////////////////////////

class Streambuf : public std::streambuf
{
public:
	Streambuf(std::streambuf* hex_char_buf)
		: _hex_char_buf{ hex_char_buf }
	{
	}

	std::streambuf::int_type underflow() override
	{
		if (_hex_char_buf->sgetn(&_hex_char, 1) != 1) {
			return traits_type::eof();
		}

		_update_bit_buffer(_hex_char);

		setg(_bit_buffer->data(), _bit_buffer->data(), std::next(_bit_buffer->data(), _bit_buffer->size()));

		return traits_type::to_int_type(*gptr());
	}

	int sync() override
	{
		auto result = this->overflow(traits_type::eof());
		_hex_char_buf->pubsync();
		return traits_type::eq_int_type(result, traits_type::eof()) ? -1 : 0;
	}

private:

	void _update_bit_buffer(char hex_char)
	{
		static StaticMap<char, std::array<char, 4>, 16> hex_to_bits
		{
			{'0', std::array{'0','0','0','0'}},
			{'1', std::array{'0','0','0','1'}},
			{'2', std::array{'0','0','1','0'}},
			{'3', std::array{'0','0','1','1'}},
			{'4', std::array{'0','1','0','0'}},
			{'5', std::array{'0','1','0','1'}},
			{'6', std::array{'0','1','1','0'}},
			{'7', std::array{'0','1','1','1'}},
			{'8', std::array{'1','0','0','0'}},
			{'9', std::array{'1','0','0','1'}},
			{'A', std::array{'1','0','1','0'}},
			{'B', std::array{'1','0','1','1'}},
			{'C', std::array{'1','1','0','0'}},
			{'D', std::array{'1','1','0','1'}},
			{'E', std::array{'1','1','1','0'}},
			{'F', std::array{'1','1','1','1'}},
		};

		_bit_buffer = &hex_to_bits.at(hex_char);
	}

	std::streambuf* _hex_char_buf;
	char _hex_char;
	std::array<char, 4>* _bit_buffer;
};

///////////////////////////////////////////////////////////////////////////////

class IStream : public std::istream
{
public:
	IStream(std::istream& is)
		: std::istream{ new Streambuf{is.rdbuf()} }
	{}

	virtual ~IStream()
	{
		delete rdbuf();
	}
};

///////////////////////////////////////////////////////////////////////////////


template<size_t SIZE>
static std::array<char, SIZE> extract_bits(std::istream& is)
{
	auto bits = std::array<char, SIZE>{};
	is.read(bits.data(), SIZE);
	if (is.fail()) {
		throw IOException("Failed to read bits from stream");
	}

	return bits;
}

///////////////////////////////////////////////////////////////////////////////

template<size_t SIZE>
static uint8_t read_bit_value(std::istream& is)
{
	return bit_chars_to_value<uint8_t, SIZE>(extract_bits<SIZE>(is));
}

///////////////////////////////////////////////////////////////////////////////

enum class PacketType
{
	operation_0,
	operation_1,
	operation_2,
	operation_3,
	literal_value,
	operation_5,
	operation_6,
	operation_7
};

///////////////////////////////////////////////////////////////////////////////

class Header
{
public:

	Header(std::istream& is)
		: _version{ read_bit_value<3>(is) }
		, _type{ static_cast<PacketType>(read_bit_value<3>(is))}
	{
	}

	uint8_t version() const { return _version; }
	PacketType type() const { return _type; }

private:

	uint8_t _version;
	PacketType _type;
};

///////////////////////////////////////////////////////////////////////////////

class LiteralValuePacket
{
public:
	LiteralValuePacket() : _version{ 0 } {}
	LiteralValuePacket(uint8_t version) : _version{ version } {}

	LiteralValuePacket(const LiteralValuePacket&) = delete;
	LiteralValuePacket& operator=(const LiteralValuePacket&) = delete;

	LiteralValuePacket(LiteralValuePacket&&) = default;
	LiteralValuePacket& operator=(LiteralValuePacket&&) = default;

	std::streamsize from_stream(std::istream& is)
	{
		_reset();
		const auto items_consumed = _read_bit_groups(is);
		return _consume_padding_bits_from_stream(is, items_consumed);
	}

	uint64_t value() const
	{
		if (_cached_value) {
			return *_cached_value;
		}

		_update_cached_value();

		return *_cached_value;
	}

private:

	void _reset()
	{
		_bits.clear();
		_cached_value.reset();
	}

	std::streamsize _read_bit_groups(std::istream& is)
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

	std::streamsize _consume_padding_bits_from_stream(std::istream& is, std::streamsize items_consumed)
	{
		const auto addition_items_to_consume = items_consumed % 4;
		is.seekg(is.tellg() + addition_items_to_consume);

		return items_consumed + addition_items_to_consume;
	}

	void _update_cached_value() const
	{
		_cached_value = bit_chars_to_value<uint64_t>(_bits);
	}

	uint8_t _version;
	std::vector<char> _bits;
	mutable std::optional<uint64_t> _cached_value;
};

///////////////////////////////////////////////////////////////////////////////

class Packet;

///////////////////////////////////////////////////////////////////////////////

class OperatorPacket
{
public:
	~OperatorPacket();

	OperatorPacket() : _version{ 0 } {}
	OperatorPacket(uint8_t version) : _version{ version } {}

	OperatorPacket(const OperatorPacket&) = delete;
	OperatorPacket& operator=(const OperatorPacket&) = delete;

	OperatorPacket(OperatorPacket&&) = default;
	OperatorPacket& operator=(OperatorPacket&&) = default;

	std::streamsize from_stream(std::istream& is);

private:
	uint8_t _version;
	std::vector<std::unique_ptr<Packet>> _child_packets;
};

///////////////////////////////////////////////////////////////////////////////

class Packet : public std::variant<LiteralValuePacket, OperatorPacket>
{
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

public:

	using Base_t = std::variant<LiteralValuePacket, OperatorPacket>;

	using Base_t::operator=;

	std::streamsize from_stream(std::istream& is)
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

		return std::visit(StreamDeserialiser{ is }, *this);
	}

	uint64_t value() const
	{
		return std::visit(ValueExtractor(), *this);
	}
};

///////////////////////////////////////////////////////////////////////////////

}
}
}

///////////////////////////////////////////////////////////////////////////////

std::istream& operator>>(std::istream& is, aoc::comms::BITS::LiteralValuePacket& packet);
std::istream& operator>>(std::istream& is, aoc::comms::BITS::OperatorPacket& packet);
std::istream& operator>>(std::istream& is, aoc::comms::BITS::Packet& packet);

///////////////////////////////////////////////////////////////////////////////
