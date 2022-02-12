#pragma once

#include "StringOperations.hpp"
#include "Exception.hpp"

///////////////////////////////////////////////////////////////////////////////

using namespace std::string_literals;

namespace aoc
{
namespace snailfish
{

///////////////////////////////////////////////////////////////////////////////

enum class ChildPosition
{
	left,
	right
};

///////////////////////////////////////////////////////////////////////////////

template<ChildPosition POSITION>
constexpr ChildPosition complement()
{
	if constexpr (POSITION == ChildPosition::left) {
		return ChildPosition::right;
	}
	else {
		return ChildPosition::left;
	}
}

///////////////////////////////////////////////////////////////////////////////

inline ChildPosition complement(ChildPosition p)
{
	if (p == ChildPosition::left) {
		return complement<ChildPosition::left>();
	}
	else {
		return complement<ChildPosition::right>();
	}
}

///////////////////////////////////////////////////////////////////////////////

class Value;

using ValuePtr_t = std::shared_ptr<Value>;

namespace detail
{
class Child;
}

///////////////////////////////////////////////////////////////////////////////

void swap(Value& a, Value& b);

///////////////////////////////////////////////////////////////////////////////

class ValueExploder
{
public:
	ValueExploder(Value& v)
		: _value{ v }
	{}

	bool explode();

private:

	std::optional<std::pair<Value*, ChildPosition>> _find_first_child_to_explode();

	static std::optional<std::pair<Value*, ChildPosition>> _recursively_find_child_to_explode(Value& val, size_t depth, std::optional<ChildPosition> child_idx = std::nullopt);

	template<ChildPosition POSITION>
	static void _half_explode(Value& to_explode, ChildPosition position);

	template<ChildPosition POSITION>
	static Value* _find_first_predecessor_not_in_position(Value& value, std::optional<ChildPosition> position);

	template<ChildPosition POSITION>
	static detail::Child* _find_last_child_in_complent_position(Value* value);

	static void _apply_value_to_child(detail::Child& child, uint32_t value);
	void _zero_exploded_child(Value& value, ChildPosition position);

	Value& _value;
};

///////////////////////////////////////////////////////////////////////////////

namespace detail
{

///////////////////////////////////////////////////////////////////////////////

class ValueSplitter
{
public:
	ValueSplitter(Value& v)
		: _value{ v }
	{}

	bool split();

private:

	static std::optional<std::pair<Value*, ChildPosition>> _recursively_find_child_to_split(Value& val, std::optional<ChildPosition> position);

	template<ChildPosition POSITION>
	static std::optional<std::pair<Value*, ChildPosition>> _recursively_find_child_to_split(Value& val);

	void _split_child(Value& value, ChildPosition position);

	Value& _value;
};

///////////////////////////////////////////////////////////////////////////////

class Child;

void swap(Child& a, Child& b);

///////////////////////////////////////////////////////////////////////////////

class Child : std::variant<uint32_t, ValuePtr_t>
{
	using Base_t = std::variant<uint32_t, ValuePtr_t>;
public:
	Child();
	Child(uint32_t value);
	Child(ValuePtr_t value, Value* parent, ChildPosition position);
	Child(const Child& other, Value* parent, ChildPosition position);

	Child(const Child&) = delete;
	Child& operator=(const Child&) = delete;

	Child(Child&& other);

	Child& operator=(Child&& other);

	bool operator==(const Child& other) const;

	void set_parent_and_position(Value* parent, ChildPosition position);
	Child clone() const;

	friend void swap(Child& a, Child& b);

	template<typename T> constexpr bool is() const;
	template<typename Result_T> const Result_T& as() const;
	template<typename Result_T> Result_T& as();
	template<typename Char_T> std::basic_string<Char_T> as_string() const;

private:
	Base_t& _as_variant() { return static_cast<Base_t&>(*this); }
	const Base_t& _as_variant() const { return static_cast<const Base_t&>(*this); }

	template<ChildPosition POSITION>
	static detail::Child _clone_child_at(ValuePtr_t::element_type* value);
};

///////////////////////////////////////////////////////////////////////////////

} // namespace: detail

///////////////////////////////////////////////////////////////////////////////

class Value
{
	friend class detail::Child;
	friend class ValueExploder;
	friend class detail::ValueSplitter;
	friend void swap(Value& a, Value& b);

	using Children_t = std::pair<detail::Child, detail::Child>;
public:
	Value();
	Value(uint32_t first, uint32_t second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos = std::nullopt);
	Value(ValuePtr_t&& first, ValuePtr_t&& second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos = std::nullopt);
	Value(uint32_t first, ValuePtr_t&& second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos = std::nullopt);
	Value(ValuePtr_t&& first, uint32_t second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos = std::nullopt);
	Value(const Value& other);

	Value& operator=(const Value& other);
	Value(Value&& other);

