#pragma once

#include "Common.hpp"
#include "StaticMap.hpp"

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
	using Stack_t = std::stack<char>;

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
		_incomplete_line_scores.clear();

		auto i = 0;

		while (!is.eof()) {
			std::getline(is, line);

			Logger::WriteMessage(std::format("{}:\n", i++).c_str());
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

	const std::vector<uint32_t>& incomplete_line_scores() const { return _incomplete_line_scores; }

	static Score score_line(const std::string& line)
	{
		auto [chunk_stack, error_char] = _parse_line(line);

		if (chunk_stack.empty()) {
			return {LineType::complete, 0};
		}

		if (std::nullopt == error_char) {
			return _handle_incomplete_line(chunk_stack);
		}

		return _handle_syntax_error(*error_char);
	}

private:

	struct IsSyntaxError
	{
		Stack_t& _chunk_stack;

		bool operator()(Stack_t::value_type c)
		{

			switch (c) {
			case '(': {
				_chunk_stack.push(')');
				return false;
			}
			case '[': {
				_chunk_stack.push(']');
				return false;
			}
			case '{': {
				_chunk_stack.push('}');
				return false;
			}
			case '<': {
				_chunk_stack.push('>');
				return false;
			}

			default:;
			}

			if (_chunk_stack.top() == c) {
				_chunk_stack.pop();
				return false;
			}

			return true;
		}
	};

	static std::pair<std::stack<char>, std::optional<char>> _parse_line(const std::string& line)
	{
		auto chunk_stack = std::stack<char>{};
		const auto error = std::find_if(line.begin(), line.end(), IsSyntaxError{ chunk_stack });

		if (error == line.end()) {
			return { std::move(chunk_stack), std::nullopt };
		}
		else {
			return { std::move(chunk_stack), *error };
		}
	}

	static Score _handle_incomplete_line(std::stack<char>& chunk_stack)
	{
		auto completion_string = std::string(chunk_stack.size(), '\0');
		std::generate(completion_string.begin(), completion_string.end(), [&chunk_stack]() -> Stack_t::value_type {
			const auto c = chunk_stack.top();
			chunk_stack.pop();
			return c;
			});

		Logger::WriteMessage(std::format("\tCompletion string: {}", completion_string).c_str());

		const auto out = std::accumulate(completion_string.begin(), completion_string.end(), uint32_t{ 0 }, [](auto curr, auto next) {
			switch (next) {
				case ')': return (5 * curr) + 1;
				case ']': return (5 * curr) + 2;
				case '}': return (5 * curr) + 3;
				case '>': return (5 * curr) + 4;
				default:
					throw Exception(std::format("Invalid character: {}", next));
			}
			});

		Logger::WriteMessage(std::format(", score: {}", out).c_str());

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
