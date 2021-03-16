/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include <rapidfuzz/details/common.hpp>

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

/*
 * count the number of bits set in a 64 bit integer
 * The code uses wikipedia's 64-bit popcount implementation:
 * http://en.wikipedia.org/wiki/Hamming_weight#Efficient_implementation
 */
static inline int popcount64(uint64_t x)
{
  const uint64_t m1  = 0x5555555555555555; //binary: 0101...
  const uint64_t m2  = 0x3333333333333333; //binary: 00110011..
  const uint64_t m4  = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
  const uint64_t h01 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...

  x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
  x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
  x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
  return (x * h01) >> 56;  //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
}

/*
 * returns a 64 bit integer with the first n bits set to 1
 */
static inline uint64_t set_bits(int n)
{
  uint64_t result = (uint64_t)-1;
  // shifting by 64 bits would be undefined behavior
  if (n < 64) {
    result += (uint64_t)1 << n;
  }
  return result;
}

template <typename CharT1, std::size_t size>
static inline std::size_t weighted_levenshtein_bitpal(basic_string_view<CharT1> s1,
  const common::PatternMatchVector<size>& block, std::size_t s2_len)
{
  uint64_t DHneg1 = ~0x0ull;
  uint64_t DHzero = 0;
  uint64_t DHpos1 = 0;

  for (auto ch2 : s1)
  {
    const uint64_t Matches = block.get(ch2);
    const uint64_t NotMatches = ~Matches;

    const uint64_t INITpos1s = DHneg1 & Matches;
    const uint64_t DVpos1shift = (((INITpos1s + DHneg1) ^ DHneg1) ^ INITpos1s);

    const uint64_t RemainDHneg1 = DHneg1 ^ (DVpos1shift >> 1);
    const uint64_t DVpos1shiftorMatch = DVpos1shift | Matches;

    const uint64_t INITzeros = (DHzero & DVpos1shiftorMatch) ;
    const uint64_t DVzeroshift = ((INITzeros << 1) + RemainDHneg1) ^ RemainDHneg1;

    const uint64_t DVneg1shift = ~(DVpos1shift | DVzeroshift);
    DHzero &= NotMatches;
    const uint64_t DHpos1orMatch = DHpos1| Matches;
    DHzero = (DVzeroshift & DHpos1orMatch) | (DVneg1shift & DHzero);
    DHpos1 = (DVneg1shift & DHpos1orMatch);
    DHneg1 = ~(DHzero | DHpos1);
  }

  std::size_t dist = s1.size() + s2_len;
  uint64_t bitmask = set_bits(s2_len);

  dist -= popcount64(DHzero & bitmask);
  dist -= popcount64(DHpos1 & bitmask) * 2;

  return dist;
}


template <typename T, typename U>
constexpr T bit_clear(T val, U bit)
{
  return val & ~(1ull << bit);
}

template <typename T, typename U>
constexpr T bit_check(T val, U bit)
{
  return (val >> bit) & 0x1;
}


