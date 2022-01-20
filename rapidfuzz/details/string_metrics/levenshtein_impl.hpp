/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once

#include <algorithm>
#include <array>
#include <limits>
#include <numeric>
#include <rapidfuzz/details/common.hpp>
#include <rapidfuzz/details/intrinsics.hpp>

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
    {0x03}, /* len_diff 0 */
    {0x01}, /* len_diff 1 */
    /* max edit distance 2 */
    {0x0F, 0x09, 0x06}, /* len_diff 0 */
    {0x0D, 0x07},       /* len_diff 1 */
    {0x05},             /* len_diff 2 */
    /* max edit distance 3 */
    {0x3F, 0x27, 0x2D, 0x39, 0x36, 0x1E, 0x1B}, /* len_diff 0 */
    {0x3D, 0x37, 0x1F, 0x25, 0x19, 0x16},       /* len_diff 1 */
    {0x35, 0x1D, 0x17},                         /* len_diff 2 */
    {0x15},                                     /* len_diff 3 */
};

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_mbleven2018(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    int64_t len_diff = std::abs(len1 - len2);
    auto possible_ops = levenshtein_mbleven2018_matrix[(max + max * max) / 2 + len_diff - 1];
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
template <typename InputIt1, typename InputIt2>
int64_t levenshtein_hyrroe2003(const common::PatternMatchVector& PM, InputIt1 first1,
                               InputIt1 last1, InputIt2 first2, InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    /* VP is set to 1^m. Shifting by bitwidth would be undefined behavior */
    uint64_t VP = (uint64_t)-1;
    uint64_t VN = 0;
    int64_t currDist = len1;

    /* mask used when computing D[m,j] in the paper 10^(m-1) */
    uint64_t mask = (uint64_t)1 << (len1 - 1);

    /* Searching */
    for (; first2 != last2; ++first2) {
        /* Step 1: Computing D0 */
        uint64_t PM_j = PM.get(*first2);
        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        currDist += bool(HP & mask);
        currDist -= bool(HN & mask);

        /* Step 4: Computing Vp and VN */
        HP = (HP << 1) | 1;
        HN = (HN << 1);

        VP = HN | ~(D0 | HP);
        VN = HP & D0;
    }

    return std::min(currDist, max + 1);
}

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_hyrroe2003_small_band(const common::BlockPatternMatchVector& PM,
                                          InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                          InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    /* VP is set to 1^m. Shifting by bitwidth would be undefined behavior */
    uint64_t VP = (uint64_t)-1;
    uint64_t VN = 0;

    int64_t currDist = len1;

    /* mask used when computing D[m,j] in the paper 10^(m-1) */
    uint64_t mask = 1ull << 63;

    const int64_t words = PM.m_val.size();

    /* Searching */
    for (int64_t i = 0; i < len2; ++i) {
        /* Step 1: Computing D0 */
        int64_t word = i / 64;
        int64_t word_pos = i % 64;

        uint64_t PM_j = PM.get(word, first2[i]) >> word_pos;

        if (word + 1 < words && word_pos != 0) {
            PM_j |= PM.get(word + 1, first2[i]) << (64 - word_pos);
        }

        /* Step 1: Computing D0 */
        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        currDist += bool(HP & mask);
        currDist -= bool(HN & mask);

        /* Step 4: Computing Vp and VN */
        VP = HN | ~((D0 >> 1) | HP);
        VN = (D0 >> 1) & HP;
    }

    return std::min(currDist, max + 1);
}

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_myers1999_block(const common::BlockPatternMatchVector& PM, InputIt1 first1,
                                    InputIt1 last1, InputIt2 first2, InputIt2 last2, int64_t max)
{
    struct Vectors {
        uint64_t VP;
        uint64_t VN;

        Vectors() : VP(~0x0ull), VN(0)
        {}
    };

    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t words = PM.m_val.size();
    int64_t currDist = len1;

    /* upper bound */
    max = std::min(max, std::max(len1, len2));

    // todo could safe up to 25% even without max when ignoring irrelevant paths
    int64_t full_band = std::min(len1, 2 * max + 1);

    if (full_band <= 64) {
        return levenshtein_hyrroe2003_small_band(PM, first1, last1, first2, last2, max);
    }

    std::vector<Vectors> vecs(words);
    uint64_t Last = (uint64_t)1 << ((len1 - 1) % 64);

    /* Searching */
    for (int64_t i = 0; i < len2; i++) {
        uint64_t HP_carry = 1;
        uint64_t HN_carry = 0;

        for (size_t word = 0; word < words - 1; word++) {
            /* Step 1: Computing D0 */
            uint64_t PM_j = PM.get(word, first2[i]);
            uint64_t VN = vecs[word].VN;
            uint64_t VP = vecs[word].VP;

            uint64_t X = PM_j | HN_carry;
            uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

            /* Step 2: Computing HP and HN */
            uint64_t HP = VN | ~(D0 | VP);
            uint64_t HN = D0 & VP;

            /* Step 3: Computing the value D[m,j] */
            // only required for last vector

            /* Step 4: Computing Vp and VN */
            uint64_t HP_carry_temp = HP_carry;
            HP_carry = HP >> 63;
            HP = (HP << 1) | HP_carry_temp;

            uint64_t HN_carry_temp = HN_carry;
            HN_carry = HN >> 63;
            HN = (HN << 1) | HN_carry_temp;

            vecs[word].VP = HN | ~(D0 | HP);
            vecs[word].VN = HP & D0;
        }

        {
            /* Step 1: Computing D0 */
            uint64_t PM_j = PM.get(words - 1, first2[i]);
            uint64_t VN = vecs[words - 1].VN;
            uint64_t VP = vecs[words - 1].VP;

            uint64_t X = PM_j | HN_carry;
            uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

            /* Step 2: Computing HP and HN */
            uint64_t HP = VN | ~(D0 | VP);
            uint64_t HN = D0 & VP;

            /* Step 3: Computing the value D[m,j] */
            currDist += bool(HP & Last);
            currDist -= bool(HN & Last);

            /* Step 4: Computing Vp and VN */
            HP = (HP << 1) | HP_carry;
            HN = (HN << 1) | HN_carry;

            vecs[words - 1].VP = HN | ~(D0 | HP);
            vecs[words - 1].VN = HP & D0;
        }
    }

    return std::min(currDist, max + 1);
}

template <typename InputIt1, typename InputIt2>
int64_t uniform_levenshtein_distance(const common::BlockPatternMatchVector& block, InputIt1 first1,
                                     InputIt1 last1, InputIt2 first2, InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    // when no differences are allowed a direct comparision is sufficient
    if (max == 0) {
        return common::equal(first1, last1, first2, last2);
    }

    if (max < std::abs(len1 - len2)) {
        return max + 1;
    }

    // important to catch, since this causes block.m_val to be empty -> raises exception on access
    if (!len2) {
        return std::min(len1, max + 1);
    }

    /* do this first, since we can not remove any affix in encoded form
     * todo actually we could at least remove the common prefix and just shift the band
     */
    if (max >= 4) {
        if (len1 < 65) {
            return levenshtein_hyrroe2003(block.m_val[0], first1, last1, first2, last2, max);
        }
        else {
            return levenshtein_myers1999_block(block, first1, last1, first2, last2, max);
        }
    }

    /* common affix does not effect Levenshtein distance */
    common::remove_common_affix(first1, last1, first2, last2);

    if (!len2) {
        return len1;
    }

    if (!len1) {
        return len2;
    }

    return levenshtein_mbleven2018(first1, last1, first2, last2, max);
}

template <typename InputIt1, typename InputIt2>
int64_t uniform_levenshtein_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                     InputIt2 last2, size_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    /* Swapping the strings so the second string is shorter */
    if (len1 < len2) {
        return uniform_levenshtein_distance(first2, last2, first1, last1, max);
    }

    // when no differences are allowed a direct comparision is sufficient
    if (max == 0) {
        return common::equal(first1, last1, first2, last2);
    }

    // at least length difference insertions/deletions required
    if (max < (len1 - len2)) {
        return max + 1;
    }

    /* common affix does not effect Levenshtein distance */
    common::remove_common_affix(first1, last1, first2, last2);

    if (!len2) {
        return len1;
    }

    if (max < 4) {
        return levenshtein_mbleven2018(first1, last1, first2, last2, max);
    }

    /* when the short strings has less then 65 elements Hyyrös' algorithm can be used */
    if (len1 < 65) {
        return levenshtein_hyrroe2003(common::PatternMatchVector(first1, last1), first1, last1,
                                      first2, last2, max);
    }
    else {
        return levenshtein_myers1999_block(common::BlockPatternMatchVector(first1, last1), first1,
                                           last1, first2, last2, max);
    }
}

