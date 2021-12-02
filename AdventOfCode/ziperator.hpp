//
// Based on: https://github.com/dpellegr/ZipIterator
// 
// C++17 implementation of ZipIterator by Dario Pellegrini <pellegrini.dario@gmail.com>
// Originally created: October 2019
//
// Includes suggestions from https://codereview.stackexchange.com/questions/231352/c17-zip-iterator-compatible-with-stdsort
//
// Licence: Creative Commons Zero v1.0 Universal
// See LICENCE.md file accompaining this file
//

#pragma once

#include <cstddef>
#include <tuple>
#include <utility>

template <typename ...T>
class ZipRef {
protected:
    std::tuple<T*...> ptr;

    template <std::size_t I = 0>
    void copy_assign(const ZipRef& z) {
        *(std::get<I>(ptr)) = *(std::get<I>(z.ptr));
        if constexpr (I + 1 < sizeof...(T)) copy_assign<I + 1>(z);
    }
    template <std::size_t I = 0>
    void val_assign(const std::tuple<T...>& t) {
        *(std::get<I>(ptr)) = std::get<I>(t);
        if constexpr (I + 1 < sizeof...(T)) val_assign<I + 1>(t);
    }

public:
    ZipRef() = delete;
    ZipRef(const ZipRef& z) = default;
    ZipRef(ZipRef&& z) = default;
    ZipRef(T* const... p) : ptr(p...) {}

    ZipRef& operator=(const ZipRef& z) { copy_assign(z); return *this; }
    ZipRef& operator=(const std::tuple<T...>& val) { val_assign(val); return *this; }

    std::tuple<T...> val() const { return std::apply([](auto&&...args) { return std::tuple((*args)...); }, ptr); }
    operator std::tuple<T...>() const { return val(); }

    template <std::size_t I = 0>
    void swap_data(const ZipRef& o) const {
        std::swap(*std::get<I>(ptr), *std::get<I>(o.ptr));
        if constexpr (I + 1 < sizeof...(T)) swap_data<I + 1>(o);
    }

    template<std::size_t N = 0>
    decltype(auto) get() { return *std::get<N>(ptr); }
    template<std::size_t N = 0>
    decltype(auto) get() const { return *std::get<N>(ptr); }

#define OPERATOR(OP) \
    bool operator OP(const ZipRef & o) const { return val() OP o.val(); } \
    inline friend bool operator OP(const ZipRef& r, const std::tuple<T...>& t) { return r.val() OP t; } \
    inline friend bool operator OP(const std::tuple<T...>& t, const ZipRef& r) { return t OP r.val(); }

    OPERATOR(== ) OPERATOR(<= ) OPERATOR(>= )
        OPERATOR(!= ) OPERATOR(< )  OPERATOR(> )
#undef OPERATOR
};

namespace std {

    template<std::size_t N, typename...T>
    struct tuple_element<N, ZipRef<T...>> {
        using type = decltype(std::get<N>(std::declval<ZipRef<T...>>().val()));
    };

    template<typename...T>
    struct tuple_size<ZipRef<T...>> : public std::integral_constant<std::size_t, sizeof...(T)> {};

    template<std::size_t N, typename...T>
    decltype(auto) get(ZipRef<T...>& r) {
        return r.template get<N>();
    }
    template<std::size_t N, typename...T>
    decltype(auto) get(const ZipRef<T...>& r) {
        return r.template get<N>();
    }

} // namespace std

template<typename ...Iterators_T>
class Ziperator {
    std::tuple<Iterators_T...> m_iterators;

    template <std::size_t ITEM_IDX = 0>
    bool one_is_equal(const Ziperator& rhs) const {
        if (std::get<ITEM_IDX>(m_iterators) == std::get<ITEM_IDX>(rhs.m_iterators))
            return true;

        if constexpr (ITEM_IDX + 1 < sizeof...(Iterators_T))
            return one_is_equal<I + 1>(rhs);

        return false;
    }

