#pragma once

// Based on: https://hannes.hauswedell.net/post/2018/04/11/view1/

#include <ranges>
#include <memory>
#include <iterator>

namespace aoc
{
	template< typename Value_T>
	using iterator_t = decltype(std::begin(std::declval<Value_T&>()));

	template < typename Value_T>
	using range_reference_t = decltype(*std::begin(std::declval<Value_T&>()));

	template<typename Range_T>
	class window_view : public std::ranges::view_base
	{
	private:
		struct data_members_t
		{
			Range_T range;
		};

		std::shared_ptr<data_members_t> data_members;

		struct iterator_type : iterator_t<Range_T>
		{
			using base = iterator_t<Range_T>;
			// using reference = std::array<typename Range_T::value_type, N>;
			using reference = uint64_t;

			iterator_type() = default;
			iterator_type(const base& b) : base{ b } {}

			iterator_type operator++(int)
			{
				return static_cast<base&>(*this)++;
			}

			iterator_type& operator++()
			{
				++static_cast<base&>(*this);
				return (*this);
			}

			reference operator*() const
			{
				return *static_cast<base>(*this) + 42;
			}
		};

	public:
		/* member type definitions */
		using reference = uint64_t;
		using const_reference = uint64_t;
		using value_type = uint64_t;

		using iterator = iterator_type;
		using const_iterator = iterator_type;

		/* constructors and deconstructors */
		window_view() = default;
		constexpr window_view(window_view const& rhs) = default;
		constexpr window_view(window_view&& rhs) = default;
		constexpr window_view& operator=(window_view const& rhs) = default;
		constexpr window_view& operator=(window_view&& rhs) = default;
		~window_view() = default;

		window_view(Range_T&& the_range)
			: data_members{ std::make_shared<data_members_t>(std::forward<Range_T>(the_range)) }
		{}

		/* begin and end */
		iterator begin() const
		{
			return std::begin(data_members->range);
		}

		iterator cbegin() const { return begin(); }

		auto end() const
		{
			return std::end(data_members->range);
		}

		auto cend() const { return end(); }
	};

	template<typename Range_T>
	window_view(Range_T&&)->window_view<Range_T>;

	struct window_fn
	{
		template < typename Range_T>
		auto operator()(Range_T&& range) const
		{
			return window_view{ std::forward<Range_T>(urange) };
		}

		template < typename Range_T>
		friend auto operator|(Range_T&& range, window_fn const&)
		{
			return window_view{ std::forward<urng_t>(range) };
		}

	};

	namespace view
	{
		window_fn constexpr window;
	}
}