template <typename CharT1, std::size_t size>
std::size_t weighted_levenshtein_bitpal_blockwise(basic_string_view<CharT1> s1,
  const common::BlockPatternMatchVector<size>& block, std::size_t s2_len)
{
  struct HorizontalDelta {
    uint64_t DHpos1;
    uint64_t DHzero;
    uint64_t DHneg1;

    HorizontalDelta()
      : DHpos1(0), DHzero(0), DHneg1(~0x0ull) {}
  };

  std::size_t words = block.m_val.size();
  std::vector<HorizontalDelta> DH(words);

  //recursion
  for (const auto& ch1 : s1)
  {
    //initialize OverFlow
    uint64_t OverFlow0 = 0;
    uint64_t OverFlow1 = 0;
    uint64_t INITzerosprevbit = 0;

    // manually unroll the loop iteration for the first word
    // since there can not be a overflow before the first iteration
    {
      uint64_t DHpos1temp    = DH[0].DHpos1;
      uint64_t DHzerotemp    = DH[0].DHzero;
      uint64_t DHneg1temp    = DH[0].DHneg1;

      const uint64_t Matches = block.get(0, ch1);

      //Complement Matches
      const uint64_t NotMatches = ~Matches;
      //Finding the vertical values
      //Find 1s
      const uint64_t INITpos1s = DHneg1temp & Matches;

      uint64_t sum = INITpos1s;
      sum += DHneg1temp;
      OverFlow0 = sum < DHneg1temp;
      const uint64_t DVpos1shift = (sum ^ DHneg1temp) ^ INITpos1s;

      //set RemainingDHneg1
      const uint64_t RemainDHneg1 = DHneg1temp ^ INITpos1s;
      //combine 1s and Matches
      const uint64_t DVpos1shiftorMatch = DVpos1shift | Matches;

      //Find 0s
      const uint64_t INITzeros = (DHzerotemp & DVpos1shiftorMatch);
      uint64_t initval = (INITzeros << 1);
      INITzerosprevbit = INITzeros >> 63;

      sum = initval;
      sum += RemainDHneg1;
      OverFlow0 |= sum < RemainDHneg1;
      const uint64_t DVzeroshift = initval ^ RemainDHneg1;

      //Find -1s
      const uint64_t DVneg1shift = ~(DVpos1shift | DVzeroshift);

      //Finding the horizontal values
      //Remove matches from DH values except 1
      DHzerotemp &= NotMatches;
      //combine 1s and Matches
      const uint64_t DHpos1orMatch = DHpos1temp | Matches;
      //Find 0s
      DHzerotemp = (DVzeroshift & DHpos1orMatch) | (DVneg1shift & DHzerotemp);
      //Find 1s
      DHpos1temp = DVneg1shift & DHpos1orMatch;
      //Find -1s
      DHneg1temp = ~(DHzerotemp | DHpos1temp);

      DH[0].DHpos1 = DHpos1temp;
      DH[0].DHzero = DHzerotemp;
      DH[0].DHneg1 = DHneg1temp;
    }

    for (std::size_t word = 1; word < words - 1; ++word) {
      uint64_t DHpos1temp    = DH[word].DHpos1;
      uint64_t DHzerotemp    = DH[word].DHzero;
      uint64_t DHneg1temp    = DH[word].DHneg1;

      const uint64_t Matches = block.get(word, ch1);

      //Complement Matches
      const uint64_t NotMatches = ~Matches;
      //Finding the vertical values
      //Find 1s
      const uint64_t INITpos1s = DHneg1temp & Matches;


      uint64_t sum = INITpos1s;
      sum += OverFlow0;
      OverFlow0 = sum < OverFlow0;
      sum += DHneg1temp;
      OverFlow0 |= sum < DHneg1temp;
      const uint64_t DVpos1shift = (sum ^ DHneg1temp) ^ INITpos1s;

      //set RemainingDHneg1
      const uint64_t RemainDHneg1 = DHneg1temp ^ INITpos1s;
      //combine 1s and Matches
      const uint64_t DVpos1shiftorMatch = DVpos1shift | Matches;

      //Find 0s
      const uint64_t INITzeros = (DHzerotemp & DVpos1shiftorMatch);
      uint64_t initval = INITzerosprevbit;
      INITzerosprevbit = INITzeros >> 63;
      initval = (INITzeros << 1) | initval;


      sum = initval;
      sum += OverFlow1;
      OverFlow1 = sum < OverFlow1;
      sum += RemainDHneg1;
      OverFlow0 |= sum < RemainDHneg1;
      const uint64_t DVzeroshift = sum ^ RemainDHneg1;

      //Find -1s
      const uint64_t DVneg1shift = ~(DVpos1shift | DVzeroshift);

      //Finding the horizontal values
      //Remove matches from DH values except 1
      DHzerotemp &= NotMatches;
      //combine 1s and Matches
      const uint64_t DHpos1orMatch = DHpos1temp | Matches;
      //Find 0s
      DHzerotemp = (DVzeroshift & DHpos1orMatch) | (DVneg1shift & DHzerotemp);
      //Find 1s
      DHpos1temp = DVneg1shift & DHpos1orMatch;
      //Find -1s
      DHneg1temp = ~(DHzerotemp | DHpos1temp);

      DH[word].DHpos1 = DHpos1temp;
      DH[word].DHzero = DHzerotemp;
      DH[word].DHneg1 = DHneg1temp;
    }

    // manually unroll the loop iteration for the last word
    // since we do not have to calculate any overflows anymore
    if (words > 1) {
      uint64_t DHpos1temp    = DH[words - 1].DHpos1;
      uint64_t DHzerotemp    = DH[words - 1].DHzero;
      uint64_t DHneg1temp    = DH[words - 1].DHneg1;

      const uint64_t Matches = block.get(words - 1, ch1);

      //Complement Matches
      const uint64_t NotMatches = ~Matches;
      //Finding the vertical values
      //Find 1s
      const uint64_t INITpos1s = DHneg1temp & Matches;

      uint64_t sum = (INITpos1s + DHneg1temp) + OverFlow0;
      const uint64_t DVpos1shift = (sum ^ DHneg1temp) ^ INITpos1s;

      //set RemainingDHneg1
      const uint64_t RemainDHneg1 = DHneg1temp ^ INITpos1s;
      //combine 1s and Matches
      const uint64_t DVpos1shiftorMatch = DVpos1shift | Matches;

      //Find 0s
      const uint64_t INITzeros = (DHzerotemp & DVpos1shiftorMatch);
      uint64_t initval = (INITzeros << 1) | INITzerosprevbit;

      sum = initval + RemainDHneg1 + OverFlow1;
      const uint64_t DVzeroshift = sum ^ RemainDHneg1;

      //Find -1s
      const uint64_t DVneg1shift = ~(DVpos1shift | DVzeroshift);

      //Finding the horizontal values
      //Remove matches from DH values except 1
      DHzerotemp &= NotMatches;
      //combine 1s and Matches
      const uint64_t DHpos1orMatch = DHpos1temp | Matches;
      //Find 0s
      DHzerotemp = (DVzeroshift & DHpos1orMatch) | (DVneg1shift & DHzerotemp);
      //Find 1s
      DHpos1temp = DVneg1shift & DHpos1orMatch;
      //Find -1s
      DHneg1temp = ~(DHzerotemp | DHpos1temp);

      DH[words - 1].DHpos1 = DHpos1temp;
      DH[words - 1].DHzero = DHzerotemp;
      DH[words - 1].DHneg1 = DHneg1temp;
    }
  }

  //find scores in last row
  std::size_t dist = s1.size() + s2_len;

  for (std::size_t word = 0; word < words-1; ++word)
  {
    dist -= popcount64(DH[word].DHzero);
    dist -= popcount64(DH[word].DHpos1) * 2;
  }

  uint64_t bitmask = set_bits(s2_len - (words - 1) * 64);
  dist -= popcount64(DH.back().DHzero & bitmask);
  dist -= popcount64(DH.back().DHpos1 & bitmask) * 2;

  return dist;
}

