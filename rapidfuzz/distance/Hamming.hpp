/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once
#include <cmath>
#include <numeric>
#include <rapidfuzz/details/common.hpp>
#include <stdexcept>

namespace rapidfuzz {

/**
 * @brief Calculates the Hamming distance between two strings.
 *
 * @details
 * Both strings require a similar length
 *
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 * @param max
 *   Maximum Hamming distance between s1 and s2, that is
 *   considered as a result. If the distance is bigger than max,
 *   max + 1 is returned instead. Default is std::numeric_limits<size_t>::max(),
 *   which deactivates this behaviour.
 *
 * @return Hamming distance between s1 and s2
 */
template <typename InputIt1, typename InputIt2>
int64_t hamming_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         int64_t score_cutoff = std::numeric_limits<int64_t>::max());

template <typename Sentence1, typename Sentence2>
int64_t hamming_distance(const Sentence1& s1, const Sentence2& s2,
                         int64_t score_cutoff = std::numeric_limits<int64_t>::max());

template <typename InputIt1, typename InputIt2>
int64_t hamming_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                           int64_t score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
int64_t hamming_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff = 0);

template <typename InputIt1, typename InputIt2>
double hamming_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff = 1.0);

template <typename Sentence1, typename Sentence2>
double hamming_normalized_distance(const Sentence1& s1, const Sentence2& s2,
                                   double score_cutoff = 1.0);

/**
 * @brief Calculates a normalized hamming similarity
 *
 * @details
 * Both string require a similar length
 *
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 * @param score_cutoff
 *   Optional argument for a score threshold as a float between 0 and 1.0.
 *   For ratio < score_cutoff 0 is returned instead. Default is 0,
 *   which deactivates this behaviour.
 *
 * @return Normalized hamming distance between s1 and s2
 *   as a float between 0 and 1.0
 */
template <typename InputIt1, typename InputIt2>
double hamming_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                     InputIt2 last2, double score_cutoff = 0.0);

template <typename Sentence1, typename Sentence2>
double hamming_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                     double score_cutoff = 0.0);

template <typename CharT1>
struct CachedHamming {
    template <typename Sentence1>
    CachedHamming(const Sentence1& s1_) : s1(common::to_string(s1_))
    {}

    template <typename InputIt1>
    CachedHamming(InputIt1 first1, InputIt1 last1) : s1(first1, last1)
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
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedHamming(const Sentence1& s1_) -> CachedHamming<char_type<Sentence1>>;

template <typename InputIt1>
CachedHamming(InputIt1 first1, InputIt1 last1) -> CachedHamming<iterator_type<InputIt1>>;
#endif

/**@}*/

} // namespace rapidfuzz

#include <rapidfuzz/distance/Hamming.impl>