	Value& operator=(Value&& other);
	bool operator==(const Value& other) const;
	bool operator!=(const Value& other) const;
	Value& operator+=(const Value& other);
	Value operator+(const Value& other);

	Value& reduce();

	Value* const parent() const;
	std::optional<ChildPosition> position() const;

	static Value from_stream(std::istream& is);
	static Value from_string(const std::string& str);

	template<typename Iter_T>
	static std::pair<ValuePtr_t, Iter_T> create(Iter_T current, Iter_T end, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos = std::nullopt);

	template<typename Char_T>
	std::basic_string<Char_T> as_string() const;

	template<ChildPosition POSITION>
	const detail::Child& child() const;

	template<ChildPosition POSITION>
	detail::Child& child();

	const detail::Child& child(ChildPosition position) const;
	detail::Child& child(ChildPosition position);

private:

	template<typename Iter_T>
	static void _validate_number_opening(const Iter_T& current);

	template<typename Iter_T>
	static std::pair<detail::Child, Iter_T> _recursively_read_child(Value& parent, ChildPosition position, Iter_T current, const Iter_T& end);

	template<typename Iter_T>
	static void _validate_second_value_is_present(const Iter_T& current, const Iter_T& end);

	template<typename Iter_T>
	static Iter_T _complete_value_read(Iter_T current, const Iter_T& end);

	template<typename Iter_T>
	static std::pair<detail::Child, Iter_T> _read_value_or_digits(Value& parent, ChildPosition position, Iter_T current, const Iter_T& end);

	template<typename Iter_T>
	static std::pair<uint32_t, Iter_T> _read_digits(Iter_T current, const Iter_T& end);

	ValuePtr_t _move_children_into_new_value();

	bool _explode();
	bool _split();

