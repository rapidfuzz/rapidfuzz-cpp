/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once

#include "Indel.hpp"

namespace rapidfuzz_reference {

template <typename InputIt1, typename InputIt2>
double ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, double score_cutoff = 0.0)
{
    return indel_similarity(first1, last1, first2, last2, score_cutoff / 100.0) * 100;
}

template <typename Sentence1, typename Sentence2>
double ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    return indel_similarity(s1, s2, score_cutoff / 100.0) * 100;
}

template <typename InputIt1, typename InputIt2>
double partial_ratio_impl(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                          double score_cutoff = 0.0)
{
    size_t len1 = static_cast<size_t>(std::distance(first1, last1));
    size_t len2 = static_cast<size_t>(std::distance(first2, last2));
    if (len1 == 0 && len2 == 0) return 100.0;

    if (len1 == 0 || len2 == 0) return 0.0;

    if (len1 > len2) return partial_ratio_impl(first2, last2, first1, last1, score_cutoff);

    double res = 0.0;
    for (ptrdiff_t i = -1 * (ptrdiff_t)len1; i < (ptrdiff_t)len2; i++) {
        ptrdiff_t start = std::max(ptrdiff_t(0), i);
        ptrdiff_t end = std::min(ptrdiff_t(len2), i + ptrdiff_t(len1));
        InputIt2 first2_ = first2 + start;
        InputIt2 last2_ = first2 + end;
        res = std::max(res, ratio(first1, last1, first2_, last2_, score_cutoff));
    }
    return res;
}

template <typename InputIt1, typename InputIt2>
double partial_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                     double score_cutoff = 0.0)
{
    size_t len1 = static_cast<size_t>(std::distance(first1, last1));
    size_t len2 = static_cast<size_t>(std::distance(first2, last2));
    if (len1 != len2) return partial_ratio_impl(first1, last1, first2, last2, score_cutoff);

    return std::max(partial_ratio_impl(first1, last1, first2, last2, score_cutoff),
                    partial_ratio_impl(first2, last2, first1, last1, score_cutoff));
}

template <typename Sentence1, typename Sentence2>
double partial_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    return partial_ratio(std::begin(s1), std::end(s1), std::begin(s2), std::end(s2), score_cutoff);
}

} // namespace rapidfuzz_reference
