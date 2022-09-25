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

#ifdef RAPIDFUZZ_SIMD
namespace experimental {
template <int MaxLen>
struct MultiLevenshtein {
private:
    constexpr static size_t get_vec_size()
    {
#    ifdef RAPIDFUZZ_AVX2
        using namespace detail::simd_avx2;
#    else
        using namespace detail::simd_sse2;
#    endif
        if constexpr (MaxLen <= 8)
            return native_simd<uint8_t>::size();
        else if constexpr (MaxLen <= 16)
            return native_simd<uint16_t>::size();
        else if constexpr (MaxLen <= 32)
            return native_simd<uint32_t>::size();
        else if constexpr (MaxLen <= 64)
            return native_simd<uint64_t>::size();

        static_assert(MaxLen <= 64);
    }

    constexpr static size_t find_block_count(size_t count)
    {
        size_t vec_size = get_vec_size();
        size_t simd_vec_count = detail::ceil_div(count, vec_size);
        return detail::ceil_div(simd_vec_count * vec_size * MaxLen, 64);
    }

public:
    MultiLevenshtein(size_t count, LevenshteinWeightTable aWeights = {1, 1, 1})
        : input_count(count), pos(0), PM(find_block_count(count) * 64), weights(aWeights)
    {
        str_lens.resize(result_count());
        if (weights.delete_cost != 1 || weights.insert_cost != 1 || weights.replace_cost > 2)
            throw std::invalid_argument("unsupported weights");
    }

    /**
     * @brief get minimum size required for result vectors passed into
     * - distance
     * - similarity
     * - normalized_distance
     * - normalized_similarity
     *
     * @return minimum vector size
     */
    size_t result_count() const
    {
        size_t vec_size = get_vec_size();
        size_t simd_vec_count = detail::ceil_div(input_count, vec_size);
        return simd_vec_count * vec_size;
    }

    template <typename Sentence1>
    void insert(const Sentence1& s1_)
    {
        insert(detail::to_begin(s1_), detail::to_end(s1_));
    }

    template <typename InputIt1>
    void insert(InputIt1 first1, InputIt1 last1)
    {
        auto len = std::distance(first1, last1);
        int block_pos = static_cast<int>((pos * MaxLen) % 64);
        auto block = (pos * MaxLen) / 64;
        assert(len <= MaxLen);

        if (pos >= input_count) throw std::invalid_argument("out of bounds insert");

        str_lens[pos] = static_cast<size_t>(len);

        for (; first1 != last1; ++first1) {
            PM.insert(block, *first1, block_pos);
            block_pos++;
        }
        pos++;
    }

    template <typename InputIt2>
    void distance(int64_t* scores, size_t score_count, InputIt2 first2, InputIt2 last2,
                  int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const
    {
        distance(scores, score_count, detail::Range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    void distance(int64_t* scores, size_t score_count, const Sentence2& s2,
                  int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const
    {
        if (score_count < result_count())
            throw std::invalid_argument("scores has to have >= result_count() elements");

        detail::Range s2_(s2);
        if constexpr (MaxLen == 8)
            detail::levenshtein_hyrroe2003_simd<uint8_t>(scores, PM, str_lens, s2_, score_cutoff);
        else if constexpr (MaxLen == 16)
            detail::levenshtein_hyrroe2003_simd<uint16_t>(scores, PM, str_lens, s2_, score_cutoff);
        else if constexpr (MaxLen == 32)
            detail::levenshtein_hyrroe2003_simd<uint32_t>(scores, PM, str_lens, s2_, score_cutoff);
        else if constexpr (MaxLen == 64)
            detail::levenshtein_hyrroe2003_simd<uint64_t>(scores, PM, str_lens, s2_, score_cutoff);
    }

    template <typename InputIt2>
    void similarity(int64_t* scores, size_t score_count, InputIt2 first2, InputIt2 last2,
                    int64_t score_cutoff = 0) const
    {
        similarity(scores, score_count, detail::Range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    void similarity(int64_t* scores, size_t score_count, const Sentence2& s2, int64_t score_cutoff = 0) const
    {
        detail::Range s2_(s2);
        distance(scores, score_count, s2_);

        for (size_t i = 0; i < input_count; ++i) {
            int64_t maximum = detail::levenshtein_maximum(str_lens[i], s2_.size(), weights);
            int64_t sim = maximum - scores[i];
            scores[i] = (sim >= score_cutoff) ? sim : 0;
        }
    }

    template <typename InputIt2>
    void normalized_distance(double* scores, size_t score_count, InputIt2 first2, InputIt2 last2,
                             double score_cutoff = 1.0) const
    {
        normalized_distance(scores, score_count, detail::Range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    void normalized_distance(double* scores, size_t score_count, const Sentence2& s2,
                             double score_cutoff = 1.0) const
    {
        if (score_count < result_count())
            throw std::invalid_argument("scores has to have >= result_count() elements");

        // reinterpretation only works when the types have the same size
        int64_t* scores_i64 = nullptr;
        if constexpr (sizeof(double) == sizeof(int64_t))
            scores_i64 = reinterpret_cast<int64_t*>(scores);
        else
            scores_i64 = new int64_t[result_count()];

        detail::Range s2_(s2);
        distance(scores_i64, result_count(), s2_);

        for (size_t i = 0; i < input_count; ++i) {
            int64_t maximum = detail::levenshtein_maximum(str_lens[i], s2_.size(), weights);
            double norm_dist = static_cast<double>(scores_i64[i]) / static_cast<double>(maximum);
            scores[i] = (norm_dist <= score_cutoff) ? norm_dist : 1.0;
        }

        if constexpr (sizeof(double) != sizeof(int64_t)) delete[] scores_i64;
    }

    template <typename InputIt2>
    void normalized_similarity(double* scores, size_t score_count, InputIt2 first2, InputIt2 last2,
                               double score_cutoff = 0.0) const
    {
        normalized_similarity(scores, score_count, detail::Range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    void normalized_similarity(double* scores, size_t score_count, const Sentence2& s2,
                               double score_cutoff = 0.0) const
    {
        normalized_distance(scores, score_count, s2);

        for (size_t i = 0; i < input_count; ++i) {
            double norm_sim = 1.0 - scores[i];
            scores[i] = (norm_sim >= score_cutoff) ? norm_sim : 0.0;
        }
    }

private:
    size_t input_count;
    size_t pos;
    detail::BlockPatternMatchVector PM;
    std::vector<size_t> str_lens;
    LevenshteinWeightTable weights;
};
} /* namespace experimental */
#endif

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
        return detail::levenshtein_maximum(static_cast<ptrdiff_t>(s1.size()), s2.size(), weights);
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
