/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#include "rapidfuzz/details/type_traits.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <rapidfuzz/details/GrowingHashmap.hpp>
#include <rapidfuzz/details/Matrix.hpp>
#include <rapidfuzz/details/PatternMatchVector.hpp>
#include <rapidfuzz/details/common.hpp>
#include <rapidfuzz/details/distance.hpp>
#include <rapidfuzz/details/intrinsics.hpp>
#include <rapidfuzz/distance/Indel.hpp>
#include <stdexcept>

namespace rapidfuzz {
namespace detail {

struct LevenshteinRow {
    uint64_t VP;
    uint64_t VN;

    LevenshteinRow() : VP(~UINT64_C(0)), VN(0)
    {}

    LevenshteinRow(uint64_t VP_, uint64_t VN_) : VP(VP_), VN(VN_)
    {}
};

template <bool RecordMatrix, bool RecordBitRow>
struct LevenshteinResult;

template <>
struct LevenshteinResult<true, false> {
    BitMatrix<uint64_t> VP;
    BitMatrix<uint64_t> VN;

    int64_t dist;
};

template <>
struct LevenshteinResult<false, true> {
    std::vector<LevenshteinRow> vecs;

    int64_t dist;
};

template <>
struct LevenshteinResult<false, false> {
    int64_t dist;
};

template <typename InputIt1, typename InputIt2>
int64_t generalized_levenshtein_wagner_fischer(Range<InputIt1> s1, Range<InputIt2> s2,
                                               LevenshteinWeightTable weights, int64_t max)
{
    size_t cache_size = static_cast<size_t>(s1.size()) + 1;
    std::vector<int64_t> cache(cache_size);
    assume(cache_size != 0);

    cache[0] = 0;
    for (size_t i = 1; i < cache_size; ++i)
        cache[i] = cache[i - 1] + weights.delete_cost;

    for (const auto& ch2 : s2) {
        auto cache_iter = cache.begin();
        int64_t temp = *cache_iter;
        *cache_iter += weights.insert_cost;

        for (const auto& ch1 : s1) {
            if (ch1 != ch2)
                temp = std::min({*cache_iter + weights.delete_cost, *(cache_iter + 1) + weights.insert_cost,
                                 temp + weights.replace_cost});
            ++cache_iter;
            std::swap(*cache_iter, temp);
        }
    }

    int64_t dist = cache.back();
    return (dist <= max) ? dist : max + 1;
}

/**
 * @brief calculates the maximum possible Levenshtein distance based on
 * string lengths and weights
 */
template <typename InputIt1, typename InputIt2>
int64_t levenshtein_maximum(Range<InputIt1> s1, Range<InputIt2> s2, LevenshteinWeightTable weights)
{
    auto len1 = s1.size();
    auto len2 = s2.size();

    int64_t max_dist = len1 * weights.delete_cost + len2 * weights.insert_cost;

    if (len1 >= len2)
        max_dist = std::min(max_dist, len2 * weights.replace_cost + (len1 - len2) * weights.delete_cost);
    else
        max_dist = std::min(max_dist, len1 * weights.replace_cost + (len2 - len1) * weights.insert_cost);

    return max_dist;
}

/**
 * @brief calculates the minimal possible Levenshtein distance based on
 * string lengths and weights
 */
template <typename InputIt1, typename InputIt2>
int64_t levenshtein_min_distance(Range<InputIt1> s1, Range<InputIt2> s2, LevenshteinWeightTable weights)
{
    return std::max((s1.size() - s2.size()) * weights.delete_cost,
                    (s2.size() - s1.size()) * weights.insert_cost);
}

template <typename InputIt1, typename InputIt2>
int64_t generalized_levenshtein_distance(Range<InputIt1> s1, Range<InputIt2> s2,
                                         LevenshteinWeightTable weights, int64_t max)
{
    int64_t min_edits = levenshtein_min_distance(s1, s2, weights);
    if (min_edits > max) return max + 1;

    /* common affix does not effect Levenshtein distance */
    remove_common_affix(s1, s2);

    return generalized_levenshtein_wagner_fischer(s1, s2, weights, max);
}

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
static constexpr std::array<std::array<uint8_t, 8>, 9> levenshtein_mbleven2018_matrix = {{
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
}};

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_mbleven2018(Range<InputIt1> s1, Range<InputIt2> s2, int64_t max)
{
    auto len1 = s1.size();
    auto len2 = s2.size();
    assert(len1 > 0);
    assert(len2 > 0);
    assert(*s1.begin() != *s2.begin());
    assert(*(s1.end() - 1) != *(s2.end() - 1));

    if (len1 < len2) return levenshtein_mbleven2018(s2, s1, max);

    auto len_diff = len1 - len2;

    if (max == 1) return max + static_cast<int64_t>(len_diff == 1 || len1 != 1);

    auto ops_index = (max + max * max) / 2 + len_diff - 1;
    auto& possible_ops = levenshtein_mbleven2018_matrix[static_cast<size_t>(ops_index)];
    int64_t dist = max + 1;

    for (uint8_t ops : possible_ops) {
        ptrdiff_t s1_pos = 0;
        ptrdiff_t s2_pos = 0;
        int64_t cur_dist = 0;
        while (s1_pos < len1 && s2_pos < len2) {
            if (s1[s1_pos] != s2[s2_pos]) {
                cur_dist++;
                if (!ops) break;
                if (ops & 1) s1_pos++;
                if (ops & 2) s2_pos++;
#if defined(__GNUC__) && !defined(__clang__) && !defined(__ICC) && __GNUC__ < 10
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wconversion"
#endif
                ops >>= 2;
#if defined(__GNUC__) && !defined(__clang__) && !defined(__ICC) && __GNUC__ < 10
#    pragma GCC diagnostic pop
#endif
            }
            else {
                s1_pos++;
                s2_pos++;
            }
        }
        cur_dist += (len1 - s1_pos) + (len2 - s2_pos);
        dist = std::min(dist, cur_dist);
    }

    return (dist <= max) ? dist : max + 1;
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
template <bool RecordMatrix, bool RecordBitRow, typename PM_Vec, typename InputIt1, typename InputIt2>
auto levenshtein_hyrroe2003(const PM_Vec& PM, Range<InputIt1> s1, Range<InputIt2> s2,
                            int64_t max = std::numeric_limits<int64_t>::max())
    -> LevenshteinResult<RecordMatrix, RecordBitRow>
{
    /* VP is set to 1^m. Shifting by bitwidth would be undefined behavior */
    uint64_t VP = ~UINT64_C(0);
    uint64_t VN = 0;

    LevenshteinResult<RecordMatrix, RecordBitRow> res;
    res.dist = s1.size();
    static_if<RecordMatrix>([&](auto f) {
        f(res).VP = BitMatrix<uint64_t>(static_cast<size_t>(s2.size()), 1, ~UINT64_C(0));
        f(res).VN = BitMatrix<uint64_t>(static_cast<size_t>(s2.size()), 1, 0);
    });

    /* mask used when computing D[m,j] in the paper 10^(m-1) */
    uint64_t mask = UINT64_C(1) << (s1.size() - 1);

    /* Searching */
    for (ptrdiff_t i = 0; i < s2.size(); ++i) {
        /* Step 1: Computing D0 */
        uint64_t PM_j = PM.get(0, s2[i]);
        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        res.dist += bool(HP & mask);
        res.dist -= bool(HN & mask);

        /* Step 4: Computing Vp and VN */
        HP = (HP << 1) | 1;
        HN = (HN << 1);

        VP = HN | ~(D0 | HP);
        VN = HP & D0;

        static_if<RecordMatrix>([&](auto f) {
            f(res).VP[static_cast<size_t>(i)][0] = VP;
            f(res).VN[static_cast<size_t>(i)][0] = VN;
        });
    }

    if (res.dist > max) res.dist = max + 1;

    static_if<RecordBitRow>([&](auto f) { f(res).vecs.emplace_back(VP, VN); });

    return res;
}

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_hyrroe2003_small_band(const BlockPatternMatchVector& PM, Range<InputIt1> s1,
                                          Range<InputIt2> s2, int64_t max)
{
    /* VP is set to 1^m. Shifting by bitwidth would be undefined behavior */
    uint64_t VP = ~UINT64_C(0) << (64 - max - 1);
    uint64_t VN = 0;

    const auto words = PM.size();
    int64_t currDist = max;
    uint64_t diagonal_mask = UINT64_C(1) << 63;
    uint64_t horizontal_mask = UINT64_C(1) << 62;
    ptrdiff_t start_pos = max + 1 - 64;

    /* score can decrease along the horizontal, but not along the diagonal */
    int64_t break_score = max + s2.size() - (s1.size() - max);

    /* Searching */
    ptrdiff_t i = 0;
    for (; i < s1.size() - max; ++i, ++start_pos) {
        /* Step 1: Computing D0 */
        uint64_t PM_j = 0;
        if (start_pos < 0) {
            PM_j = PM.get(0, s2[i]) << (-start_pos);
        }
        else {
            size_t word = static_cast<size_t>(start_pos) / 64;
            size_t word_pos = static_cast<size_t>(start_pos) % 64;

            PM_j = PM.get(word, s2[i]) >> word_pos;

            if (word + 1 < words && word_pos != 0) PM_j |= PM.get(word + 1, s2[i]) << (64 - word_pos);
        }
        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        currDist += !bool(D0 & diagonal_mask);

        if (currDist > break_score) return max + 1;

        /* Step 4: Computing Vp and VN */
        VP = HN | ~((D0 >> 1) | HP);
        VN = (D0 >> 1) & HP;
    }

    for (; i < s2.size(); ++i, ++start_pos) {
        /* Step 1: Computing D0 */
        uint64_t PM_j = 0;
        if (start_pos < 0) {
            PM_j = PM.get(0, s2[i]) << (-start_pos);
        }
        else {
            size_t word = static_cast<size_t>(start_pos) / 64;
            size_t word_pos = static_cast<size_t>(start_pos) % 64;

            PM_j = PM.get(word, s2[i]) >> word_pos;

            if (word + 1 < words && word_pos != 0) PM_j |= PM.get(word + 1, s2[i]) << (64 - word_pos);
        }
        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        currDist += bool(HP & horizontal_mask);
        currDist -= bool(HN & horizontal_mask);
        horizontal_mask >>= 1;

        if (currDist > break_score) return max + 1;

        /* Step 4: Computing Vp and VN */
        VP = HN | ~((D0 >> 1) | HP);
        VN = (D0 >> 1) & HP;
    }

    return (currDist <= max) ? currDist : max + 1;
}

template <bool RecordMatrix, typename InputIt1, typename InputIt2>
auto levenshtein_hyrroe2003_small_band(Range<InputIt1> s1, Range<InputIt2> s2, int64_t max)
    -> LevenshteinResult<RecordMatrix, false>
{
    assert(max <= s1.size());
    assert(max <= s2.size());

    /* VP is set to 1^m. Shifting by bitwidth would be undefined behavior */
    uint64_t VP = ~UINT64_C(0) << (64 - max - 1);
    uint64_t VN = 0;

    LevenshteinResult<RecordMatrix, false> res;
    res.dist = max;
    static_if<RecordMatrix>([&](auto f) {
        f(res).VP = BitMatrix<uint64_t>(static_cast<size_t>(s2.size()), 1, 0, max + 2 - 64, 1);
        f(res).VN = BitMatrix<uint64_t>(static_cast<size_t>(s2.size()), 1, 0, max + 2 - 64, 1);
    });

    uint64_t diagonal_mask = UINT64_C(1) << 63;
    uint64_t horizontal_mask = UINT64_C(1) << 62;

    /* score can decrease along the horizontal, but not along the diagonal */
    int64_t break_score = max + s2.size() - (s1.size() - max);
    HybridGrowingHashmap<typename Range<InputIt1>::value_type, std::pair<ptrdiff_t, uint64_t>> PM;

    for (ptrdiff_t j = -max; j < 0; ++j) {
        auto& x = PM[s1[j + max]];
        x.second = shr64(x.second, j - x.first) | (UINT64_C(1) << 63);
        x.first = j;
    }

    /* Searching */
    ptrdiff_t i = 0;
    for (; i < s1.size() - max; ++i) {
        /* Step 1: Computing D0 */
        /* update bitmasks online */
        uint64_t PM_j = 0;
        if (i + max < s1.size()) {
            auto& x = PM[s1[i + max]];
            x.second = shr64(x.second, i - x.first) | (UINT64_C(1) << 63);
            x.first = i;
        }
        {
            auto x = PM.get(s2[i]);
            PM_j = shr64(x.second, i - x.first);
        }

        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        res.dist += !bool(D0 & diagonal_mask);

        if (res.dist > break_score) {
            res.dist = max + 1;
            return res;
        }

        /* Step 4: Computing Vp and VN */
        VP = HN | ~((D0 >> 1) | HP);
        VN = (D0 >> 1) & HP;

        static_if<RecordMatrix>([&](auto f) {
            f(res).VP[static_cast<size_t>(i)][0] = VP;
            f(res).VN[static_cast<size_t>(i)][0] = VN;
        });
    }

    for (; i < s2.size(); ++i) {
        /* Step 1: Computing D0 */
        /* update bitmasks online */
        uint64_t PM_j = 0;
        if (i + max < s1.size()) {
            auto& x = PM[s1[i + max]];
            x.second = shr64(x.second, i - x.first) | (UINT64_C(1) << 63);
            x.first = i;
        }
        {
            auto x = PM.get(s2[i]);
            PM_j = shr64(x.second, i - x.first);
        }

        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        res.dist += bool(HP & horizontal_mask);
        res.dist -= bool(HN & horizontal_mask);
        horizontal_mask >>= 1;

        if (res.dist > break_score) {
            res.dist = max + 1;
            return res;
        }

        /* Step 4: Computing Vp and VN */
        VP = HN | ~((D0 >> 1) | HP);
        VN = (D0 >> 1) & HP;

        static_if<RecordMatrix>([&](auto f) {
            f(res).VP[static_cast<size_t>(i)][0] = VP;
            f(res).VN[static_cast<size_t>(i)][0] = VN;
        });
    }

    if (res.dist > max) res.dist = max + 1;

    return res;
}

template <bool RecordMatrix, bool RecordBitRow, typename InputIt1, typename InputIt2>
auto levenshtein_hyrroe2003_block(const BlockPatternMatchVector& PM, Range<InputIt1> s1, Range<InputIt2> s2,
                                  int64_t max = std::numeric_limits<int64_t>::max())
    -> LevenshteinResult<RecordMatrix, RecordBitRow>
{
    auto words = PM.size();
    std::vector<LevenshteinRow> vecs(words);
    uint64_t Last = UINT64_C(1) << ((s1.size() - 1) % 64);

    LevenshteinResult<RecordMatrix, RecordBitRow> res;
    res.dist = s1.size();
    static_if<RecordMatrix>([&](auto f) {
        f(res).VP = BitMatrix<uint64_t>(static_cast<size_t>(s2.size()), words, ~UINT64_C(0));
        f(res).VN = BitMatrix<uint64_t>(static_cast<size_t>(s2.size()), words, 0);
    });

    /* Searching */
    for (ptrdiff_t i = 0; i < s2.size(); ++i) {
        uint64_t HP_carry = 1;
        uint64_t HN_carry = 0;

        for (size_t word = 0; word < words - 1; word++) {
            /* Step 1: Computing D0 */
            uint64_t PM_j = PM.get(word, s2[i]);
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

            static_if<RecordMatrix>([&](auto f) {
                f(res).VP[static_cast<size_t>(i)][word] = vecs[word].VP;
                f(res).VN[static_cast<size_t>(i)][word] = vecs[word].VN;
            });
        }

        {
            /* Step 1: Computing D0 */
            uint64_t PM_j = PM.get(words - 1, s2[i]);
            uint64_t VN = vecs[words - 1].VN;
            uint64_t VP = vecs[words - 1].VP;

            uint64_t X = PM_j | HN_carry;
            uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

            /* Step 2: Computing HP and HN */
            uint64_t HP = VN | ~(D0 | VP);
            uint64_t HN = D0 & VP;

            /* Step 3: Computing the value D[m,j] */
            res.dist += bool(HP & Last);
            res.dist -= bool(HN & Last);

            /* Step 4: Computing Vp and VN */
            HP = (HP << 1) | HP_carry;
            HN = (HN << 1) | HN_carry;

            vecs[words - 1].VP = HN | ~(D0 | HP);
            vecs[words - 1].VN = HP & D0;

            static_if<RecordMatrix>([&](auto f) {
                f(res).VP[static_cast<size_t>(i)][words - 1] = vecs[words - 1].VP;
                f(res).VN[static_cast<size_t>(i)][words - 1] = vecs[words - 1].VN;
            });
        }
    }

    if (res.dist > max) res.dist = max + 1;

    static_if<RecordBitRow>([&](auto f) { f(res).vecs = std::move(vecs); });

    return res;
}

template <typename InputIt1, typename InputIt2>
int64_t uniform_levenshtein_distance(const BlockPatternMatchVector& block, Range<InputIt1> s1,
                                     Range<InputIt2> s2, int64_t max)
{
    /* upper bound */
    max = std::min(max, std::max<int64_t>(s1.size(), s2.size()));

    // when no differences are allowed a direct comparision is sufficient
    if (max == 0) return !std::equal(s1.begin(), s1.end(), s2.begin(), s2.end());

    if (max < std::abs(s1.size() - s2.size())) return max + 1;

    // important to catch, since this causes block to be empty -> raises exception on access
    if (s1.empty()) {
        return (s2.size() <= max) ? s2.size() : max + 1;
    }

    /* do this first, since we can not remove any affix in encoded form
     * todo actually we could at least remove the common prefix and just shift the band
     */
    if (max >= 4) {
        // todo could safe up to 25% even without max when ignoring irrelevant paths
        // in the upper and lower corner
        int64_t full_band = std::min<int64_t>(s1.size(), 2 * max + 1);

        if (s1.size() < 65)
            return levenshtein_hyrroe2003<false, false>(block, s1, s2, max).dist;
        else if (full_band <= 64)
            return levenshtein_hyrroe2003_small_band(block, s1, s2, max);
        else
            return levenshtein_hyrroe2003_block<false, false>(block, s1, s2, max).dist;
    }

    /* common affix does not effect Levenshtein distance */
    remove_common_affix(s1, s2);
    if (s1.empty() || s2.empty()) return s1.size() + s2.size();

    return levenshtein_mbleven2018(s1, s2, max);
}

template <typename InputIt1, typename InputIt2>
int64_t uniform_levenshtein_distance(Range<InputIt1> s1, Range<InputIt2> s2, int64_t max)
{
    /* Swapping the strings so the second string is shorter */
    if (s1.size() < s2.size()) return uniform_levenshtein_distance(s2, s1, max);

    /* upper bound */
    max = std::min(max, std::max<int64_t>(s1.size(), s2.size()));

    // when no differences are allowed a direct comparision is sufficient
    if (max == 0) return !std::equal(s1.begin(), s1.end(), s2.begin(), s2.end());

    // at least length difference insertions/deletions required
    if (max < (s1.size() - s2.size())) return max + 1;

    /* common affix does not effect Levenshtein distance */
    remove_common_affix(s1, s2);
    if (s1.empty() || s2.empty()) return s1.size() + s2.size();

    if (max < 4) return levenshtein_mbleven2018(s1, s2, max);

    // todo could safe up to 25% even without max when ignoring irrelevant paths
    // in the upper and lower corner
    int64_t full_band = std::min<int64_t>(s1.size(), 2 * max + 1);

    /* when the short strings has less then 65 elements Hyyrös' algorithm can be used */
    if (s2.size() < 65)
        return levenshtein_hyrroe2003<false, false>(PatternMatchVector(s2), s2, s1, max).dist;
    else if (full_band <= 64)
        return levenshtein_hyrroe2003_small_band<false>(s1, s2, max).dist;
    else
        return levenshtein_hyrroe2003_block<false, false>(BlockPatternMatchVector(s1), s1, s2, max).dist;
}

/**
 * @brief recover alignment from bitparallel Levenshtein matrix
 */
template <typename InputIt1, typename InputIt2>
void recover_alignment(Editops& editops, Range<InputIt1> s1, Range<InputIt2> s2,
                       const LevenshteinResult<true, false>& matrix, size_t src_pos, size_t dest_pos,
                       size_t editop_pos)
{
    size_t dist = static_cast<size_t>(matrix.dist);
    size_t col = static_cast<size_t>(s1.size());
    size_t row = static_cast<size_t>(s2.size());

    while (row && col) {
        /* Deletion */
        if (matrix.VP.test_bit(row - 1, col - 1)) {
            assert(dist > 0);
            dist--;
            col--;
            editops[editop_pos + dist].type = EditType::Delete;
            editops[editop_pos + dist].src_pos = col + src_pos;
            editops[editop_pos + dist].dest_pos = row + dest_pos;
        }
        else {
            row--;

            /* Insertion */
            if (row && matrix.VN.test_bit(row - 1, col - 1)) {
                assert(dist > 0);
                dist--;
                editops[editop_pos + dist].type = EditType::Insert;
                editops[editop_pos + dist].src_pos = col + src_pos;
                editops[editop_pos + dist].dest_pos = row + dest_pos;
            }
            /* Match/Mismatch */
            else {
                col--;

                /* Replace (Matches are not recorded) */
                if (s1[static_cast<ptrdiff_t>(col)] != s2[static_cast<ptrdiff_t>(row)]) {
                    assert(dist > 0);
                    dist--;
                    editops[editop_pos + dist].type = EditType::Replace;
                    editops[editop_pos + dist].src_pos = col + src_pos;
                    editops[editop_pos + dist].dest_pos = row + dest_pos;
                }
            }
        }
    }

    while (col) {
        dist--;
        col--;
        editops[editop_pos + dist].type = EditType::Delete;
        editops[editop_pos + dist].src_pos = col + src_pos;
        editops[editop_pos + dist].dest_pos = row + dest_pos;
    }

    while (row) {
        dist--;
        row--;
        editops[editop_pos + dist].type = EditType::Insert;
        editops[editop_pos + dist].src_pos = col + src_pos;
        editops[editop_pos + dist].dest_pos = row + dest_pos;
    }
}

template <typename InputIt1, typename InputIt2>
void levenshtein_align(Editops& editops, Range<InputIt1> s1, Range<InputIt2> s2,
                       int64_t max = std::numeric_limits<int64_t>::max(), size_t src_pos = 0,
                       size_t dest_pos = 0, size_t editop_pos = 0)
{
    /* upper bound */
    max = std::min(max, std::max<int64_t>(s1.size(), s2.size()));
    int64_t full_band = std::min<int64_t>(s1.size(), 2 * max + 1);

    LevenshteinResult<true, false> matrix;
    if (s1.empty() || s2.empty()) {
        matrix.dist = s1.size() + s2.size();
    }
    else if (s1.size() <= 64)
        matrix = levenshtein_hyrroe2003<true, false>(PatternMatchVector(s1), s1, s2);
    else if (full_band <= 64)
        matrix = levenshtein_hyrroe2003_small_band<true>(s1, s2, max);
    else
        matrix = levenshtein_hyrroe2003_block<true, false>(BlockPatternMatchVector(s1), s1, s2);

    assert(matrix.dist <= max);
    if (matrix.dist != 0) {
        if (editops.size() == 0) editops.resize(static_cast<size_t>(matrix.dist));

        recover_alignment(editops, s1, s2, matrix, src_pos, dest_pos, editop_pos);
    }
}

template <typename InputIt1, typename InputIt2>
LevenshteinResult<false, true> levenshtein_row(Range<InputIt1> s1, Range<InputIt2> s2)
{
    return levenshtein_hyrroe2003_block<false, true>(BlockPatternMatchVector(s1), s1, s2);
}

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_distance(Range<InputIt1> s1, Range<InputIt2> s2,
                             LevenshteinWeightTable weights = {1, 1, 1},
                             int64_t max = std::numeric_limits<int64_t>::max())
{
    if (weights.insert_cost == weights.delete_cost) {
        /* when insertions + deletions operations are free there can not be any edit distance */
        if (weights.insert_cost == 0) return 0;

        /* uniform Levenshtein multiplied with the common factor */
        if (weights.insert_cost == weights.replace_cost) {
            // max can make use of the common divisor of the three weights
            int64_t new_max = ceil_div(max, weights.insert_cost);
            int64_t distance = uniform_levenshtein_distance(s1, s2, new_max);
            distance *= weights.insert_cost;
            return (distance <= max) ? distance : max + 1;
        }
        /*
         * when replace_cost >= insert_cost + delete_cost no substitutions are performed
         * therefore this can be implemented as InDel distance multiplied with the common factor
         */
        else if (weights.replace_cost >= weights.insert_cost + weights.delete_cost) {
            // max can make use of the common divisor of the three weights
            int64_t new_max = ceil_div(max, weights.insert_cost);
            int64_t distance = rapidfuzz::indel_distance(s1, s2, new_max);
            distance *= weights.insert_cost;
            return (distance <= max) ? distance : max + 1;
        }
    }

    return generalized_levenshtein_wagner_fischer(s1, s2, weights, max);
}
struct HirschbergPos {
    int64_t left_score;
    int64_t right_score;
    ptrdiff_t s1_mid;
    ptrdiff_t s2_mid;
};

template <typename InputIt1, typename InputIt2>
HirschbergPos find_hirschberg_pos(Range<InputIt1> s1, Range<InputIt2> s2)
{
    HirschbergPos hpos = {};
    hpos.s2_mid = s2.size() / 2;
    size_t s1_len = static_cast<size_t>(s1.size());
    int64_t best_score = std::numeric_limits<int64_t>::max();
    int64_t left_score = hpos.s2_mid;
    std::vector<int64_t> right_scores(s1_len + 1, 0);
    assume(right_scores.size() != 0);
    right_scores[0] = s2.size() - hpos.s2_mid;

    {
        auto right_row = levenshtein_row(s1.reversed(), s2.subseq(hpos.s2_mid).reversed());
        for (size_t i = 0; i < s1_len; ++i) {
            size_t col_pos = i % 64;
            size_t col_word = i / 64;
            uint64_t col_mask = UINT64_C(1) << col_pos;

            right_scores[i + 1] = right_scores[i];
            right_scores[i + 1] -= bool(right_row.vecs[col_word].VN & col_mask);
            right_scores[i + 1] += bool(right_row.vecs[col_word].VP & col_mask);
        }
    }

    auto left_row = levenshtein_row(s1, s2.subseq(0, hpos.s2_mid));
    for (size_t i = 0; i < s1_len; ++i) {
        size_t col_pos = i % 64;
        size_t col_word = i / 64;
        uint64_t col_mask = UINT64_C(1) << col_pos;
        left_score -= bool(left_row.vecs[col_word].VN & col_mask);
        left_score += bool(left_row.vecs[col_word].VP & col_mask);

        if (right_scores[s1_len - i - 1] + left_score < best_score) {
            best_score = right_scores[s1_len - i - 1] + left_score;
            hpos.left_score = left_score;
            hpos.right_score = right_scores[s1_len - i - 1];
            hpos.s1_mid = static_cast<ptrdiff_t>(i + 1);
        }
    }

    assert(hpos.left_score >= 0);
    assert(hpos.right_score >= 0);
    assert(levenshtein_distance(s1, s2) == hpos.left_score + hpos.right_score);

    return hpos;
}

template <typename InputIt1, typename InputIt2>
void levenshtein_align_hirschberg(Editops& editops, Range<InputIt1> s1, Range<InputIt2> s2,
                                  size_t src_pos = 0, size_t dest_pos = 0, size_t editop_pos = 0,
                                  int64_t max = std::numeric_limits<int64_t>::max())
{
    // todo add blockwise implementation of levenshtein matrix / row

    /* prefix and suffix are no-ops, which do not need to be added to the editops */
    StringAffix affix = remove_common_affix(s1, s2);
    src_pos += affix.prefix_len;
    dest_pos += affix.prefix_len;

    ptrdiff_t matrix_size = 2 * s1.size() * s2.size() / 8;
    if (matrix_size < 1024 * 1024 || s1.size() < 65 || s2.size() < 10) {
        levenshtein_align(editops, s1, s2, max, src_pos, dest_pos, editop_pos);
    }
    /* Hirschbergs algorithm */
    else {
        auto hpos = find_hirschberg_pos(s1, s2);

        if (editops.size() == 0) editops.resize(static_cast<size_t>(hpos.left_score + hpos.right_score));

        levenshtein_align_hirschberg(editops, s1.subseq(0, hpos.s1_mid), s2.subseq(0, hpos.s2_mid), src_pos,
                                     dest_pos, editop_pos, hpos.left_score);
        levenshtein_align_hirschberg(editops, s1.subseq(hpos.s1_mid), s2.subseq(hpos.s2_mid),
                                     src_pos + static_cast<size_t>(hpos.s1_mid),
                                     dest_pos + static_cast<size_t>(hpos.s2_mid),
                                     editop_pos + static_cast<size_t>(hpos.left_score), hpos.right_score);
    }
}

template <typename InputIt1, typename InputIt2>
Editops levenshtein_editops(Range<InputIt1> s1, Range<InputIt2> s2)
{
    Editops editops;
    levenshtein_align_hirschberg(editops, s1, s2);
    editops.set_src_len(static_cast<size_t>(s1.size()));
    editops.set_dest_len(static_cast<size_t>(s2.size()));
    return editops;
}

class Levenshtein : public DistanceBase<Levenshtein, LevenshteinWeightTable> {
    friend DistanceBase<Levenshtein, LevenshteinWeightTable>;
    friend NormalizedMetricBase<Levenshtein, LevenshteinWeightTable>;

    template <typename InputIt1, typename InputIt2>
    static int64_t maximum(Range<InputIt1> s1, Range<InputIt2> s2, LevenshteinWeightTable weights)
    {
        return levenshtein_maximum(s1, s2, weights);
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t _distance(Range<InputIt1> s1, Range<InputIt2> s2, LevenshteinWeightTable weights,
                             int64_t score_cutoff)
    {
        return levenshtein_distance(s1, s2, weights, score_cutoff);
    }
};

} // namespace detail
} // namespace rapidfuzz
