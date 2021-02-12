/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#include "rapidfuzz/details/common.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <array>
#include <limits>

namespace rapidfuzz {
namespace string_metric {
namespace detail {

/*
 * An encoded mbleven model table.
 *
 * Each 8-bit integer represents an edit sequence, with using two
 * bits for a single operation.
 *
 * Each Row of 8 integers represent all possible combinations
 * of edit sequences for a gived maximum edit distance and length
 * difference between the two strings, that is below the maximum
 * edit distance
 *
 *   01 = DELETE, 10 = INSERT, 11 = SUBSTITUTE
 *
 * For example, 3F -> 0b111111 means three substitutions
 */
static constexpr uint8_t weighted_levenshtein_mbleven2018_matrix[14][8] = {
  /* max edit distance 1 */
  {0}, /* case does not occur */              /* len_diff 0 */
  {0x01},                                     /* len_diff 1 */
  /* max edit distance 2 */
  {0x03, 0x09, 0x06},                         /* len_diff 0 */
  {0x01},                                     /* len_diff 1 */
  {0x05},                                     /* len_diff 2 */
  /* max edit distance 3 */
  {0x03, 0x09, 0x06},                         /* len_diff 0 */
  {0x25, 0x19, 0x16, 0x0D, 0x07},             /* len_diff 1 */
  {0x05},                                     /* len_diff 2 */
  {0x15},                                     /* len_diff 3 */
  /* max edit distance 4 */
  {0x0F, 0x39, 0x36, 0x1E, 0x1B, 0x2D, 0x27}, /* len_diff 0 */
  {0x0D, 0x07, 0x19, 0x16, 0x25},             /* len_diff 1 */
  {0x35, 0x1D, 0x17},                         /* len_diff 2 */
  {0x15},                                     /* len_diff 3 */
  {0x55},                                     /* len_diff 4 */
};

template <typename CharT1, typename CharT2>
std::size_t weighted_levenshtein_mbleven2018(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2, std::size_t max)
{
  std::size_t len_diff = s1.size() - s2.size();
  auto possible_ops = weighted_levenshtein_mbleven2018_matrix[(max + max * max) / 2 + len_diff - 1];
  std::size_t dist = max + 1;

  for (int pos = 0; possible_ops[pos] != 0; ++pos) {
    uint8_t ops = possible_ops[pos];
    std::size_t s1_pos = 0;
    std::size_t s2_pos = 0;
    std::size_t cur_dist = 0;

    while (s1_pos < s1.size() && s2_pos < s2.size()) {
      if (s1[s1_pos] != s2[s2_pos]) {
        // substitutions have a weight of 2
        if ((ops & 0x3) == 3) {
          cur_dist += 2;
        } else {
          cur_dist++;
        }

        if (!ops) break;
        if (ops & 1) s1_pos++;
        if (ops & 2) s2_pos++;
        ops >>= 2;
      } else {
        s1_pos++;
        s2_pos++;
      }
    }

    cur_dist += (s1.size() - s1_pos) + (s2.size() - s2_pos);
    dist = std::min(dist, cur_dist);
  }

  return (dist > max) ? -1 : dist;
}

template <typename CharT1, std::size_t size>
static inline std::size_t weighted_levenshtein_bitpal(basic_string_view<CharT1> s1,
  const common::blockmap_entry<size>& block, std::size_t s2_len)
{
  uint64_t DHneg1 = ~0x0ull;
  uint64_t DHzero = 0;
  uint64_t DHpos1 = 0;

  for (std::size_t i = 0; i < s1.size(); ++i)
  {
    uint64_t Matches = block.get(s1[i]);
    uint64_t NotMatches = ~Matches;

    uint64_t INITpos1s = DHneg1 & Matches;
    uint64_t DVpos1shift = (((INITpos1s + DHneg1) ^ DHneg1) ^ INITpos1s);

    uint64_t RemainDHneg1 = DHneg1 ^ (DVpos1shift >> 1);
    uint64_t DVpos1shiftorMatch = DVpos1shift | Matches;

    uint64_t INITzeros = (DHzero & DVpos1shiftorMatch) ;
    uint64_t DVzeroshift = ((INITzeros << 1) + RemainDHneg1) ^ RemainDHneg1;

    uint64_t DVneg1shift = ~(DVpos1shift | DVzeroshift);
    DHzero &= NotMatches;
    uint64_t DHpos1orMatch = DHpos1| Matches;
    DHzero = (DVzeroshift & DHpos1orMatch) | (DVneg1shift & DHzero);
    DHpos1 = (DVneg1shift & DHpos1orMatch);
    DHneg1 = ~(DHzero | DHpos1);
  }

  std::size_t dist = s1.size();

  for (std::size_t i = 0; i < s2_len; i++)
  {
    uint64_t bitmask = 1ull << i;
    dist -= ((DHzero & bitmask) >> i) * 1 + ((DHpos1 & bitmask) >> i) * 2 - 1;
  }

  return dist;
}

template <typename CharT1, typename CharT2>
std::size_t weighted_levenshtein_bitpal(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2)
{
  common::blockmap_entry<sizeof(CharT2)> block;

  for (std::size_t i = 0; i < s2.size(); i++){
    block.insert(s2[i], i);
  }

  return weighted_levenshtein_bitpal(s1, block, s2.size());
}

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

//TODO this implementation needs some cleanup
template <typename CharT1, typename CharT2, std::size_t size>
std::size_t weighted_levenshtein(basic_string_view<CharT1> s1,
  const common::blockmap_entry<size>& block, basic_string_view<CharT2> s2,
  std::size_t max)
{
  // when no differences are allowed a direct comparision is sufficient
  if (max == 0) {
    if (s1.size() != s2.size()) {
      return -1;
    }
    return std::equal(s1.begin(), s1.end(), s2.begin()) ? 0 : -1;
  }

  // when the strings have a similar length each difference causes
  // at least a edit distance of 2, so a direct comparision is sufficient
  if (max == 1) {
    if (s1.size() == s2.size()) {
      return std::equal(s1.begin(), s1.end(), s2.begin()) ? 0 : -1;
    }
  }

  // at least length difference insertions/deletions required
  std::size_t len_diff = (s1.size() < s2.size()) ? s2.size() - s1.size() : s1.size() - s2.size();
  if (len_diff > max) {
    return -1;
  }

  // to this first, since we can not remove any affix in encoded form
  if (max >= 5) {
    std::size_t dist = weighted_levenshtein_bitpal(s1, block, s2.size());
    return (dist > max) ? -1 : dist;
  }

  // The Levenshtein distance between <prefix><string1><suffix> and <prefix><string2><suffix>
  // is similar to the distance between <string1> and <string2>, so they can be removed in linear time
  common::remove_common_affix(s1, s2);

  if (s2.empty()) {
    return s1.size();
  }

  if (s1.empty()) {
    return s2.size();
  }

  if (s1.size() > s2.size()) {
    return weighted_levenshtein_mbleven2018(s1, s2, max);
  } else {
    return weighted_levenshtein_mbleven2018(s2, s1, max);
  }
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

  // when the strings have a similar length each difference causes
  // at least a edit distance of 2, so a direct comparision is sufficient
  if (max == 1) {
    if (s1.size() == s2.size()) {
      return std::equal(s1.begin(), s1.end(), s2.begin()) ? 0 : -1;
    }
  }

  // at least length difference insertions/deletions required
  if (s1.size() - s2.size() > max) {
    return -1;
  }

  // The Levenshtein distance between <prefix><string1><suffix> and <prefix><string2><suffix>
  // is similar to the distance between <string1> and <string2>, so they can be removed in linear time
  common::remove_common_affix(s1, s2);

  if (s2.empty()) {
    return s1.size();
  }

  if (max < 5) {
    return weighted_levenshtein_mbleven2018(s1, s2, max);
  }

  // when the second string has less than 64 characters bitpal can be used
  if (s2.size() < 65) {
    std::size_t dist = weighted_levenshtein_bitpal(s1, s2);
    return (dist > max) ? -1 : dist;
  }

  // find uncommon chars in the two sequences to exit early in many cases in
  // linear time
  // TODO add blockwise BitPal implementation
  if ((max < s1.size() + s2.size()) && (common::count_uncommon_chars(s1, s2) > max)) {
    return -1;
  }

  return weighted_levenshtein_wagner_fischer(s1, s2, max);
}

template <typename CharT1, typename CharT2, std::size_t size>
double normalized_weighted_levenshtein(basic_string_view<CharT1> s1,
  const common::blockmap_entry<size>& block, basic_string_view<CharT2> s2,
  const double score_cutoff)
{
  if (s1.empty() || s2.empty()) {
    return 100.0 * static_cast<double>(s1.empty() && s2.empty());
  }

  std::size_t lensum = s1.size() + s2.size();

  auto cutoff_distance = common::score_cutoff_to_distance(score_cutoff, lensum);

  std::size_t dist = weighted_levenshtein(s1, block, s2, cutoff_distance);
  return (dist != (std::size_t)-1)
    ? common::norm_distance(dist, lensum, score_cutoff)
    : 0.0;
}

template <typename CharT1, typename CharT2>
double normalized_weighted_levenshtein(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2, const double score_cutoff)
{
  if (s1.empty() || s2.empty()) {
    return 100.0 * static_cast<double>(s1.empty() && s2.empty());
  }

  std::size_t lensum = s1.size() + s2.size();

  auto cutoff_distance = common::score_cutoff_to_distance(score_cutoff, lensum);

  std::size_t dist = weighted_levenshtein(s1, s2, cutoff_distance);
  return (dist != (std::size_t)-1)
    ? common::norm_distance(dist, lensum, score_cutoff)
    : 0.0;
}

} // namespace detail
} // namespace string_metric
} // namespace rapidfuzz
