/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once
#include "common.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <unordered_map>
#include <vector>

namespace rapidfuzz_reference {

template <typename InputIt1, typename InputIt2>
Matrix<int64_t> damerau_levenshtein_matrix(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
    ptrdiff_t len1 = std::distance(first1, last1);
    ptrdiff_t len2 = std::distance(first2, last2);
    ptrdiff_t infinite = len1 + len2;

    std::unordered_map<uint32_t, int64_t> da;
    Matrix<int64_t> matrix(static_cast<size_t>(len1) + 2, static_cast<size_t>(len2) + 2);
    matrix(0, 0) = infinite;

    for (ptrdiff_t i = 0; i <= len1; ++i) {
        matrix(i + 1, 0) = infinite;
        matrix(i + 1, 1) = i;
    }
    for (ptrdiff_t i = 0; i <= len2; ++i) {
        matrix(0, i + 1) = infinite;
        matrix(1, i + 1) = i;
    }

    for (ptrdiff_t pos1 = 0; pos1 < len1; ++pos1) {
        ptrdiff_t db = 0;
        for (ptrdiff_t pos2 = 0; pos2 < len2; ++pos2) {
            int64_t i1 = da[static_cast<uint32_t>(first2[pos2])];
            ptrdiff_t j1 = db;
            ptrdiff_t cost = 1;
            if (first1[pos1] == first2[pos2]) {
                cost = 0;
                db = pos2 + 1;
            }

            matrix(pos1 + 2, pos2 + 2) =
                std::min({matrix(pos1 + 1, pos2 + 1) + cost, matrix(pos1 + 2, pos2 + 1) + 1,
                          matrix(pos1 + 1, pos2 + 2) + 1, matrix(i1, j1) + (pos1 - i1) + 1 + (pos2 - j1)

                });
        }

        da[first1[pos1]] = pos1 + 1;
    }

    return matrix;
}

template <typename InputIt1, typename InputIt2>
int64_t damerau_levenshtein_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                     int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    auto matrix = damerau_levenshtein_matrix(first1, last1, first2, last2);
    int64_t dist = matrix.back();
    return (dist <= score_cutoff) ? dist : score_cutoff + 1;
}

template <typename Sentence1, typename Sentence2>
int64_t damerau_levenshtein_distance(const Sentence1& s1, const Sentence2& s2,
                                     int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return damerau_levenshtein_distance(std::begin(s1), std::end(s1), std::begin(s2), std::end(s2),
                                        score_cutoff);
}

} // namespace rapidfuzz_reference
