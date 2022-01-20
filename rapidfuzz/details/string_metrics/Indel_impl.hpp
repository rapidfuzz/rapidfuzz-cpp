/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once

#include <rapidfuzz/details/common.hpp>
#include <rapidfuzz/details/intrinsics.hpp>

#include <algorithm>
#include <array>
#include <limits>
#include <stdexcept>
#include <string>

namespace rapidfuzz {
namespace Indel {
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
 *   0x1 = 01 = DELETE,
 *   0x2 = 10 = INSERT
 *
 * 0x5 -> DEL + DEL
 * 0x6 -> DEL + INS
 * 0x9 -> INS + DEL
 * 0xA -> INS + INS
 */
static constexpr uint8_t indel_mbleven2018_matrix[14][7] = {
    /* max edit distance 1 */
    {0},
    /* case does not occur */ /* len_diff 0 */
    {0x01},                   /* len_diff 1 */
    /* max edit distance 2 */
    {0x09, 0x06}, /* len_diff 0 */
    {0x01},       /* len_diff 1 */
    {0x05},       /* len_diff 2 */
    /* max edit distance 3 */
    {0x09, 0x06},       /* len_diff 0 */
    {0x25, 0x19, 0x16}, /* len_diff 1 */
    {0x05},             /* len_diff 2 */
    {0x15},             /* len_diff 3 */
    /* max edit distance 4 */
    {0x96, 0x66, 0x5A, 0x99, 0x69, 0xA5}, /* len_diff 0 */
    {0x25, 0x19, 0x16},                   /* len_diff 1 */
    {0x65, 0x56, 0x95, 0x59},             /* len_diff 2 */
    {0x15},                               /* len_diff 3 */
    {0x55},                               /* len_diff 4 */
};

template <typename InputIt1, typename InputIt2>
int64_t indel_mbleven2018(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                          int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t len_diff = std::abs(len1 - len2);
    auto possible_ops = indel_mbleven2018_matrix[(max + max * max) / 2 + len_diff - 1];
    int64_t dist = max + 1;

    for (int pos = 0; possible_ops[pos] != 0; ++pos) {
        int ops = possible_ops[pos];
        int64_t s1_pos = 0;
        int64_t s2_pos = 0;
        int64_t cur_dist = 0;

        while (s1_pos < len1 && s2_pos < len2) {
            if (first1[s1_pos] != first2[s2_pos]) {
                cur_dist++;

                if (!ops) break;
                if (ops & 1) s1_pos++;
                if (ops & 2) s2_pos++;
                ops >>= 2;
            }
            else {
                s1_pos++;
                s2_pos++;
            }
        }

        cur_dist += (len1 - s1_pos) + (len2 - s2_pos);
        dist = std::min(dist, cur_dist);
    }

    return std::min(dist, max + 1);
}

template <int64_t N, typename PMV, typename InputIt1, typename InputIt2>
static inline int64_t longest_common_subsequence_unroll(const PMV& block, InputIt1 first1,
                                                        InputIt1 last1, InputIt2 first2,
                                                        InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    uint64_t S[N];
    for (int64_t i = 0; i < N; ++i) {
        S[i] = ~0x0ull;
    }

    for (; first2 != last2; ++last2) {
        uint64_t carry = 0;
        uint64_t Matches[N];
        uint64_t u[N];
        uint64_t x[N];
        for (int64_t i = 0; i < N; ++i) {
            Matches[i] = block.get(i, *first2);
            u[i] = S[i] & Matches[i];
            x[i] = intrinsics::addc64(S[i], u[i], carry, &carry);
            S[i] = x[i] | (S[i] - u[i]);
        }
    }

    int64_t res = 0;
    for (int64_t i = 0; i < N; ++i) {
        res += intrinsics::popcount64(~S[i]);
    }

    int64_t dist = len1 + len2 - 2 * res;
    return std::min(dist, max + 1);
}

template <typename InputIt1, typename InputIt2>
static inline int64_t
longest_common_subsequence_blockwise(const common::BlockPatternMatchVector& block, InputIt1 first1,
                                     InputIt1 last1, InputIt2 first2, InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    int64_t words = block.m_val.size();
    std::vector<uint64_t> S(words, ~0x0ull);

    for (; first2 != last2; ++last2) {
        uint64_t carry = 0;
        for (int64_t word = 0; word < words; ++word) {
            const uint64_t Matches = block.get(word, *first2);
            uint64_t Stemp = S[word];

            uint64_t u = Stemp & Matches;

            uint64_t x = intrinsics::addc64(Stemp, u, carry, &carry);
            S[word] = x | (Stemp - u);
        }
    }

    int64_t res = 0;
    for (uint64_t Stemp : S) {
        res += intrinsics::popcount64(~Stemp);
    }

    int64_t dist = len1 + len2 - 2 * res;
    return std::min(dist, max + 1);
}

template <typename InputIt1, typename InputIt2>
int64_t longest_common_subsequence(const common::BlockPatternMatchVector& block, InputIt1 first1,
                                   InputIt1 last1, InputIt2 first2, InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t nr = (len1 / 64) + bool(len1 % 64);
    switch (nr) {
    case 0:
        return std::min(len2, max + 1);
    case 1:
        return longest_common_subsequence_unroll<1>(block, first1, last1, first2, last2, max);
    case 2:
        return longest_common_subsequence_unroll<2>(block, first1, last1, first2, last2, max);
    case 3:
        return longest_common_subsequence_unroll<3>(block, first1, last1, first2, last2, max);
    case 4:
        return longest_common_subsequence_unroll<4>(block, first1, last1, first2, last2, max);
    case 5:
        return longest_common_subsequence_unroll<5>(block, first1, last1, first2, last2, max);
    case 6:
        return longest_common_subsequence_unroll<6>(block, first1, last1, first2, last2, max);
    case 7:
        return longest_common_subsequence_unroll<7>(block, first1, last1, first2, last2, max);
    case 8:
        return longest_common_subsequence_unroll<8>(block, first1, last1, first2, last2, max);
    default:
        return longest_common_subsequence_blockwise(block, first1, last1, first2, last2, max);
    }
}

template <typename InputIt1, typename InputIt2>
int64_t longest_common_subsequence(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t nr = (len1 / 64) + bool(len1 % 64);
    switch (nr) {
    case 1:
    {
        auto block = common::PatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<1>(block, first1, last1, first2, last2, max);
    }
    case 2:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<2>(block, first1, last1, first2, last2, max);
    }
    case 3:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<3>(block, first1, last1, first2, last2, max);
    }
    case 4:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<4>(block, first1, last1, first2, last2, max);
    }
    case 5:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<5>(block, first1, last1, first2, last2, max);
    }
    case 6:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<6>(block, first1, last1, first2, last2, max);
    }
    case 7:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<7>(block, first1, last1, first2, last2, max);
    }
    case 8:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<8>(block, first1, last1, first2, last2, max);
    }
    default:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_blockwise(block, first1, last1, first2, last2, max);
    }
    }
}

