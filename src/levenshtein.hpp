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

template <typename Sentence1, typename Sentence2>
struct LevFilter {
  bool not_zero;
  Sentence1 s1_view;
  Sentence2 s2_view;

  LevFilter(bool not_zero, Sentence1 s1_view, Sentence2 s2_view)
      : not_zero(not_zero), s1_view(s1_view), s2_view(s2_view)
  {}
};

template <typename Sentence1, typename Sentence2, typename CharT1 = char_type<Sentence1>,
          typename CharT2 = char_type<Sentence2>>
LevFilter<Sentence1, Sentence2> quick_lev_filter(Sentence1 s1, Sentence2 s2, const double min_ratio = 0);

template <typename Sentence1, typename Sentence2>
double normalized_weighted_impl(const Sentence1& s1, const Sentence2& s2, std::size_t lensum,
                                const double min_ratio = 0.0);
} // namespace detail

struct WeightTable {
  std::size_t insert_cost;
  std::size_t delete_cost;
  std::size_t replace_cost;
};

template <typename Sentence1, typename Sentence2>
std::size_t distance(const Sentence1& s1, const Sentence2& s2);

template <typename Sentence1, typename Sentence2>
std::size_t weighted_distance(const Sentence1& s1, const Sentence2& s2);

template <typename Sentence1, typename Sentence2>
std::size_t generic_distance(const Sentence1& s1, const Sentence2& s2,
                             WeightTable weights = {1, 1, 1});

template <typename Sentence1, typename Sentence2>
double normalized_distance(const Sentence1& s1, const Sentence2& s2, const double min_ratio = 0.0);

template <typename Sentence1, typename Sentence2>
double normalized_weighted_distance(const Sentence1& s1, const Sentence2& s2,
                                    const double min_ratio = 0.0);

/**@}*/

} // namespace levenshtein
} // namespace rapidfuzz

#include "levenshtein.txx"
