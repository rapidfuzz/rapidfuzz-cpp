/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#include "levenshtein.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

namespace rapidfuzz {

template <typename Sentence1, typename Sentence2>
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

  std::size_t dist = distance(sentence1, sentence2);

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
