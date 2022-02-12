#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "SnailfishNumbers.hpp"

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{
namespace snailfish
{

///////////////////////////////////////////////////////////////////////////////

detail::Child::Child() : Base_t{}
{
}

///////////////////////////////////////////////////////////////////////////////

detail::Child::Child(uint32_t value)
	: Base_t{ value }
{
}

///////////////////////////////////////////////////////////////////////////////

detail::Child::Child(ValuePtr_t value, Value* parent, ChildPosition position)
	: Base_t{ std::move(value) }
{
	set_parent_and_position(parent, position);
}

///////////////////////////////////////////////////////////////////////////////

detail::Child::Child(const Child& other, Value* parent, ChildPosition position)
	: Base_t{ other }
{
	set_parent_and_position(parent, position);
}

///////////////////////////////////////////////////////////////////////////////

detail::Child::Child(Child&& other)
	: Base_t{ std::forward<Child>(other) }
{
	if (this->is<ValuePtr_t>()) {
		this->as<Value>()._children.first.set_parent_and_position(&this->as<Value>(), ChildPosition::left);
		this->as<Value>()._children.second.set_parent_and_position(&this->as<Value>(), ChildPosition::right);
	}
}

///////////////////////////////////////////////////////////////////////////////

detail::Child& detail::Child::operator=(Child&& other)
{
	Child temp{ std::forward<Child>(other) };
	this->swap(temp);

	return *this;
}

///////////////////////////////////////////////////////////////////////////////

void detail::Child::set_parent_and_position(Value* parent, ChildPosition position)
{
	std::visit([parent, position](auto&& arg) {
		using Arg_t = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<Arg_t, ValuePtr_t>) {
			arg->_parent_and_position = { parent, position };
		}
		}, _as_variant());
}

///////////////////////////////////////////////////////////////////////////////

detail::Child detail::Child::clone() const
{
	return std::visit([](auto&& arg) -> Child {
		using Arg_t = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<Arg_t, uint32_t>) {
			return { arg };
		}
		else if constexpr (std::is_same_v<Arg_t, ValuePtr_t>) {
			auto out = std::make_shared<Value>();

			out->_children.first = _clone_child_at<ChildPosition::left>(arg.get());
			out->_children.second = _clone_child_at<ChildPosition::right>(arg.get());

			return { std::move(out), arg.get(), *arg->position() };
		}
		}, _as_variant());
}

///////////////////////////////////////////////////////////////////////////////

bool detail::Child::operator==(const Child& other) const
{
	return std::visit([&other](auto&& arg_outer) -> bool {
		using ArgOuter_t = std::decay_t<decltype(arg_outer)>;
		if constexpr (std::is_same_v<ArgOuter_t, uint32_t>) {

			return std::visit([&arg_outer](auto&& arg_inner) -> bool {
				using ArgInner_t = std::decay_t<decltype(arg_inner)>;
				if constexpr (std::is_same_v<ArgInner_t, uint32_t>) {
					return arg_outer == arg_inner;
				}
				else {
					return false;
				}
				}, other._as_variant());

		}
		else if constexpr (std::is_same_v<ArgOuter_t, ValuePtr_t>) {

			return std::visit([&arg_outer](auto&& arg_inner) -> bool {
				using ArgInner_t = std::decay_t<decltype(arg_inner)>;
				if constexpr (std::is_same_v<ArgInner_t, ValuePtr_t>) {
					return *arg_outer == *arg_inner;
				}
				else {
					return false;
				}
				}, other._as_variant());
		}
		}, _as_variant());
}

///////////////////////////////////////////////////////////////////////////////

void detail::swap(Child& a, Child& b)
{
	using std::swap;

	swap(static_cast<Child::Base_t&>(a), static_cast<Child::Base_t&>(b));
}

///////////////////////////////////////////////////////////////////////////////

void swap(Value& a, Value& b)
{
	using std::swap;
	swap(a._parent_and_position, b._parent_and_position);
	swap(a._children, b._children);
}

///////////////////////////////////////////////////////////////////////////////

Value::Value()
	: _parent_and_position{ std::nullopt }
	, _children{}
{
}

