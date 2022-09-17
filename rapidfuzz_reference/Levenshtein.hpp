/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace rapidfuzz_reference {

struct LevenshteinWeightTable {
    int64_t insert_cost;
    int64_t delete_cost;
    int64_t replace_cost;
};

static inline void assume(bool b)
{
#if defined(__clang__)
    __builtin_assume(b);
#elif defined(__GNUC__) || defined(__GNUG__)
    if (!b) __builtin_unreachable();
#elif defined(_MSC_VER)
    __assume(b);
#endif
}

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                             LevenshteinWeightTable weights = {1, 1, 1},
                             int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    ptrdiff_t len1 = std::distance(first1, last1);
    ptrdiff_t len2 = std::distance(first2, last2);

    size_t cache_size = static_cast<size_t>(len1) + 1;
    std::vector<int64_t> cache(cache_size);
    assume(cache_size != 0);

    cache[0] = 0;
    for (size_t i = 1; i < cache_size; ++i)
        cache[i] = cache[i - 1] + weights.delete_cost;

    for (ptrdiff_t s2_pos = 0; s2_pos < len2; ++s2_pos) {
        auto cache_iter = cache.begin();
        int64_t temp = *cache_iter;
        *cache_iter += weights.insert_cost;

        for (ptrdiff_t s1_pos = 0; s1_pos < len1; ++s1_pos) {
            if (first1[s1_pos] != first2[s2_pos])
                temp = std::min({*cache_iter + weights.delete_cost, *(cache_iter + 1) + weights.insert_cost,
                                 temp + weights.replace_cost});
            ++cache_iter;
            std::swap(*cache_iter, temp);
        }
    }

    int64_t dist = cache.back();
    return (dist <= score_cutoff) ? dist : score_cutoff + 1;
}

template <typename Sentence1, typename Sentence2>
int64_t levenshtein_distance(const Sentence1& s1, const Sentence2& s2,
                             LevenshteinWeightTable weights = {1, 1, 1},
                             int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return levenshtein_distance(std::begin(s1), std::end(s1), std::begin(s2), std::end(s2), weights,
                                score_cutoff);
}

} // namespace rapidfuzz_reference