    template <std::size_t ITEM_IDX = 0>
    bool none_is_equal(const Ziperator& rhs) const {
        if (std::get<ITEM_IDX>(m_iterators) == std::get<ITEM_IDX>(rhs.m_iterators))
            return false;

        if constexpr (ITEM_IDX + 1 < sizeof...(Iterators_T))
            return none_is_equal<I + 1>(rhs);

        return true;
    }

public:
    using iterator_category = std::common_type_t<typename std::iterator_traits<Iterators_T>::iterator_category...>;
    using difference_type = std::common_type_t<typename std::iterator_traits<Iterators_T>::difference_type...>;
    using value_type = std::tuple<typename std::iterator_traits<Iterators_T>::value_type ...>;
    using pointer = std::tuple<typename std::iterator_traits<Iterators_T>::pointer ...>;
    using reference = ZipRef<std::remove_reference_t<typename std::iterator_traits<Iterators_T>::reference>...>;

    Ziperator() = default;
    Ziperator(const Ziperator& rhs) = default;
    Ziperator(Ziperator&& rhs) = default;
    Ziperator(const Iterators_T&... rhs) : m_iterators(rhs...) {}

    Ziperator& operator=(const Ziperator& rhs) = default;
    Ziperator& operator=(Ziperator&& rhs) = default;

    Ziperator& operator+=(const difference_type distance) {
        std::apply([&distance](auto&&...args) {((std::advance(args, distance)), ...); }, m_iterators);
        return *this;
    }

    Ziperator& operator-=(const difference_type distance) { return operator+=(-distance); }

    reference operator* () const {
        return std::apply([](auto&&...args) { return reference(&(*(args))...); }, m_iterators);
    }

    pointer operator->() const {
        return std::apply([](auto&&...args) { return pointer(&(*(args))...); }, m_iterators);
    }

    reference operator[](difference_type rhs) const { return *(operator+(rhs)); }

    Ziperator& operator++() { return operator+=(1); }
    Ziperator& operator--() { return operator+=(-1); }
    
    Ziperator operator++(int) {
        Ziperator tmp(*this);
        operator++();
        return tmp;
    }

    Ziperator operator--(int) {
        Ziperator tmp(*this);
        operator--();
        return tmp;
    }

    difference_type operator-(const Ziperator& rhs) const {
        return std::get<0>(m_iterators) - std::get<0>(rhs.m_iterators);
    }

    Ziperator operator+(const difference_type distance) const {
        Ziperator tmp(*this);
        tmp += distance;
        return tmp;
    }

    Ziperator operator-(const difference_type distance) const {
        Ziperator tmp(*this);
        tmp -= distance;
        return tmp;
    }

    inline friend Ziperator operator+(const difference_type distance, const Ziperator& z) { return z + distance; }
    inline friend Ziperator operator-(const difference_type distance, const Ziperator& z) { return z - distance; }

    // Since operator== and operator!= are often used to terminate cycles,
    // defining them as follow prevents incrementing behind the end() of a container
    bool operator==(const Ziperator& rhs) const { return  one_is_equal(rhs); }
    bool operator!=(const Ziperator& rhs) const { return none_is_equal(rhs); }

#define OPERATOR(OP) \
    bool operator OP(const Ziperator& rhs) const { return it OP rhs.it; }

    OPERATOR(<= )
    OPERATOR(>= )
    OPERATOR(< )
    OPERATOR(> )

#undef OPERATOR

};

template<typename ...Container>
class Zip {
    std::tuple<Container&...> zip;

public:
    Zip() = delete;
    Zip(const Zip& z) = default;
    Zip(Zip&& z) = default;
    Zip(Container&... z) : zip(z...) {}

#define HELPER(OP) \
    auto OP(){return std::apply([](auto&&... args){ return ZipIter((args.OP())...);}, zip);} \
    auto c##OP() const {return std::apply([](auto&&... args){ return ZipIter((args.c##OP())...);}, zip);} \
    auto OP() const {return this->c##OP();}

    HELPER(begin) HELPER(end)
        HELPER(rbegin) HELPER(rend)
#undef HELPER
};

using std::swap;
template<typename ...T> void swap(const ZipRef<T...>& a, const ZipRef<T...>& b) { a.swap_data(b); }

#include <sstream>

template< class Ch, class Tr, class...Iterators_T, std::enable_if_t<(sizeof...(Iterators_T) > 0), int> = 0>
            auto& operator<<(std::basic_ostream<Ch, Tr>& os, const ZipRef<Iterators_T...>& t) {
                std::basic_stringstream<Ch, Tr> ss;
                ss << "[ ";
                std::apply([&ss](auto&&... args) {((ss << args << ", "), ...); }, t.val());
                ss.seekp(-2, ss.cur);
                ss << " ]";
                return os << ss.str();
            }