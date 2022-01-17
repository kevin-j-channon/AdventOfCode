#pragma once

namespace aoc
{

template<typename Key_T, typename Value_T, size_t ITEM_COUNT>
class StaticMap
{
	enum class Direction { forward, reverse };

public:
	using value_type = std::pair<Key_T, Value_T>;
	using size_type = size_t;
	using This_t = StaticMap<Key_T, Value_T, ITEM_COUNT>;

	constexpr StaticMap(std::initializer_list<value_type>&& init)
	{
		std::copy(init.begin(), init.end(), m_items.begin());
	}

	constexpr size_type size() const { return m_items.size(); }

	const Value_T& at(const Key_T& k) const
	{
		return get<Key_T, Value_T, Direction::forward>(k);
	}

	Value_T& at(const Key_T& k)
	{
		return get<Key_T, Value_T, Direction::forward>(k);
	}

	const Key_T& with(const Value_T& v) const
	{
		return get<Value_T, Key_T, Direction::reverse>(v);
	}

private:

	template<typename Target_T, typename Result_T, Direction DIRECTION>
	const Result_T& get(const Target_T& t) const
	{
		const auto it = std::find_if(m_items.cbegin(), m_items.cend(), [&t](const auto& x) { return std::get<static_cast<int>(DIRECTION)>(x) == t; });
		if (m_items.cend() == it) {
			throw std::out_of_range("Invalid item");
		}

		return std::get<1 - static_cast<int>(DIRECTION)>(*it);
	}

	template<typename Target_T, typename Result_T, Direction DIRECTION>
	Result_T& get(const Target_T& t)
	{
		return const_cast<Result_T&>(const_cast<const This_t*>(this)->get<Target_T, Result_T, DIRECTION>(t));
	}

	std::array<value_type, ITEM_COUNT> m_items;
};

}
