/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include "utils.hpp"

#include <cmath>
#include <numeric>
#include <tuple>
#include <vector>

namespace rapidfuzz {
namespace levenshtein {

/**
 * @defgroup Levenshtein Levenshtein
 * Levenshtein functions
 * @{
 */

// These functions are ment for internal usage only and not guaranteed to have a
// stable interface
namespace detail {

template <typename CharT1, typename CharT2>
struct LevFilter {
  bool not_zero;
  basic_string_view<CharT1> s1_view;
  basic_string_view<CharT2> s2_view;

  LevFilter(bool not_zero, basic_string_view<CharT1> s1_view, basic_string_view<CharT2> s2_view)
      : not_zero(not_zero), s1_view(s1_view), s2_view(s2_view)
  {}
};

template <typename CharT1, typename CharT2>
LevFilter<CharT1, CharT2> quick_lev_filter(basic_string_view<CharT1> s1,
                                           basic_string_view<CharT2> s2,
                                           double min_ratio = 0);

template <typename Sentence1, typename Sentence2>
double normalized_weighted_impl(const Sentence1& s1, const Sentence2& s2, std::size_t lensum,
                                double min_ratio = 0.0);
} // namespace detail

struct WeightTable {
  std::size_t insert_cost;
  std::size_t delete_cost;
  std::size_t replace_cost;
};

template <typename Sentence1, typename Sentence2>
std::size_t distance(const Sentence1& s1, const Sentence2& s2,
                     std::size_t max = std::numeric_limits<std::size_t>::max());

template <typename Sentence1, typename Sentence2>
std::size_t weighted_distance(const Sentence1& s1, const Sentence2& s2,
                              std::size_t max = std::numeric_limits<std::size_t>::max());

// TODO add max distance like it is done for distance/weighted_distance
template <typename Sentence1, typename Sentence2>
std::size_t generic_distance(const Sentence1& s1, const Sentence2& s2,
                             WeightTable weights = {1, 1, 1},
                             std::size_t max = std::numeric_limits<std::size_t>::max());

template <typename Sentence1, typename Sentence2>
double normalized_distance(const Sentence1& s1, const Sentence2& s2, double min_ratio = 0.0);

template <typename Sentence1, typename Sentence2>
double normalized_weighted_distance(const Sentence1& s1, const Sentence2& s2,
                                    double min_ratio = 0.0);

template <typename Sentence1, typename Sentence2>
std::size_t hamming(const Sentence1& s1, const Sentence2& s2);

template <typename Sentence1, typename Sentence2>
double normalized_hamming(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0);

/**@}*/

} // namespace levenshtein
} // namespace rapidfuzz

#include "levenshtein.txx"
