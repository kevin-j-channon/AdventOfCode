#pragma once

#include "StringOperations.hpp"
#include "Exception.hpp"

#include <any>

namespace aoc
{
namespace snailfish
{

class Value
{
	struct Child : public std::variant<uint32_t, std::shared_ptr<Value>>
	{
		using std::variant<uint32_t, std::shared_ptr<Value>>::operator=;

		template<typename Char_T>
		std::basic_string<Char_T> to_string() const
		{
			return std::visit(
				[](auto&& arg) -> std::basic_string<Char_T> {
					using Arg_t = std::decay_t<decltype(arg)>;
					if constexpr (std::is_same_v<Arg_t, uint32_t>) {
						if constexpr (std::is_same_v<Char_T, char>) {
							return std::to_string(arg);
						}
						else if constexpr (std::is_same_v<Char_T, wchar_t>) {
							return std::to_wstring(arg);
						}
					}
					else if constexpr (std::is_same_v<Arg_t, std::shared_ptr<Value>>) {
						return arg->as_string<Char_T>();
					}
				},
				*this
			);
		}
	};

public:

	Value()
		: _children{ uint32_t{0}, uint32_t{0} }
	{}

	Value(uint32_t left, uint32_t right) 
		: _children{ left, right }
	{}

	auto operator<=>(const Value&) const = default;

	static std::shared_ptr<Value> from_stream(std::istream& is)
	{
		auto line = std::string{};
		std::getline(is, line);

		if (line.empty()) {
			throw IOException("Failed to create snailfish Value - empty line");
		}

		return create(line.begin(), line.end()).first;
	}

	template<typename Iter_T>
	static std::pair<std::shared_ptr<Value>, Iter_T> create(Iter_T current, Iter_T end)
	{
		if (*current != '[') {
			throw IOException("Failed to creaate snailfish Value - invalid openning");
		}

		auto out = std::make_unique<Value>();

		std::advance(current, 1);
		std::tie(out->_children.first, current) = _read_value_or_digits(std::move(current), end);

		if (current == end || *current != ',') {
			throw IOException("Failed to create snailfish value - Unexpected non-numeric value");
		}

		std::advance(current, 1);
		std::tie(out->_children.second, current) = _read_value_or_digits(std::move(current), end);

		if (current == end || *current != ']') {
			throw IOException("Failed to create snailfish value - Unexpected value termination");
		}

		// Consume the closing bracket at the end of the number.
		std::advance(current, 1);

		return { std::move(out), std::move(current) };
	}

	template<typename Char_T>
	std::basic_string<Char_T> as_string() const
	{
		if constexpr (std::is_same_v<Char_T, char>) {
			return std::format("[{},{}]", _children.first.to_string<Char_T>(), _children.second.to_string<Char_T>());
		}
		else if constexpr (std::is_same_v<Char_T, wchar_t>) {
			return std::format(L"[{},{}]", _children.first.to_string<Char_T>(), _children.second.to_string<Char_T>());
		}
	}

private:

	template<typename Iter_T>
	static std::pair<Child, Iter_T> _read_value_or_digits(Iter_T current, Iter_T end)
	{
		if (*current == '[') {
			const auto [out, current] = Value::create(current, end);
			return std::pair<Child, Iter_T>( Child(std::move(out)), std::move(current) );
		}
		else {
			const auto [out, current] = _read_digits(current, end);
			return std::pair<Child, Iter_T>( Child(std::move(out)), std::move(current) );
		}
	}

	template<typename Iter_T>
	static std::pair<uint32_t, Iter_T> _read_digits(Iter_T current, Iter_T end)
	{
		std::stringstream digits;
		for (; current != end && std::isdigit(*current); ++current) {
			digits << *current;
		}

		return { string_to<uint32_t>(digits.str()), current };
	}

	static bool _is_valid_value_terminator(char c)
	{
		switch (c) {
		case ',':
			return true;
		default:;
		}

		return false;
	}

	std::pair<Child, Child> _children;
};

}
}
