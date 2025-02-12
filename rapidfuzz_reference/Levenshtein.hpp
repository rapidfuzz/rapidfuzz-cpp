/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once
#include "common.hpp"
#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

namespace rapidfuzz_reference {

struct LevenshteinWeightTable {
    size_t insert_cost;
    size_t delete_cost;
    size_t replace_cost;
};

static inline size_t levenshtein_maximum(size_t len1, size_t len2, LevenshteinWeightTable weights)
{
    size_t max_dist = len1 * weights.delete_cost + len2 * weights.insert_cost;

    if (len1 >= len2)
        max_dist = std::min(max_dist, len2 * weights.replace_cost + (len1 - len2) * weights.delete_cost);
    else
        max_dist = std::min(max_dist, len1 * weights.replace_cost + (len2 - len1) * weights.insert_cost);

    return max_dist;
}

template <typename InputIt1, typename InputIt2>
Matrix<size_t> levenshtein_matrix(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                  LevenshteinWeightTable weights = {1, 1, 1})
{
    size_t len1 = static_cast<size_t>(std::distance(first1, last1));
    size_t len2 = static_cast<size_t>(std::distance(first2, last2));

    Matrix<size_t> matrix(len1 + 1, len2 + 1);

    for (size_t i = 0; i <= len1; ++i)
        matrix(i, 0) = i * weights.delete_cost;
    for (size_t i = 0; i <= len2; ++i)
        matrix(0, i) = i * weights.insert_cost;

    for (size_t pos1 = 0; pos1 < len1; ++pos1) {
        for (size_t pos2 = 0; pos2 < len2; ++pos2) {
            size_t cost = (first1[pos1] == first2[pos2]) ? 0 : weights.replace_cost;

            matrix(pos1 + 1, pos2 + 1) =
                std::min({matrix(pos1, pos2 + 1) + weights.delete_cost,
                          matrix(pos1 + 1, pos2) + weights.insert_cost, matrix(pos1, pos2) + cost});
        }
    }

    return matrix;
}

template <typename Sentence1, typename Sentence2>
Matrix<size_t> levenshtein_matrix(const Sentence1& s1, const Sentence2& s2,
                                  LevenshteinWeightTable weights = {1, 1, 1})
{
    return levenshtein_matrix(std::begin(s1), std::end(s1), std::begin(s2), std::end(s2), weights);
}

template <typename InputIt1, typename InputIt2>
size_t levenshtein_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                            LevenshteinWeightTable weights = {1, 1, 1},
                            size_t score_cutoff = std::numeric_limits<size_t>::max())
{
    auto matrix = levenshtein_matrix(first1, last1, first2, last2, weights);
    size_t dist = matrix.back();
    return (dist <= score_cutoff) ? dist : score_cutoff + 1;
}

template <typename Sentence1, typename Sentence2>
size_t levenshtein_distance(const Sentence1& s1, const Sentence2& s2,
                            LevenshteinWeightTable weights = {1, 1, 1},
                            size_t score_cutoff = std::numeric_limits<size_t>::max())
{
    return levenshtein_distance(std::begin(s1), std::end(s1), std::begin(s2), std::end(s2), weights,
                                score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double levenshtein_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                              LevenshteinWeightTable weights = {1, 1, 1}, double score_cutoff = 0.0)
{
    size_t len1 = static_cast<size_t>(std::distance(first1, last1));
    size_t len2 = static_cast<size_t>(std::distance(first2, last2));
    size_t dist = levenshtein_distance(first1, last1, first2, last2, weights);
    size_t max = levenshtein_maximum(len1, len2, weights);
    double sim = 1.0 - (double)dist / max;
    return (sim >= score_cutoff) ? sim : 0.0;
}

template <typename Sentence1, typename Sentence2>
double levenshtein_similarity(const Sentence1& s1, const Sentence2& s2,
                              LevenshteinWeightTable weights = {1, 1, 1}, double score_cutoff = 0.0)
{
    return levenshtein_similarity(std::begin(s1), std::end(s1), std::begin(s2), std::end(s2), weights,
                                  score_cutoff);
}

} // namespace rapidfuzz_reference
