#pragma once

#include "StringOperations.hpp"
#include "Exception.hpp"

using namespace std::string_literals;

namespace aoc
{
namespace snailfish
{

enum class ChildPosition
{
	first,
	second
};

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
public:
	using Ptr_t = std::shared_ptr<Value>;

private:
	class Child : std::variant<uint32_t, Ptr_t>
	{
		using Base_t = std::variant<uint32_t, Ptr_t>;
	public:
		Child() : Base_t{}
		{
			Logger::WriteMessage("Child()\n");
		}

		Child(uint32_t value)
			: Base_t{ value }
		{
			Logger::WriteMessage(std::format("Child(uint32_t {})\n", value).c_str());
		}

		Child(Ptr_t value, Value* parent, ChildPosition position)
			: Base_t{ std::move(value) }
		{
			set_parent_and_position(parent, position);

			//Logger::WriteMessage(std::format("Child(Ptr_t value={}, Value* parrent={}, ChildPosition position={})\n",
				//value->as_string<char>(), (uint64_t)parent, static_cast<int>(position)).c_str());
		}

		Child(const Child& other, Value* parent, ChildPosition position)
			: Base_t{ other }
		{
			//Logger::WriteMessage(std::format("Child(const Child& value={}, Value* parent={}, ChildPosition position={})\n",
			//	other.as_string<char>(), (uint64_t)parent, static_cast<int>(position)).c_str());
			set_parent_and_position(parent, position);
		}

		/*

		Child& operator=(const Child& other)
		{
			Child temp{ other };
			swap(temp);

			return *this;
		}
		*/

		Child(const Child&) = delete;
		Child& operator=(const Child&) = delete;

		Child(Child&& other)
			: Base_t{ std::forward<Child>(other) }
		{
			if (this->is<Ptr_t>()) {
				this->as<Value>()._children.first.set_parent_and_position(&this->as<Value>(), ChildPosition::first);
				this->as<Value>()._children.second.set_parent_and_position(&this->as<Value>(), ChildPosition::second);
			}
		}

		Child& operator=(Child&& other)
		{
			Child temp{ std::forward<Child>(other) };
			this->swap(temp);

			return *this;
		}

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

		void set_parent_and_position(Value* parent, ChildPosition position)
		{
			std::visit([parent, position](auto&& arg) {
				using Arg_t = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<Arg_t, Ptr_t>) {
					arg->_parent_and_position = { parent, position };
				}
				}, _as_variant());
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

					out->_children.first = _clone_child_at<ChildPosition::first>(arg.get());
					out->_children.second = _clone_child_at<ChildPosition::second>(arg.get());

					return { std::move(out), arg.get(), *arg->position() };
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
		constexpr bool is() const
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
			if constexpr (std::is_same_v<std::decay_t<Result_T>, Value>)
			{
				return *std::get<Ptr_t>(_as_variant());
			}
			else {
				return std::get<Result_T>(_as_variant());
			}
		}

		template<typename Result_T>
		Result_T& as()
		{
			if constexpr (std::is_same_v<std::decay_t<Result_T>, Value>)
			{
				return *std::get<Ptr_t>(_as_variant());
			}
			else {
				return std::get<Result_T>(_as_variant());
			}
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

		template<ChildPosition POSITION>
		static Child _clone_child_at(Ptr_t::element_type* value)
		{
			auto child = value->child<POSITION>().clone();
			if (child.is<Ptr_t>()) {
				child.as<Value>()._parent_and_position = { value, POSITION };
			}

			return child;
		}
	};

	using Children_t = std::pair<Child, Child>;

public:

	Value()
		: _parent_and_position{ std::nullopt }
		, _children{}
	{
		Logger::WriteMessage("Default-constructed a value\n");
	}

	Value(uint32_t first, uint32_t second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos = std::nullopt)
		: _parent_and_position{ parent_and_pos }
		, _children{ first, second }
	{
		Logger::WriteMessage("Created a value with Value(uint32_t first, uint32_t second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos = std::nullopt)\n");
	}

	Value(Ptr_t&& first, Ptr_t&& second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos = std::nullopt)
		: _parent_and_position{ parent_and_pos }
		, _children{ {first, this, ChildPosition::first}, {second, this, ChildPosition::second} }
	{
		Logger::WriteMessage("Created a value with Value(Ptr_t&& first, Ptr_t&& second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos = std::nullopt)\n");
	}

	Value(uint32_t first, Ptr_t&& second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos = std::nullopt)
		: _parent_and_position{ parent_and_pos }
		, _children{ first, {second, this, ChildPosition::second} }
	{
		Logger::WriteMessage("Created a value with Value(uint32_t first, Ptr_t&& second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos = std::nullopt)\n");
	}

	Value(Ptr_t&& first, uint32_t second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos = std::nullopt)
		: _parent_and_position{ parent_and_pos }
		, _children{ {first, this, ChildPosition::first}, second }
	{
		Logger::WriteMessage("Created a value with Value(Ptr_t&& first, uint32_t second, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos = std::nullopt)\n");
	}

