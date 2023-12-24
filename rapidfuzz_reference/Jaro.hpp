/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once
#include <algorithm>
#include <cstddef>
#include <vector>

namespace rapidfuzz_reference {

template <typename InputIt1, typename InputIt2>
double jaro_similarity(InputIt1 P_first, InputIt1 P_last, InputIt2 T_first, InputIt2 T_last,
                       double score_cutoff = 0.0)
{
    size_t P_len = static_cast<size_t>(std::distance(P_first, P_last));
    size_t T_len = static_cast<size_t>(std::distance(T_first, T_last));

    if (score_cutoff > 1.0) return 0.0;

    if (!P_len || !T_len) return double(!P_len && !T_len);

    std::vector<int> P_flag(P_len + 1);
    std::vector<int> T_flag(T_len + 1);

    size_t Bound = std::max(P_len, T_len) / 2;
    if (Bound > 0) Bound--;

    size_t CommonChars = 0;
    for (size_t i = 0; i < T_len; i++) {
        size_t lowlim = (i >= Bound) ? i - Bound : 0;
        size_t hilim = (i + Bound <= P_len - 1) ? (i + Bound) : P_len - 1;
        for (size_t j = lowlim; j <= hilim; j++) {
            if (!P_flag[j] && (P_first[static_cast<ptrdiff_t>(j)] == T_first[static_cast<ptrdiff_t>(i)])) {
                T_flag[i] = 1;
                P_flag[j] = 1;
                CommonChars++;
                break;
            }
        }
    }

    // Count the number of transpositions
    size_t Transpositions = 0;
    size_t k = 0;
    for (size_t i = 0; i < T_len; i++) {
        if (T_flag[i]) {
            size_t j = k;
            for (; j < P_len; j++) {
                if (P_flag[j]) {
                    k = j + 1;
                    break;
                }
            }
            if (T_first[static_cast<ptrdiff_t>(i)] != P_first[static_cast<ptrdiff_t>(j)]) Transpositions++;
        }
    }

    Transpositions /= 2;
    double Sim = 0;
    Sim += static_cast<double>(CommonChars) / static_cast<double>(P_len);
    Sim += static_cast<double>(CommonChars) / static_cast<double>(T_len);
    Sim += (static_cast<double>(CommonChars) - static_cast<double>(Transpositions)) /
           static_cast<double>(CommonChars);
    Sim /= 3.0;
    return (Sim >= score_cutoff) ? Sim : 0;
}

template <typename Sentence1, typename Sentence2>
double jaro_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    return jaro_similarity(std::begin(s1), std::end(s1), std::begin(s2), std::end(s2), score_cutoff);
}

} /* namespace rapidfuzz_reference */
