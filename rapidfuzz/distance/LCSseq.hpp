/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once
#include <rapidfuzz/distance/LCSseq_impl.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::LCSseq::distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t lcs_seq_distance(const Sentence1& s1, const Sentence2& s2,
                         int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::LCSseq::distance(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                           int64_t score_cutoff = 0)
{
    return detail::LCSseq::similarity(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t lcs_seq_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff = 0)
{
    return detail::LCSseq::similarity(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double lcs_seq_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff = 1.0)
{
    return detail::LCSseq::normalized_distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double lcs_seq_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    return detail::LCSseq::normalized_distance(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double lcs_seq_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                     double score_cutoff = 0.0)
{
    return detail::LCSseq::normalized_similarity(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double lcs_seq_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    return detail::LCSseq::normalized_similarity(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
Editops lcs_seq_editops(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
    return detail::lcs_seq_editops(detail::Range(first1, last1), detail::Range(first2, last2));
}

template <typename Sentence1, typename Sentence2>
Editops lcs_seq_editops(const Sentence1& s1, const Sentence2& s2)
{
    return detail::lcs_seq_editops(detail::Range(s1), detail::Range(s2));
}

template <typename CharT1>
struct CachedLCSseq : detail::CachedSimilarityBase<CachedLCSseq<CharT1>> {
    template <typename Sentence1>
    explicit CachedLCSseq(const Sentence1& s1_) : CachedLCSseq(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt1>
    CachedLCSseq(InputIt1 first1, InputIt1 last1) : s1(first1, last1), PM(detail::Range(first1, last1))
    {}

private:
    friend detail::CachedSimilarityBase<CachedLCSseq<CharT1>>;
    friend detail::CachedNormalizedMetricBase<CachedLCSseq<CharT1>>;

    template <typename InputIt2>
    int64_t maximum(detail::Range<InputIt2> s2) const
    {
        return std::max(static_cast<ptrdiff_t>(s1.size()), s2.size());
    }

    template <typename InputIt2>
    int64_t _similarity(detail::Range<InputIt2> s2, int64_t score_cutoff) const
    {
        return detail::lcs_seq_similarity(PM, detail::Range(s1), s2, score_cutoff);
    }

    std::basic_string<CharT1> s1;
    detail::BlockPatternMatchVector PM;
};

template <typename Sentence1>
explicit CachedLCSseq(const Sentence1& s1_) -> CachedLCSseq<char_type<Sentence1>>;

template <typename InputIt1>
CachedLCSseq(InputIt1 first1, InputIt1 last1) -> CachedLCSseq<iter_value_t<InputIt1>>;

} // namespace rapidfuzz
