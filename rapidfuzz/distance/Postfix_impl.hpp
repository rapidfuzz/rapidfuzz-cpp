/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once
#include "rapidfuzz/details/common.hpp"
#include <rapidfuzz/details/Range.hpp>
#include <rapidfuzz/details/distance.hpp>

namespace rapidfuzz::detail {

class Postfix : public SimilarityBase<Postfix, int64_t, 0, std::numeric_limits<int64_t>::max()> {
    friend SimilarityBase<Postfix, int64_t, 0, std::numeric_limits<int64_t>::max()>;
    friend NormalizedMetricBase<Postfix>;

    template <typename InputIt1, typename InputIt2>
    static int64_t maximum(Range<InputIt1> s1, Range<InputIt2> s2)
    {
        return std::max(s1.size(), s2.size());
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t _similarity(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_cutoff,
                               [[maybe_unused]] int64_t score_hint)
    {
        int64_t dist = static_cast<int64_t>(remove_common_suffix(s1, s2));
        return (dist >= score_cutoff) ? dist : 0;
    }
};

} // namespace rapidfuzz::detail
