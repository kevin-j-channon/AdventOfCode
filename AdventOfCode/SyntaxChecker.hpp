#pragma once

#include "Common.hpp"

#include <istream>
#include <string>
#include <vector>
#include <cstdint>
#include <stack>


namespace aoc
{

class SyntaxChecker
{
public:
	static uint32_t score_lines(std::istream& is)
	{
		auto line = std::string{};

		auto out = uint32_t{ 0 };

		while (!is.eof()) {
			std::getline(is, line);
			out += score_line(line);
		}

		return out;
	}

	static uint32_t score_line(const std::string& line)
	{
		auto chunk_stack = std::stack<char>{};
		const auto error = std::find_if(line.begin(), line.end(), [&chunk_stack](auto c) {
				switch (c) {
				case '(': {
					chunk_stack.push(')');
					return false;
				}
				case '[': {
					chunk_stack.push(']');
					return false;
				}
				case '{': {
					chunk_stack.push('}');
					return false;
				}
				case '<': {
					chunk_stack.push('>');
					return false;
				}

				default:;
				}

				if (chunk_stack.top() == c) {
					chunk_stack.pop();
					return false;
				}

				return true;
			});

		if (error == line.end()) {
			return 0;
		}

		switch(chunk_stack.top()) {
		case ')': return 3;
		case ']': return 57;
		case '}': return 1197;
		case '>': return 25137;
		default:;
		}

		throw Exception(std::format("Invalid character: {}", chunk_stack.top()));
	}
};

}
