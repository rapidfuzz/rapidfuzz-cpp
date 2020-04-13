/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#include "string_utils.hpp"
#include <algorithm>
#include <cwctype>
#include <cctype>

inline string_view_vec<char> string_utils::splitSV(const boost::basic_string_view<char>& str)
{
    string_view_vec<char> output;

    auto first = str.data(), second = str.data(), last = first + str.size();
    for (; second != last && first != last; first = second + 1) {
        second = std::find_if(first, last, [](const char& c) {
                return std::isspace(c);
            });

        if (first != second) {
            output.emplace_back(first, second - first);
        }
    }

    return output;
}

inline string_view_vec<wchar_t> string_utils::splitSV(const boost::basic_string_view<wchar_t>& str)
{
    string_view_vec<wchar_t> output;

    auto first = str.data(), second = str.data(), last = first + str.size();
    for (; second != last && first != last; first = second + 1) {
        second = std::find_if(first, last, [](const wchar_t& c) {
                return std::iswspace(c);
            });

        if (first != second) {
            output.emplace_back(first, second - first);
        }
    }

    return output;
}

template<typename CharT>
string_view_vec<CharT> splitSV(const std::basic_string<CharT>& str)
{
    return splitSV(boost::basic_string_view<CharT>(str));
}

template<typename CharT>
std::size_t string_utils::joined_size(const string_view_vec<CharT>& x)
{
    if (x.empty()) {
        return 0;
    }

    // there is a whitespace between each word
    std::size_t result = x.size() - 1;
    for (const auto& y : x) {
        result += y.size();
    }

    return result;
}

template<typename CharT>
std::basic_string<CharT> string_utils::join(const string_view_vec<CharT>& sentence)
{
    if (sentence.empty()) {
        return std::basic_string<CharT>();
    }

    auto sentence_iter = sentence.begin();
    std::basic_string<CharT> result{ *sentence_iter };
    const std::basic_string<CharT> whitespace{ 0x20 };
    ++sentence_iter;
    for (; sentence_iter != sentence.end(); ++sentence_iter) {
        result.append(whitespace).append(std::basic_string<CharT>{ *sentence_iter });
    }
    return result;
}

/**
 * Finds the longest common prefix between two ranges
 */
template <typename InputIterator1, typename InputIterator2>
inline auto common_prefix_length(InputIterator1 first1, InputIterator1 last1,
    InputIterator2 first2, InputIterator2 last2)
{
    return std::distance(first1, std::mismatch(first1, last1, first2, last2).first);
}

/**
 * Removes common prefix of two string views
 */
template<typename CharT>
std::size_t remove_common_prefix(boost::basic_string_view<CharT>& a, boost::basic_string_view<CharT>& b)
{
    auto prefix = common_prefix_length(a.begin(), a.end(), b.begin(), b.end());
    a.remove_prefix(prefix);
    b.remove_prefix(prefix);
    return prefix;
}

/**
 * Removes common suffix of two string views
 */
template<typename CharT>
std::size_t remove_common_suffix(boost::basic_string_view<CharT>& a, boost::basic_string_view<CharT>& b)
{
    auto suffix = common_prefix_length(a.rbegin(), a.rend(), b.rbegin(), b.rend());
    a.remove_suffix(suffix);
    b.remove_suffix(suffix);
    return suffix;
}

/**
 * Removes common affix of two string views
 */
template<typename CharT>
Affix string_utils::remove_common_affix(boost::basic_string_view<CharT>& a, boost::basic_string_view<CharT>& b)
{
    return Affix{
        remove_common_prefix(a, b),
        remove_common_suffix(a, b)
    };
}

inline void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](const char& ch) {
            return !std::isspace(ch);
        }));
}

inline void ltrim(std::wstring& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](const wchar_t& ch) {
            return !std::iswspace(ch);
        }));
}

inline void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](const char& ch) {
            return !std::isspace(ch);
        }).base(), s.end());
}

inline void rtrim(std::wstring& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](const wchar_t& ch) {
            return !std::iswspace(ch);
        }).base(), s.end());
}

template<typename CharT>
void string_utils::trim(std::basic_string<CharT>& s)
{
    ltrim(s);
    rtrim(s);
}

void string_utils::lower_case(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

void string_utils::lower_case(std::wstring& s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::towlower);
}

template<typename CharT>
void string_utils::replace_non_alnum(std::basic_string<CharT>& s) {
    // replace punctuation and control control characters
    // iscntrl and !alnum did replace e.g. some cyrillic characters aswell
    // so the ranges are manually checked
    std::replace_if(s.begin(), s.end(), [](CharT ch) {
        return std::ispunct(ch) || (int)ch < 0x1F || (int)ch == 0x7F;
    }, (CharT)0x20);
}

template<typename CharT>
std::basic_string<CharT> string_utils::default_process(std::basic_string<CharT> s)
{
    replace_non_alnum(s);
    trim(s);
    lower_case(s);
    return s;
}
