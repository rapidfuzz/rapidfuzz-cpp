/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#include <rapidfuzz/distance/Jaro.hpp>

namespace rapidfuzz::detail {

template <typename InputIt1, typename InputIt2>
double jaro_winkler_similarity(Range<InputIt1> P, Range<InputIt2> T, double prefix_weight,
                               double score_cutoff)
{
    int64_t P_len = P.size();
    int64_t T_len = T.size();
    int64_t min_len = std::min(P_len, T_len);
    int64_t prefix = 0;
    int64_t max_prefix = std::min<int64_t>(min_len, 4);

    for (; prefix < max_prefix; ++prefix)
        if (T[prefix] != P[prefix]) break;

    double jaro_score_cutoff = score_cutoff;
    if (jaro_score_cutoff > 0.7) {
        double prefix_sim = static_cast<double>(prefix) * prefix_weight;

        if (prefix_sim >= 1.0)
            jaro_score_cutoff = 0.7;
        else
            jaro_score_cutoff = std::max(0.7, (prefix_sim - jaro_score_cutoff) / (prefix_sim - 1.0));
    }

    double Sim = jaro_similarity(P, T, jaro_score_cutoff);
    if (Sim > 0.7) Sim += static_cast<double>(prefix) * prefix_weight * (1.0 - Sim);

    return (Sim >= score_cutoff) ? Sim : 0;
}

template <typename InputIt1, typename InputIt2>
double jaro_winkler_similarity(const BlockPatternMatchVector& PM, Range<InputIt1> P, Range<InputIt2> T,
                               double prefix_weight, double score_cutoff)
{
    int64_t P_len = P.size();
    int64_t T_len = T.size();
    int64_t min_len = std::min(P_len, T_len);
    int64_t prefix = 0;
    int64_t max_prefix = std::min<int64_t>(min_len, 4);

    for (; prefix < max_prefix; ++prefix)
        if (T[prefix] != P[prefix]) break;

    double jaro_score_cutoff = score_cutoff;
    if (jaro_score_cutoff > 0.7) {
        double prefix_sim = static_cast<double>(prefix) * prefix_weight;

        if (prefix_sim >= 1.0)
            jaro_score_cutoff = 0.7;
        else
            jaro_score_cutoff = std::max(0.7, (prefix_sim - jaro_score_cutoff) / (prefix_sim - 1.0));
    }

    double Sim = jaro_similarity(PM, P, T, jaro_score_cutoff);
    if (Sim > 0.7) Sim += static_cast<double>(prefix) * prefix_weight * (1.0 - Sim);

    return (Sim >= score_cutoff) ? Sim : 0;
}

class JaroWinkler : public SimilarityBase<JaroWinkler, double, 0, 1, double> {
    friend SimilarityBase<JaroWinkler, double, 0, 1, double>;
    friend NormalizedMetricBase<JaroWinkler, double>;

    template <typename InputIt1, typename InputIt2>
    static double maximum(Range<InputIt1>, Range<InputIt2>, double) noexcept
    {
        return 1.0;
    }

    template <typename InputIt1, typename InputIt2>
    static double _similarity(Range<InputIt1> s1, Range<InputIt2> s2, double prefix_weight,
                              double score_cutoff, [[maybe_unused]] double score_hint)
    {
        return jaro_winkler_similarity(s1, s2, prefix_weight, score_cutoff);
    }
};

} // namespace rapidfuzz::detail
