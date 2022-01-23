/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once
#include <rapidfuzz/details/common.hpp>

#include <cmath>
#include <limits>

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2>
int64_t indel_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                       int64_t max = std::numeric_limits<int64_t>::max());

template <typename Sentence1, typename Sentence2>
int64_t indel_distance(const Sentence1& s1, const Sentence2& s2,
                       int64_t max = std::numeric_limits<int64_t>::max());

template <typename InputIt1, typename InputIt2>
int64_t indel_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         int64_t score_cutoff = 0.0);

template <typename Sentence1, typename Sentence2>
int64_t indel_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff = 0.0);

template <typename InputIt1, typename InputIt2>
double indel_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                 double score_cutoff = 1.0);

template <typename Sentence1, typename Sentence2>
double indel_normalized_distance(const Sentence1& s1, const Sentence2& s2,
                                 double score_cutoff = 1.0);

template <typename InputIt1, typename InputIt2>
double indel_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff = 0.0);

template <typename Sentence1, typename Sentence2>
double indel_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                   double score_cutoff = 0.0);

template <typename InputIt1, typename InputIt2>
Editops indel_editops(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2);

template <typename Sentence1, typename Sentence2>
Editops indel_editops(const Sentence1& s1, const Sentence2& s2);

template <typename CharT1>
struct CachedIndel {
    template <typename Sentence1>
    CachedIndel(const Sentence1& s1_)
        : s1(common::to_string(s1_)), PM(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt1>
    CachedIndel(InputIt1 first1, InputIt1 last1) : s1(first1, last1), PM(first1, last1)
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
CachedIndel(const Sentence1& s1_) -> CachedIndel<char_type<Sentence1>>;

template <typename InputIt1>
CachedIndel(InputIt1 first1, InputIt1 last1) -> CachedIndel<iterator_type<InputIt1>>;
#endif

} // namespace rapidfuzz

#include <rapidfuzz/distance/Indel.impl>