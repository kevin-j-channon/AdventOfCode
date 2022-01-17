#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "StaticMap.hpp"

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{
namespace comms
{

///////////////////////////////////////////////////////////////////////////////

using HexStream_t = std::istream;

///////////////////////////////////////////////////////////////////////////////

class BitsStreambuf : public std::streambuf
{
public:
	BitsStreambuf(std::streambuf* hex_char_buf)
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

class BitsInputStream : public std::istream
{
public:
	BitsInputStream(std::istream& is)
		: std::istream{ new BitsStreambuf{is.rdbuf()} }
	{}

	virtual ~BitsInputStream()
	{
		delete rdbuf();
	}
};

///////////////////////////////////////////////////////////////////////////////

class LiteralValuePacket {};
class OperatorPacket {};

using BitsPacket = std::variant<LiteralValuePacket, OperatorPacket>;

///////////////////////////////////////////////////////////////////////////////

}
}

///////////////////////////////////////////////////////////////////////////////