template <typename InputIt1, typename InputIt2>
double uniform_levenshtein_normalized_distance(const common::BlockPatternMatchVector& block,
                                               InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                               InputIt2 last2, double score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t maximum = std::max(len1, len2);
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(maximum * score_cutoff));
    int64_t dist =
        uniform_levenshtein_distance(block, first1, last1, first2, last2, cutoff_distance);
    double norm_dist = (maximum) ? dist / maximum : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : double(maximum);
}

template <typename InputIt1, typename InputIt2>
double uniform_levenshtein_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                               InputIt2 last2, double score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t maximum = std::max(len1, len2);
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(maximum * score_cutoff));
    int64_t dist = uniform_levenshtein_distance(first1, last1, first2, last2, cutoff_distance);
    double norm_dist = (maximum) ? dist / maximum : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : double(maximum);
}

template <typename InputIt1, typename InputIt2>
int64_t uniform_levenshtein_similarity(const common::BlockPatternMatchVector& block,
                                       InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                       InputIt2 last2, int64_t score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t maximum = std::max(len1, len2);
    int64_t cutoff_distance = maximum - score_cutoff;
    int64_t dist =
        uniform_levenshtein_distance(block, first1, last1, first2, last2, cutoff_distance);
    int64_t sim = maximum - dist;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename InputIt1, typename InputIt2>
int64_t uniform_levenshtein_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                       InputIt2 last2, int64_t score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t maximum = std::max(len1, len2);
    int64_t cutoff_distance = maximum - score_cutoff;
    int64_t dist = uniform_levenshtein_distance(first1, last1, first2, last2, cutoff_distance);
    int64_t sim = maximum - dist;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename InputIt1, typename InputIt2>
double uniform_levenshtein_normalized_similarity(const common::BlockPatternMatchVector& block,
                                                 InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                                 InputIt2 last2, double score_cutoff)
{
    double norm_dist = uniform_levenshtein_normalized_distance(block, first1, last1, first2, last2,
                                                               1.0 - score_cutoff);
    double norm_sim = 1.0 - norm_dist;
    return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
}

template <typename InputIt1, typename InputIt2>
double uniform_levenshtein_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                                 InputIt2 last2, double score_cutoff)
{
    double norm_dist =
        uniform_levenshtein_normalized_distance(first1, last1, first2, last2, 1.0 - score_cutoff);
    double norm_sim = 1.0 - norm_dist;
    return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
}

} // namespace detail
} // namespace string_metric
} // namespace rapidfuzz
