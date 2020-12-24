/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#include "rapidfuzz/details/common.hpp"
#include <numeric>
#include <algorithm>
#include <array>
#include <limits>


namespace rapidfuzz {
namespace string_metric {
namespace detail {

template <typename CharT1, typename CharT2>
std::size_t generic_levenshtein_wagner_fischer(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2,
                                               LevenshteinWeightTable weights, std::size_t max)
{
  std::vector<std::size_t> cache(s2.size() + 1);

  cache[0] = 0;
  for (std::size_t i = 1; i < cache.size(); ++i) {
    cache[i] = cache[i - 1] + weights.delete_cost;
  }

  for (const auto& char1 : s1) {
    auto cache_iter = cache.begin();
    std::size_t temp = *cache_iter;
    *cache_iter += weights.insert_cost;

    for (const auto& char2 : s2) {
      if (char1 != char2) {
        temp = std::min({*cache_iter + weights.delete_cost, *(cache_iter + 1) + weights.insert_cost,
                         temp + weights.replace_cost});
      }
      ++cache_iter;
      std::swap(*cache_iter, temp);
    }
  }

  return (cache.back() <= max) ? cache.back() : -1;
}

template <typename CharT1, typename CharT2>
std::size_t generic_levenshtein(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2,
                                LevenshteinWeightTable weights, std::size_t max)
{
  // Swapping the strings so the second string is shorter
  if (s1.size() < s2.size()) {
    std::swap(weights.insert_cost, weights.delete_cost);
    return generic_levenshtein(s2, s1, weights, max);
  }

  // The Levenshtein distance between <prefix><string1><suffix> and <prefix><string2><suffix>
  // is similar to the distance between <string1> and <string2>, so they can be removed in linear time
  common::remove_common_affix(s1, s2);

  return generic_levenshtein_wagner_fischer(s1, s2, weights, max);
}

} // namespace detail
} // namespace levenshtein
} // namespace rapidfuzz