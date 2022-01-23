#pragma once

#include "Exception.hpp"

///////////////////////////////////////////////////////////////////////////////

enum class SplitBehaviour
{
    none,
    drop_empty
};

///////////////////////////////////////////////////////////////////////////////

template<typename Char_T>
std::vector<std::basic_string<Char_T>> split(const std::basic_string<Char_T>& str, Char_T delimiter, SplitBehaviour behaviour = SplitBehaviour::none)
{
    auto result = std::vector<std::basic_string<Char_T>>();

    auto begin = str.cbegin();
    while (true) {
        if (SplitBehaviour::drop_empty == behaviour) {
            begin = std::find_if(begin, str.end(), [delimiter](auto c) { return c != delimiter; });
        }

        if (str.cend() == begin) {
            break;
        }

        auto pos = std::find(begin, str.cend(), delimiter);

        result.emplace_back(begin, pos);

        if (str.cend() == pos) {
            break;
        }
        else {
            begin = std::next(pos);
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////

template<typename Char_T>
std::vector<std::basic_string<Char_T>> split(const std::basic_string<Char_T>& str, const std::basic_string<Char_T>& delimiter, SplitBehaviour behaviour = SplitBehaviour::none)
{
    using Str_t = std::basic_string<Char_T>;

    if (delimiter.empty()) {
        throw aoc::InvalidArgException("Cannot split on an empty delimiter");
    }

    auto result = std::vector<Str_t>();

    const auto delim_len = delimiter.length();

    const auto str_len = str.length();

    if (str_len <= delim_len) {
        return result;
    }

    auto pos = typename Str_t::size_type{ 0 };
    while (pos != Str_t::npos && pos != str_len) {
        const auto next_delim_start_pos = str.find(delimiter, pos);
        if (next_delim_start_pos == Str_t::npos) {
            result.push_back(str.substr(pos, next_delim_start_pos - pos));
            return result;
        }

        if (SplitBehaviour::drop_empty == behaviour && next_delim_start_pos == pos + delimiter.length()) {
            pos = next_delim_start_pos + delim_len;
            continue;
        }

        result.push_back(str.substr(pos, next_delim_start_pos - pos));
        pos = next_delim_start_pos + delim_len;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////

namespace
{
    template<typename Char_T>
    struct IsNotWhitespace;

    template<>
    struct IsNotWhitespace<char>
    {
        bool operator()(const char ch) { return !std::isspace(ch); }
    };

    template<>
    struct IsNotWhitespace<wchar_t>
    {
        bool operator()(const wchar_t ch) { return !std::iswspace(ch); }
    };
}

///////////////////////////////////////////////////////////////////////////////

template<typename Char_T>
std::basic_string<Char_T> strip(const std::basic_string<Char_T>& str)
{
    const auto begin = std::find_if(str.cbegin(), str.cend(), IsNotWhitespace<Char_T>());
    const auto end = std::find_if(str.crbegin(), str.crend(), IsNotWhitespace<Char_T>()).base();

    return std::basic_string<Char_T>(begin, end);
}

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
inline auto string_to(const std::string& str) -> Value_T
{
    static_assert(false, "Unimplemented string conversion");
}

///////////////////////////////////////////////////////////////////////////////

template<>
inline auto string_to(const std::string& str) -> int
{
    return std::stoi(str);
}

///////////////////////////////////////////////////////////////////////////////

template<>
inline auto string_to(const std::string& str) -> long
{
    return std::stol(str);
}

///////////////////////////////////////////////////////////////////////////////

template<>
inline auto string_to(const std::string& str) -> unsigned int
{
    return std::stoul(str);
}

///////////////////////////////////////////////////////////////////////////////

template<>
inline auto string_to(const std::string& str) -> unsigned long
{
    return std::stoul(str);
}

///////////////////////////////////////////////////////////////////////////////

template<>
inline auto string_to(const std::string& str) -> long long
{
    return std::stoll(str);
}

///////////////////////////////////////////////////////////////////////////////

template<>
inline auto string_to(const std::string& str) -> unsigned long long
{
    return std::stoull(str);
}

///////////////////////////////////////////////////////////////////////////////

template<typename Char_T>
std::basic_string<Char_T> join(const std::vector<std::basic_string<Char_T>>& strings, Char_T delimiter)
{
    switch (strings.size())
    {
    case 0:
        return std::basic_string<Char_T>();
    case 1:
        return strings.front();
    default:;   // Fall through to main body of function.
    }

    const auto size = std::accumulate(strings.cbegin(), strings.cend(), size_t(0), [](const auto& curr, const auto& str) {
        return curr + str.length();
        }) + strings.size() - 1;

        std::basic_string<Char_T> result(size, Char_T(0));
        auto pos = std::copy(strings.front().cbegin(), strings.front().cend(), result.begin());
        std::for_each(std::next(strings.cbegin()), strings.cend(), [&pos, delimiter](const auto& s) {
            *pos++ = delimiter;
            pos = std::copy(s.cbegin(), s.cend(), pos);
            });

        return result;
}

///////////////////////////////////////////////////////////////////////////////
