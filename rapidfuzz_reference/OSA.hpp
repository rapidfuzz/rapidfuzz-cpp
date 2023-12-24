/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once
#include "common.hpp"
#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

namespace rapidfuzz_reference {

template <typename InputIt1, typename InputIt2>
Matrix<size_t> osa_matrix(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
    size_t len1 = static_cast<size_t>(std::distance(first1, last1));
    size_t len2 = static_cast<size_t>(std::distance(first2, last2));

    Matrix<size_t> matrix(static_cast<size_t>(len1) + 1, static_cast<size_t>(len2) + 1);

    for (size_t i = 0; i <= len1; ++i)
        matrix(i, 0) = i;
    for (size_t i = 0; i <= len2; ++i)
        matrix(0, i) = i;

    for (size_t pos1 = 0; pos1 < len1; ++pos1) {
        for (size_t pos2 = 0; pos2 < len2; ++pos2) {
            size_t cost = (first1[pos1] == first2[pos2]) ? 0 : 1;

            matrix(pos1 + 1, pos2 + 1) =
                std::min({matrix(pos1, pos2 + 1) + 1, matrix(pos1 + 1, pos2) + 1, matrix(pos1, pos2) + cost});

            if (pos1 == 0 || pos2 == 0) continue;
            if (first1[pos1] != first2[pos2 - 1]) continue;
            if (first1[pos1 - 1] != first2[pos2]) continue;

            matrix(pos1 + 1, pos2 + 1) =
                std::min(matrix(pos1 + 1, pos2 + 1), matrix(pos1 - 1, pos2 - 1) + cost);
        }
    }

    return matrix;
}

template <typename InputIt1, typename InputIt2>
size_t osa_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                    size_t score_cutoff = std::numeric_limits<size_t>::max())
{
    auto matrix = osa_matrix(first1, last1, first2, last2);
    size_t dist = matrix.back();
    return (dist <= score_cutoff) ? dist : score_cutoff + 1;
}

template <typename Sentence1, typename Sentence2>
size_t osa_distance(const Sentence1& s1, const Sentence2& s2,
                    size_t score_cutoff = std::numeric_limits<size_t>::max())
{
    return osa_distance(std::begin(s1), std::end(s1), std::begin(s2), std::end(s2), score_cutoff);
}

} // namespace rapidfuzz_reference
