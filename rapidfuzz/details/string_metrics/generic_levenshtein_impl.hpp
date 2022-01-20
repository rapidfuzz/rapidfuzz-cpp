/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once

#include <algorithm>
#include <array>
#include <limits>
#include <numeric>
#include <rapidfuzz/details/common.hpp>

namespace rapidfuzz {
namespace string_metric {
namespace detail {

template <typename InputIt1, typename InputIt2>
int64_t generalized_levenshtein_wagner_fischer(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                               InputIt2 last2, LevenshteinWeightTable weights,
                                               int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t cache_size = len1 + 1;
    std::vector<int64_t> cache(cache_size);

    cache[0] = 0;
    for (int64_t i = 1; i < cache_size; ++i) {
        cache[i] = cache[i - 1] + (int64_t)weights.delete_cost;
    }

    for (; first2 != last2; ++first2) {
        auto cache_iter = cache.begin();
        int64_t temp = *cache_iter;
        *cache_iter += (int64_t)weights.insert_cost;

        auto _first1 = first1;
        for (; _first1 != last1; ++_first1) {
            if (*_first1 != *first2) {
                temp = std::min({*cache_iter + (int64_t)weights.delete_cost,
                                 *(cache_iter + 1) + (int64_t)weights.insert_cost,
                                 temp + (int64_t)weights.replace_cost});
            }
            ++cache_iter;
            std::swap(*cache_iter, temp);
        }
    }

    return std::min(cache.back(), max + 1);
}

/**
 * @brief calculates the maximum possible Levenshtein distance based on
 * string lengths and weights
 */
template <typename InputIt1, typename InputIt2>
int64_t levenshtein_maximum(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                            LevenshteinWeightTable weights)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    int64_t max_dist = len1 * (int64_t)weights.delete_cost + len2 * (int64_t)weights.insert_cost;

    if (len1 >= len2) {
        max_dist = std::min(max_dist, len2 * (int64_t)weights.replace_cost +
                                          (len1 - len2) * (int64_t)weights.delete_cost);
    }
    else {
        max_dist = std::min(max_dist, len1 * (int64_t)weights.replace_cost +
                                          (len2 - len1) * (int64_t)weights.insert_cost);
    }

    return max_dist;
}

/**
 * @brief calculates the minimal possible Levenshtein distance based on
 * string lengths and weights
 */
template <typename InputIt1, typename InputIt2>
int64_t levenshtein_min_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                 LevenshteinWeightTable weights)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    return std::max((len1 - len2) * (int64_t)weights.delete_cost,
                    (len2 - len1) * (int64_t)weights.insert_cost);
}

template <typename InputIt1, typename InputIt2>
int64_t generalized_levenshtein_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                         InputIt2 last2, LevenshteinWeightTable weights,
                                         int64_t max)
{
    int64_t min_edits = levenshtein_min_distance(first1, last1, first2, last2, weights);
    if (min_edits > max) {
        return max + 1;
    }

    /* common affix does not effect Levenshtein distance */
    common::remove_common_affix(first1, last1, first2, last2);

    return generalized_levenshtein_wagner_fischer(first1, last1, first2, last2, weights, max);
}

template <typename InputIt1, typename InputIt2>
double generalized_levenshtein_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                                   InputIt2 last2, LevenshteinWeightTable weights,
                                                   double score_cutoff)
{
    int64_t maximum = levenshtein_maximum(first1, last1, first2, last2, weights);
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(maximum * score_cutoff));
    int64_t dist =
        generalized_levenshtein_distance(first1, last1, first2, last2, weights, cutoff_distance);
    double norm_dist = (maximum) ? dist / maximum : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : double(maximum);
}

template <typename InputIt1, typename InputIt2>
int64_t generalized_levenshtein_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                           InputIt2 last2, LevenshteinWeightTable weights,
                                           int64_t score_cutoff)
{
    int64_t maximum = levenshtein_maximum(first1, last1, first2, last2, weights);
    int64_t cutoff_distance = maximum - score_cutoff;
    int64_t dist =
        generalized_levenshtein_distance(first1, last1, first2, last2, weights, cutoff_distance);
    int64_t sim = maximum - dist;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename InputIt1, typename InputIt2>
double generalized_levenshtein_normalized_similarity(InputIt1 first1, InputIt1 last1,
                                                     InputIt2 first2, InputIt2 last2,
                                                     LevenshteinWeightTable weights,
                                                     double score_cutoff)
{
    double norm_dist = generalized_levenshtein_normalized_distance(first1, last1, first2, last2,
                                                                   weights, 1.0 - score_cutoff);
    double norm_sim = 1.0 - norm_dist;
    return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
}

} // namespace detail
} // namespace string_metric
} // namespace rapidfuzz
