/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once
#include "Indel.hpp"

#include <cmath>
#include <limits>

namespace rapidfuzz_reference {

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                           int64_t score_cutoff = 0)
{
    int64_t maximum = std::distance(first1, last1) + std::distance(first2, last2);
    int64_t dist = indel_distance(first1, last1, first2, last2);
    int64_t sim = (maximum - dist) / 2;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename Sentence1, typename Sentence2>
int64_t lcs_seq_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff = 0)
{
    return lcs_seq_similarity(std::begin(s1), std::end(s1), std::begin(s2), std::end(s2), score_cutoff);
}

} // namespace rapidfuzz_reference