template <typename CharT1, typename CharT2>
std::size_t weighted_levenshtein_bitpal(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2)
{
  if (s2.size() < 65) {
    return weighted_levenshtein_bitpal(s1, common::PatternMatchVector<sizeof(CharT2)>(s2), s2.size());
  } else {
    return weighted_levenshtein_bitpal_blockwise(
      s1, common::BlockPatternMatchVector<sizeof(CharT2)>(s2), s2.size());
  }
}

//TODO this implementation needs some cleanup
template <typename CharT1, typename CharT2, std::size_t size>
std::size_t weighted_levenshtein(basic_string_view<CharT1> s1,
  const common::BlockPatternMatchVector<size>& block, basic_string_view<CharT2> s2,
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

  // do this first, since we can not remove any affix in encoded form
  if (max >= 5) {
    std::size_t dist = 0;
    if (s2.size() < 65) {
      dist = weighted_levenshtein_bitpal(s1, block.m_val[0], s2.size());
    } else {
      dist = weighted_levenshtein_bitpal_blockwise(s1, block, s2.size());
    }

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

  std::size_t dist = weighted_levenshtein_bitpal(s1, s2);
  return (dist > max) ? -1 : dist;
}

template <typename CharT1, typename CharT2, std::size_t size>
double normalized_weighted_levenshtein(basic_string_view<CharT1> s1,
  const common::BlockPatternMatchVector<size>& block, basic_string_view<CharT2> s2,
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
