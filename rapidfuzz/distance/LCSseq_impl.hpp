/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#include <rapidfuzz/details/Matrix.hpp>
#include <rapidfuzz/details/PatternMatchVector.hpp>
#include <rapidfuzz/details/common.hpp>
#include <rapidfuzz/details/distance.hpp>
#include <rapidfuzz/details/intrinsics.hpp>

#include <algorithm>
#include <array>

namespace rapidfuzz {
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
static constexpr std::array<std::array<uint8_t, 7>, 14> lcs_seq_mbleven2018_matrix = {{
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
}};

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_mbleven2018(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_cutoff)
{
    auto len1 = s1.size();
    auto len2 = s2.size();

    if (len1 < len2) return lcs_seq_mbleven2018(s2, s1, score_cutoff);

    auto len_diff = len1 - len2;
    int64_t max_misses = static_cast<ptrdiff_t>(len1) - score_cutoff;
    auto ops_index = (max_misses + max_misses * max_misses) / 2 + len_diff - 1;
    auto& possible_ops = lcs_seq_mbleven2018_matrix[static_cast<size_t>(ops_index)];
    int64_t max_len = 0;

    for (uint8_t ops : possible_ops) {
        ptrdiff_t s1_pos = 0;
        ptrdiff_t s2_pos = 0;
        int64_t cur_len = 0;

        while (s1_pos < len1 && s2_pos < len2) {
            if (s1[s1_pos] != s2[s2_pos]) {
                if (!ops) break;
                if (ops & 1)
                    s1_pos++;
                else if (ops & 2)
                    s2_pos++;
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
                cur_len++;
                s1_pos++;
                s2_pos++;
            }
        }

        max_len = std::max(max_len, cur_len);
    }

    return (max_len >= score_cutoff) ? max_len : 0;
}

template <size_t N, typename PMV, typename InputIt1, typename InputIt2>
static inline int64_t longest_common_subsequence_unroll(const PMV& block, Range<InputIt1>, Range<InputIt2> s2,
                                                        int64_t score_cutoff)
{
    uint64_t S[N];
    unroll<size_t, N>([&](size_t i) { S[i] = ~UINT64_C(0); });

    for (const auto& ch : s2) {
        uint64_t carry = 0;
        unroll<size_t, N>([&](size_t i) {
            uint64_t Matches = block.get(i, ch);
            uint64_t u = S[i] & Matches;
            uint64_t x = addc64(S[i], u, carry, &carry);
            S[i] = x | (S[i] - u);
        });
    }

    int64_t res = 0;
    unroll<size_t, N>([&](size_t i) { res += popcount(~S[i]); });

    return (res >= score_cutoff) ? res : 0;
}

template <typename InputIt1, typename InputIt2>
static inline int64_t longest_common_subsequence_blockwise(const BlockPatternMatchVector& block,
                                                           Range<InputIt1>, Range<InputIt2> s2,
                                                           int64_t score_cutoff)
{
    auto words = block.size();
    std::vector<uint64_t> S(words, ~UINT64_C(0));

    for (const auto& ch : s2) {
        uint64_t carry = 0;
        for (size_t word = 0; word < words; ++word) {
            const uint64_t Matches = block.get(word, ch);
            uint64_t Stemp = S[word];

            uint64_t u = Stemp & Matches;

            uint64_t x = addc64(Stemp, u, carry, &carry);
            S[word] = x | (Stemp - u);
        }
    }

    int64_t res = 0;
    for (uint64_t Stemp : S)
        res += popcount(~Stemp);

    return (res >= score_cutoff) ? res : 0;
}

template <typename InputIt1, typename InputIt2>
int64_t longest_common_subsequence(const BlockPatternMatchVector& block, Range<InputIt1> s1,
                                   Range<InputIt2> s2, int64_t score_cutoff)
{
    auto nr = ceil_div(s1.size(), 64);
    switch (nr) {
    case 0: return 0;
    case 1: return longest_common_subsequence_unroll<1>(block, s1, s2, score_cutoff);
    case 2: return longest_common_subsequence_unroll<2>(block, s1, s2, score_cutoff);
    case 3: return longest_common_subsequence_unroll<3>(block, s1, s2, score_cutoff);
    case 4: return longest_common_subsequence_unroll<4>(block, s1, s2, score_cutoff);
    case 5: return longest_common_subsequence_unroll<5>(block, s1, s2, score_cutoff);
    case 6: return longest_common_subsequence_unroll<6>(block, s1, s2, score_cutoff);
    case 7: return longest_common_subsequence_unroll<7>(block, s1, s2, score_cutoff);
    case 8: return longest_common_subsequence_unroll<8>(block, s1, s2, score_cutoff);
    default: return longest_common_subsequence_blockwise(block, s1, s2, score_cutoff);
    }
}

template <typename InputIt1, typename InputIt2>
int64_t longest_common_subsequence(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_cutoff)
{
    auto nr = ceil_div(s1.size(), 64);
    switch (nr) {
    case 0: return 0;
    case 1: return longest_common_subsequence_unroll<1>(PatternMatchVector(s1), s1, s2, score_cutoff);
    case 2: return longest_common_subsequence_unroll<2>(BlockPatternMatchVector(s1), s1, s2, score_cutoff);
    case 3: return longest_common_subsequence_unroll<3>(BlockPatternMatchVector(s1), s1, s2, score_cutoff);
    case 4: return longest_common_subsequence_unroll<4>(BlockPatternMatchVector(s1), s1, s2, score_cutoff);
    case 5: return longest_common_subsequence_unroll<5>(BlockPatternMatchVector(s1), s1, s2, score_cutoff);
    case 6: return longest_common_subsequence_unroll<6>(BlockPatternMatchVector(s1), s1, s2, score_cutoff);
    case 7: return longest_common_subsequence_unroll<7>(BlockPatternMatchVector(s1), s1, s2, score_cutoff);
    case 8: return longest_common_subsequence_unroll<8>(BlockPatternMatchVector(s1), s1, s2, score_cutoff);
    default: return longest_common_subsequence_blockwise(BlockPatternMatchVector(s1), s1, s2, score_cutoff);
    }
}

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_similarity(const BlockPatternMatchVector& block, Range<InputIt1> s1, Range<InputIt2> s2,
                           int64_t score_cutoff)
{
    auto len1 = s1.size();
    auto len2 = s2.size();
    int64_t max_misses = static_cast<int64_t>(len1) + len2 - 2 * score_cutoff;

    /* no edits are allowed */
    if (max_misses == 0 || (max_misses == 1 && len1 == len2))
        return std::equal(s1.begin(), s1.end(), s2.begin(), s2.end()) ? len1 : 0;

    if (max_misses < std::abs(len1 - len2)) return 0;

    // do this first, since we can not remove any affix in encoded form
    if (max_misses >= 5) return longest_common_subsequence(block, s1, s2, score_cutoff);

    /* common affix does not effect Levenshtein distance */
    auto affix = remove_common_affix(s1, s2);
    int64_t lcs_sim = static_cast<int64_t>(affix.prefix_len + affix.suffix_len);
    if (!s1.empty() && !s2.empty()) lcs_sim += lcs_seq_mbleven2018(s1, s2, score_cutoff - lcs_sim);

    return lcs_sim;
}

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_similarity(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_cutoff)
{
    auto len1 = s1.size();
    auto len2 = s2.size();

    // Swapping the strings so the second string is shorter
    if (len1 < len2) return lcs_seq_similarity(s2, s1, score_cutoff);

    int64_t max_misses = static_cast<int64_t>(len1) + len2 - 2 * score_cutoff;

    /* no edits are allowed */
    if (max_misses == 0 || (max_misses == 1 && len1 == len2))
        return std::equal(s1.begin(), s1.end(), s2.begin(), s2.end()) ? len1 : 0;

    if (max_misses < std::abs(len1 - len2)) return 0;

    /* common affix does not effect Levenshtein distance */
    auto affix = remove_common_affix(s1, s2);
    int64_t lcs_sim = static_cast<int64_t>(affix.prefix_len + affix.suffix_len);
    if (s1.size() && s2.size()) {
        if (max_misses < 5)
            lcs_sim += lcs_seq_mbleven2018(s1, s2, score_cutoff - lcs_sim);
        else
            lcs_sim += longest_common_subsequence(s1, s2, score_cutoff - lcs_sim);
    }

    return lcs_sim;
}

struct LLCSBitMatrix {
    LLCSBitMatrix(size_t rows, size_t cols, ptrdiff_t _dist = 0) : S(rows, cols, ~UINT64_C(0)), dist(_dist)
    {}

