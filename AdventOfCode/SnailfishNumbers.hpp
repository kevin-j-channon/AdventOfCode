#pragma once

#include "StringOperations.hpp"
#include "Exception.hpp"

#include <any>

namespace aoc
{
namespace snailfish
{

class Value;

class ValueExploder
{
public:
	ValueExploder(Value& v)
		: _value{ v }
	{}

	bool explode();

private:

	std::optional<std::pair<Value*, size_t>> _find_first_child_to_explode();

	static std::optional<std::pair<Value*, size_t>> _recursively_find_child_to_explode(Value& val, size_t depth, std::optional<size_t> child_idx = std::nullopt);

	Value& _value;
};

class Value
{
	using Ptr_t = std::shared_ptr<Value>;

	class Child : std::variant<uint32_t, Ptr_t>
	{
		using Base_t = std::variant<uint32_t, Ptr_t>;
	public:

		using Base_t::operator=;

		Child() : Base_t{} {}

		Child(Base_t value)
			: Base_t{ std::move(value) }
		{}

		Child(const Child& other)
			: Base_t{ other }
		{
		}

		Child& operator=(const Child& other)
		{
			Child temp{ other };
			swap(temp);

			return *this;
		}

		Child(Child&&) = default;
		Child& operator=(Child&&) = default;

		template<typename Char_T>
		std::basic_string<Char_T> as_string() const
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
					else if constexpr (std::is_same_v<Arg_t, Ptr_t>) {
						return arg->as_string<Char_T>();
					}
				},
				_as_variant()
			);
		}

		Child clone() const
		{
			return std::visit([](auto&& arg) -> Child {
					using Arg_t = std::decay_t<decltype(arg)>;
					if constexpr (std::is_same_v<Arg_t, uint32_t>) {
						return { arg };
					}
					else if constexpr (std::is_same_v<Arg_t, Ptr_t>) {
						auto out = std::make_shared<Value>();
						out->_children.first = arg->_children.first.clone();
						out->_children.second = arg->_children.second.clone();
						return { std::move(out)};
					}
				}, _as_variant());
		}

		friend void swap(Child& a, Child& b)
		{
			using std::swap;

			a.swap(b);
		}

		bool operator==(const Child& other) const
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
				else if constexpr (std::is_same_v<ArgOuter_t, Ptr_t>) {

					return std::visit([&arg_outer](auto&& arg_inner) -> bool {
						using ArgInner_t = std::decay_t<decltype(arg_inner)>;
						if constexpr (std::is_same_v<ArgInner_t, Ptr_t>) {
							return *arg_outer == *arg_inner;
						}
						else {
							return false;
						}
						}, other._as_variant());
				}
			}, _as_variant());
		}

		template<typename T>
		bool is() const
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

		template<typename Result_T>
		const Result_T& as() const
		{
			return std::get<Result_T>(_as_variant());
		}

		template<typename Result_T>
		Result_T& as()
		{
			return std::get<Result_T>(_as_variant());
		}
	private:

		Base_t& _as_variant()
		{
			return static_cast<Base_t&>(*this);
		}

		const Base_t& _as_variant() const
		{
			return static_cast<const Base_t&>(*this);
		}
	};

	using Children_t = std::pair<Child, Child>;

public:

	Value()
		: _parent{ nullptr }
		, _children{ uint32_t{0}, uint32_t{0} }
	{}

	Value(uint32_t first, uint32_t second, Value* parent = nullptr) 
		: _parent{ parent }
		, _children{ first, second }
	{}

	Value(Ptr_t&& first, Ptr_t&& second, Value* parent = nullptr)
		: _parent{ parent }
		, _children{ first, second }
	{}

	Value(uint32_t first, Ptr_t && second, Value* parent = nullptr)
		: _parent{ parent }
		, _children{ first, second }
	{}

	Value(Ptr_t&& first, uint32_t second, Value* parent = nullptr)
		: _parent{ parent }
		, _children{ first, second }
	{}

	Value(const Value& other)
		: _parent{ other._parent }
		, _children{ other._children.first.clone(), other._children.second.clone() }
	{}

	Value& operator=(const Value& other)
	{
		Value temp{ other };
		swap(temp, *this);

		return *this;
	}

	Value(Value&&) = default;
	Value& operator=(Value&&) = default;

	friend void swap(Value& a, Value& b)
	{
		using std::swap;
		swap(a._parent, b._parent);
		swap(a._children, b._children);
	}

	bool operator==(const Value& other) const
	{
		if (_parent != other._parent) {
			return false;
		}

		if (_children.first != other._children.first) {
			return false;
		}

		if (_children.second != other._children.second) {
			return false;
		}

		return true;
	}

	bool operator!=(const Value& other) const
	{
		return !(*this == other);
	}

	Value& operator+=(const Value& other)
	{
		auto new_child = std::make_shared<Value>(other);
		new_child->_parent = this;
		_children = Children_t{ _move_children_into_new_value() , std::move(new_child) };

		return *this;
	}

	Value operator+(const Value& other)
	{
		auto out = *this;
		out += other;

		return std::move(out);
	}

	Value& reduce()
	{
		while (_explode() || _split());

		return *this;
	}

	static Value from_stream(std::istream& is)
	{
		auto line = std::string{};
		std::getline(is, line);
		return from_string(line);
	}

	static Value from_string(const std::string& str)
	{
		if (str.empty()) {
			throw IOException("Failed to create snailfish Value - empty line");
		}

		return *create(str.begin(), str.end()).first;
	}

	template<typename Iter_T>
	static std::pair<Ptr_t, Iter_T> create(Iter_T current, Iter_T end, Value* parent = nullptr)
	{
		_validate_number_opening(current);

		auto out = std::make_shared<Value>();
		out->_parent = parent;

		auto first_child = Child{};
		auto second_child = Child{};

		std::tie(first_child, current) = _recursively_read_child(*out, std::move(current), end);
		
		_validate_second_value_is_present(current, end);
		
		std::tie(second_child, current) = _recursively_read_child(*out, std::move(current), end);
		
		current = _complete_value_read(std::move(current), end);
		
		out->_children.first = std::move(first_child);
		out->_children.second = std::move(second_child);

		return { std::move(out), std::move(current) };
	}

	template<typename Char_T>
	std::basic_string<Char_T> as_string() const
	{
		if constexpr (std::is_same_v<Char_T, char>) {
			return std::format("[{},{}]", _children.first.as_string<Char_T>(), _children.second.as_string<Char_T>());
		}
		else if constexpr (std::is_same_v<Char_T, wchar_t>) {
			return std::format(L"[{},{}]", _children.first.as_string<Char_T>(), _children.second.as_string<Char_T>());
		}
	}

	Value* const parent() const { return _parent; }

