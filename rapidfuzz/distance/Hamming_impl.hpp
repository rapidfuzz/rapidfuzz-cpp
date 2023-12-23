/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once
#include <rapidfuzz/details/Range.hpp>
#include <rapidfuzz/details/distance.hpp>
#include <stdexcept>

namespace rapidfuzz::detail {

class Hamming : public DistanceBase<Hamming, int64_t, 0, std::numeric_limits<int64_t>::max(), bool> {
    friend DistanceBase<Hamming, int64_t, 0, std::numeric_limits<int64_t>::max(), bool>;
    friend NormalizedMetricBase<Hamming, bool>;

    template <typename InputIt1, typename InputIt2>
    static int64_t maximum(Range<InputIt1> s1, Range<InputIt2> s2, bool)
    {
        return std::max(s1.size(), s2.size());
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t _distance(Range<InputIt1> s1, Range<InputIt2> s2, bool pad, int64_t score_cutoff,
                             [[maybe_unused]] int64_t score_hint)
    {
        if (!pad && s1.size() != s2.size()) throw std::invalid_argument("Sequences are not the same length.");

        size_t min_len = std::min(s1.size(), s2.size());
        int64_t dist = static_cast<int64_t>(std::max(s1.size(), s2.size()));
        auto iter_s1 = s1.begin();
        auto iter_s2 = s2.begin();
        for (size_t i = 0; i < min_len; ++i)
            dist -= bool(*(iter_s1++) == *(iter_s2++));

        return (dist <= score_cutoff) ? dist : score_cutoff + 1;
    }
};

template <typename InputIt1, typename InputIt2>
Editops hamming_editops(Range<InputIt1> s1, Range<InputIt2> s2, bool pad, int64_t)
{
    if (!pad && s1.size() != s2.size()) throw std::invalid_argument("Sequences are not the same length.");

    Editops ops;
    size_t min_len = std::min(s1.size(), s2.size());
    size_t i = 0;
    for (; i < min_len; ++i)
        if (s1[i] != s2[i]) ops.emplace_back(EditType::Replace, i, i);

    for (; i < s1.size(); ++i)
        ops.emplace_back(EditType::Delete, i, s2.size());

    for (; i < s2.size(); ++i)
        ops.emplace_back(EditType::Insert, s1.size(), i);

    ops.set_src_len(s1.size());
    ops.set_dest_len(s2.size());
    return ops;
}

} // namespace rapidfuzz::detail
