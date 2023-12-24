/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once

#include "Levenshtein.hpp"
#include <limits>

namespace rapidfuzz_reference {

template <typename InputIt1, typename InputIt2>
size_t indel_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                      size_t score_cutoff = std::numeric_limits<size_t>::max())
{
    return levenshtein_distance(first1, last1, first2, last2, {1, 1, 2}, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
size_t indel_distance(const Sentence1& s1, const Sentence2& s2,
                      size_t score_cutoff = std::numeric_limits<size_t>::max())
{
    return levenshtein_distance(s1, s2, {1, 1, 2}, score_cutoff);
}

} // namespace rapidfuzz_reference
