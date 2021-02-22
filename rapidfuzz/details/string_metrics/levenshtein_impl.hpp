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
static constexpr uint8_t levenshtein_mbleven2018_matrix[9][8] = {
  /* max edit distance 1 */
  {0x03},                                     /* len_diff 0 */
  {0x01},                                     /* len_diff 1 */
  /* max edit distance 2 */
  {0x0F, 0x09, 0x06},                         /* len_diff 0 */
  {0x0D, 0x07},                               /* len_diff 1 */
  {0x05},                                     /* len_diff 2 */
  /* max edit distance 3 */
  {0x3F, 0x27, 0x2D, 0x39, 0x36, 0x1E, 0x1B}, /* len_diff 0 */
  {0x3D, 0x37, 0x1F, 0x25, 0x19, 0x16},       /* len_diff 1 */
  {0x35, 0x1D, 0x17},                         /* len_diff 2 */
  {0x15},                                     /* len_diff 3 */
};

template <typename CharT1, typename CharT2>
std::size_t levenshtein_mbleven2018(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2, std::size_t max)
{
  std::size_t len_diff = s1.size() - s2.size();
  auto possible_ops = levenshtein_mbleven2018_matrix[(max + max * max) / 2 + len_diff - 1];
  std::size_t dist = max + 1;

  for (int pos = 0; possible_ops[pos] != 0; ++pos) {
    uint8_t ops = possible_ops[pos];
    std::size_t s1_pos = 0;
    std::size_t s2_pos = 0;
    std::size_t cur_dist = 0;
    while (s1_pos < s1.size() && s2_pos < s2.size()) {
      if (s1[s1_pos] != s2[s2_pos]) {
        cur_dist++;
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

/**
 * @brief Bitparallel implementation of the Levenshtein distance.
 *
 * This implementation requires the first string to have a length <= 64.
 * The algorithm used is described @cite hyrro_2002 and has a time complexity
 * of O(N). Comments and variable names in the implementation follow the
 * paper. This implementation is used internally when the strings are short enough
 *
 * @tparam CharT1 This is the char type of the first sentence
 * @tparam CharT2 This is the char type of the second sentence
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 *
 * @return returns the levenshtein distance between s1 and s2
 */
template <typename CharT1, typename CharT2>
std::size_t levenshtein_hyrroe2003(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2)
{
/* Preprocessing */
  /* pattern match vector with uint32_t support */
  common::PatternMatchVector<sizeof(CharT1)> PM(s1);

  /* VP is set to 1^m. Shifting by bitwidth would be undefined behavior */
  uint64_t VP = (uint64_t)-1;
  if (s1.size() < 64) {
    VP += (uint64_t)1 << s1.size();
  }

  uint64_t VN = 0;
  std::size_t currDist = s1.size();
  /* mask used when computing D[m,j] in the paper 10^(m-1) */
  uint64_t mask = (uint64_t)1 << (s1.size() - 1);

/* Searching */
  for (const auto& ch2 : s2) {
    /* Step 1: Computing D0 */
    uint64_t PM_j = PM.get(ch2);
    uint64_t X = PM_j | VN;
    uint64_t D0 = (((X & VP) + VP) ^ VP) | X;

    /* Step 2: Computing HP and HN */
    uint64_t HP = VN | ~(D0 | VP);
    uint64_t HN = D0 & VP;

    /* Step 3: Computing the value D[m,j] */
    if (HP & mask) { currDist++; }
    if (HN & mask) { currDist--; }

    /* Step 4: Computing Vp and VN */
    X  = (HP << 1) | 1;
    VP = (HN << 1) | ~(D0 | X);
    VN =  X & D0;
  }

  return currDist;
}

#define CDIV(a,b) ((a) / (b) + ((a) % (b) > 0))
#define BIT(i,n) (((i) >> (n)) & 1)
#define FLIP(i,n) ((i) ^ ((uint64_t) 1 << (n)))

/* this is mostly taken from https://github.com/fujimotos/polyleven */
template <typename CharT1, typename CharT2>
std::size_t levenshtein_myers1999_block(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2)
{
  common::BlockPatternMatchVector<sizeof(CharT2)> map(s1);
  std::size_t hsize = CDIV(s2.size(), 64);
  std::size_t vsize = CDIV(s1.size(), 64);
  std::size_t Score = s1.size();

  std::vector<uint64_t> Phc(hsize, (uint64_t)-1);
  std::vector<uint64_t> Mhc(hsize, 0);
  uint64_t Last = (uint64_t)1 << ((s1.size() - 1) % 64);

  for (std::size_t b = 0; b < vsize; b++) {
    uint64_t Mv = 0;
    uint64_t Pv = (uint64_t) -1;
    Score = s1.size();

    for (std::size_t i = 0; i < s2.size(); i++) {
      uint64_t Eq = map.get(b, s2[i]);

      uint8_t Pb = BIT(Phc[i / 64], i % 64);
      uint8_t Mb = BIT(Mhc[i / 64], i % 64);

      uint64_t Xv = Eq | Mv;
      uint64_t Xh = ((((Eq | Mb) & Pv) + Pv) ^ Pv) | Eq | Mb;

      uint64_t Ph = Mv | ~ (Xh | Pv);
      uint64_t Mh = Pv & Xh;

      if (Ph & Last) Score++;
      if (Mh & Last) Score--;

      if ((Ph >> 63) ^ Pb)
        Phc[i / 64] = FLIP(Phc[i / 64], i % 64);

      if ((Mh >> 63) ^ Mb)
        Mhc[i / 64] = FLIP(Mhc[i / 64], i % 64);

      Ph = (Ph << 1) | Pb;
      Mh = (Mh << 1) | Mb;

      Pv = Mh | ~ (Xv | Ph);
      Mv = Ph & Xv;
    }
  }

  return Score;
}

template <typename CharT1, typename CharT2>
std::size_t levenshtein_wagner_fischer(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2, std::size_t max)
{
  if (max > s1.size()) {
    max = s1.size();
  }

  const std::size_t len_diff = s1.size() - s2.size();
  std::vector<std::size_t> cache(s1.size());
  std::iota(cache.begin(), cache.begin() + max, 1);
  std::fill(cache.begin() + max, cache.end(), max + 1);

  const std::size_t offset = max - len_diff;
  const bool haveMax = max < s1.size();

  std::size_t jStart = 0;
  std::size_t jEnd = max;

  std::size_t current = 0;
  std::size_t left;
  std::size_t above;
  std::size_t s2_pos = 0;

  for (const auto& char2 : s2) {
    left = s2_pos;
    above = s2_pos + 1;

    jStart += (s2_pos > offset) ? 1 : 0;
    jEnd += (jEnd < s1.size()) ? 1 : 0;

    for (std::size_t j = jStart; j < jEnd; j++) {
      above = current;
      current = left;
      left = cache[j];

      if (char2 != s1[j]) {

        // Insertion
        if (left < current) current = left;

        // Deletion
        if (above < current) current = above;

        ++current;
      }

      cache[j] = current;
    }

    if (haveMax && cache[s2_pos + len_diff] > max) {
      return std::numeric_limits<std::size_t>::max();
    }
    ++s2_pos;
  }

  return (cache.back() <= max) ? cache.back() : std::numeric_limits<std::size_t>::max();
}

template <typename CharT1, typename CharT2>
std::size_t levenshtein(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2, std::size_t max)
{
  /* Swapping the strings so the first string is shorter.
   * Swapping has no effect on the score since Insertion and Deletion have the
   * the same weight */
  if (s1.size() > s2.size()) {
    return levenshtein(s2, s1, max);
  }

  // when no differences are allowed a direct comparision is sufficient
  if (max == 0) {
    if (s1.size() != s2.size()) {
      return -1;
    }
    return std::equal(s1.begin(), s1.end(), s2.begin()) ? 0 : -1;
  }

  // at least length difference insertions/deletions required
  if (s2.size() - s1.size() > max) {
    return -1;
  }

  /* The Levenshtein distance between
   * <prefix><string1><suffix> and <prefix><string2><suffix>
   * is similar to the distance between <string1> and <string2>,
   * so they can be removed in linear time */
  common::remove_common_affix(s1, s2);

  if (s1.empty()) {
    return s2.size();
  }

  if (max < 4) {
    return levenshtein_mbleven2018(s1, s2, max);
  }

  /* when the short strings has less then 65 elements Hyyrös' algorithm can be used */
  if (s2.size() < 65) {
    std::size_t dist = levenshtein_hyrroe2003(s1, s2);
    return (dist > max) ? -1 : dist;
  }

  /* replace this with myers algorithm in the future */
  std::size_t dist = levenshtein_myers1999_block(s1, s2);
  return (dist > max) ? -1 : dist;
}


template <typename CharT1, typename CharT2>
double normalized_levenshtein(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2, const double score_cutoff)
{
  if (s1.empty() || s2.empty()) {
    return 100.0 * static_cast<double>(s1.empty() && s2.empty());
  }

  /* calculate the maximum possible edit distance with
   * Insertion/Deletion/Substitution = 1 */
  std::size_t max_dist = std::max(s1.size(), s2.size());

  auto cutoff_distance = common::score_cutoff_to_distance(score_cutoff, max_dist);

  std::size_t dist = levenshtein(s1, s2, cutoff_distance);
  return (dist != (std::size_t)-1)
    ? common::norm_distance(dist, max_dist, score_cutoff)
    : 0.0;
}

} // namespace detail
} // namespace levenshtein
} // namespace rapidfuzz
