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
