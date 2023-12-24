/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once
#include "Jaro.hpp"

namespace rapidfuzz_reference {

template <typename InputIt1, typename InputIt2,
          typename = std::enable_if_t<!std::is_same_v<InputIt2, double>>>
double jaro_winkler_similarity(InputIt1 P_first, InputIt1 P_last, InputIt2 T_first, InputIt2 T_last,
                               double prefix_weight = 0.1, double score_cutoff = 0.0)
{
    int64_t min_len = std::min(std::distance(P_first, P_last), std::distance(T_first, T_last));
    size_t max_prefix = std::min(static_cast<size_t>(min_len), size_t(4));

    size_t prefix = 0;
    for (; prefix < max_prefix; ++prefix)
        if (T_first[static_cast<ptrdiff_t>(prefix)] != P_first[static_cast<ptrdiff_t>(prefix)]) break;

    double Sim = jaro_similarity(P_first, P_last, T_first, T_last);
    if (Sim > 0.7) Sim += static_cast<double>(prefix) * prefix_weight * (1.0 - Sim);

    return (Sim >= score_cutoff) ? Sim : 0;
}

template <typename Sentence1, typename Sentence2>
double jaro_winkler_similarity(const Sentence1& s1, const Sentence2& s2, double prefix_weight = 0.1,
                               double score_cutoff = 0.0)
{
    return jaro_winkler_similarity(std::begin(s1), std::end(s1), std::begin(s2), std::end(s2), prefix_weight,
                                   score_cutoff);
}

} /* namespace rapidfuzz_reference */
