#pragma once

#include <vector>
#include <string>
#include <cctype>
#include <cwctype>
#include <algorithm>

enum class SplitBehaviour
{
    none,
    drop_empty
};

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

template<typename Char_T>
std::basic_string<Char_T> strip(const std::basic_string<Char_T>& str)
{
    const auto begin = std::find_if(str.cbegin(), str.cend(), IsNotWhitespace<Char_T>());
    const auto end = std::find_if(str.crbegin(), str.crend(), IsNotWhitespace<Char_T>()).base();

    return std::basic_string<Char_T>(begin, end);
}

template<typename Value_T>
Value_T string_to(const std::string& str)
{
    static_assert(false, "Unimplemented string conversion");
}

template<>
int string_to(const std::string& str)
{
    return std::stoi(str);
}

template<>
long string_to(const std::string& str)
{
    return std::stol(str);
}

template<>
unsigned int string_to(const std::string& str)
{
    return std::stol(str);
}

template<>
unsigned long string_to(const std::string& str)
{
    return std::stoul(str);
}

template<>
long long string_to(const std::string& str)
{
    return std::stoul(str);
}

template<>
unsigned long long string_to(const std::string& str)
{
    return std::stoull(str);
}