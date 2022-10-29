/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once

#include "rapidfuzz/details/Range.hpp"
#include <limits>
#include <rapidfuzz/distance/JaroWinkler_impl.hpp>

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2,
          typename = std::enable_if_t<!std::is_same_v<InputIt2, double>>>
double jaro_winkler_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                             double prefix_weight = 0.1, double score_cutoff = 1.0)
{
    return detail::JaroWinkler::distance(first1, last1, first2, last2, prefix_weight, score_cutoff,
                                         score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double jaro_winkler_distance(const Sentence1& s1, const Sentence2& s2, double prefix_weight = 0.1,
                             double score_cutoff = 1.0)
{
    return detail::JaroWinkler::distance(s1, s2, prefix_weight, score_cutoff, score_cutoff);
}

template <typename InputIt1, typename InputIt2,
          typename = std::enable_if_t<!std::is_same_v<InputIt2, double>>>
double jaro_winkler_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                               double prefix_weight = 0.1, double score_cutoff = 0.0)
{
    return detail::JaroWinkler::similarity(first1, last1, first2, last2, prefix_weight, score_cutoff,
                                           score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double jaro_winkler_similarity(const Sentence1& s1, const Sentence2& s2, double prefix_weight = 0.1,
                               double score_cutoff = 0.0)
{
    return detail::JaroWinkler::similarity(s1, s2, prefix_weight, score_cutoff, score_cutoff);
}

template <typename InputIt1, typename InputIt2,
          typename = std::enable_if_t<!std::is_same_v<InputIt2, double>>>
double jaro_winkler_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                        double prefix_weight = 0.1, double score_cutoff = 1.0)
{
    return detail::JaroWinkler::normalized_distance(first1, last1, first2, last2, prefix_weight, score_cutoff,
                                                    score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double jaro_winkler_normalized_distance(const Sentence1& s1, const Sentence2& s2, double prefix_weight = 0.1,
                                        double score_cutoff = 1.0)
{
    return detail::JaroWinkler::normalized_distance(s1, s2, prefix_weight, score_cutoff, score_cutoff);
}

template <typename InputIt1, typename InputIt2,
          typename = std::enable_if_t<!std::is_same_v<InputIt2, double>>>
double jaro_winkler_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                          double prefix_weight = 0.1, double score_cutoff = 0.0)
{
    return detail::JaroWinkler::normalized_similarity(first1, last1, first2, last2, prefix_weight,
                                                      score_cutoff, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double jaro_winkler_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                          double prefix_weight = 0.1, double score_cutoff = 0.0)
{
    return detail::JaroWinkler::normalized_similarity(s1, s2, prefix_weight, score_cutoff, score_cutoff);
}

template <typename CharT1>
struct CachedJaroWinkler : public detail::CachedSimilarityBase<CachedJaroWinkler<CharT1>, double, 0, 1> {
    template <typename Sentence1>
    explicit CachedJaroWinkler(const Sentence1& s1_, double _prefix_weight = 0.1)
        : CachedJaroWinkler(detail::to_begin(s1_), detail::to_end(s1_), _prefix_weight)
    {}

    template <typename InputIt1>
    CachedJaroWinkler(InputIt1 first1, InputIt1 last1, double _prefix_weight = 0.1)
        : prefix_weight(_prefix_weight), s1(first1, last1), PM(detail::Range(first1, last1))
    {}

private:
    friend detail::CachedSimilarityBase<CachedJaroWinkler<CharT1>, double, 0, 1>;
    friend detail::CachedNormalizedMetricBase<CachedJaroWinkler<CharT1>>;

    template <typename InputIt2>
    double maximum(detail::Range<InputIt2>) const
    {
        return 1.0;
    }

    template <typename InputIt2>
    double _similarity(detail::Range<InputIt2> s2, double score_cutoff,
                       [[maybe_unused]] double score_hint) const
    {
        return detail::jaro_winkler_similarity(PM, detail::Range(s1), s2, prefix_weight, score_cutoff);
    }

    double prefix_weight;
    std::basic_string<CharT1> s1;
    detail::BlockPatternMatchVector PM;
};

template <typename Sentence1>
explicit CachedJaroWinkler(const Sentence1& s1_, double _prefix_weight = 0.1)
    -> CachedJaroWinkler<char_type<Sentence1>>;

template <typename InputIt1>
CachedJaroWinkler(InputIt1 first1, InputIt1 last1, double _prefix_weight = 0.1)
    -> CachedJaroWinkler<iter_value_t<InputIt1>>;

} // namespace rapidfuzz
