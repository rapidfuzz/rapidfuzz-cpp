/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#include "levenshtein.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <array>
#include <limits.h>
#include <iostream>


namespace rapidfuzz {

static const uint64_t bitparallel_distance_masks[64] = {
    0x0000000000000001, 0x0000000000000003, 0x0000000000000007, 0x000000000000000f,
    0x000000000000001f, 0x000000000000003f, 0x000000000000007f, 0x00000000000000ff,
    0x00000000000001ff, 0x00000000000003ff, 0x00000000000007ff, 0x0000000000000fff,
    0x0000000000001fff, 0x0000000000003fff, 0x0000000000007fff, 0x000000000000ffff,
    0x000000000001ffff, 0x000000000003ffff, 0x000000000007ffff, 0x00000000000fffff,
    0x00000000001fffff, 0x00000000003fffff, 0x00000000007fffff, 0x0000000000ffffff,
    0x0000000001ffffff, 0x0000000003ffffff, 0x0000000007ffffff, 0x000000000fffffff,
    0x000000001fffffff, 0x000000003fffffff, 0x000000007fffffff, 0x00000000ffffffff,
    0x00000001ffffffff, 0x00000003ffffffff, 0x00000007ffffffff, 0x0000000fffffffff,
    0x0000001fffffffff, 0x0000003fffffffff, 0x0000007fffffffff, 0x000000ffffffffff,
    0x000001ffffffffff, 0x000003ffffffffff, 0x000007ffffffffff, 0x00000fffffffffff,
    0x00001fffffffffff, 0x00003fffffffffff, 0x00007fffffffffff, 0x0000ffffffffffff,
    0x0001ffffffffffff, 0x0003ffffffffffff, 0x0007ffffffffffff, 0x000fffffffffffff,
    0x001fffffffffffff, 0x003fffffffffffff, 0x007fffffffffffff, 0x00ffffffffffffff,
    0x01ffffffffffffff, 0x03ffffffffffffff, 0x07ffffffffffffff, 0x0fffffffffffffff,
    0x1fffffffffffffff, 0x3fffffffffffffff, 0x7fffffffffffffff, 0xffffffffffffffff,
};

template <typename Sentence1, typename Sentence2>
std::size_t levenshtein::detail::bitparallel_distance(const Sentence1& sentence1, const Sentence2& sentence2)
{
  if (sentence1.size() > 64 || sentence2.size() > 64) {
    return std::numeric_limits<std::size_t>::max();
  }
  std::array<uint64_t, 256> posbits;

  for (int i=0; i < sentence1.size(); i++){
    posbits[(unsigned int)sentence1[i]] |= 0x1ull << i;
  }

  std::size_t dist = sentence1.size();
  uint64_t mask = 0x1ull << (sentence1.size() - 1);
  uint64_t VP   = bitparallel_distance_masks[sentence1.size() - 1];
  uint64_t VN   = 0;

  for (const auto& char2 : sentence2) {
    uint64_t y = posbits[(unsigned int)char2];
    uint64_t X  = y | VN;
    uint64_t D0 = ((VP + (X & VP)) ^ VP) | X;
    uint64_t HN = VP & D0;
    uint64_t HP = VN | ~(VP|D0);
    X  = (HP << 1) | 1;
    VN =  X & D0;
    VP = (HN << 1) | ~(X | D0);
    if (HP & mask) { dist++; }
    if (HN & mask) { dist--; }
  }
  return dist;
}

template <typename Sentence1, typename Sentence2, typename CharT1, typename CharT2>
std::size_t levenshtein::distance(const Sentence1& s1, const Sentence2& s2, std::size_t max)
{
  auto sentence1 = utils::to_string_view(s1);
  auto sentence2 = utils::to_string_view(s2);

  // Swapping the strings so the first string is shorter
  if (sentence1.size() > sentence2.size()) {
    return distance(sentence2, sentence1, max);
  }

  // Ignoring common suffix and prefix
  utils::remove_common_affix(sentence1, sentence2);

  if (sentence1.empty()) {
    return (sentence2.length() > max) ? std::numeric_limits<std::size_t>::max()
                                      : sentence2.length();
  }

  const std::size_t len_diff = sentence2.length() - sentence1.length();

  if (len_diff > max) {
    return std::numeric_limits<std::size_t>::max();
  }

  // use bitparallel implementation when possible which has linear runtime
  if (sizeof(CharT1) == 1 && sizeof(CharT2) == 1) {
    if (sentence1.size() <= 64) {
      std::size_t dist = detail::bitparallel_distance(sentence1, sentence2);
      return (dist > max) ? std::numeric_limits<std::size_t>::max() : dist;
    } else if (sentence2.size() <= 64) {
      std::size_t dist = detail::bitparallel_distance(sentence2, sentence1);
      return (dist > max) ? std::numeric_limits<std::size_t>::max() : dist;
    }
  }

  if (max > sentence2.length()) {
    max = sentence2.length();
  }

  std::vector<std::size_t> cache(sentence2.length());
  std::iota(cache.begin(), cache.begin() + max, 1);
  std::fill(cache.begin() + max, cache.end(), max + 1);

  const std::size_t offset = max - len_diff;
  const bool haveMax = max < sentence2.length();

  std::size_t jStart = 0;
  std::size_t jEnd = max;

  std::size_t current = 0;
  std::size_t left;
  std::size_t above;
  std::size_t sentence1_pos = 0;

  for (const auto& char1 : sentence1) {
    left = sentence1_pos;
    above = sentence1_pos + 1;

    jStart += (sentence1_pos > offset) ? 1 : 0;
    jEnd += (jEnd < sentence2.length()) ? 1 : 0;

    for (std::size_t j = jStart; j < jEnd; j++) {
      above = current;
      current = left;
      left = cache[j];

      if (char1 != sentence2[j]) {

        // Insertion
        if (left < current) current = left;

        // Deletion
        if (above < current) current = above;

        ++current;
      }

      cache[j] = current;
    }

    if (haveMax && cache[sentence1_pos + len_diff] > max) {
      return std::numeric_limits<std::size_t>::max();
    }
    ++sentence1_pos;
  }

  return (cache.back() <= max) ? cache.back() : std::numeric_limits<std::size_t>::max();
}

template <typename Sentence1, typename Sentence2>
std::size_t levenshtein::weighted_distance(const Sentence1& s1, const Sentence2& s2,
                                           std::size_t max)
{
  auto sentence1 = utils::to_string_view(s1);
  auto sentence2 = utils::to_string_view(s2);

  // Swapping the strings so the first string is shorter
  if (sentence1.size() > sentence2.size()) {
    return weighted_distance(sentence2, sentence1, max);
  }

  // Ignoring common suffix and prefix
  utils::remove_common_affix(sentence1, sentence2);

  if (sentence1.empty()) {
    return (sentence2.length() > max) ? std::numeric_limits<std::size_t>::max()
                                      : sentence2.length();
  }

  const std::size_t len_diff = sentence2.length() - sentence1.length();

  if (len_diff > max) {
    return std::numeric_limits<std::size_t>::max();
  }

  std::size_t max_shift = (max <= sentence2.length()) ? max : sentence2.length();

  std::vector<std::size_t> cache(sentence2.length());
  std::iota(cache.begin(), cache.begin() + max_shift, 1);
  std::fill(cache.begin() + max_shift, cache.end(), max + 1);

  const std::size_t offset = max_shift - len_diff;
  const bool haveMax = max < (2 * sentence1.length() + len_diff);

  std::size_t jStart = 0;
  std::size_t jEnd = max_shift;

  std::size_t sentence1_pos = 0;

  for (const auto& char1 : sentence1) {
    auto cache_iter = cache.begin();
    std::size_t current_cache = sentence1_pos;
    std::size_t result = sentence1_pos + 1;

    jStart += (sentence1_pos > offset) ? 1 : 0;
    jEnd += (jEnd < sentence2.length()) ? 1 : 0;

    for (const auto& char2 : sentence2) {
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

    if (haveMax && cache[sentence1_pos + len_diff] > max) {
      return std::numeric_limits<std::size_t>::max();
    }

    ++sentence1_pos;
  }

  return (cache.back() <= max) ? cache.back() : std::numeric_limits<std::size_t>::max();
}

template <typename Sentence1, typename Sentence2>
std::size_t levenshtein::generic_distance(const Sentence1& s1, const Sentence2& s2,
                                          WeightTable weights, std::size_t max)
{
  auto sentence1 = utils::to_string_view(s1);
  auto sentence2 = utils::to_string_view(s2);

  if (sentence1.size() > sentence2.size()) {
    std::swap(weights.insert_cost, weights.delete_cost);
    return generic_distance(sentence2, sentence1, weights, max);
  }

  utils::remove_common_affix(sentence1, sentence2);

  std::vector<std::size_t> cache(sentence1.size() + 1);

  cache[0] = 0;
  for (std::size_t i = 1; i < cache.size(); ++i) {
    cache[i] = cache[i - 1] + weights.delete_cost;
  }

  for (const auto& char2 : sentence2) {
    auto cache_iter = cache.begin();
    std::size_t temp = *cache_iter;
    *cache_iter += weights.insert_cost;

    for (const auto& char1 : sentence1) {
      if (char1 != char2) {
        temp = std::min({*cache_iter + weights.delete_cost, *(cache_iter + 1) + weights.insert_cost,
                         temp + weights.replace_cost});
      }
      ++cache_iter;
      std::swap(*cache_iter, temp);
    }
  }

  return (cache.back() <= max) ? cache.back() : std::numeric_limits<std::size_t>::max();
}

template <typename Sentence1, typename Sentence2>
double levenshtein::normalized_distance(const Sentence1& s1, const Sentence2& s2,
                                        const double min_ratio)
{
  auto sentence1 = utils::to_string_view(s1);
  auto sentence2 = utils::to_string_view(s2);

  if (sentence1.empty() || sentence2.empty()) {
    return sentence1.empty() && sentence2.empty();
  }

  std::size_t sentence1_len = sentence1.length();
  std::size_t sentence2_len = sentence2.length();
  std::size_t max_len = std::max(sentence1_len, sentence2_len);

  // constant time calculation to find a string ratio based on the string length
  // so it can exit early without running any levenshtein calculations
  std::size_t min_distance = (sentence1_len > sentence2_len) ? sentence1_len - sentence2_len
                                                             : sentence2_len - sentence1_len;

  double len_ratio = utils::norm_distance(min_distance, max_len) / 100.0;
  if (len_ratio < min_ratio) {
    return 0.0;
  }

  std::size_t dist = distance(sentence1, sentence2);;
  double ratio = utils::norm_distance(dist, max_len) / 100.0;
  return (ratio >= min_ratio) ? ratio : 0.0;
}

template <typename Sentence1, typename Sentence2>
double levenshtein::normalized_weighted_distance(const Sentence1& s1, const Sentence2& s2,
                                                 const double min_ratio)
{
  auto sentence1 = utils::to_string_view(s1);
  auto sentence2 = utils::to_string_view(s2);

  if (sentence1.empty() || sentence2.empty()) {
    return sentence1.empty() && sentence2.empty();
  }

  std::size_t sentence1_len = sentence1.length();
  std::size_t sentence2_len = sentence2.length();
  std::size_t lensum = sentence1_len + sentence2_len;

  auto lev_filter = detail::quick_lev_filter(sentence1, sentence2, min_ratio);

  if (!lev_filter.not_zero) {
    return 0.0;
  }

  auto cutoff_distance = static_cast<std::size_t>(static_cast<double>(lensum) * (1.0 - min_ratio));

  // calculate the levenshtein distance in quadratic time
  std::size_t dist = weighted_distance(lev_filter.s1_view, lev_filter.s2_view, cutoff_distance);
  double ratio = utils::norm_distance(dist, lensum) / 100.0;
  return utils::result_cutoff(ratio, min_ratio);
}

template <typename CharT1, typename CharT2>
levenshtein::detail::LevFilter<CharT1, CharT2>
levenshtein::detail::quick_lev_filter(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2,
                                      const double min_ratio)
{
  if (utils::is_zero(min_ratio)) {
    return {true, s1, s2};
  }

  std::size_t s1_len = s1.length();
  std::size_t s2_len = s2.length();
  std::size_t lensum = s1_len + s2_len;

  auto cutoff_distance = static_cast<std::size_t>(static_cast<double>(lensum) * (1.0 - min_ratio));

  // constant time calculation to find a string ratio based on the string length
  // so it can exit early without running any levenshtein calculations
  std::size_t length_distance = (s1_len > s2_len) ? s1_len - s2_len : s2_len - s1_len;

  if (length_distance > cutoff_distance) {
    return {false, s1, s2};
  }

  // remove common prefix + suffix in constant time
  utils::remove_common_affix(s1, s2);

  if (s1.empty()) {
    double ratio = utils::norm_distance(s2.length(), lensum) / 100.0;
    return {ratio >= min_ratio, s1, s2};
  }

  if (s2.empty()) {
    double ratio = utils::norm_distance(s1.length(), lensum) / 100.0;
    return {ratio >= min_ratio, s1, s2};
  }

  // find uncommon chars in the two sequences to exit early in many cases in
  // linear time
  std::size_t uncommon_char_distance = utils::count_uncommon_chars(s1, s2);
  return {uncommon_char_distance <= cutoff_distance, s1, s2};
}

template <typename Sentence1, typename Sentence2>
std::size_t levenshtein::hamming(const Sentence1& s1, const Sentence2& s2)
{
  auto sentence1 = utils::to_string_view(s1);
  auto sentence2 = utils::to_string_view(s2);

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
double levenshtein::normalized_hamming(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
  auto sentence1 = utils::to_string_view(s1);
  auto sentence2 = utils::to_string_view(s2);
  return utils::norm_distance(
    hamming(sentence1, sentence2), sentence1.size(), score_cutoff
  );
}

} // namespace rapidfuzz
