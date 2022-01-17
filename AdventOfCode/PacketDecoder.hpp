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

class Header
{
public:

	Header(std::istream& is)
		: _version{ _read_3bit_value(is) }
		, _operation{ _read_3bit_value(is) }
	{
	}

	uint8_t version() const { return _version; }
	uint8_t operation() const { return _operation; }

private:

	static uint8_t _read_3bit_value(std::istream& is)
	{
		auto bit_chars = std::array{ '0', '0', '0' };
		is.read(bit_chars.data(), 3);
		if (is.fail()) {
			throw IOException("Failed to read header");
		}

		return _bits_to_value(bit_chars);
	}

	static uint8_t _bits_to_value(const std::array<char, 3>& chars)
	{
		static constexpr auto chars_to_value = StaticMap<std::array<char, 3>, uint8_t, 8>{
			{ std::array{ '0', '0', '0' }, 0 },
			{ std::array{ '0', '0', '1' }, 1 },
			{ std::array{ '0', '1', '0' }, 2 },
			{ std::array{ '0', '1', '1' }, 3 },
			{ std::array{ '1', '0', '0' }, 4 },
			{ std::array{ '1', '0', '1' }, 5 },
			{ std::array{ '1', '1', '0' }, 6 },
			{ std::array{ '1', '1', '1' }, 7 }
		};

		return chars_to_value.at(chars);
	}

	uint8_t _version;
	uint8_t _operation;
};

///////////////////////////////////////////////////////////////////////////////

class LiteralValuePacket {};
class OperatorPacket {};

using BitsPacket = std::variant<LiteralValuePacket, OperatorPacket>;

///////////////////////////////////////////////////////////////////////////////

}
}
}

///////////////////////////////////////////////////////////////////////////////
