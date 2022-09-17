/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once
#include <array>
#include <cmath>
#include <cstring>
#include <limits>
#include <rapidfuzz/details/Range.hpp>
#include <rapidfuzz/details/SplittedSentenceView.hpp>
#include <rapidfuzz/details/intrinsics.hpp>
#include <rapidfuzz/details/type_traits.hpp>
#include <rapidfuzz/details/types.hpp>
#include <vector>

namespace rapidfuzz::detail {

template <typename InputIt1, typename InputIt2, typename InputIt3>
struct DecomposedSet {
    SplittedSentenceView<InputIt1> difference_ab;
    SplittedSentenceView<InputIt2> difference_ba;
    SplittedSentenceView<InputIt3> intersection;
    DecomposedSet(SplittedSentenceView<InputIt1> diff_ab, SplittedSentenceView<InputIt2> diff_ba,
                  SplittedSentenceView<InputIt3> intersect)
        : difference_ab(std::move(diff_ab)),
          difference_ba(std::move(diff_ba)),
          intersection(std::move(intersect))
    {}
};

/**
 * @defgroup Common Common
 * Common utilities shared among multiple functions
 * @{
 */

static inline double NormSim_to_NormDist(double score_cutoff, double imprecision = 0.00001)
{
    return std::min(1.0, 1.0 - score_cutoff + imprecision);
}

static inline void assume(bool b)
{
#if defined(__clang__)
    __builtin_assume(b);
#elif defined(__GNUC__) || defined(__GNUG__)
    if (!b) __builtin_unreachable();
#elif defined(_MSC_VER)
    __assume(b);
#endif
}

template <typename InputIt1, typename InputIt2>
DecomposedSet<InputIt1, InputIt2, InputIt1> set_decomposition(SplittedSentenceView<InputIt1> a,
                                                              SplittedSentenceView<InputIt2> b);

constexpr double result_cutoff(double result, double score_cutoff)
{
    return (result >= score_cutoff) ? result : 0;
}

template <int Max = 1>
constexpr double norm_distance(int64_t dist, int64_t lensum, double score_cutoff = 0)
{
    double max = static_cast<double>(Max);
    return result_cutoff((lensum > 0) ? (max - max * static_cast<double>(dist) / static_cast<double>(lensum))
                                      : max,
                         score_cutoff);
}

template <int Max = 1>
static inline int64_t score_cutoff_to_distance(double score_cutoff, int64_t lensum)
{
    return static_cast<int64_t>(std::ceil(static_cast<double>(lensum) * (1.0 - score_cutoff / Max)));
}

template <typename InputIt1, typename InputIt2>
StringAffix remove_common_affix(Range<InputIt1>& s1, Range<InputIt2>& s2);

template <typename InputIt1, typename InputIt2>
size_t remove_common_prefix(Range<InputIt1>& s1, Range<InputIt2>& s2);

template <typename InputIt1, typename InputIt2>
size_t remove_common_suffix(Range<InputIt1>& s1, Range<InputIt2>& s2);

template <typename InputIt, typename CharT = iter_value_t<InputIt>>
SplittedSentenceView<InputIt> sorted_split(InputIt first, InputIt last);

/**@}*/

} // namespace rapidfuzz::detail

#include <rapidfuzz/details/common_impl.hpp>