///////////////////////////////////////////////////////////////////////////////

Value::Value(uint32_t first, uint32_t second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos)
	: _parent_and_position{ parent_and_pos }
	, _children{ first, second }
{
}

///////////////////////////////////////////////////////////////////////////////

Value::Value(ValuePtr_t&& first, ValuePtr_t&& second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos)
	: _parent_and_position{ parent_and_pos }
	, _children{ {first, this, ChildPosition::left}, {second, this, ChildPosition::right} }
{
}

///////////////////////////////////////////////////////////////////////////////

Value::Value(uint32_t first, ValuePtr_t&& second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos)
	: _parent_and_position{ parent_and_pos }
	, _children{ first, {second, this, ChildPosition::right} }
{
}

///////////////////////////////////////////////////////////////////////////////

Value::Value(ValuePtr_t&& first, uint32_t second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos)
	: _parent_and_position{ parent_and_pos }
	, _children{ {first, this, ChildPosition::left}, second }
{
}

///////////////////////////////////////////////////////////////////////////////

Value::Value(const Value& other)
	: _parent_and_position{ other._parent_and_position }
	, _children{ {other._children.first.clone(), this, ChildPosition::left}, {other._children.second.clone(), this, ChildPosition::right} }
{
}

///////////////////////////////////////////////////////////////////////////////

Value& Value::operator=(const Value& other)
{
	Value temp{ other };
	swap(temp, *this);

	return *this;
}

///////////////////////////////////////////////////////////////////////////////

Value::Value(Value&& other)
	: _parent_and_position{ std::move(other._parent_and_position) }
	, _children{ std::move(other._children) }
{
}

///////////////////////////////////////////////////////////////////////////////

Value& Value::operator=(Value&& other)
{
	Value temp{ std::forward<Value>(other) };
	swap(temp, *this);

	return *this;
}

///////////////////////////////////////////////////////////////////////////////

