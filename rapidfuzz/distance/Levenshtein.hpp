/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once
#include <limits>
#include <rapidfuzz/details/Range.hpp>
#include <rapidfuzz/distance/Levenshtein_impl.hpp>

namespace rapidfuzz {

/**
 * @brief Calculates the minimum number of insertions, deletions, and substitutions
 * required to change one sequence into the other according to Levenshtein with custom
 * costs for insertion, deletion and substitution
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
 * @param weights
 *   The weights for the three operations in the form
 *   (insertion, deletion, substitution). Default is {1, 1, 1},
 *   which gives all three operations a weight of 1.
 * @param max
 *   Maximum Levenshtein distance between s1 and s2, that is
 *   considered as a result. If the distance is bigger than max,
 *   max + 1 is returned instead. Default is std::numeric_limits<int64_t>::max(),
 *   which deactivates this behaviour.
 *
 * @return returns the levenshtein distance between s1 and s2
 *
 * @remarks
 * @parblock
 * Depending on the input parameters different optimized implementation are used
 * to improve the performance. Worst-case performance is ``O(m * n)``.
 *
 * <b>Insertion = Deletion = Substitution:</b>
 *
 *    This is known as uniform Levenshtein distance and is the distance most commonly
 *    referred to as Levenshtein distance. The following implementation is used
 *    with a worst-case performance of ``O([N/64]M)``.
 *
 *    - if max is 0 the similarity can be calculated using a direct comparision,
 *      since no difference between the strings is allowed.  The time complexity of
 *      this algorithm is ``O(N)``.
 *
 *    - A common prefix/suffix of the two compared strings does not affect
 *      the Levenshtein distance, so the affix is removed before calculating the
 *      similarity.
 *
 *    - If max is <= 3 the mbleven algorithm is used. This algorithm
 *      checks all possible edit operations that are possible under
 *      the threshold `max`. The time complexity of this algorithm is ``O(N)``.
 *
 *    - If the length of the shorter string is <= 64 after removing the common affix
 *      Hyyrös' algorithm is used, which calculates the Levenshtein distance in
 *      parallel. The algorithm is described by @cite hyrro_2002. The time complexity of this
 *      algorithm is ``O(N)``.
 *
 *    - If the length of the shorter string is >= 64 after removing the common affix
 *      a blockwise implementation of Myers' algorithm is used, which calculates
 *      the Levenshtein distance in parallel (64 characters at a time).
 *      The algorithm is described by @cite myers_1999. The time complexity of this
 *      algorithm is ``O([N/64]M)``.
 *
 *
 * <b>Insertion = Deletion, Substitution >= Insertion + Deletion:</b>
 *
 *    Since every Substitution can be performed as Insertion + Deletion, this variant
 *    of the Levenshtein distance only uses Insertions and Deletions. Therefore this
 *    variant is often referred to as InDel-Distance.  The following implementation
 *    is used with a worst-case performance of ``O([N/64]M)``.
 *
 *    - if max is 0 the similarity can be calculated using a direct comparision,
 *      since no difference between the strings is allowed.  The time complexity of
 *      this algorithm is ``O(N)``.
 *
 *    - if max is 1 and the two strings have a similar length, the similarity can be
 *      calculated using a direct comparision aswell, since a substitution would cause
 *      a edit distance higher than max. The time complexity of this algorithm
 *      is ``O(N)``.
 *
 *    - A common prefix/suffix of the two compared strings does not affect
 *      the Levenshtein distance, so the affix is removed before calculating the
 *      similarity.
 *
 *    - If max is <= 4 the mbleven algorithm is used. This algorithm
 *      checks all possible edit operations that are possible under
 *      the threshold `max`. As a difference to the normal Levenshtein distance this
 *      algorithm can even be used up to a threshold of 4 here, since the higher weight
 *      of substitutions decreases the amount of possible edit operations.
 *      The time complexity of this algorithm is ``O(N)``.
 *
 *    - If the length of the shorter string is <= 64 after removing the common affix
 *      Hyyrös' lcs algorithm is used, which calculates the InDel distance in
 *      parallel. The algorithm is described by @cite hyrro_lcs_2004 and is extended with support
 *      for UTF32 in this implementation. The time complexity of this
 *      algorithm is ``O(N)``.
 *
 *    - If the length of the shorter string is >= 64 after removing the common affix
 *      a blockwise implementation of Hyyrös' lcs algorithm is used, which calculates
 *      the Levenshtein distance in parallel (64 characters at a time).
 *      The algorithm is described by @cite hyrro_lcs_2004. The time complexity of this
 *      algorithm is ``O([N/64]M)``.
 *
 * <b>Other weights:</b>
 *
 *   The implementation for other weights is based on Wagner-Fischer.
 *   It has a performance of ``O(N * M)`` and has a memory usage of ``O(N)``.
 *   Further details can be found in @cite wagner_fischer_1974.
 * @endparblock
 *
 * @par Examples
 * @parblock
 * Find the Levenshtein distance between two strings:
 * @code{.cpp}
 * // dist is 2
 * int64_t dist = levenshtein_distance("lewenstein", "levenshtein");
 * @endcode
 *
 * Setting a maximum distance allows the implementation to select
 * a more efficient implementation:
 * @code{.cpp}
 * // dist is 2
 * int64_t dist = levenshtein_distance("lewenstein", "levenshtein", {1, 1, 1}, 1);
 * @endcode
 *
 * It is possible to select different weights by passing a `weight` struct.
 * @code{.cpp}
 * // dist is 3
 * int64_t dist = levenshtein_distance("lewenstein", "levenshtein", {1, 1, 2});
 * @endcode
 * @endparblock
 */
template <typename InputIt1, typename InputIt2>
int64_t levenshtein_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                             LevenshteinWeightTable weights = {1, 1, 1},
                             int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::Levenshtein::distance(first1, last1, first2, last2, weights, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t levenshtein_distance(const Sentence1& s1, const Sentence2& s2,
                             LevenshteinWeightTable weights = {1, 1, 1},
                             int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::Levenshtein::distance(s1, s2, weights, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                               LevenshteinWeightTable weights = {1, 1, 1}, int64_t score_cutoff = 0.0)
{
    return detail::Levenshtein::similarity(first1, last1, first2, last2, weights, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t levenshtein_similarity(const Sentence1& s1, const Sentence2& s2,
                               LevenshteinWeightTable weights = {1, 1, 1}, int64_t score_cutoff = 0.0)
{
    return detail::Levenshtein::similarity(s1, s2, weights, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double levenshtein_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                       LevenshteinWeightTable weights = {1, 1, 1}, double score_cutoff = 1.0)
{
    return detail::Levenshtein::normalized_distance(first1, last1, first2, last2, weights, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double levenshtein_normalized_distance(const Sentence1& s1, const Sentence2& s2,
                                       LevenshteinWeightTable weights = {1, 1, 1}, double score_cutoff = 1.0)
{
    return detail::Levenshtein::normalized_distance(s1, s2, weights, score_cutoff);
}

/**
 * @brief Calculates a normalized levenshtein distance using custom
 * costs for insertion, deletion and substitution.
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
 * @param weights
 *   The weights for the three operations in the form
 *   (insertion, deletion, substitution). Default is {1, 1, 1},
 *   which gives all three operations a weight of 1.
 * @param score_cutoff
 *   Optional argument for a score threshold as a float between 0 and 1.0.
 *   For ratio < score_cutoff 0 is returned instead. Default is 0,
 *   which deactivates this behaviour.
 *
 * @return Normalized weighted levenshtein distance between s1 and s2
 *   as a double between 0 and 1.0
 *
 * @see levenshtein()
 *
 * @remarks
 * @parblock
 * The normalization of the Levenshtein distance is performed in the following way:
 *
 * \f{align*}{
 *   ratio &= \frac{distance(s1, s2)}{max_dist}
 * \f}
 * @endparblock
 *
 *
 * @par Examples
 * @parblock
 * Find the normalized Levenshtein distance between two strings:
 * @code{.cpp}
 * // ratio is 81.81818181818181
 * double ratio = normalized_levenshtein("lewenstein", "levenshtein");
 * @endcode
 *
 * Setting a score_cutoff allows the implementation to select
 * a more efficient implementation:
 * @code{.cpp}
 * // ratio is 0.0
 * double ratio = normalized_levenshtein("lewenstein", "levenshtein", {1, 1, 1}, 85.0);
 * @endcode
 *
 * It is possible to select different weights by passing a `weight` struct
 * @code{.cpp}
 * // ratio is 85.71428571428571
 * double ratio = normalized_levenshtein("lewenstein", "levenshtein", {1, 1, 2});
 * @endcode
 * @endparblock
 */
template <typename InputIt1, typename InputIt2>
double levenshtein_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                         LevenshteinWeightTable weights = {1, 1, 1},
                                         double score_cutoff = 0.0)
{
    return detail::Levenshtein::normalized_similarity(first1, last1, first2, last2, weights, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double levenshtein_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                         LevenshteinWeightTable weights = {1, 1, 1},
                                         double score_cutoff = 0.0)
{
    return detail::Levenshtein::normalized_similarity(s1, s2, weights, score_cutoff);
}

/**
 * @brief Return list of EditOp describing how to turn s1 into s2.
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
 *
 * @return Edit operations required to turn s1 into s2
 */
template <typename InputIt1, typename InputIt2>
Editops levenshtein_editops(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                            int64_t score_hint = std::numeric_limits<int64_t>::max())
{
    return detail::levenshtein_editops(detail::Range(first1, last1), detail::Range(first2, last2),
                                       score_hint);
}

template <typename Sentence1, typename Sentence2>
Editops levenshtein_editops(const Sentence1& s1, const Sentence2& s2,
                            int64_t score_hint = std::numeric_limits<int64_t>::max())
{
    return detail::levenshtein_editops(detail::Range(s1), detail::Range(s2), score_hint);
}

template <typename CharT1>
struct CachedLevenshtein : public detail::CachedDistanceBase<CachedLevenshtein<CharT1>> {
    template <typename Sentence1>
    explicit CachedLevenshtein(const Sentence1& s1_, LevenshteinWeightTable aWeights = {1, 1, 1})
        : CachedLevenshtein(detail::to_begin(s1_), detail::to_end(s1_), aWeights)
    {}

    template <typename InputIt1>
    CachedLevenshtein(InputIt1 first1, InputIt1 last1, LevenshteinWeightTable aWeights = {1, 1, 1})
        : s1(first1, last1), PM(detail::Range(first1, last1)), weights(aWeights)
    {}

private:
    friend detail::CachedDistanceBase<CachedLevenshtein<CharT1>>;
    friend detail::CachedNormalizedMetricBase<CachedLevenshtein<CharT1>>;

    template <typename InputIt2>
    int64_t maximum(detail::Range<InputIt2> s2) const
    {
        return detail::levenshtein_maximum(detail::Range(s1), s2, weights);
    }

    template <typename InputIt2>
    int64_t _distance(detail::Range<InputIt2> s2, int64_t score_cutoff) const
    {
        if (weights.insert_cost == weights.delete_cost) {
            /* when insertions + deletions operations are free there can not be any edit distance */
            if (weights.insert_cost == 0) return 0;

            /* uniform Levenshtein multiplied with the common factor */
            if (weights.insert_cost == weights.replace_cost) {
                // max can make use of the common divisor of the three weights
                int64_t new_max = detail::ceil_div(score_cutoff, weights.insert_cost);
                int64_t dist = detail::uniform_levenshtein_distance(PM, detail::Range(s1), s2, new_max);
                dist *= weights.insert_cost;

                return (dist <= score_cutoff) ? dist : score_cutoff + 1;
            }
            /*
             * when replace_cost >= insert_cost + delete_cost no substitutions are performed
             * therefore this can be implemented as InDel distance multiplied with the common factor
             */
            else if (weights.replace_cost >= weights.insert_cost + weights.delete_cost) {
                // max can make use of the common divisor of the three weights
                int64_t new_max = detail::ceil_div(score_cutoff, weights.insert_cost);
                int64_t dist = detail::indel_distance(PM, detail::Range(s1), s2, new_max);
                dist *= weights.insert_cost;
                return (dist <= score_cutoff) ? dist : score_cutoff + 1;
            }
        }

        return detail::generalized_levenshtein_distance(detail::Range(s1), s2, weights, score_cutoff);
    }

    std::basic_string<CharT1> s1;
    detail::BlockPatternMatchVector PM;
    LevenshteinWeightTable weights;
};

template <typename Sentence1>
explicit CachedLevenshtein(const Sentence1& s1_, LevenshteinWeightTable aWeights = {1, 1, 1})
    -> CachedLevenshtein<char_type<Sentence1>>;

template <typename InputIt1>
CachedLevenshtein(InputIt1 first1, InputIt1 last1, LevenshteinWeightTable aWeights = {1, 1, 1})
    -> CachedLevenshtein<iter_value_t<InputIt1>>;

} // namespace rapidfuzz