	std::optional<std::pair<Value*, ChildPosition>> _parent_and_position;
	Children_t _children;
};

///////////////////////////////////////////////////////////////////////////////

namespace detail
{

///////////////////////////////////////////////////////////////////////////////

template<typename T>
constexpr bool Child::is() const
{
	return std::visit([](auto&& arg) -> bool {
		using Arg_t = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<Arg_t, T>) {
			return true;
		}
		else {
			return false;
		}
		}, _as_variant());
}

///////////////////////////////////////////////////////////////////////////////

template<typename Result_T>
const Result_T& Child::as() const
{
	if constexpr (std::is_same_v<std::decay_t<Result_T>, Value>)
	{
		return *std::get<ValuePtr_t>(_as_variant());
	}
	else {
		return std::get<Result_T>(_as_variant());
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename Result_T>
Result_T& Child::as()
{
	if constexpr (std::is_same_v<std::decay_t<Result_T>, Value>)
	{
		return *std::get<ValuePtr_t>(_as_variant());
	}
	else {
		return std::get<Result_T>(_as_variant());
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename Char_T>
std::basic_string<Char_T> Child::as_string() const
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
			else if constexpr (std::is_same_v<Arg_t, ValuePtr_t>) {
				return arg->as_string<Char_T>();
			}
		},
		_as_variant()
			);
}

///////////////////////////////////////////////////////////////////////////////

template<ChildPosition POSITION>
detail::Child Child::_clone_child_at(ValuePtr_t::element_type* value)
{
	auto child = value->child<POSITION>().clone();
	if (child.is<ValuePtr_t>()) {
		child.as<Value>()._parent_and_position = { value, POSITION };
	}

	return child;
}

///////////////////////////////////////////////////////////////////////////////

}	// namespace: detail

///////////////////////////////////////////////////////////////////////////////

template<typename Iter_T>
static std::pair<ValuePtr_t, Iter_T> Value::create(Iter_T current, Iter_T end, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos)
{
	_validate_number_opening(current);

	auto out = std::make_shared<Value>();
	out->_parent_and_position = parent_and_pos;

	auto first_child = detail::Child{};
	auto second_child = detail::Child{};

	std::tie(first_child, current) = _recursively_read_child(*out, ChildPosition::left, std::move(current), end);

	_validate_second_value_is_present(current, end);

	std::tie(second_child, current) = _recursively_read_child(*out, ChildPosition::right, std::move(current), end);

	current = _complete_value_read(std::move(current), end);

	out->_children.first = std::move(first_child);
	out->_children.second = std::move(second_child);

	return { std::move(out), std::move(current) };
}

///////////////////////////////////////////////////////////////////////////////

template<typename Char_T>
std::basic_string<Char_T> Value::as_string() const
{
	auto get_fmt = []() {
		if constexpr (std::is_same_v<Char_T, char>) {
			return "[{},{}]";
		}
		else if constexpr (std::is_same_v<Char_T, wchar_t>) {
			return L"[{},{}]";
		}
	};

	return std::format(get_fmt(), _children.first.as_string<Char_T>(), _children.second.as_string<Char_T>());
}

///////////////////////////////////////////////////////////////////////////////

template<ChildPosition POSITION>
const detail::Child& Value::child() const
{
	if constexpr (POSITION == ChildPosition::left) {
		return _children.first;
	}
	else if constexpr (POSITION == ChildPosition::right) {
		return _children.second;
	}
	else {
		static_assert(std::false_type::value, "Invalid child position");
	}
}

///////////////////////////////////////////////////////////////////////////////

template<ChildPosition POSITION>
detail::Child& Value::child()
{
	return const_cast<detail::Child&>(const_cast<const Value*>(this)->child<POSITION>());
}

///////////////////////////////////////////////////////////////////////////////

template<typename Iter_T>
void Value::_validate_number_opening(const Iter_T& current)
{
	if (*current != '[') {
		throw IOException("Failed to creaate snailfish Value - invalid openning");
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename Iter_T>
std::pair<detail::Child, Iter_T> Value::_recursively_read_child(Value& parent, ChildPosition position, Iter_T current, const Iter_T& end)
{
	std::advance(current, 1);
	return _read_value_or_digits(parent, position, std::move(current), end);
}

///////////////////////////////////////////////////////////////////////////////

template<typename Iter_T>
void Value::_validate_second_value_is_present(const Iter_T& current, const Iter_T& end)
{
	if (current == end || *current != ',') {
		throw IOException("Failed to create snailfish value - Unexpected non-numeric value");
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename Iter_T>
Iter_T Value::_complete_value_read(Iter_T current, const Iter_T& end)
{
	if (current == end || *current != ']') {
		throw IOException("Failed to create snailfish value - Unexpected value termination");
	}

	// Consume the closing bracket at the end of the number.
	std::advance(current, 1);

	return current;
}

///////////////////////////////////////////////////////////////////////////////

template<typename Iter_T>
std::pair<detail::Child, Iter_T> Value::_read_value_or_digits(Value& parent, ChildPosition position, Iter_T current, const Iter_T& end)
{
	if (*current == '[') {
		auto [out, current] = Value::create(current, end, { {&parent, position} });
		return std::pair<detail::Child, Iter_T>{ detail::Child{ std::move(out), &parent, position }, std::move(current) };
	}
	else {
		const auto [out, current] = _read_digits(current, end);
		return std::pair<detail::Child, Iter_T>{ detail::Child{ std::move(out), &parent, position }, std::move(current) };
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename Iter_T>
std::pair<uint32_t, Iter_T> Value::_read_digits(Iter_T current, const Iter_T& end)
{
	std::stringstream digits;
	for (; current != end && std::isdigit(*current); ++current) {
		digits << *current;
	}

	return { string_to<uint32_t>(digits.str()), current };
}

///////////////////////////////////////////////////////////////////////////////

template<ChildPosition POSITION>
Value* ValueExploder::_find_first_predecessor_not_in_position(Value& value, std::optional<ChildPosition> position)
{
	auto predecessor = value.parent();
	while (predecessor && position == POSITION) {
		position = predecessor->position();
		predecessor = predecessor->parent();
	}

	return predecessor;
}

///////////////////////////////////////////////////////////////////////////////

template<ChildPosition POSITION>
detail::Child* ValueExploder::_find_last_child_in_complent_position(Value* value)
{
	auto* child = &value->child<POSITION>();

	while (child->is<ValuePtr_t>()) {
		child = &child->as<Value>().child<complement<POSITION>()>();
	}

	return child;
}

///////////////////////////////////////////////////////////////////////////////

template<ChildPosition POSITION>
void ValueExploder::_half_explode(Value& to_explode, ChildPosition position)
{
	auto predecessor = _find_first_predecessor_not_in_position<POSITION>(to_explode, position);
	if (!predecessor) {
		return;
	}

	auto child = _find_last_child_in_complent_position<POSITION>(predecessor);
	_apply_value_to_child(*child, to_explode.child<POSITION>().as<uint32_t>());
}

///////////////////////////////////////////////////////////////////////////////

template<ChildPosition POSITION>
std::optional<std::pair<Value*, ChildPosition>> detail::ValueSplitter::_recursively_find_child_to_split(Value& val)
{
	if (val.child<POSITION>().is<ValuePtr_t>()) {
		if (auto explode_details = _recursively_find_child_to_split(val.child<POSITION>().as<Value>(), POSITION)) {
			return explode_details;
		}
	}

	if (val.child<POSITION>().is<uint32_t>() && val.child<POSITION>().as<uint32_t>() > 9) {
		return { {&val, POSITION } };
	}

	return std::nullopt;
}

///////////////////////////////////////////////////////////////////////////////

}	// namespace: snailfish
}	// namespace: aoc

///////////////////////////////////////////////////////////////////////////////