template <typename InputIt1, typename InputIt2>
int64_t indel_distance(const common::BlockPatternMatchVector& block, InputIt1 first1,
                       InputIt1 last1, InputIt2 first2, InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    /* no edits are allowed */
    if (max == 0 || (max == 1 && len1 == len2)) {
        return common::equal(first1, last1, first2, last2);
    }

    if (max < std::abs(len1 - len2)) {
        return max + 1;
    }

    // do this first, since we can not remove any affix in encoded form
    if (max >= 5) {
        return longest_common_subsequence(block, first1, last1, first2, last2, max);
    }

    /* common affix does not effect Levenshtein distance */
    common::remove_common_affix(first1, last1, first2, last2);
    len1 = std::distance(first1, last1);
    len2 = std::distance(first2, last2);
    if (!len1 || !len2) {
        return len1 + len2;
    }

    return indel_mbleven2018(first1, last1, first2, last2, max);
}

template <typename InputIt1, typename InputIt2>
int64_t indel_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                       int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    // Swapping the strings so the second string is shorter
    if (len1 < len2) {
        return indel_distance(first2, last2, first1, last1, max);
    }

    /* no edits are allowed */
    if (max == 0 || (max == 1 && len1 == len2)) {
        return common::equal(first1, last1, first2, last2);
    }

    if (max < std::abs(len1 - len2)) {
        return max + 1;
    }

    /* common affix does not effect Levenshtein distance */
    common::remove_common_affix(first1, last1, first2, last2);
    len1 = std::distance(first1, last1);
    len2 = std::distance(first2, last2);
    if (!len1 || !len2) {
        return len1 + len2;
    }

    if (max < 5) {
        return indel_mbleven2018(first1, last1, first2, last2, max);
    }

    return longest_common_subsequence(first1, last1, first2, last2, max);
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_distance(const common::BlockPatternMatchVector& block, InputIt1 first1,
                                 InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                 double score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t maximum = len1 + len2;
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(maximum * score_cutoff));
    int64_t dist = indel_distance(block, first1, last1, first2, last2, cutoff_distance);
    double norm_dist = (maximum) ? dist / maximum : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : double(maximum);
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                 double score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t maximum = len1 + len2;
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(maximum * score_cutoff));
    int64_t dist = indel_distance(first1, last1, first2, last2, cutoff_distance);
    double norm_dist = (maximum) ? dist / maximum : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : double(maximum);
}

template <typename InputIt1, typename InputIt2>
int64_t indel_similarity(const common::BlockPatternMatchVector& block, InputIt1 first1,
                         InputIt1 last1, InputIt2 first2, InputIt2 last2, int64_t score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t maximum = len1 + len2;
    int64_t cutoff_distance = maximum - score_cutoff;
    int64_t dist = indel_distance(block, first1, last1, first2, last2, cutoff_distance);
    int64_t sim = maximum - dist;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename InputIt1, typename InputIt2>
int64_t indel_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         int64_t score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t maximum = len1 + len2;
    int64_t cutoff_distance = maximum - score_cutoff;
    int64_t dist = indel_distance(first1, last1, first2, last2, cutoff_distance);
    int64_t sim = maximum - dist;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_similarity(const common::BlockPatternMatchVector& block, InputIt1 first1,
                                   InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff)
{
    double norm_dist =
        indel_normalized_distance(block, first1, last1, first2, last2, 1.0 - score_cutoff);
    double norm_sim = 1.0 - norm_dist;
    return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff)
{
    double norm_dist = indel_normalized_distance(first1, last1, first2, last2, 1.0 - score_cutoff);
    double norm_sim = 1.0 - norm_dist;
    return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
}

} // namespace detail
} // namespace Indel
} // namespace rapidfuzz
