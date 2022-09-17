/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once
#include <cmath>
#include <numeric>
#include <rapidfuzz/details/common.hpp>
#include <rapidfuzz/distance/OSA_impl.hpp>

namespace rapidfuzz {

/**
 * @brief Calculates the optimal string alignment (OSA) distance between two strings.
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
 *   Maximum OSA distance between s1 and s2, that is
 *   considered as a result. If the distance is bigger than max,
 *   max + 1 is returned instead. Default is std::numeric_limits<size_t>::max(),
 *   which deactivates this behaviour.
 *
 * @return OSA distance between s1 and s2
 */
template <typename InputIt1, typename InputIt2>
int64_t osa_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::OSA::distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t osa_distance(const Sentence1& s1, const Sentence2& s2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::OSA::distance(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
int64_t osa_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                       int64_t score_cutoff = 0)
{
    return detail::OSA::similarity(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t osa_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff = 0)
{
    return detail::OSA::similarity(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double osa_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                               double score_cutoff = 1.0)
{
    return detail::OSA::normalized_distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double osa_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    return detail::OSA::normalized_distance(s1, s2, score_cutoff);
}

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
double osa_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                 double score_cutoff = 0.0)
{
    return detail::OSA::normalized_similarity(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double osa_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    return detail::OSA::normalized_similarity(s1, s2, score_cutoff);
}

template <typename CharT1>
struct CachedOSA : public detail::CachedDistanceBase<CachedOSA<CharT1>> {
    template <typename Sentence1>
    explicit CachedOSA(const Sentence1& s1_) : CachedOSA(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt1>
    CachedOSA(InputIt1 first1, InputIt1 last1) : s1(first1, last1), PM(detail::Range(first1, last1))
    {}

private:
    friend detail::CachedDistanceBase<CachedOSA<CharT1>>;
    friend detail::CachedNormalizedMetricBase<CachedOSA<CharT1>>;

    template <typename InputIt2>
    int64_t maximum(detail::Range<InputIt2> s2) const
    {
        return std::max(static_cast<ptrdiff_t>(s1.size()), s2.size());
    }

    template <typename InputIt2>
    int64_t _distance(detail::Range<InputIt2> s2, int64_t score_cutoff) const
    {
        int64_t res;
        if (s1.empty())
            res = s2.size();
        else if (s2.empty())
            res = static_cast<int64_t>(s1.size());
        else if (s1.size() < 64)
            res = detail::osa_hyrroe2003(PM, detail::Range(s1), s2, score_cutoff);
        else
            res = detail::osa_hyrroe2003_block(PM, detail::Range(s1), s2, score_cutoff);

        return (res <= score_cutoff) ? res : score_cutoff + 1;
    }

    std::basic_string<CharT1> s1;
    detail::BlockPatternMatchVector PM;
};

template <typename Sentence1>
CachedOSA(const Sentence1& s1_) -> CachedOSA<char_type<Sentence1>>;

template <typename InputIt1>
CachedOSA(InputIt1 first1, InputIt1 last1) -> CachedOSA<iter_value_t<InputIt1>>;
/**@}*/

} // namespace rapidfuzz
