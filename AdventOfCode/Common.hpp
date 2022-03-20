#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "StringOperations.hpp"
#include "Exception.hpp"

#include <armadillo>

#include <stdexcept>
#include <string>
#include <vector>
#include <istream>
#include <cmath>

///////////////////////////////////////////////////////////////////////////////

namespace aoc
{

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
class ValueRange
{
public:
	using Value_t = Value_T;
	using This_t = ValueRange<Value_t>;

	struct ConstIterator
	{
		using iterator_category = std::random_access_iterator_tag;
		using value_type = Value_t;
		using difference_type = std::make_signed_t<value_type>;
		using pointer = const value_type*;
		using reference = const value_type&;

		explicit ConstIterator(value_type begin)
			: _current{ begin }
		{}

		auto operator<=>(const ConstIterator&) const = default;

		reference operator*() const { return _current; }

		ConstIterator& operator++()
		{
			++_current;
			return *this;
		}

		ConstIterator operator++(int)
		{
			ConstIterator out = *this;
			++(*this);
			return out;
		}

		ConstIterator& operator--()
		{
			--_current;
			return *this;
		}

		ConstIterator operator--(int)
		{
			ConstIterator out = *this;
			--(*this);
			return out;
		}

		ConstIterator& operator+=(difference_type increment)
		{
			_current += increment;
			return *this;
		}

		ConstIterator& operator-=(difference_type increment)
		{
			_current -= increment;
			return *this;
		}

		friend ConstIterator operator+(ConstIterator it_1, difference_type increment)
		{
			it_1 += increment;
			return it_1;
		}

		friend ConstIterator operator-(ConstIterator it_1, difference_type decrement)
		{
			it_1 -= decrement;
			return it_1;
		}

		friend difference_type operator-(const ConstIterator& it_1, const ConstIterator& it_2)
		{
			return it_1._current - it_2._current;
		}

	private:
		value_type _current;
	};

	ValueRange() : _min{ 0 }, _max{ 0 } {}

	constexpr ValueRange(Value_t max_)
		: _min{ 0 }
		, _max{ std::move(max_) }
	{
		if (_min > _max) {
			throw InvalidArgException("Range min is greater than max");
		}
	}

	constexpr ValueRange(Value_t min_, Value_t max_)
		: _min{ std::move(min_) }
		, _max{ std::move(max_) }
	{
		if (_min > _max) {
			throw InvalidArgException("Range min is greater than max");
		}
	}

	ValueRange(const This_t&) = default;
	This_t& operator=(const This_t&) = default;

	ValueRange(This_t&&) = default;
	This_t& operator=(This_t&&) = default;

	bool operator==(const This_t& other) const
	{
		return _min == other.x && _max == other.y;
	}

	bool operator!=(const This_t& other) const
	{
		return !(*this == other);
	}

	ValueRange& from_stream(std::istream& is) try
	{
		if (is.eof())
			return *this;

		auto str = std::string{};
		is >> str;

		from_string(str);

		return *this;
	}
	catch (const Exception&)
	{
		is.setstate(std::ios::failbit);
		throw;
	}
	catch (const std::invalid_argument&)
	{
		is.setstate(std::ios::failbit);
		throw aoc::IOException("Failed to extract ValueRange from stream");
	}
	catch (const std::out_of_range&)
	{
		is.setstate(std::ios::failbit);
		throw aoc::IOException("Failed to extract ValueRange from stream");
	}

	ValueRange& from_string(const std::string& str)
	{
		using namespace std::string_literals;

		auto min_max_str = split(str, ".."s, SplitBehaviour::drop_empty);
		if (min_max_str.size() != 2) {
			throw aoc::IOException("Failed to read ValueRange");
		}

		try {
			const auto min = string_to<Value_T>(min_max_str[0]);
			const auto max = string_to<Value_T>(min_max_str[1]);

			if (min > max) {
				throw IOException("Range min is greater than max");
			}

			_min = min;
			_max = max;
		}
		catch (const std::out_of_range& e) {
			throw IOException(std::format("Failed to read ValueRange: {}", e.what()));
		}
		catch (const std::invalid_argument& e) {
			throw IOException(std::format("Failed to read ValueRange: {}", e.what()));
		}

		return *this;
	}

	const Value_t& min() const { return _min; }
	const Value_t& max() const { return _max; }