    Matrix<uint64_t> S;

    ptrdiff_t dist;
};

/**
 * @brief recover alignment from bitparallel Levenshtein matrix
 */
template <typename InputIt1, typename InputIt2>
Editops recover_alignment(Range<InputIt1> s1, Range<InputIt2> s2, const LLCSBitMatrix& matrix,
                          StringAffix affix)
{
    auto len1 = s1.size();
    auto len2 = s2.size();
    auto dist = static_cast<size_t>(matrix.dist);
    Editops editops(dist);
    editops.set_src_len(len1 + affix.prefix_len + affix.suffix_len);
    editops.set_dest_len(len2 + affix.prefix_len + affix.suffix_len);

    if (dist == 0) return editops;

    auto col = len1;
    auto row = len2;

    while (row && col) {
        uint64_t col_pos = col - 1;
        uint64_t col_word = col_pos / 64;
        col_pos = col_pos % 64;
        uint64_t mask = UINT64_C(1) << col_pos;

        /* Deletion */
        if (matrix.S[row - 1][col_word] & mask) {
            assert(dist > 0);
            dist--;
            col--;
            editops[dist].type = EditType::Delete;
            editops[dist].src_pos = col + affix.prefix_len;
            editops[dist].dest_pos = row + affix.prefix_len;
        }
        else {
            row--;

            /* Insertion */
            if (row && (~matrix.S[row - 1][col_word]) & mask) {
                assert(dist > 0);
                dist--;
                editops[dist].type = EditType::Insert;
                editops[dist].src_pos = col + affix.prefix_len;
                editops[dist].dest_pos = row + affix.prefix_len;
            }
            /* Match */
            else {
                col--;
                assert(s1[col] == s2[row]);
            }
        }
    }

    while (col) {
        dist--;
        col--;
        editops[dist].type = EditType::Delete;
        editops[dist].src_pos = col + affix.prefix_len;
        editops[dist].dest_pos = row + affix.prefix_len;
    }

    while (row) {
        dist--;
        row--;
        editops[dist].type = EditType::Insert;
        editops[dist].src_pos = col + affix.prefix_len;
        editops[dist].dest_pos = row + affix.prefix_len;
    }

    return editops;
}

template <size_t N, typename PMV, typename InputIt1, typename InputIt2>
LLCSBitMatrix llcs_matrix_unroll(const PMV& block, Range<InputIt1> s1, Range<InputIt2> s2)
{
    auto len1 = s1.size();
    auto len2 = s2.size();
    uint64_t S[N];
    unroll<size_t, N>([&](size_t i) { S[i] = ~UINT64_C(0); });

    LLCSBitMatrix matrix(len2, N);

    for (ptrdiff_t i = 0; i < len2; ++i) {
        uint64_t carry = 0;
        unroll<size_t, N>([&](size_t word) {
            uint64_t Matches = block.get(word, s2[i]);
            uint64_t u = S[word] & Matches;
            uint64_t x = addc64(S[word], u, carry, &carry);
            S[word] = matrix.S[i][word] = x | (S[word] - u);
        });
    }

    int64_t res = 0;
    unroll<size_t, N>([&](size_t i) { res += popcount(~S[i]); });

    matrix.dist = static_cast<ptrdiff_t>(static_cast<int64_t>(len1) + len2 - 2 * res);

    return matrix;
}

template <typename InputIt1, typename InputIt2>
LLCSBitMatrix llcs_matrix_blockwise(const BlockPatternMatchVector& block, Range<InputIt1> s1,
                                    Range<InputIt2> s2)
{
    auto len1 = s1.size();
    auto len2 = s2.size();
    auto words = block.size();
    /* todo could be replaced with access to matrix which would slightly
     * reduce memory usage */
    std::vector<uint64_t> S(words, ~UINT64_C(0));
    LLCSBitMatrix matrix(len2, words);

    for (size_t i = 0; i < static_cast<size_t>(len2); ++i) {
        uint64_t carry = 0;
        for (size_t word = 0; word < words; ++word) {
            const uint64_t Matches = block.get(word, s2[i]);
            uint64_t Stemp = S[word];

            uint64_t u = Stemp & Matches;

            uint64_t x = addc64(Stemp, u, carry, &carry);
            S[word] = matrix.S[i][word] = x | (Stemp - u);
        }
    }

    int64_t res = 0;
    for (uint64_t Stemp : S)
        res += popcount(~Stemp);

    matrix.dist = static_cast<ptrdiff_t>(static_cast<std::int64_t>(len1) + len2 - 2 * res);

    return matrix;
}

template <typename InputIt1, typename InputIt2>
LLCSBitMatrix llcs_matrix(Range<InputIt1> s1, Range<InputIt2> s2)
{
    auto nr = ceil_div(s1.size(), 64);
    switch (nr) {
    case 0: return LLCSBitMatrix(0, 0, s1.size() + s2.size());
    case 1: return llcs_matrix_unroll<1>(PatternMatchVector(s1), s1, s2);
    case 2: return llcs_matrix_unroll<2>(BlockPatternMatchVector(s1), s1, s2);
    case 3: return llcs_matrix_unroll<3>(BlockPatternMatchVector(s1), s1, s2);
    case 4: return llcs_matrix_unroll<4>(BlockPatternMatchVector(s1), s1, s2);
    case 5: return llcs_matrix_unroll<5>(BlockPatternMatchVector(s1), s1, s2);
    case 6: return llcs_matrix_unroll<6>(BlockPatternMatchVector(s1), s1, s2);
    case 7: return llcs_matrix_unroll<7>(BlockPatternMatchVector(s1), s1, s2);
    case 8: return llcs_matrix_unroll<8>(BlockPatternMatchVector(s1), s1, s2);
    default: return llcs_matrix_blockwise(BlockPatternMatchVector(s1), s1, s2);
    }
}

template <typename InputIt1, typename InputIt2>
Editops lcs_seq_editops(Range<InputIt1> s1, Range<InputIt2> s2)
{
    /* prefix and suffix are no-ops, which do not need to be added to the editops */
    StringAffix affix = remove_common_affix(s1, s2);

    return recover_alignment(s1, s2, llcs_matrix(s1, s2), affix);
}

class LCSseq : public SimilarityBase<LCSseq> {
    friend SimilarityBase<LCSseq>;
    friend NormalizedMetricBase<LCSseq>;

    template <typename InputIt1, typename InputIt2>
    static int64_t maximum(Range<InputIt1> s1, Range<InputIt2> s2)
    {
        return std::max(s1.size(), s2.size());
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t _similarity(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_cutoff)
    {
        return lcs_seq_similarity(s1, s2, score_cutoff);
    }
};

} // namespace detail
} // namespace rapidfuzz