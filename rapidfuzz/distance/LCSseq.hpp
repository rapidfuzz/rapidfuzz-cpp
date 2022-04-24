/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once
#include <rapidfuzz/details/common.hpp>

#include <cmath>
#include <limits>

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         int64_t max = std::numeric_limits<int64_t>::max());

template <typename Sentence1, typename Sentence2>
int64_t lcs_seq_distance(const Sentence1& s1, const Sentence2& s2,
                         int64_t max = std::numeric_limits<int64_t>::max());

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                           int64_t score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
int64_t lcs_seq_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff = 0);

template <typename InputIt1, typename InputIt2>
double lcs_seq_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff = 1.0);

template <typename Sentence1, typename Sentence2>
double lcs_seq_normalized_distance(const Sentence1& s1, const Sentence2& s2,
                                   double score_cutoff = 1.0);

template <typename InputIt1, typename InputIt2>
double lcs_seq_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                     InputIt2 last2, double score_cutoff = 0.0);

template <typename Sentence1, typename Sentence2>
double lcs_seq_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                     double score_cutoff = 0.0);

template <typename InputIt1, typename InputIt2>
Editops lcs_seq_editops(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2);

template <typename Sentence1, typename Sentence2>
Editops lcs_seq_editops(const Sentence1& s1, const Sentence2& s2);

template <typename CharT1>
struct CachedLCSseq {
    template <typename Sentence1>
    CachedLCSseq(const Sentence1& s1_) : CachedLCSseq(common::to_begin(s1_), common::to_end(s1_))
    {}

    template <typename InputIt1>
    CachedLCSseq(InputIt1 first1, InputIt1 last1) : s1(first1, last1), PM(first1, last1)
    {}

    template <typename InputIt2>
    int64_t distance(InputIt2 first2, InputIt2 last2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const;

    template <typename Sentence2>
    int64_t distance(const Sentence2& s2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const;

    template <typename InputIt2>
    int64_t similarity(InputIt2 first2, InputIt2 last2, int64_t score_cutoff = 0) const;

    template <typename Sentence2>
    int64_t similarity(const Sentence2& s2, int64_t score_cutoff = 0) const;

    template <typename InputIt2>
    double normalized_distance(InputIt2 first2, InputIt2 last2, double score_cutoff = 1.0) const;

    template <typename Sentence2>
    double normalized_distance(const Sentence2& s2, double score_cutoff = 1.0) const;

    template <typename InputIt2>
    double normalized_similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0.0) const;

    template <typename Sentence2>
    double normalized_similarity(const Sentence2& s2, double score_cutoff = 0.0) const;

private:
    std::basic_string<CharT1> s1;
    common::BlockPatternMatchVector PM;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedLCSseq(const Sentence1& s1_) -> CachedLCSseq<char_type<Sentence1>>;

template <typename InputIt1>
CachedLCSseq(InputIt1 first1, InputIt1 last1) -> CachedLCSseq<iter_value_t<InputIt1>>;
#endif

} // namespace rapidfuzz

#include <rapidfuzz/distance/LCSseq.impl>