/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */
/* Copyright © 2011 Adam Cohen */

#include "details/matching_blocks.hpp"
#include "string_metric.hpp"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <vector>

namespace rapidfuzz {

template <typename Sentence1, typename Sentence2>
percent fuzz::ratio(const Sentence1& s1, const Sentence2& s2, const percent score_cutoff)
{
  return string_metric::normalized_levenshtein(s1, s2, {1, 1, 2}, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
percent fuzz::quick_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
  if (common::is_zero(real_quick_ratio(s1, s2, score_cutoff))) {
    return 0;
  }

  size_t distance = common::count_uncommon_chars(s1, s2);
  size_t lensum = s1.length() + s2.length();
  return common::norm_distance(distance, lensum, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
percent fuzz::real_quick_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
  size_t s1_len = s1.length();
  size_t s2_len = s2.length();
  size_t distance = (s1_len > s2_len) ? s1_len - s2_len : s2_len - s1_len;

  size_t lensum = s1_len + s2_len;
  return common::norm_distance(distance, lensum, score_cutoff);
}

template <typename Sentence1, typename Sentence2, typename CharT1, typename CharT2>
percent fuzz::partial_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
  if (score_cutoff > 100) {
    return 0;
  }

  auto s1_view = common::to_string_view(s1);
  auto s2_view = common::to_string_view(s2);

  if (s1_view.empty() || s2_view.empty()) {
    return static_cast<double>(s1_view.empty() && s2_view.empty()) * 100.0;
  }

  if (s1_view.length() > s2_view.length()) {
    return partial_ratio(s2_view, s1_view, score_cutoff);
  }

  if (s1_view.length() == s2_view.length()) {
    // when both strings use the same start address, length and share a common
    // character size, they are guaranteed to be similar
    // this can occur e.g. for string literals
    // Especially in Python this occurs often, since similar strings are often
    // stored only once to save memory
    if ((sizeof(CharT1) == sizeof(CharT2)) && s1_view.data() == s2_view.data()) {
      return 100;
    }

    if (std::equal(s1_view.begin(), s1_view.end(), s2_view.begin())) {
      return 100;
    }
  }

  size_t short_len = s1_view.length();

  auto blocks = get_matching_blocks(s1_view, s2_view);

  // when there is a full match exit early
  for (const auto& block : blocks) {
    if (block.length == short_len) {
      return 100;
    }
  }

  double max_ratio = 0;
  for (const auto& block : blocks) {
    size_t long_start = (block.dpos > block.spos) ? block.dpos - block.spos : 0;
    auto long_substr = s2_view.substr(long_start, short_len);

    double ls_ratio = ratio(s1_view, long_substr, score_cutoff);

    if (ls_ratio > 99.5) {
      return 100;
    }

    if (ls_ratio > max_ratio) {
      score_cutoff = max_ratio = ls_ratio;
    }
  }

  return max_ratio;
}

template <typename Sentence1, typename Sentence2, typename CharT1, typename CharT2>
percent fuzz::token_sort_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
  if (score_cutoff > 100) return 0;

  return ratio(common::sorted_split(s1).join(),
               common::sorted_split(s2).join(), score_cutoff);
}

template <typename Sentence1, typename Sentence2, typename CharT1, typename CharT2>
percent fuzz::partial_token_sort_ratio(const Sentence1& s1, const Sentence2& s2,
                                       percent score_cutoff)
{
  if (score_cutoff > 100) return 0;

  return partial_ratio(common::sorted_split(s1).join(),
                       common::sorted_split(s2).join(), score_cutoff);
}

template <typename Sentence1, typename Sentence2>
percent fuzz::token_set_ratio(const Sentence1& s1, const Sentence2& s2, const percent score_cutoff)
{
  if (score_cutoff > 100) return 0;

  auto tokens_a = common::sorted_split(s1);
  auto tokens_b = common::sorted_split(s2);

  auto decomposition = common::set_decomposition(tokens_a, tokens_b);
  auto intersect = decomposition.intersection;
  auto diff_ab = decomposition.difference_ab;
  auto diff_ba = decomposition.difference_ba;

  // one sentence is part of the other one
  if (!intersect.empty() && (diff_ab.empty() || diff_ba.empty())) {
    return 100;
  }

  auto diff_ab_joined = diff_ab.join();
  auto diff_ba_joined = diff_ba.join();

  size_t ab_len = diff_ab_joined.length();
  size_t ba_len = diff_ba_joined.length();
  size_t sect_len = intersect.length();

  // string length sect+ab <-> sect and sect+ba <-> sect
  size_t sect_ab_len = sect_len + !!sect_len + ab_len;
  size_t sect_ba_len = sect_len + !!sect_len + ba_len;

  percent result = 0;
  auto cutoff_distance = common::score_cutoff_to_distance(score_cutoff, ab_len + ba_len);
  size_t dist = string_metric::levenshtein(diff_ab_joined, diff_ba_joined, {1, 1, 2}, cutoff_distance);
  if (dist != (std::size_t)-1) {
    result = common::norm_distance(dist, sect_ab_len + sect_ba_len, score_cutoff);
  }

  // exit early since the other ratios are 0
  if (intersect.empty()) {
    return result;
  }

  // levenshtein distance sect+ab <-> sect and sect+ba <-> sect
  // since only sect is similar in them the distance can be calculated based on
  // the length difference
  std::size_t sect_ab_dist = !!sect_len + ab_len;
  percent sect_ab_ratio = common::norm_distance(sect_ab_dist, sect_len + sect_ab_len, score_cutoff);

  std::size_t sect_ba_dist = !!sect_len + ba_len;
  percent sect_ba_ratio = common::norm_distance(sect_ba_dist, sect_len + sect_ba_len, score_cutoff);

  return std::max({result, sect_ab_ratio, sect_ba_ratio});
}

template <typename Sentence1, typename Sentence2, typename CharT1, typename CharT2>
percent fuzz::partial_token_set_ratio(const Sentence1& s1, const Sentence2& s2,
                                      percent score_cutoff)
{
  if (score_cutoff > 100) return 0;

  auto decomposition = common::set_decomposition(common::sorted_split(s1),
                                                common::sorted_split(s2));

  // exit early when there is a common word in both sequences
  if (!decomposition.intersection.empty()) return 100;

  return partial_ratio(decomposition.difference_ab.join(), decomposition.difference_ba.join(),
                       score_cutoff);
}

template <typename Sentence1, typename Sentence2>
percent fuzz::token_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
  if (score_cutoff > 100) return 0;