	auto begin() const { return ConstIterator{ _min }; }
	auto end() const { return ConstIterator{ _max + 1 }; }

private:
	Value_T _max;
	Value_T _min;
};

///////////////////////////////////////////////////////////////////////////////

template<size_t BASE, size_t EXPONENT>
struct Exp
{
	static constexpr size_t value = BASE * Exp<BASE, EXPONENT - 1>::value;
};

template<size_t BASE>
struct Exp<BASE, 0>
{
	static constexpr size_t value = 1;
};

///////////////////////////////////////////////////////////////////////////////

template<typename Container_T>
class PairwiseCombinationIterator
{
public:
	using iterator_category = std::input_iterator_tag;
	using value_type = std::pair<typename Container_T::const_iterator, typename Container_T::const_iterator>;
	using reference = const value_type&;
	using pointer = const value_type*;
	using difference_type = std::ptrdiff_t;

	PairwiseCombinationIterator(const Container_T& values)
		: _at_end{ values.begin() == values.end() }
		, _begin{ values.begin() }
		, _end{ values.end() }
		, _current{ values.begin(), values.begin() }
	{}

	PairwiseCombinationIterator()
		: _at_end{ true }
	{}

	PairwiseCombinationIterator(const PairwiseCombinationIterator& other)
		: _at_end{ other._at_end }
		, _begin{ other._begin }
		, _end{ other._end }
		, _current{ other._current }
	{}

	PairwiseCombinationIterator& operator=(const PairwiseCombinationIterator& other)
	{
		auto temp = other;
		this->swap(temp);

		return *this;
	}

	PairwiseCombinationIterator(PairwiseCombinationIterator&& other) = default;
	PairwiseCombinationIterator& operator=(PairwiseCombinationIterator&& other) = default;

	bool operator==(const PairwiseCombinationIterator& other) const
	{
		if (_at_end && other._at_end) {
			return true;
		}

		if (_at_end != other._at_end) {
			return false;
		}

		if (_current != other._current) {
			return false;
		}

		return true;
	}

	bool operator!=(const PairwiseCombinationIterator& other) const
	{
		return !(*this == other);
	}

	reference operator*() const
	{
		return _current;
	}

	pointer operator->() const
	{
		return &_current;
	}

	PairwiseCombinationIterator& operator++()
	{
		if (_at_end) {
			throw OutOfRangeException("");
		}

		++_current.second;
		if (_current.second == _end) {
			_current.second = _begin;
			++_current.first;
		}

		if (_current.first == _end) {
			_at_end = true;
		}

		return *this;
	}

	PairwiseCombinationIterator operator++(int)
	{
		auto temp = PairwiseCombinationIterator{ *this };
		++(*this);
		return temp;
	}

	void swap(PairwiseCombinationIterator& other)
	{
		std::swap(_at_end, other._at_end);
		std::swap(_begin, other._begin);
		std::swap(_end, other._end);
		std::swap(_current, other._current);
	}

private:
	bool _at_end;
	typename Container_T::const_iterator _begin, _end;
	value_type _current;
};

///////////////////////////////////////////////////////////////////////////////

struct Capacity
{
	explicit Capacity(size_t c) : value{ c } {}
	size_t value;
};

struct Size
{
	explicit Size(size_t s) : value{ s } {}
	size_t value;
};

template<typename Value_T, typename Alloc_T = std::allocator<Value_T>>
std::vector<Value_T, Alloc_T> make_vector(Capacity c, Size s = Size{0})
{
	auto out = std::vector<Value_T, Alloc_T>(s.value);
	out.reserve(c.value);
	return out;
}

///////////////////////////////////////////////////////////////////////////////

template<typename Result_T, typename Range_T>
Result_T range_to(Range_T range)
{
	auto out = Result_T{};
	if constexpr (std::ranges::sized_range<Range_T>) {
		out.reserve(range.size());
	}

	std::ranges::copy(range, std::back_inserter(out));

	return out;
}

}	// namespace: aoc

///////////////////////////////////////////////////////////////////////////////

namespace std
{

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
inline istream& operator>>(std::istream& is, aoc::ValueRange<Value_T>& range)
{
	range.from_stream(is);
	return is;
}

///////////////////////////////////////////////////////////////////////////////

}	// namespace: std

///////////////////////////////////////////////////////////////////////////////
