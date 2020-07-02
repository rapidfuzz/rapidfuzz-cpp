/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include "details/SentenceView.hpp"
#include "details/SplittedSentenceView.hpp"
#include "details/type_traits.hpp"
#include "details/types.hpp"
#include <cmath>
#include <tuple>
#include <vector>

namespace rapidfuzz {

template <typename CharT1, typename CharT2, typename CharT3>
struct DecomposedSet {
  SplittedSentenceView<CharT1> difference_ab;
  SplittedSentenceView<CharT2> difference_ba;
  SplittedSentenceView<CharT3> intersection;
  DecomposedSet(SplittedSentenceView<CharT1> diff_ab, SplittedSentenceView<CharT2> diff_ba,
                SplittedSentenceView<CharT3> intersect)
      : difference_ab(std::move(diff_ab)),
        difference_ba(std::move(diff_ba)),
        intersection(std::move(intersect))
  {}
};

namespace utils {

/**
 * @defgroup Utils Utils
 * Utility functions
 * @{
 */

template <typename CharT1, typename CharT2>
DecomposedSet<CharT1, CharT2, CharT1> set_decomposition(SplittedSentenceView<CharT1> a,
                                                        SplittedSentenceView<CharT2> b);

percent result_cutoff(const double result, const percent score_cutoff);

percent norm_distance(std::size_t dist, std::size_t lensum, percent score_cutoff = 0);

template <typename T>
bool is_zero(T a, T tolerance = std::numeric_limits<T>::epsilon());

template <
    typename Sentence, typename CharT = char_type<Sentence>,
    typename = std::enable_if_t<is_explicitly_convertible_v<Sentence, basic_string_view<CharT>>>>
basic_string_view<CharT> to_string_view(const Sentence& str);

template <
    typename Sentence, typename CharT = char_type<Sentence>,
    typename = std::enable_if_t<!is_explicitly_convertible_v<Sentence, basic_string_view<CharT>>>>
basic_string_view<CharT> to_string_view(Sentence str);

template <typename CharT1, typename CharT2>
StringAffix remove_common_affix(basic_string_view<CharT1>& a, basic_string_view<CharT2>& b);

template <typename Sentence>
std::array<unsigned int, 32> char_freq(const Sentence& str);

template <typename Sentence>
std::array<unsigned int, 32> char_freq(const std::vector<Sentence>& str);

std::array<unsigned int, 32> char_freq(const std::array<unsigned int, 32>& freq);

template <typename Sentence1, typename Sentence2>
std::size_t count_uncommon_chars(const Sentence1& s1, const Sentence2& s2);

template <typename CharT>
void lower_case(std::basic_string<CharT>& s);

template <typename CharT>
void replace_non_alnum(std::basic_string<CharT>& s);

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

template <typename Sentence, typename CharT = char_type<Sentence>>
std::basic_string<CharT> default_process(Sentence s);

/**@}*/

} // namespace utils
} // namespace rapidfuzz

#include "utils.txx"
