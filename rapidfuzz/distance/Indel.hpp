/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once

#include <limits>
#include <rapidfuzz/distance/Indel_impl.hpp>

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2>
int64_t indel_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                       int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::Indel::distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t indel_distance(const Sentence1& s1, const Sentence2& s2,
                       int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::Indel::distance(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
int64_t indel_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         int64_t score_cutoff = 0.0)
{
    return detail::Indel::similarity(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t indel_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff = 0.0)
{
    return detail::Indel::similarity(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                 double score_cutoff = 1.0)
{
    return detail::Indel::normalized_distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double indel_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    return detail::Indel::normalized_distance(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff = 0.0)
{
    return detail::Indel::normalized_similarity(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double indel_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    return detail::Indel::normalized_similarity(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
Editops indel_editops(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
    return lcs_seq_editops(first1, last1, first2, last2);
}

template <typename Sentence1, typename Sentence2>
Editops indel_editops(const Sentence1& s1, const Sentence2& s2)
{
    return lcs_seq_editops(s1, s2);
}

template <typename CharT1>
struct CachedIndel : public detail::CachedDistanceBase<CachedIndel<CharT1>> {
    template <typename Sentence1>
    explicit CachedIndel(const Sentence1& s1_) : CachedIndel(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt1>
    CachedIndel(InputIt1 first1, InputIt1 last1) : s1(first1, last1), PM(detail::Range(first1, last1))
    {}

private:
    friend detail::CachedDistanceBase<CachedIndel<CharT1>>;
    friend detail::CachedNormalizedMetricBase<CachedIndel<CharT1>>;

    template <typename InputIt2>
    int64_t maximum(detail::Range<InputIt2> s2) const
    {
        return static_cast<int64_t>(s1.size()) + s2.size();
    }

    template <typename InputIt2>
    int64_t _distance(detail::Range<InputIt2> s2, int64_t score_cutoff) const
    {
        return detail::indel_distance(PM, detail::Range(s1), s2, score_cutoff);
    }

    std::basic_string<CharT1> s1;
    detail::BlockPatternMatchVector PM;
};

template <typename Sentence1>
explicit CachedIndel(const Sentence1& s1_) -> CachedIndel<char_type<Sentence1>>;

template <typename InputIt1>
CachedIndel(InputIt1 first1, InputIt1 last1) -> CachedIndel<iter_value_t<InputIt1>>;

} // namespace rapidfuzz
