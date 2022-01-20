#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "StaticMap.hpp"
#include "Common.hpp"

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
	{}

	std::streambuf::int_type underflow() override;
	int sync() override;

private:

	void _update_bit_buffer(char hex_char);

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

	std::streamsize from_stream(std::istream& is);
	uint64_t value() const;
	uint8_t version() const { return _version; }

private:

	void _reset();
	std::streamsize _read_bit_groups(std::istream& is);
	std::streamsize _consume_padding_bits_from_stream(std::istream& is, std::streamsize items_consumed);
	void _update_cached_value() const;

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
	std::vector<const Packet*> children() const;
	uint8_t version() const { return _version; }

private:

	enum class LengthType
	{
		toal_length = '0',
		number_of_packets = '1'
	};

	std::streamsize _read_total_length_block(std::istream& is);
	std::streamsize _read_total_number_blocks(std::istream& is);
	std::streamsize _deserialize_and_add_subpackets(std::istream& is);

	uint8_t _version;
	std::vector<std::unique_ptr<Packet>> _child_packets;
};

///////////////////////////////////////////////////////////////////////////////

class Packet : public std::variant<LiteralValuePacket, OperatorPacket>
{
public:

	using Base_t = std::variant<LiteralValuePacket, OperatorPacket>;

	using Base_t::operator=;

	std::streamsize from_stream(std::istream& is);
	uint64_t value() const;
	uint8_t version() const;

	std::vector<const Packet*> children() const;
};

///////////////////////////////////////////////////////////////////////////////

class PacketEnumerator
{
public:
	PacketEnumerator(const Packet& root_packet)
		: _root(root_packet)
	{}

	template<typename Result_T, typename Reduce_T>
	Result_T reduce(Reduce_T reducer, Result_T initial_value = Result_T{})
	{
		_recursive_apply_reduce(_root, reducer, initial_value);
		return initial_value;
	}

private:

	template<typename Result_T, typename Reduce_T>
	void _recursive_apply_reduce(const Packet& pkt, Reduce_T& reducer, Result_T& result) const
	{
		result = reducer(result, pkt);
		
		for (const auto* child_pkt : pkt.children()) {
			_recursive_apply_reduce(*child_pkt, reducer, result);
		}
	}

	const Packet& _root;
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
