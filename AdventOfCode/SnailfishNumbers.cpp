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
	auto new_child = std::make_shared<Value>(other);
	_children = Children_t{ {_move_children_into_new_value(), this, ChildPosition::left }, { std::move(new_child), this, ChildPosition::right } };

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
	value->_parent_and_position = { {this, ChildPosition::left} };

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

	auto [to_explode, position] = *explode_details;

	Logger::WriteMessage(std::format("To explode: {}\n", to_explode->as_string<char>()).c_str());

	assert(to_explode != nullptr);

	//
	// Handle the first child of the value to be exploded.
	// 
	// Find the first second child of a predecessor.
	{
		Logger::WriteMessage("Looking for value to the left\n");
		auto predecessor = to_explode->parent();
		Logger::WriteMessage(std::format("First preddecessor, {}, has position {}\n", predecessor->as_string<char>(), (int)*predecessor->position()).c_str());

		auto pos = std::make_optional<ChildPosition>(position);
		while (predecessor && pos == ChildPosition::left) {
			pos = predecessor->position();
			predecessor = predecessor->parent();
			if (predecessor) {
				Logger::WriteMessage(std::format("Moved to preddecessor, {}, with position {}\n",
					predecessor->as_string<char>(), predecessor->position() ? (int)*predecessor->position() : -1).c_str());
			}
			else {
				Logger::WriteMessage("Moved past root of number - no suitable predecessor found\n");
			}
		}

		if (predecessor) {
			Logger::WriteMessage(std::format("Left predecessor: {}\n", predecessor->as_string<char>()).c_str());

			// We found a suitable value to the left of the value to explode; step one child to the left.
			auto* child = &predecessor->child<ChildPosition::left>();

			// Now need to traverse as far down and to the right as possible.
			while (child->is<ValuePtr_t>()) {
				child = &child->as<Value>().child<ChildPosition::right>();
			}

			Logger::WriteMessage(std::format("Target child: {}\n", child->as_string<char>()).c_str());

			// Now the child is the one that will receive the left value from the exploded value.
			*child = child->as<uint32_t>() + to_explode->child<ChildPosition::left>().as<uint32_t>();
		}
		else
		{
			Logger::WriteMessage("No predecessor found\n");
		}
	}

	//
	// Handle the second child of the value to be exploded.
	//
	{
		Logger::WriteMessage("Looking for value to the right\n");
		auto predecessor = to_explode->parent();
		Logger::WriteMessage(std::format("First preddecessor, {}, has position {}\n", predecessor->as_string<char>(), (int) * predecessor->position()).c_str());

		auto pos = std::make_optional<ChildPosition>(position);
		while (predecessor && pos == ChildPosition::right) {
			pos = predecessor->position();
			predecessor = predecessor->parent();

			if (predecessor) {
				Logger::WriteMessage(std::format("Moved to preddecessor, {}, with position {}\n",
					predecessor->as_string<char>(), predecessor->position() ? (int)*predecessor->position() : -1).c_str());
			}
			else {
				Logger::WriteMessage("Moved past root of number - no suitable predecessor found\n");
			}
		}

		if (predecessor) {
			Logger::WriteMessage(std::format("Right predecessor: {}\n", predecessor->as_string<char>()).c_str());

			// We found a suitable value to the right of the value to explode; step one child to the right.
			auto* child = &predecessor->child<ChildPosition::right>();

			Logger::WriteMessage(std::format("Stepped to the right and found child: {}\n", child->as_string<char>()).c_str());

			// Now need to traverse as far down and to the left as possible.
			while (child->is<ValuePtr_t>()) {
				child = &child->as<Value>().child<ChildPosition::left>();
			}

			Logger::WriteMessage(std::format("Target child: {}\n", child->as_string<char>()).c_str());

			// Now the child is the one that will receive the right value from the exploded value.
			*child = child->as<uint32_t>() + to_explode->child<ChildPosition::right>().as<uint32_t>();
		}
	}

	//
	// Replace the exploded child with a '0' value
	//
	{
		auto parent = to_explode->parent();
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

	return true;
}

///////////////////////////////////////////////////////////////////////////////

std::optional<std::pair<Value*, ChildPosition>> ValueExploder::_find_first_child_to_explode()
{
	return _recursively_find_child_to_explode(_value, 0);
}

///////////////////////////////////////////////////////////////////////////////

std::optional<std::pair<Value*, ChildPosition>> ValueExploder::_recursively_find_child_to_explode(Value& val, size_t depth, std::optional<ChildPosition> child_idx)
{
	if (val._children.first.is<ValuePtr_t>()) {
		auto explode_details = _recursively_find_child_to_explode(val._children.first.as<Value>(), depth + 1, ChildPosition::left);
		if (explode_details) {
			return explode_details;
		}
	}

	if (val._children.second.is<ValuePtr_t>()) {
		auto explode_details = _recursively_find_child_to_explode(val._children.second.as <Value>(), depth + 1, ChildPosition::right);
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
