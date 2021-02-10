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
 * @defgroup string_metric string_metric
 * Different useful string_metrics
 * @{
 */


/**
 * @brief Calculates the minimum number of insertions, deletions, and substitutions
 * required to change one sequence into the other according to Levenshtein with custom
 * costs for insertion, deletion and substitution
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 * @param weights
 *   The weights for the three operations in the form
 *   (insertion, deletion, substitution). Default is {1, 1, 1},
 *   which gives all three operations a weight of 1.
 * @param max
 *   Maximum Levenshtein distance between s1 and s2, that is
 *   considered as a result. If the distance is bigger than max,
 *   -1 is returned instead. Default is std::numeric_limits<std::size_t>::max(),
 *   which deactivates this behaviour.
 *
 * @return returns the levenshtein distance between s1 and s2
 *
 * @remarks
 * @parblock
 * Depending on the input parameters different optimized implementation are used
 * to improve the performance. Worst-case performance is ``O(m * n)``.
 *
 * <b>Insertion = 1, Deletion = 1, Substitution = 1:</b>
 *   - if max is 0 the similarity can be calculated using a direct comparision,
 *     since no difference between the strings is allowed.  The time complexity of
 *     this algorithm is ``O(N)``.
 *
 *   - A common prefix/suffix of the two compared strings does not affect
 *     the Levenshtein distance, so the affix is removed before calculating the
 *     similarity.
 *
 *   - If max is ≤ 3 the mbleven algorithm is used. This algorithm
 *     checks all possible edit operations that are possible under
 *     the threshold `max`. The time complexity of this algorithm is ``O(N)``.
 *
 *   - If the length of the shorter string is ≤ 64 after removing the common affix
 *     Hyyrös' algorithm is used, which calculates the Levenshtein distance in
 *     parallel. The algorithm is described by [1]_. The time complexity of this
 *     algorithm is ``O(N)``.
 *
 *   - In all other cases the Levenshtein distance is calculated using
 *     Wagner-Fischer with Ukkonens optimization as described by @cite wagner_fischer_1974.
 *     The time complexity of this algorithm is ``O(N * M)``.
 *     In the future this should be replaced by Myers algorithm (with blocks),
 *     which performs the calculation in parallel aswell (64 characters at a time).
 *     Myers algorithm is described in @cite myers_1999.
 *
 *
 * <b>Insertion = 1, Deletion = 1, Substitution = 2:</b>
 *   - if max is 0 the similarity can be calculated using a direct comparision,
 *     since no difference between the strings is allowed.  The time complexity of
 *     this algorithm is ``O(N)``.
 *
 *   - if max is 1 and the two strings have a similar length, the similarity can be
 *     calculated using a direct comparision aswell, since a substitution would cause
 *     a edit distance higher than max. The time complexity of this algorithm
 *     is ``O(N)``.
 *
 *   - A common prefix/suffix of the two compared strings does not affect
 *     the Levenshtein distance, so the affix is removed before calculating the
 *     similarity.
 *
 *   - If max is ≤ 4 the mbleven algorithm is used. This algorithm
 *     checks all possible edit operations that are possible under
 *     the threshold `max`. As a difference to the normal Levenshtein distance this
 *     algorithm can even be used up to a threshold of 4 here, since the higher weight
 *     of substitutions decreases the amount of possible edit operations.
 *     The time complexity of this algorithm is ``O(N)``.
 *
 *   - If the length of the shorter string is ≤ 64 after removing the common affix
 *     the BitPAl algorithm is used, which calculates the Levenshtein distance in
 *     parallel. The algorithm is described by @cite bitpal_2014 and is extended with
 *     support for UTF32 in this implementation. The time complexity of this
 *     algorithm is ``O(N)``.
 *
 *   - If both strings only use extended Ascii a blockwise BitPAl algorithm is used.
 *     It calculated the Levenshtein distance in blocks of 63 characters and
 *     therefor has a time complexity of ``O(N*M/63)``
 *
 *   - In all other cases the Levenshtein distance is calculated using
 *     Wagner-Fischer with Ukkonens optimization as described by @cite wagner_fischer_1974.
 *     The time complexity of this algorithm is ``O(N * M)``.
 *     This will be removed once UTF32 support is added to the blockwise BitPAl
 *     algorithm.
 *
 * <b>Other weights:</b>
 *   The implementation for other weights is based on Wagner-Fischer.
 *   It has a performance of ``O(N * M)`` and has a memory usage of ``O(N)``.
 *   Further details can be found in @cite wagner_fischer_1974.
 * @endparblock
 *
 * @par Examples
 * @parblock
 * Find the Levenshtein distance between two strings:
 * @code{.cpp}
 * // dist is 2
 * std::size_t dist = levenshtein("lewenstein", "levenshtein");
 * @endcode
 *
 * Setting a maximum distance allows the implementation to select
 * a more efficient implementation:
 * @code{.cpp}
 * // dist is -1
 * std::size_t dist = levenshtein("lewenstein", "levenshtein", {1, 1, 1}, 1);
 * @endcode
 *
 * It is possible to select different weights by passing a `weight` struct.
 * Internally s1 and s2 might be swapped, so insertion and deletion
 * cost should usually have the same value.
 * @code{.cpp}
 * // dist is 3
 * std::size_t dist = levenshtein("lewenstein", "levenshtein", {1, 1, 2});
 * @endcode
 * @endparblock
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

/**
 * @brief Calculates a normalized levenshtein distance using custom
 * costs for insertion, deletion and substitution.
 *
 * @details
 * So far only the following combinations are supported:
 * - weights = (1, 1, 1)
 * - weights = (1, 1, 2)
 *
 * further combinations might be supported in the future
 *
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 * @param weights
 *   The weights for the three operations in the form
 *   (insertion, deletion, substitution). Default is {1, 1, 1},
 *   which gives all three operations a weight of 1.
 * @param score_cutoff
 *   Optional argument for a score threshold as a float between 0 and 100.
 *   For ratio < score_cutoff 0 is returned instead. Default is 0,
 *   which deactivates this behaviour.
 *
 * @return Normalized weighted levenshtein distance between s1 and s2
 *   as a double between 0 and 100
 *
 * @see levenshtein()
 *
 * @remarks
 * @parblock
 * Depending on the provided weights the normalisation is performed in different
 * ways:
 *
 * <b>Insertion = 1, Deletion = 1, Substitution = 1:</b>
 *   \f$ratio = 100 \cdot \frac{distance(s1, s2)}{max(len(s1), len(s2))}\f$
 *
 * <b>Insertion = 1, Deletion = 1, Substitution = 2:</b>
 *   \f$ratio = 100 \cdot \frac{distance(s1, s2)}{len(s1) + len(s2)}\f$
 *
 * Different weights are currently not supported, since the library has no algorithm
 * for normalization yet.
 * @endparblock
 *
 *
 * @par Examples
 * @parblock
 * Find the normalized Levenshtein distance between two strings:
 * @code{.cpp}
 * // ratio is 81.81818181818181
 * double ratio = normalized_levenshtein("lewenstein", "levenshtein");
 * @endcode
 *
 * Setting a score_cutoff allows the implementation to select
 * a more efficient implementation:
 * @code{.cpp}
 * // ratio is 0.0
 * double ratio = normalized_levenshtein("lewenstein", "levenshtein", {1, 1, 1}, 85.0);
 * @endcode
 *
 * It is possible to select different weights by passing a `weight` struct
 * Internally s1 and s2 might be swapped, so insertion and deletion
 * cost should usually have the same value.
 * @code{.cpp}
 * // ratio is 85.71428571428571
 * double ratio = normalized_levenshtein("lewenstein", "levenshtein", {1, 1, 2});
 * @endcode
 * @endparblock
 */
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

/**
 * @brief Calculates the Hamming distance between two strings.
 *
 * @details
 * Both string require a similar length
 *
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 *
 * @return Hamming distance between s1 and s2
 */
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

/**
 * @brief Calculates a normalized hamming distance
 *
 * @details
 * Both string require a similar length
 *
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 * @param score_cutoff
 *   Optional argument for a score threshold as a float between 0 and 100.
 *   For ratio < score_cutoff 0 is returned instead. Default is 0,
 *   which deactivates this behaviour.
 *
 * @return Normalized hamming distance between s1 and s2
 *   as a float between 0 and 100
 */
template <typename Sentence1, typename Sentence2>
double normalized_hamming(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
  auto sentence1 = common::to_string_view(s1);
  auto sentence2 = common::to_string_view(s2);
  return common::norm_distance(
    hamming(sentence1, sentence2), sentence1.size(), score_cutoff
  );
}

template<typename Sentence1>
struct CachedNormalizedHamming {
  using CharT1 = char_type<Sentence1>;

  CachedNormalizedHamming(const Sentence1& s1)
    : s1_view(common::to_string_view(s1)) {}

  template<typename Sentence2>
  double ratio(const Sentence2& s2, percent score_cutoff = 0) const {
    return normalized_hamming(s1_view, s2, score_cutoff);
  }

private:
  rapidfuzz::basic_string_view<CharT1> s1_view;
};


/**@}*/

} // namespace levenshtein
} // namespace rapidfuzz
