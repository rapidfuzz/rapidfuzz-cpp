/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once
#include <rapidfuzz/details/Range.hpp>
#include <rapidfuzz/details/distance.hpp>
#include <stdexcept>

namespace rapidfuzz {
namespace detail {

class Hamming : public DistanceBase<Hamming> {
    friend DistanceBase<Hamming>;
    friend NormalizedMetricBase<Hamming>;

    template <typename InputIt1, typename InputIt2>
    static int64_t maximum(Range<InputIt1> s1, Range<InputIt2>)
    {
        return s1.size();
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t _distance(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_cutoff)
    {
        if (s1.size() != s2.size()) throw std::invalid_argument("Sequences are not the same length.");

        int64_t dist = 0;
        for (ptrdiff_t i = 0; i < s1.size(); ++i)
            dist += bool(s1[i] != s2[i]);

        return (dist <= score_cutoff) ? dist : score_cutoff + 1;
    }
};
} // namespace detail
} // namespace rapidfuzz
