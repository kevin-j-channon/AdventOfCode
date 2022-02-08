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
		this->as<Value>()._children.first.set_parent_and_position(&this->as<Value>(), ChildPosition::first);
		this->as<Value>()._children.second.set_parent_and_position(&this->as<Value>(), ChildPosition::second);
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

			out->_children.first = _clone_child_at<ChildPosition::first>(arg.get());
			out->_children.second = _clone_child_at<ChildPosition::second>(arg.get());

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
	, _children{ {first, this, ChildPosition::first}, {second, this, ChildPosition::second} }
{
}

///////////////////////////////////////////////////////////////////////////////

Value::Value(uint32_t first, ValuePtr_t&& second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos)
	: _parent_and_position{ parent_and_pos }
	, _children{ first, {second, this, ChildPosition::second} }
{
}

///////////////////////////////////////////////////////////////////////////////

Value::Value(ValuePtr_t&& first, uint32_t second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos)
	: _parent_and_position{ parent_and_pos }
	, _children{ {first, this, ChildPosition::first}, second }
{
}

///////////////////////////////////////////////////////////////////////////////

Value::Value(const Value& other)
	: _parent_and_position{ other._parent_and_position }
	, _children{ {other._children.first.clone(), this, ChildPosition::first}, {other._children.second.clone(), this, ChildPosition::second} }
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
	auto new_child = std::make_shared<Value>(other);
	_children = Children_t{ {_move_children_into_new_value(), this, ChildPosition::first }, { std::move(new_child), this, ChildPosition::second } };

	return *this;
}

///////////////////////////////////////////////////////////////////////////////

Value Value::operator+(const Value& other)
{
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

std::optional<ChildPosition> Value::position() const
{
	return _parent_and_position ? std::optional<ChildPosition>{_parent_and_position->second} : std::nullopt;
}

///////////////////////////////////////////////////////////////////////////////

ValuePtr_t Value::_move_children_into_new_value()
{
	auto value = std::make_shared<Value>();
	value->_children = std::move(_children);
	value->_parent_and_position = { {this, ChildPosition::first} };

	return std::move(value);
}

///////////////////////////////////////////////////////////////////////////////

bool Value::_explode()
{
	return ValueExploder{ *this }.explode();
}

///////////////////////////////////////////////////////////////////////////////

bool Value::_split()
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////

bool ValueExploder::explode()
{
	auto explode_details = _find_first_child_to_explode();
	if (!explode_details) {
		return false;
	}

	assert(explode_details->first != nullptr);

	auto& to_explode = *explode_details->first;
	if (explode_details->second == 0) {
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////

std::optional<std::pair<Value*, size_t>> ValueExploder::_find_first_child_to_explode()
{
	return _recursively_find_child_to_explode(_value, 0);
}

///////////////////////////////////////////////////////////////////////////////

std::optional<std::pair<Value*, size_t>> ValueExploder::_recursively_find_child_to_explode(Value& val, size_t depth, std::optional<size_t> child_idx)
{
	if (val._children.first.is<ValuePtr_t>()) {
		auto explode_details = _recursively_find_child_to_explode(val._children.first.as<Value>(), depth + 1, 0);
		if (explode_details) {
			return explode_details;
		}
	}

	if (val._children.second.is<ValuePtr_t>()) {
		auto explode_details = _recursively_find_child_to_explode(val._children.second.as <Value>(), depth + 1, 1);
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

}
}

///////////////////////////////////////////////////////////////////////////////
