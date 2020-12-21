/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#include "rapidfuzz/utils.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <array>
#include <limits>


namespace rapidfuzz {
namespace string_metric {
namespace detail {

template <typename CharT1, typename CharT2>
std::size_t weighted_levenshtein_wagner_fischer(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2, std::size_t max)
{
  std::size_t len_diff = s1.size() - s2.size();
  std::size_t max_shift = (max <= s1.size()) ? max : s1.size();

  std::vector<std::size_t> cache(s1.size());
  std::iota(cache.begin(), cache.begin() + max_shift, 1);
  std::fill(cache.begin() + max_shift, cache.end(), max + 1);

  const std::size_t offset = max_shift - len_diff;
  const bool haveMax = max < (2 * s2.size() + len_diff);

  std::size_t jStart = 0;
  std::size_t jEnd = max_shift;

  std::size_t s2_pos = 0;

  for (const auto& char2 : s2) {
    auto cache_iter = cache.begin();
    std::size_t current_cache = s2_pos;
    std::size_t result = s2_pos + 1;

    jStart += (s2_pos > offset) ? 1 : 0;
    jEnd += (jEnd < s1.size()) ? 1 : 0;

    for (const auto& char1 : s1) {
      if (char1 == char2) {
        result = current_cache;
      }
      else {
        ++result;
      }
      current_cache = *cache_iter;
      if (result > current_cache + 1) {
        result = current_cache + 1;
      }

      *cache_iter = result;
      ++cache_iter;
    }

    if (haveMax && cache[s2_pos + len_diff] > max) {
      return -1;
    }

    ++s2_pos;
  }

  return (cache.back() <= max) ? cache.back() : -1;
}

template <typename CharT1, typename CharT2>
std::size_t weighted_levenshtein(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2, std::size_t max)
{
  // Swapping the strings so the second string is shorter
  if (s1.size() < s2.size()) {
    return weighted_levenshtein(s2, s1, max);
  }

  // when no differences are allowed a direct comparision is sufficient
  if (max == 0) {
    if (s1.size() != s2.size()) {
      return -1;
    }
    return std::equal(s1.begin(), s1.end(), s2.begin()) ? 0 : -1;
  }

  // at least length difference insertions/deletions required
  if (s1.size() - s2.size() > max) {
    return -1;
  }

  // The Levenshtein distance between <prefix><string1><suffix> and <prefix><string2><suffix>
  // is similar to the distance between <string1> and <string2>, so they can be removed in linear time
  utils::remove_common_affix(s1, s2);

  if (s2.empty()) {
    return (s2.size() > max) ? -1 : s2.size();
  }

  if (s1.size() - s2.size() > max) {
    return -1;
  }

  // when both strings only hold characters < 256 and the short strings has less
  // then 65 elements BitPAl algorithm can be used
  // TODO
  /*if (sizeof(CharT1) == 1 && sizeof(CharT2) == 1) {
    if (s2.size() < 65) {
      std::size_t dist = levenshtein_bitpal(s1, s2);
      return (dist > max) ? -1 : dist;
    }
  }*/

  // find uncommon chars in the two sequences to exit early in many cases in
  // linear time
  // TODO after adding BitPal this might no longer be relevant,
  // sind it only helps with the quadratic runtime of Wagner-Fischer
  if (utils::count_uncommon_chars(s1, s2) > max) {
    return -1;
  }

  return weighted_levenshtein_wagner_fischer(s1, s2, max);
}

template <typename CharT1, typename CharT2>
double normalized_weighted_levenshtein(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2, const double score_cutoff)
{
  if (s1.empty() || s2.empty()) {
    return s1.empty() && s2.empty();
  }

  std::size_t lensum = s1.size() + s2.size();

  auto cutoff_distance = utils::score_cutoff_to_distance(score_cutoff, lensum);

  std::size_t dist = weighted_levenshtein(s1, s2, cutoff_distance);
  return (dist != (std::size_t)-1)
    ? utils::norm_distance(dist, lensum, score_cutoff)
    : 0.0;
}

} // namespace detail
} // namespace string_metric
} // namespace rapidfuzz
