/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#include <rapidfuzz/details/PatternMatchVector.hpp>
#include <rapidfuzz/details/Range.hpp>
#include <rapidfuzz/details/common.hpp>
#include <rapidfuzz/details/distance.hpp>
#include <rapidfuzz/details/intrinsics.hpp>
#include <rapidfuzz/distance/LCSseq.hpp>

namespace rapidfuzz::detail {

template <typename InputIt1, typename InputIt2>
int64_t indel_distance(const BlockPatternMatchVector& block, Range<InputIt1> s1, Range<InputIt2> s2,
                       int64_t score_cutoff)
{
    int64_t maximum = s1.size() + s2.size();
    int64_t lcs_cutoff = std::max<int64_t>(0, maximum / 2 - score_cutoff);
    int64_t lcs_sim = lcs_seq_similarity(block, s1, s2, lcs_cutoff);
    int64_t dist = maximum - 2 * lcs_sim;
    return (dist <= score_cutoff) ? dist : score_cutoff + 1;
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_distance(const BlockPatternMatchVector& block, Range<InputIt1> s1, Range<InputIt2> s2,
                                 double score_cutoff)
{
    int64_t maximum = s1.size() + s2.size();
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(static_cast<double>(maximum) * score_cutoff));
    int64_t dist = indel_distance(block, s1, s2, cutoff_distance);
    double norm_dist = (maximum) ? static_cast<double>(dist) / static_cast<double>(maximum) : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : 1.0;
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_similarity(const BlockPatternMatchVector& block, Range<InputIt1> s1,
                                   Range<InputIt2> s2, double score_cutoff)
{
    double cutoff_score = NormSim_to_NormDist(score_cutoff);
    double norm_dist = indel_normalized_distance(block, s1, s2, cutoff_score);
    double norm_sim = 1.0 - norm_dist;
    return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
}

class Indel : public DistanceBase<Indel> {
    friend DistanceBase<Indel>;
    friend NormalizedMetricBase<Indel>;

    template <typename InputIt1, typename InputIt2>
    static int64_t maximum(Range<InputIt1> s1, Range<InputIt2> s2)
    {
        return s1.size() + s2.size();
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t _distance(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_cutoff)
    {
        int64_t maximum = Indel::maximum(s1, s2);
        int64_t lcs_cutoff = std::max<int64_t>(0, maximum / 2 - score_cutoff);
        int64_t lcs_sim = LCSseq::similarity(s1, s2, lcs_cutoff);
        int64_t dist = maximum - 2 * lcs_sim;
        return (dist <= score_cutoff) ? dist : score_cutoff + 1;
    }
};

} // namespace rapidfuzz::detail
