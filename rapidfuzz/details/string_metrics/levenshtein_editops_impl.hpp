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

struct LevenshteinBitMatrix {
    LevenshteinBitMatrix(uint64_t rows, uint64_t cols)
        : VP(rows, cols, (uint64_t)-1), VN(rows, cols, 0), dist(0)
    {}

    common::Matrix<uint64_t> VP;
    common::Matrix<uint64_t> VN;

    size_t dist;
};

/**
 * @brief recover alignment from bitparallel Levenshtein matrix
 */
template <typename CharT1, typename CharT2>
Editops recover_alignment(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2,
                          const LevenshteinBitMatrix& matrix, StringAffix affix)
{
    size_t dist = matrix.dist;
    Editops editops(dist);
    editops.set_src_len(s1.size() + affix.prefix_len + affix.suffix_len);
    editops.set_dest_len(s2.size() + affix.prefix_len + affix.suffix_len);

    if (dist == 0) {
        return editops;
    }

    size_t row = s1.size();
    size_t col = s2.size();

    while (row && col) {
        uint64_t col_pos = col - 1;
        uint64_t col_word = col_pos / 64;
        col_pos = col_pos % 64;
        uint64_t mask = 1ull << col_pos;

        /* Insertion */
        if (matrix.VP[row - 1][col_word] & mask) {
            dist--;
            col--;
            editops[dist].type = EditType::Insert;
            editops[dist].src_pos = row + affix.prefix_len;
            editops[dist].dest_pos = col + affix.prefix_len;
        }
        else {
            row--;

            /* Deletion */
            if (row && matrix.VN[row - 1][col_word] & mask) {
                dist--;
                editops[dist].type = EditType::Delete;
                editops[dist].src_pos = row + affix.prefix_len;
                editops[dist].dest_pos = col + affix.prefix_len;
            }
            /* Match/Mismatch */
            else {
                col--;

                /* Replace (Matches are not recorded) */
                if (s1[row] != s2[col]) {
                    dist--;
                    editops[dist].type = EditType::Replace;
                    editops[dist].src_pos = row + affix.prefix_len;
                    editops[dist].dest_pos = col + affix.prefix_len;
                }
            }
        }
    }

    while (col) {
        dist--;
        col--;
        editops[dist].type = EditType::Insert;
        editops[dist].src_pos = row + affix.prefix_len;
        editops[dist].dest_pos = col + affix.prefix_len;
    }

    while (row) {
        dist--;
        row--;
        editops[dist].type = EditType::Delete;
        editops[dist].src_pos = row + affix.prefix_len;
        editops[dist].dest_pos = col + affix.prefix_len;
    }

    return editops;
}

template <typename CharT1>
LevenshteinBitMatrix levenshtein_matrix_hyrroe2003(basic_string_view<CharT1> s2,
                                                   const common::PatternMatchVector& PM,
                                                   size_t s1_len)
{
    /* VP is set to 1^m. Shifting by bitwidth would be undefined behavior */
    uint64_t VP = (uint64_t)-1;
    uint64_t VN = 0;

    LevenshteinBitMatrix matrix(s2.size(), 1);
    matrix.dist = s1_len;

    /* mask used when computing D[m,j] in the paper 10^(m-1) */
    uint64_t mask = (uint64_t)1 << (s1_len - 1);

    /* Searching */
    for (size_t i = 0; i < s2.size(); ++i) {
        /* Step 1: Computing D0 */
        uint64_t PM_j = PM.get(s2[i]);
        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        matrix.dist += bool(HP & mask);
        matrix.dist -= bool(HN & mask);

        /* Step 4: Computing Vp and VN */
        HP = (HP << 1) | 1;
        HN = (HN << 1);

        VP = matrix.VP[i][0] = HN | ~(D0 | HP);
        VN = matrix.VN[i][0] = HP & D0;
    }

    return matrix;
}

template <typename CharT1>
LevenshteinBitMatrix levenshtein_matrix_hyrroe2003_block(basic_string_view<CharT1> s2,
                                                         const common::BlockPatternMatchVector& PM,
                                                         size_t s1_len)
{
    /* todo could be replaced with access to matrix which would slightly
     * reduce memory usage */
    struct Vectors {
        uint64_t VP;
        uint64_t VN;

        Vectors() : VP(~0x0ull), VN(0)
        {}
    };

    const size_t words = PM.m_val.size();
    LevenshteinBitMatrix matrix(s2.size(), words);
    matrix.dist = s1_len;

    std::vector<Vectors> vecs(words);
    const uint64_t Last = (uint64_t)1 << ((s1_len - 1) % 64);

    /* Searching */
    for (size_t i = 0; i < s2.size(); i++) {
        uint64_t HP_carry = 1;
        uint64_t HN_carry = 0;

        // uint64_t PM_j = PM.get(0, s2[i]);

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

            vecs[word].VP = matrix.VP[i][word] = HN | ~(D0 | HP);
            vecs[word].VN = matrix.VN[i][word] = HP & D0;
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
            matrix.dist += bool(HP & Last);
            matrix.dist -= bool(HN & Last);

            /* Step 4: Computing Vp and VN */
            HP = (HP << 1) | HP_carry;
            HN = (HN << 1) | HN_carry;

            vecs[words - 1].VP = matrix.VP[i][words - 1] = HN | ~(D0 | HP);
            vecs[words - 1].VN = matrix.VN[i][words - 1] = HP & D0;
        }
    }

    return matrix;
}

template <typename CharT1, typename CharT2>
LevenshteinBitMatrix levenshtein_matrix(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2)
{
    if (s2.empty()) {
        LevenshteinBitMatrix matrix(0, 0);
        matrix.dist = s1.size();
        return matrix;
    }
    else if (s1.empty()) {
        LevenshteinBitMatrix matrix(0, 0);
        matrix.dist = s2.size();
        return matrix;
    }
    else if (s2.size() <= 64) {
        return levenshtein_matrix_hyrroe2003(s1, common::PatternMatchVector(s2), s2.size());
    }
    else {
        return levenshtein_matrix_hyrroe2003_block(s1, common::BlockPatternMatchVector(s2),
                                                   s2.size());
    }
}

template <typename CharT1, typename CharT2>
Editops levenshtein_editops(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2)
{
    /* prefix and suffix are no-ops, which do not need to be added to the editops */
    StringAffix affix = common::remove_common_affix(s1, s2);

    return recover_alignment(s1, s2, levenshtein_matrix(s1, s2), affix);
}

} // namespace detail
} // namespace string_metric
} // namespace rapidfuzz
