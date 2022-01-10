/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include <algorithm>
#include <array>
#include <limits>
#include <numeric>
#include <rapidfuzz/details/common.hpp>
#include <rapidfuzz/details/intrinsics.hpp>

namespace rapidfuzz {
namespace string_metric {
namespace detail {

struct LLCSBitMatrix {
    LLCSBitMatrix(uint64_t rows, uint64_t cols)
        : S(rows, cols, (uint64_t)-1), dist(0)
    {}

    common::Matrix<uint64_t> S;

    size_t dist;
};

/**
 * @brief recover alignment from bitparallel Levenshtein matrix
 */
template <typename CharT1, typename CharT2>
std::vector<LevenshteinEditOp> recover_alignment(
    basic_string_view<CharT1> s1, basic_string_view<CharT2> s2,
    const LLCSBitMatrix& matrix, size_t prefix_len
)
{
    size_t dist = matrix.dist;
    std::vector<LevenshteinEditOp> editops(dist);

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
        if (matrix.S[row - 1][col_word] & mask)
        {
            dist--;
            col--;
            editops[dist].type = LevenshteinEditType::Insert;
            editops[dist].src_pos = row + prefix_len;
            editops[dist].dest_pos = col + prefix_len;
        }
        else {
            row--;

            /* Deletion */
            if (row && (~matrix.S[row - 1][col_word]) & mask)
            {
                dist--;
                editops[dist].type = LevenshteinEditType::Delete;
                editops[dist].src_pos = row + prefix_len;
                editops[dist].dest_pos = col + prefix_len;
            }
            /* Match */
            else
            {
                col--;
                assert(s1[row] == s2[col]);
            }
        }
    }

    while (col)
    {
        dist--;
        col--;
        editops[dist].type = LevenshteinEditType::Insert;
        editops[dist].src_pos = row + prefix_len;
        editops[dist].dest_pos = col + prefix_len;
    }

    while (row)
    {
        dist--;
        row--;
        editops[dist].type = LevenshteinEditType::Delete;
        editops[dist].src_pos = row + prefix_len;
        editops[dist].dest_pos = col + prefix_len;
    }

    return editops;
}


template <std::size_t N, typename CharT1>
LLCSBitMatrix llcs_matrix_unroll(basic_string_view<CharT1> s1,
                            const common::PatternMatchVector* block,
                            std::size_t s2_len)
{
    std::uint64_t S[N];
    for (std::size_t i = 0; i < N; ++i)
    {
        S[i] = ~0x0ull;
    }

    LLCSBitMatrix matrix(s1.size(), N);

    for (size_t i = 0; i < s1.size(); ++i) {
        uint64_t carry = 0;
        std::uint64_t Matches[N];
        std::uint64_t u[N];
        std::uint64_t x[N];
        for (std::size_t word = 0; word < N; ++word)
        {
            Matches[word] = block[word].get(s1[i]);
            u[word] = S[word] & Matches[word];
            x[word] = intrinsics::addc64(S[word], u[word], carry, &carry);
            S[word] = matrix.S[i][word] = x[word] | (S[word] - u[word]);
        }
    }

    std::size_t res = 0;
    for (std::size_t i = 0; i < N; ++i)
    {
        res += intrinsics::popcount64(~S[i]);
    }

    matrix.dist = s1.size() + s2_len - 2 * res;

    return matrix;
}


template <typename CharT1>
LLCSBitMatrix llcs_matrix_blockwise(basic_string_view<CharT1> s1,
                            const common::BlockPatternMatchVector& block,
                            std::size_t s2_len)
{
    std::size_t words = block.m_val.size();
    /* todo could be replaced with access to matrix which would slightly
     * reduce memory usage */
    std::vector<std::uint64_t> S(words, ~0x0ull);
    LLCSBitMatrix matrix(s1.size(), words);

    for (size_t i = 0; i < s1.size(); ++i) {
        uint64_t carry = 0;
        for (std::size_t word = 0; word < words; ++word) {
            const uint64_t Matches = block.get(word, s1[i]);
            uint64_t Stemp = S[word];

            uint64_t u = Stemp & Matches;

            uint64_t x = intrinsics::addc64(Stemp, u, carry, &carry);
            S[word] = matrix.S[i][word] = x | (Stemp - u);
        }
    }

    std::size_t res = 0;
    for (uint64_t Stemp : S) {
        res += intrinsics::popcount64(~Stemp);
    }

    matrix.dist = s1.size() + s2_len - 2 * res;

    return matrix;
}

template <typename CharT1, typename CharT2>
LLCSBitMatrix llcs_matrix(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2)
{
    if (s2.empty())
    {
        LLCSBitMatrix matrix(0, 0);
        matrix.dist = s1.size();
        return matrix;
    }
    else if (s1.empty())
    {
        LLCSBitMatrix matrix(0, 0);
        matrix.dist = s2.size();
        return matrix;
    }
    else if (s2.size() <= 64)
    {
        common::PatternMatchVector block(s2);
        return llcs_matrix_unroll<1>(s1, &block, s2.size());
    }
    else
    {
        common::BlockPatternMatchVector block(s2);
        switch(block.m_val.size())
        {
        case 1:  return llcs_matrix_unroll<1>(s1, &block.m_val[0], s2.size());
        case 2:  return llcs_matrix_unroll<2>(s1, &block.m_val[0], s2.size());
        case 3:  return llcs_matrix_unroll<3>(s1, &block.m_val[0], s2.size());
        case 4:  return llcs_matrix_unroll<4>(s1, &block.m_val[0], s2.size());
        case 5:  return llcs_matrix_unroll<5>(s1, &block.m_val[0], s2.size());
        case 6:  return llcs_matrix_unroll<6>(s1, &block.m_val[0], s2.size());
        case 7:  return llcs_matrix_unroll<7>(s1, &block.m_val[0], s2.size());
        case 8:  return llcs_matrix_unroll<8>(s1, &block.m_val[0], s2.size());
        default: return llcs_matrix_blockwise(s1, block, s2.size());
        }
    }
}

template <typename CharT1, typename CharT2>
std::vector<LevenshteinEditOp> llcs_editops(basic_string_view<CharT1> s1,
                                                   basic_string_view<CharT2> s2)
{
    /* prefix and suffix are no-ops, which do not need to be added to the editops */
    StringAffix affix = common::remove_common_affix(s1, s2);

    return recover_alignment(s1, s2, llcs_matrix(s1, s2), affix.prefix_len);
}

} // namespace detail
} // namespace string_metric
} // namespace rapidfuzz