private:

	friend class ValueExploder;

	template<typename Iter_T>
	static void _validate_number_opening(const Iter_T& current)
	{
		if (*current != '[') {
			throw IOException("Failed to creaate snailfish Value - invalid openning");
		}
	}

	template<typename Iter_T>
	static std::pair<Child, Iter_T> _recursively_read_child(Value& parent, Iter_T current, const Iter_T& end)
	{
		std::advance(current, 1);
		return _read_value_or_digits(parent, std::move(current), end);
	}

	template<typename Iter_T>
	static void _validate_second_value_is_present(const Iter_T& current, const Iter_T& end)
	{
		if (current == end || *current != ',') {
			throw IOException("Failed to create snailfish value - Unexpected non-numeric value");
		}
	}

	template<typename Iter_T>
	static Iter_T _complete_value_read(Iter_T current, const Iter_T& end)
	{
		if (current == end || *current != ']') {
			throw IOException("Failed to create snailfish value - Unexpected value termination");
		}

		// Consume the closing bracket at the end of the number.
		std::advance(current, 1);

		return current;
	}

	template<typename Iter_T>
	static std::pair<Child, Iter_T> _read_value_or_digits(Value& parent, Iter_T current, const Iter_T& end)
	{
		if (*current == '[') {
			auto [out, current] = Value::create(current, end, &parent);
			return std::pair<Child, Iter_T>{ Child(std::move(out)), std::move(current) };
		}
		else {
			const auto [out, current] = _read_digits(current, end);
			return std::pair<Child, Iter_T>{ Child(std::move(out)), std::move(current) };
		}
	}

	template<typename Iter_T>
	static std::pair<uint32_t, Iter_T> _read_digits(Iter_T current, const Iter_T& end)
	{
		std::stringstream digits;
		for (; current != end && std::isdigit(*current); ++current) {
			digits << *current;
		}

		return { string_to<uint32_t>(digits.str()), current };
	}

	Ptr_t _move_children_into_new_value()
	{
		auto value = std::make_shared<Value>();
		value->_children = std::move(_children);
		value->_parent = this;

		return std::move(value);
	}

	bool _explode()
	{
		return ValueExploder{ *this }.explode();
	}

	

	bool _split()
	{
		return false;
	}

	Value* _parent;
	std::pair<Child, Child> _children;
};

bool ValueExploder::explode()
{
	auto explode_details = _find_first_child_to_explode();
	if (!explode_details) {
		return false;
	}

	assert(explode_details->first != nullptr);



	return true;
}

std::optional<std::pair<Value*, size_t>> ValueExploder::_find_first_child_to_explode()
{
	return _recursively_find_child_to_explode(_value, 0);
}

std::optional<std::pair<Value*, size_t>> ValueExploder::_recursively_find_child_to_explode(Value& val, size_t depth, std::optional<size_t> child_idx)
{
	if (val._children.first.is<Value::Ptr_t>()) {
		auto explode_details = _recursively_find_child_to_explode(*val._children.first.as<Value::Ptr_t>(), depth + 1, 0);
		if (explode_details) {
			return explode_details;
		}
	}

	if (val._children.second.is<Value::Ptr_t>()) {
		auto explode_details = _recursively_find_child_to_explode(*val._children.second.as < Value::Ptr_t > (), depth + 1, 1);
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

}
}
