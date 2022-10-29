/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once

#include "rapidfuzz/details/Range.hpp"
#include <limits>
#include <rapidfuzz/distance/Jaro_impl.hpp>

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2>
double jaro_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                     double score_cutoff = 1.0)
{
    return detail::Jaro::distance(first1, last1, first2, last2, score_cutoff, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double jaro_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    return detail::Jaro::distance(s1, s2, score_cutoff, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double jaro_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                       double score_cutoff = 0.0)
{
    return detail::Jaro::similarity(first1, last1, first2, last2, score_cutoff, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double jaro_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    return detail::Jaro::similarity(s1, s2, score_cutoff, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double jaro_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                double score_cutoff = 1.0)
{
    return detail::Jaro::normalized_distance(first1, last1, first2, last2, score_cutoff, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double jaro_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    return detail::Jaro::normalized_distance(s1, s2, score_cutoff, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double jaro_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                  double score_cutoff = 0.0)
{
    return detail::Jaro::normalized_similarity(first1, last1, first2, last2, score_cutoff, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double jaro_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    return detail::Jaro::normalized_similarity(s1, s2, score_cutoff, score_cutoff);
}

template <typename CharT1>
struct CachedJaro : public detail::CachedSimilarityBase<CachedJaro<CharT1>, double, 0, 1> {
    template <typename Sentence1>
    explicit CachedJaro(const Sentence1& s1_) : CachedJaro(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt1>
    CachedJaro(InputIt1 first1, InputIt1 last1) : s1(first1, last1), PM(detail::Range(first1, last1))
    {}

private:
    friend detail::CachedSimilarityBase<CachedJaro<CharT1>, double, 0, 1>;
    friend detail::CachedNormalizedMetricBase<CachedJaro<CharT1>>;

    template <typename InputIt2>
    double maximum(detail::Range<InputIt2>) const
    {
        return 1.0;
    }

    template <typename InputIt2>
    double _similarity(detail::Range<InputIt2> s2, double score_cutoff,
                       [[maybe_unused]] double score_hint) const
    {
        return detail::jaro_similarity(PM, detail::Range(s1), s2, score_cutoff);
    }

    std::basic_string<CharT1> s1;
    detail::BlockPatternMatchVector PM;
};

template <typename Sentence1>
explicit CachedJaro(const Sentence1& s1_) -> CachedJaro<char_type<Sentence1>>;

template <typename InputIt1>
CachedJaro(InputIt1 first1, InputIt1 last1) -> CachedJaro<iter_value_t<InputIt1>>;

} // namespace rapidfuzz
