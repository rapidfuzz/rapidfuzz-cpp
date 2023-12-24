/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once
#include <limits>
#include <stdexcept>

namespace rapidfuzz_reference {

template <typename InputIt1, typename InputIt2>
size_t hamming_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                        size_t score_cutoff = std::numeric_limits<size_t>::max())
{
    ptrdiff_t len1 = std::distance(first1, last1);
    ptrdiff_t len2 = std::distance(first2, last2);
    if (len1 != len2) throw std::invalid_argument("Sequences are not the same length.");

    size_t dist = 0;
    for (ptrdiff_t i = 0; i < len1; ++i)
        dist += bool(first1[i] != first2[i]);

    return (dist <= score_cutoff) ? dist : score_cutoff + 1;
}

template <typename Sentence1, typename Sentence2>
size_t hamming_distance(const Sentence1& s1, const Sentence2& s2,
                        size_t score_cutoff = std::numeric_limits<size_t>::max())
{
    return hamming_similarity(std::begin(s1), std::end(s1), std::begin(s2), std::end(s2), score_cutoff);
}

} // namespace rapidfuzz_reference
