/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include "type_traits.hpp"
#include "types.hpp"
#include <cmath>
#include <tuple>
#include <vector>

namespace rapidfuzz {
namespace utils {

/**
 * @defgroup Utils Utils
 * Utility functions
 * @{
 */

template <typename CharT1, typename CharT2>
DecomposedSet<CharT1, CharT2, CharT1> set_decomposition(string_view_vec<CharT1> a,
                                       string_view_vec<CharT2> b);

percent result_cutoff(const double result, const percent score_cutoff);

template <typename T>
static bool is_zero(T a, T tolerance = std::numeric_limits<T>::epsilon())
{
  return std::fabs(a) <= tolerance;
}

template <typename Sentence, typename CharT = char_type<Sentence>,
          typename = std::enable_if_t<
              is_explicitly_convertible_v<Sentence, basic_string_view<CharT>>>>
basic_string_view<CharT> to_string_view(const Sentence& str)
{
  return basic_string_view<CharT>(str);
}

template <typename Sentence, typename CharT = char_type<Sentence>,
          typename = std::enable_if_t<
              !is_explicitly_convertible_v<Sentence, basic_string_view<CharT>>>>
basic_string_view<CharT> to_string_view(Sentence str)
{
  return basic_string_view<CharT>(str.data(), str.size());
}

template <typename CharT>
string_view_vec<CharT> splitSV(const basic_string_view<CharT>& s);

template <typename Sentence, typename CharT = char_type<Sentence>>
string_view_vec<CharT> splitSV(const Sentence& str);

template <typename CharT>
std::size_t joined_size(const string_view_vec<CharT>& x);

template <typename CharT>
std::basic_string<CharT> join(const string_view_vec<CharT>& sentence);

template <typename CharT1, typename CharT2>
StringAffix remove_common_affix(basic_string_view<CharT1>& a,
                                basic_string_view<CharT2>& b);

template <typename Sentence>
std::array<unsigned int, 32> char_freq(const Sentence& str);

template <typename Sentence>
std::array<unsigned int, 32> char_freq(const std::vector<Sentence>& str);

std::array<unsigned int, 32>
char_freq(const std::array<unsigned int, 32>& freq);

template <typename Sentence1, typename Sentence2>
std::size_t count_uncommon_chars(const Sentence1& s1, const Sentence2& s2);

template <typename CharT> void lower_case(std::basic_string<CharT>& s);

template <typename CharT> void replace_non_alnum(std::basic_string<CharT>& s);

/**
 * @brief removes any non alphanumeric characters, trim whitespaces from
 * beginning/end and lowercase the string
 *
 * @tparam CharT char type of the string
 *
 * @param s string to process
 *
 * @return returns the processed string
 */
template <typename CharT>
std::basic_string<CharT> default_process(std::basic_string<CharT> s);

/**@}*/

} // namespace utils
} // namespace rapidfuzz

#include "utils.txx"
