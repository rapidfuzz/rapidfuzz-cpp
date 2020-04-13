/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include <boost/utility/string_view.hpp>
#include <vector>
#include <tuple>

// taken from https://stackoverflow.com/questions/52737760/how-to-define-string-literal-with-character-type-that-depends-on-template-parame/52742402#52742402
#define TOWSTRING_(x) L##x
#define TOWSTRING(x) TOWSTRING_(x)  
#define CHARTYPE_STR(C, STR) (std::get<const C*>(std::tuple<const char*, const wchar_t*>(STR, TOWSTRING(STR))))

template<typename CharT>
using string_view_vec = std::vector<boost::basic_string_view<CharT>>;

template<typename CharT>
struct Sentence {
    boost::basic_string_view<CharT> sentence;
    uint64_t bitmap = 0;
    Sentence(boost::basic_string_view<CharT> sentence, uint64_t bitmap)
        : sentence(sentence), bitmap(bitmap) {}
    Sentence(boost::basic_string_view<CharT> sentence)
        : sentence(sentence), bitmap(0) {}
    Sentence(std::basic_string<CharT> sentence, uint64_t bitmap)
        : sentence(boost::basic_string_view<CharT>(sentence)), bitmap(bitmap) {}
    Sentence(std::basic_string<CharT> sentence)
        : sentence(boost::basic_string_view<CharT>(sentence)), bitmap(0) {}
};

struct Affix {
    std::size_t prefix_len;
    std::size_t suffix_len;
};

namespace string_utils {

string_view_vec<char> splitSV(const boost::basic_string_view<char>& str);

string_view_vec<wchar_t> splitSV(const boost::basic_string_view<wchar_t>& str);

template<typename CharT>
string_view_vec<CharT> splitSV(const std::basic_string<CharT>& str);

template<typename CharT>
std::size_t joined_size(const string_view_vec<CharT>& x);

template<typename CharT>
std::basic_string<CharT> join(const string_view_vec<CharT>& sentence);

template<typename CharT>
Affix remove_common_affix(boost::basic_string_view<CharT>& a, boost::basic_string_view<CharT>& b);

template<typename CharT>
void trim(std::basic_string<CharT>& s);

void lower_case(std::string& s);
void lower_case(std::wstring& s);

template<typename CharT>
void replace_non_alnum(std::basic_string<CharT>& s);

template<typename CharT>
std::basic_string<CharT> default_process(std::basic_string<CharT> s);
}

#include "string_utils.txx"