  auto tokens_a = common::sorted_split(s1);
  auto tokens_b = common::sorted_split(s2);

  auto decomposition = common::set_decomposition(tokens_a, tokens_b);
  auto intersect = decomposition.intersection;
  auto diff_ab = decomposition.difference_ab;
  auto diff_ba = decomposition.difference_ba;

  if (!intersect.empty() && (diff_ab.empty() || diff_ba.empty())) {
    return 100;
  }

  auto diff_ab_joined = diff_ab.join();
  auto diff_ba_joined = diff_ba.join();

  size_t ab_len = diff_ab_joined.length();
  size_t ba_len = diff_ba_joined.length();
  size_t sect_len = intersect.length();

  percent result = ratio(tokens_a.join(), tokens_b.join(), score_cutoff);

  // string length sect+ab <-> sect and sect+ba <-> sect
  size_t sect_ab_len = sect_len + !!sect_len + ab_len;
  size_t sect_ba_len = sect_len + !!sect_len + ba_len;

  auto cutoff_distance = common::score_cutoff_to_distance(score_cutoff, ab_len + ba_len);
  size_t dist = string_metric::levenshtein(diff_ab_joined, diff_ba_joined, {1, 1, 2}, cutoff_distance);
  if (dist != (std::size_t)-1) {
    result = std::max(result, common::norm_distance(dist, 2 * sect_ba_len, score_cutoff));
  }

  // exit early since the other ratios are 0
  if (!sect_len) {
    return result;
  }

  // levenshtein distance sect+ab <-> sect and sect+ba <-> sect
  // since only sect is similar in them the distance can be calculated based on
  // the length difference
  std::size_t sect_ab_dist = !!sect_len + ab_len;
  percent sect_ab_ratio = common::norm_distance(sect_ab_dist, sect_len + sect_ab_len, score_cutoff);

  std::size_t sect_ba_dist = !!sect_len + ba_len;
  percent sect_ba_ratio = common::norm_distance(sect_ba_dist, sect_len + sect_ba_len, score_cutoff);

  return std::max({result, sect_ab_ratio, sect_ba_ratio});
}

template <typename Sentence1, typename Sentence2, typename CharT1, typename CharT2>
percent fuzz::partial_token_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
  if (score_cutoff > 100) return 0;

  auto tokens_a = common::sorted_split(s1);
  auto tokens_b = common::sorted_split(s2);

  auto decomposition = common::set_decomposition(tokens_a, tokens_b);

  // exit early when there is a common word in both sequences
  if (!decomposition.intersection.empty()) return 100;

  auto diff_ab = decomposition.difference_ab;
  auto diff_ba = decomposition.difference_ba;

  percent result = partial_ratio(tokens_a.join(), tokens_b.join(), score_cutoff);

  // do not calculate the same partial_ratio twice
  if (tokens_a.word_count() == diff_ab.word_count() &&
      tokens_b.word_count() == diff_ba.word_count()) {
    return result;
  }

  score_cutoff = std::max(score_cutoff, result);
  return std::max(result, partial_ratio(diff_ab.join(), diff_ba.join(), score_cutoff));
}

template <typename Sentence1, typename Sentence2>
percent fuzz::WRatio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
  if (score_cutoff > 100) return 0;

  constexpr double UNBASE_SCALE = 0.95;

  auto s1_view = common::to_string_view(s1);
  auto s2_view = common::to_string_view(s2);

  size_t len_a = s1_view.length();
  size_t len_b = s2_view.length();
  double len_ratio = (len_a > len_b) ? static_cast<double>(len_a) / static_cast<double>(len_b)
                                     : static_cast<double>(len_b) / static_cast<double>(len_a);


  percent end_ratio = ratio(s1, s2, score_cutoff);

  if (len_ratio < 1.5) {
    score_cutoff = std::max(score_cutoff, end_ratio + 0.00001) / UNBASE_SCALE;
    return std::max(end_ratio, token_ratio(s1, s2, score_cutoff) * UNBASE_SCALE);
  }

  const double PARTIAL_SCALE = (len_ratio < 8.0) ? 0.9 : 0.6;

  // increase the score_cutoff by a small step so it might be able to exit early
  score_cutoff = std::max(score_cutoff, end_ratio + 0.00001) / PARTIAL_SCALE;
  end_ratio = std::max(end_ratio, partial_ratio(s1, s2, score_cutoff) * PARTIAL_SCALE);

  score_cutoff = std::max(score_cutoff, end_ratio + 0.00001) / UNBASE_SCALE;
  return std::max(end_ratio,
                  partial_token_ratio(s1, s2, score_cutoff) * UNBASE_SCALE * PARTIAL_SCALE);
}

template <typename Sentence1, typename Sentence2>
percent fuzz::QRatio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
  return ratio(s1, s2, score_cutoff);
}

} // namespace rapidfuzz
