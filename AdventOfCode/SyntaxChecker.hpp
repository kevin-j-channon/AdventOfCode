#pragma once

#include "Common.hpp"

#include <istream>
#include <string>
#include <vector>
#include <cstdint>
#include <stack>
#include <optional>
#include <utility>
#include <algorithm>


namespace aoc
{

class SyntaxChecker
{
public:

	enum class LineType
	{
		complete,
		incomplete,
		syntax_error
	};

	struct Score
	{
		LineType type;
		uint32_t value;
	};

	SyntaxChecker& score_lines(std::istream& is)
	{
		auto line = std::string{};

		_syntax_error_score = uint32_t{ 0 };

		while (!is.eof()) {
			std::getline(is, line);
			const auto [type, value] = score_line(line);
			switch(type) {
			case LineType::syntax_error: {
				_syntax_error_score += value;
				break;
			}
			case LineType::incomplete: {
				_incomplete_line_scores.push_back(value);
				break;
			}
			}
		}

		std::sort(_incomplete_line_scores.begin(), _incomplete_line_scores.end());

		return *this;
	}

	uint32_t syntax_error_score() const { return _syntax_error_score; }

	uint32_t incomplete_line_score() const
	{
		if (_incomplete_line_scores.empty()) {
			return 0;
		}

		return _incomplete_line_scores[_incomplete_line_scores.size() / 2];
	}

	static Score score_line(const std::string& line)
	{
		auto [chunk_stack, error_char] = _parse_line(line);

		if (chunk_stack.empty()) {
			return {LineType::complete, 0};
		}

		if (!error_char) {
			return _handle_incomplete_line(chunk_stack);
		}

		return _handle_syntax_error(*error_char);
	}

private:

	static std::pair<std::stack<char>, std::optional<char>> _parse_line(const std::string& line)
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
			return { std::move(chunk_stack), std::nullopt };
		}
		else {
			return { std::move(chunk_stack), *error };
		}
	}

	static Score _handle_incomplete_line(std::stack<char>& chunk_stack)
	{
		auto out = uint32_t{ 0 };

		while (!chunk_stack.empty()) {
			const auto c = chunk_stack.top();

			out *= 5;

			switch (c) {
			case ')': out += 1; break;
			case ']': out += 2; break;
			case '}': out += 3; break;
			case '>': out += 4; break;
			default:
				throw Exception(std::format("Invalid character: {}", c));
			}

			chunk_stack.pop();
		}

		return { LineType::incomplete, out };
	}

	static Score _handle_syntax_error(char error_char)
	{
		switch (error_char) {
		case ')': return { LineType::syntax_error, 3 };
		case ']': return { LineType::syntax_error, 57 };
		case '}': return { LineType::syntax_error, 1197 };
		case '>': return { LineType::syntax_error, 25137 };
		default:;
		}

		throw Exception(std::format("Invalid character: {}", error_char));
	}

	std::vector<uint32_t> _incomplete_line_scores;
	uint32_t _syntax_error_score{ 0 };
};

}