bool Value::operator==(const Value& other) const
{
	if (_children.first != other._children.first) {
		return false;
	}

	if (_children.second != other._children.second) {
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////

bool Value::operator!=(const Value& other) const
{
	return !(*this == other);
}

///////////////////////////////////////////////////////////////////////////////

Value& Value::operator+=(const Value& other)
{
	if (*this == Value{}) {
		*this = other;
	}
	else if (other == Value{}) {
		// Do nothing.
	}
	else {
		auto new_child = std::make_shared<Value>(other);
		_children = Children_t{ {_move_children_into_new_value(), this, ChildPosition::left }, { std::move(new_child), this, ChildPosition::right } };

		reduce();
	}

	return *this;
}

///////////////////////////////////////////////////////////////////////////////

Value Value::operator+(const Value& other) const
{
	if (other == Value{}) {
		return *this;
	}

	auto out = *this;
	out += other;
	return std::move(out);
}

///////////////////////////////////////////////////////////////////////////////

Value& Value::reduce()
{
	while (_explode() || _split());

	return *this;
}

///////////////////////////////////////////////////////////////////////////////

Value Value::from_stream(std::istream& is)
{
	auto line = std::string{};
	std::getline(is, line);
	return from_string(line);
}

///////////////////////////////////////////////////////////////////////////////

Value Value::from_string(const std::string& str)
{
	if (str.empty()) {
		throw IOException("Failed to create snailfish Value - empty line");
	}

	return *create(str.begin(), str.end()).first;
}

///////////////////////////////////////////////////////////////////////////////

Value* const Value::parent() const
{
	return _parent_and_position ? _parent_and_position->first : nullptr;
}

///////////////////////////////////////////////////////////////////////////////

const detail::Child& Value::child(ChildPosition position) const
{
	return position == ChildPosition::left ? child<ChildPosition::left>() : child<ChildPosition::right>();
}

///////////////////////////////////////////////////////////////////////////////

detail::Child& Value::child(ChildPosition position)
{
	return position == ChildPosition::left ? child<ChildPosition::left>() : child<ChildPosition::right>();
}

///////////////////////////////////////////////////////////////////////////////

std::optional<ChildPosition> Value::position() const
{
	return _parent_and_position ? std::optional<ChildPosition>{_parent_and_position->second} : std::nullopt;
}

///////////////////////////////////////////////////////////////////////////////

ValuePtr_t Value::_move_children_into_new_value()
{
	auto value = std::make_shared<Value>();
	value->_children = std::move(_children);
	value->_parent_and_position = { {this, ChildPosition::left} };

	return std::move(value);
}

///////////////////////////////////////////////////////////////////////////////

bool Value::_explode()
{
	return detail::ValueExploder{ *this }.explode();
}

///////////////////////////////////////////////////////////////////////////////

bool Value::_split()
{
	return detail::ValueSplitter{ *this }.split();
}

///////////////////////////////////////////////////////////////////////////////

bool detail::ValueSplitter::split()
{
	if (auto split_details = _recursively_find_child_to_split(_value, std::nullopt)) {
		auto [value, position] = *split_details;
		_split_child(*value, position);

		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////

std::optional<std::pair<Value*, ChildPosition>> detail::ValueSplitter::_recursively_find_child_to_split(Value& val, std::optional<ChildPosition> position)
{
	if (auto explode_details = _recursively_find_child_to_split<ChildPosition::left>(val))
	{
		return explode_details;
	}

	if (auto explode_details = _recursively_find_child_to_split<ChildPosition::right>(val))
	{
		return explode_details;
	}

	return std::nullopt;
}

///////////////////////////////////////////////////////////////////////////////

void detail::ValueSplitter::_split_child(Value& value, ChildPosition position)
{
	auto& child = value.child(position);
	const auto numerical_value = child.as<uint32_t>();
	child = detail::Child{ std::make_shared<Value>(numerical_value / 2, numerical_value - (numerical_value / 2)), &value, position };
}

///////////////////////////////////////////////////////////////////////////////

bool detail::ValueExploder::explode()
{
	if (auto explode_details = _find_first_child_to_explode()) {
		auto [to_explode, position] = *explode_details;
		assert(to_explode != nullptr);

		_half_explode<ChildPosition::left>(*to_explode, position);
		_half_explode<ChildPosition::right>(*to_explode, position);
		_zero_exploded_child(*to_explode, position);

		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////

void detail::ValueExploder::_zero_exploded_child(Value& value, ChildPosition position)
{
	auto parent = value.parent();
	switch (position) {
	case ChildPosition::left: {
		parent->child<ChildPosition::left>() = detail::Child(uint32_t{ 0 }, parent, position);
		break;
	}
	case ChildPosition::right: {
		parent->child<ChildPosition::right>() = detail::Child(uint32_t{ 0 }, parent, position);
		break;
	}
	default:;
	}
}

///////////////////////////////////////////////////////////////////////////////

std::optional<std::pair<Value*, ChildPosition>> detail::ValueExploder::_find_first_child_to_explode()
{
	return _recursively_find_child_to_explode(_value, 0);
}

///////////////////////////////////////////////////////////////////////////////

std::optional<std::pair<Value*, ChildPosition>> detail::ValueExploder::_recursively_find_child_to_explode(Value& val, size_t depth, std::optional<ChildPosition> child_idx)
{
	if (val.child<ChildPosition::left>().is<ValuePtr_t>()) {
		auto explode_details = _recursively_find_child_to_explode(val.child<ChildPosition::left>().as<Value>(), depth + 1, ChildPosition::left);
		if (explode_details) {
			return explode_details;
		}
	}

	if (val.child<ChildPosition::right>().is<ValuePtr_t>()) {
		auto explode_details = _recursively_find_child_to_explode(val.child<ChildPosition::right>().as <Value>(), depth + 1, ChildPosition::right);
		if (explode_details) {
			return explode_details;
		}
	}

	if (depth > 3 && child_idx != std::nullopt) {
		return { { &val, *child_idx } };
	}
	else {
		return std::nullopt;
	}
}

///////////////////////////////////////////////////////////////////////////////

void detail::ValueExploder::_apply_value_to_child(detail::Child& child, uint32_t value)
{
	child = child.as<uint32_t>() + value;
}

///////////////////////////////////////////////////////////////////////////////

}
}

///////////////////////////////////////////////////////////////////////////////
