/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include "utils.hpp"

#include <tuple>
#include <vector>

namespace rapidfuzz {

struct Affix {
    std::size_t prefix_len;
    std::size_t suffix_len;
};

namespace string_utils {

template<typename CharT>
string_view_vec<CharT> splitSV(const basic_string_view<CharT>& s);

template<typename CharT>
string_view_vec<CharT> splitSV(const std::basic_string<CharT>& str);

template<typename CharT>
std::size_t joined_size(const string_view_vec<CharT>& x);

template<typename CharT>
std::basic_string<CharT> join(const string_view_vec<CharT>& sentence);

template<typename CharT>
Affix remove_common_affix(basic_string_view<CharT>& a, basic_string_view<CharT>& b);

template<typename Sentence1, typename Sentence2>
std::size_t count_uncommon_chars(const Sentence1 &s1, const Sentence2 &s2);

template<typename CharT>
void lower_case(std::basic_string<CharT>& s);

template<typename CharT>
void replace_non_alnum(std::basic_string<CharT>& s);

template<typename CharT>
std::basic_string<CharT> default_process(std::basic_string<CharT> s);

} /* string_utils */ } /* rapidfuzz */

#include "string_utils.txx"
