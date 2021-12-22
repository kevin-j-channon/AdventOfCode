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
		uint64_t value;
	};

	SyntaxChecker& score_lines(std::istream& is)
	{
		auto line = std::string{};

		_syntax_error_score = uint64_t{ 0 };
		_incomplete_line_scores.clear();

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

	uint64_t syntax_error_score() const { return _syntax_error_score; }

	uint64_t incomplete_line_score() const
	{
		if (_incomplete_line_scores.empty()) {
			return 0;
		}

		return _incomplete_line_scores[_incomplete_line_scores.size() / 2];
	}

	const std::vector<uint64_t>& incomplete_line_scores() const { return _incomplete_line_scores; }

	static Score score_line(const std::string& line)
	{
		auto [chunk_stack, error_char] = _parse_line(line);

		if (chunk_stack.empty()) {
			return {LineType::complete, 0};
		}

		if (std::nullopt == error_char) {
			return _score_incomplete_line(chunk_stack);
		}

		return _score_syntax_error(*error_char);
	}

	bool check_incomplete_lines(std::istream& data, std::istream& opennings) const
	{
		auto incomplete_opennings = std::vector<std::string>{};
		while (!opennings.eof()) {
			auto line = std::string{};
			std::getline(opennings, line);
			incomplete_opennings.push_back(std::move(line));
		}

		auto it_openning = incomplete_opennings.begin();

		while (!data.eof()) {
			auto line = std::string{};
			std::getline(data, line);

			auto [stack, error] = _parse_line(line);
			if (!error) {
				auto completion_string = std::string(stack.size(), '\0');
				std::generate(completion_string.begin(), completion_string.end(), [&stack]() -> Stack_t::value_type {
					const auto c = stack.top();
					stack.pop();
					return c;
					});

				auto check_line = *(it_openning++) + completion_string;

				const auto [s2, _] = _parse_line(check_line);

				if (!s2.empty()) {
					return false;
				}
			}
		}

		return true;
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

	static Score _score_incomplete_line(std::stack<char>& chunk_stack)
	{
		auto out = uint64_t{ 0 };

		while (!chunk_stack.empty()) {
			out *= 5;

			switch (chunk_stack.top()) {
			case ')': out += 1; break;
			case ']': out += 2; break;
			case '}': out += 3; break;
			case '>': out += 4; break;
			default:
				throw Exception(std::format("Invalid character: {}", chunk_stack.top()));
			}

			chunk_stack.pop();
		}

		return { LineType::incomplete, out };
	}

	static Score _score_syntax_error(char error_char)
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

	std::vector<uint64_t> _incomplete_line_scores;
	uint64_t _syntax_error_score{ 0 };
};

}
