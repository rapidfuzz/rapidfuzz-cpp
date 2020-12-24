/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include "details/common.hpp"
#include "details/string_metrics/levenshtein_impl.hpp"
#include "details/string_metrics/weighted_levenshtein_impl.hpp"
#include "details/string_metrics/generic_levenshtein_impl.hpp"

#include <cmath>
#include <numeric>
#include <tuple>
#include <vector>
#include <stdexcept>

namespace rapidfuzz {
namespace string_metric {

/**
 * @defgroup Levenshtein Levenshtein
 * Levenshtein functions
 * @{
 */

template <typename Sentence1, typename Sentence2>
std::size_t levenshtein(const Sentence1& s1, const Sentence2& s2,
                        LevenshteinWeightTable weights = {1, 1, 1},
                        std::size_t max = std::numeric_limits<std::size_t>::max())
{
  auto sentence1 = common::to_string_view(s1);
  auto sentence2 = common::to_string_view(s2);

  if (weights.insert_cost == 1 && weights.delete_cost == 1) {
    if (weights.replace_cost == 1) {
      return detail::levenshtein(sentence1, sentence2, max);
    }
    else if (weights.replace_cost == 2) {
      return detail::weighted_levenshtein(sentence1, sentence2, max);
    }
  }

  return detail::generic_levenshtein(sentence1, sentence2, weights, max);
}

template <typename Sentence1, typename Sentence2>
double normalized_levenshtein(const Sentence1& s1, const Sentence2& s2,
                              LevenshteinWeightTable weights = {1, 1, 1},
                              double score_cutoff = 0.0)
{
  auto sentence1 = common::to_string_view(s1);
  auto sentence2 = common::to_string_view(s2);

  if (weights.insert_cost == 1 && weights.delete_cost == 1) {
    if (weights.replace_cost == 1) {
      return detail::normalized_levenshtein(sentence1, sentence2, score_cutoff);
    }
    else if (weights.replace_cost == 2) {
      return detail::normalized_weighted_levenshtein(sentence1, sentence2, score_cutoff);
    }
  }

  throw std::invalid_argument("The provided weights are not supported");
}


template <typename Sentence1, typename Sentence2>
std::size_t hamming(const Sentence1& s1, const Sentence2& s2)
{
  auto sentence1 = common::to_string_view(s1);
  auto sentence2 = common::to_string_view(s2);

  if (sentence1.size() != sentence2.size()) {
    throw std::invalid_argument("s1 and s2 are not the same length.");
  }

  std::size_t hamm = 0;

  for (std::size_t i = 0; i < sentence1.length(); i++) {
      if (sentence1[i] != sentence2[i]) {
          hamm++;
      }
  }

  return hamm;
}

template <typename Sentence1, typename Sentence2>
double normalized_hamming(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
  auto sentence1 = common::to_string_view(s1);
  auto sentence2 = common::to_string_view(s2);
  return common::norm_distance(
    hamming(sentence1, sentence2), sentence1.size(), score_cutoff
  );
}

/**@}*/

} // namespace levenshtein
} // namespace rapidfuzz