	Value(const Value& other)
		: _parent_and_position{ other._parent_and_position }
		, _children{ {other._children.first.clone(), this, ChildPosition::first}, {other._children.second.clone(), this, ChildPosition::second} }
	{
		Logger::WriteMessage("Copying value\n");

		Logger::WriteMessage(std::format("   Child 0: {}\n", _children.first.as_string<char>()).c_str());
		Logger::WriteMessage(std::format("   Child 1: {}\n", _children.second.as_string<char>()).c_str());
	}

	Value& operator=(const Value& other)
	{
		Logger::WriteMessage("Copy-assigning value\n");

		Value temp{ other };
		swap(temp, *this);

		return *this;
	}

	Value(Value&& other)
		: _parent_and_position{ std::move(other._parent_and_position) }
		, _children{ std::move(other._children) }
	{
		Logger::WriteMessage("Moving value\n");
	}

	Value& operator=(Value&& other)
	{
		Logger::WriteMessage("Move-assigning value\n");

		Value temp{ std::forward<Value>(other) };
		swap(temp, *this);

		return *this;
	}

	friend void swap(Value& a, Value& b)
	{
		using std::swap;
		swap(a._parent_and_position, b._parent_and_position);
		swap(a._children, b._children);
	}

	bool operator==(const Value& other) const
	{
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
		_children = Children_t{ {_move_children_into_new_value(), this, ChildPosition::first }, { std::move(new_child), this, ChildPosition::second } };

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
	static std::pair<Ptr_t, Iter_T> create(Iter_T current, Iter_T end, std::optional<std::pair<Value*, ChildPosition>> parent_and_pos = std::nullopt)
	{
		_validate_number_opening(current);

		auto out = std::make_shared<Value>();
		out->_parent_and_position = parent_and_pos;

		Logger::WriteMessage("Creating two children\n");
		auto first_child = Child{};
		auto second_child = Child{};

		std::tie(first_child, current) = _recursively_read_child(*out, ChildPosition::first, std::move(current), end);

		_validate_second_value_is_present(current, end);

		std::tie(second_child, current) = _recursively_read_child(*out, ChildPosition::second, std::move(current), end);

		current = _complete_value_read(std::move(current), end);

		out->_children.first = std::move(first_child);
		out->_children.second = std::move(second_child);

		return { std::move(out), std::move(current) };
	}

	template<typename Char_T>
	std::basic_string<Char_T> as_string() const
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

	Value* const parent() const
	{
		return _parent_and_position ? _parent_and_position->first : nullptr;
	}

	std::optional<ChildPosition> position() const
	{
		return _parent_and_position ? std::optional<ChildPosition>{_parent_and_position->second} : std::nullopt;
	}

	template<ChildPosition POSITION>
	const Child& child() const
	{
		if constexpr (POSITION == ChildPosition::first) {
			return _children.first;
		}
		else if constexpr (POSITION == ChildPosition::second) {
			return _children.second;
		}
		else {
			static_assert(std::false_type::value, "Invalid child position");
		}
	}

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
	static std::pair<Child, Iter_T> _recursively_read_child(Value& parent, ChildPosition position, Iter_T current, const Iter_T& end)
	{
		std::advance(current, 1);
		return _read_value_or_digits(parent, position, std::move(current), end);
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
	static std::pair<Child, Iter_T> _read_value_or_digits(Value& parent, ChildPosition position, Iter_T current, const Iter_T& end)
	{
		if (*current == '[') {
			auto [out, current] = Value::create(current, end, { {&parent, position} });
			return std::pair<Child, Iter_T>{ Child{ std::move(out), &parent, position }, std::move(current) };
		}
		else {
			const auto [out, current] = _read_digits(current, end);
			return std::pair<Child, Iter_T>{ Child{ std::move(out), &parent, position }, std::move(current) };
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
		value->_parent_and_position = { {this, ChildPosition::first} };

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

	std::optional<std::pair<Value*, ChildPosition>> _parent_and_position;
	Children_t _children;
};

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

std::optional<std::pair<Value*, size_t>> ValueExploder::_find_first_child_to_explode()
{
	return _recursively_find_child_to_explode(_value, 0);
}

std::optional<std::pair<Value*, size_t>> ValueExploder::_recursively_find_child_to_explode(Value& val, size_t depth, std::optional<size_t> child_idx)
{
	if (val._children.first.is<Value::Ptr_t>()) {
		auto explode_details = _recursively_find_child_to_explode(val._children.first.as<Value>(), depth + 1, 0);
		if (explode_details) {
			return explode_details;
		}
	}

	if (val._children.second.is<Value::Ptr_t>()) {
		auto explode_details = _recursively_find_child_to_explode(val._children.second.as <Value> (), depth + 1, 1);
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
