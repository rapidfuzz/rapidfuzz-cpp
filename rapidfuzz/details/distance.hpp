/* SPDX-License-Identifier: MIT */
/* Copyright © 2022 Max Bachmann */

#pragma once

#include <cmath>
#include <rapidfuzz/details/Range.hpp>
#include <rapidfuzz/details/common.hpp>

namespace rapidfuzz::detail {

template <typename T, typename... Args>
struct NormalizedMetricBase {
    template <typename InputIt1, typename InputIt2>
    static double normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                      Args... args, double score_cutoff)
    {
        return _normalized_distance(Range(first1, last1), Range(first2, last2), std::forward<Args>(args)...,
                                    score_cutoff);
    }

    template <typename Sentence1, typename Sentence2>
    static double normalized_distance(const Sentence1& s1, const Sentence2& s2, Args... args,
                                      double score_cutoff)
    {
        return _normalized_distance(Range(s1), Range(s2), std::forward<Args>(args)..., score_cutoff);
    }

    template <typename InputIt1, typename InputIt2>
    static double normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                        Args... args, double score_cutoff)
    {
        return _normalized_similarity(Range(first1, last1), Range(first2, last2), std::forward<Args>(args)...,
                                      score_cutoff);
    }

    template <typename Sentence1, typename Sentence2>
    static double normalized_similarity(const Sentence1& s1, const Sentence2& s2, Args... args,
                                        double score_cutoff)
    {
        return _normalized_similarity(Range(s1), Range(s2), std::forward<Args>(args)..., score_cutoff);
    }

protected:
    template <typename InputIt1, typename InputIt2>
    static double _normalized_distance(Range<InputIt1> s1, Range<InputIt2> s2, Args... args,
                                       double score_cutoff)
    {
        auto maximum = T::maximum(s1, s2, args...);
        int64_t cutoff_distance =
            static_cast<int64_t>(std::ceil(static_cast<double>(maximum) * score_cutoff));
        int64_t dist = T::_distance(s1, s2, std::forward<Args>(args)..., cutoff_distance);
        double norm_dist = (maximum) ? static_cast<double>(dist) / static_cast<double>(maximum) : 0.0;
        return (norm_dist <= score_cutoff) ? norm_dist : 1.0;
    }

    template <typename InputIt1, typename InputIt2>
    static double _normalized_similarity(Range<InputIt1> s1, Range<InputIt2> s2, Args... args,
                                         double score_cutoff)
    {
        double cutoff_score = NormSim_to_NormDist(score_cutoff);
        double norm_dist = _normalized_distance(s1, s2, std::forward<Args>(args)..., cutoff_score);
        double norm_sim = 1.0 - norm_dist;
        return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
    }

    NormalizedMetricBase(){};
    friend T;
};

template <typename T, typename... Args>
struct DistanceBase : public NormalizedMetricBase<T, Args...> {
    template <typename InputIt1, typename InputIt2>
    static int64_t distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, Args... args,
                            int64_t score_cutoff)
    {
        return T::_distance(Range(first1, last1), Range(first2, last2), std::forward<Args>(args)...,
                            score_cutoff);
    }

    template <typename Sentence1, typename Sentence2>
    static int64_t distance(const Sentence1& s1, const Sentence2& s2, Args... args, int64_t score_cutoff)
    {
        return T::_distance(Range(s1), Range(s2), std::forward<Args>(args)..., score_cutoff);
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, Args... args,
                              int64_t score_cutoff)
    {
        return _similarity(Range(first1, last1), Range(first2, last2), std::forward<Args>(args)...,
                           score_cutoff);
    }

    template <typename Sentence1, typename Sentence2>
    static int64_t similarity(const Sentence1& s1, const Sentence2& s2, Args... args, int64_t score_cutoff)
    {
        return _similarity(Range(s1), Range(s2), std::forward<Args>(args)..., score_cutoff);
    }

protected:
    template <typename InputIt1, typename InputIt2>
    static int64_t _similarity(Range<InputIt1> s1, Range<InputIt2> s2, Args... args, int64_t score_cutoff)
    {
        auto maximum = T::maximum(s1, s2, args...);
        int64_t cutoff_distance = maximum - score_cutoff;
        int64_t dist = T::_distance(s1, s2, std::forward<Args>(args)..., cutoff_distance);
        int64_t sim = maximum - dist;
        return (sim >= score_cutoff) ? sim : 0;
    }

    DistanceBase(){};
    friend T;
};

template <typename T, typename... Args>
struct SimilarityBase : public NormalizedMetricBase<T, Args...> {
    template <typename InputIt1, typename InputIt2>
    static int64_t distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, Args... args,
                            int64_t score_cutoff)
    {
        return _distance(Range(first1, last1), Range(first2, last2), std::forward<Args>(args)...,
                         score_cutoff);
    }

    template <typename Sentence1, typename Sentence2>
    static int64_t distance(const Sentence1& s1, const Sentence2& s2, Args... args, int64_t score_cutoff)
    {
        return _distance(Range(s1), Range(s2), std::forward<Args>(args)..., score_cutoff);
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, Args... args,
                              int64_t score_cutoff)
    {
        return T::_similarity(Range(first1, last1), Range(first2, last2), std::forward<Args>(args)...,
                              score_cutoff);
    }

    template <typename Sentence1, typename Sentence2>
    static int64_t similarity(const Sentence1& s1, const Sentence2& s2, Args... args, int64_t score_cutoff)
    {
        return T::_similarity(Range(s1), Range(s2), std::forward<Args>(args)..., score_cutoff);
    }

protected:
    template <typename InputIt1, typename InputIt2>
    static int64_t _distance(Range<InputIt1> s1, Range<InputIt2> s2, Args... args, int64_t score_cutoff)
    {
        auto maximum = T::maximum(s1, s2, args...);
        int64_t cutoff_similarity = std::max<int64_t>(0, maximum - score_cutoff);
        int64_t sim = T::_similarity(s1, s2, std::forward<Args>(args)..., cutoff_similarity);
        int64_t dist = maximum - sim;
        return (dist <= score_cutoff) ? dist : score_cutoff + 1;
    }

    SimilarityBase(){};
    friend T;
};

template <typename T>
struct CachedNormalizedMetricBase {
    template <typename InputIt2>
    double normalized_distance(InputIt2 first2, InputIt2 last2, double score_cutoff = 1.0) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._normalized_distance(Range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    double normalized_distance(const Sentence2& s2, double score_cutoff = 1.0) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._normalized_distance(Range(s2), score_cutoff);
    }

    template <typename InputIt2>
    double normalized_similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0.0) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._normalized_similarity(Range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    double normalized_similarity(const Sentence2& s2, double score_cutoff = 0.0) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._normalized_similarity(Range(s2), score_cutoff);
    }

protected:
    template <typename InputIt2>
    double _normalized_distance(Range<InputIt2> s2, double score_cutoff) const
    {
        const T& derived = static_cast<const T&>(*this);
        auto maximum = derived.maximum(s2);
        int64_t cutoff_distance =
            static_cast<int64_t>(std::ceil(static_cast<double>(maximum) * score_cutoff));
        int64_t dist = derived._distance(s2, cutoff_distance);
        double norm_dist = (maximum) ? static_cast<double>(dist) / static_cast<double>(maximum) : 0.0;
        return (norm_dist <= score_cutoff) ? norm_dist : 1.0;
    }

    template <typename InputIt2>
    double _normalized_similarity(Range<InputIt2> s2, double score_cutoff) const
    {
        double cutoff_score = NormSim_to_NormDist(score_cutoff);
        double norm_dist = _normalized_distance(s2, cutoff_score);
        double norm_sim = 1.0 - norm_dist;
        return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
    }

    CachedNormalizedMetricBase(){};
    friend T;
};

template <typename T>
struct CachedDistanceBase : public CachedNormalizedMetricBase<T> {
    template <typename InputIt2>
    int64_t distance(InputIt2 first2, InputIt2 last2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._distance(Range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    int64_t distance(const Sentence2& s2, int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._distance(Range(s2), score_cutoff);
    }

    template <typename InputIt2>
    int64_t similarity(InputIt2 first2, InputIt2 last2, int64_t score_cutoff = 0) const
    {
        return _similarity(Range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    int64_t similarity(const Sentence2& s2, int64_t score_cutoff = 0) const
    {
        return _similarity(Range(s2), score_cutoff);
    }

protected:
    template <typename InputIt2>
    int64_t _similarity(Range<InputIt2> s2, int64_t score_cutoff) const
    {
        const T& derived = static_cast<const T&>(*this);
        auto maximum = derived.maximum(s2);
        int64_t cutoff_distance = maximum - score_cutoff;
        int64_t dist = derived._distance(s2, cutoff_distance);
        int64_t sim = maximum - dist;
        return (sim >= score_cutoff) ? sim : 0;
    }

    CachedDistanceBase(){};
    friend T;
};

template <typename T>
struct CachedSimilarityBase : public CachedNormalizedMetricBase<T> {
    template <typename InputIt2>
    int64_t distance(InputIt2 first2, InputIt2 last2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const
    {
        return _distance(Range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    int64_t distance(const Sentence2& s2, int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const
    {
        return _distance(Range(s2), score_cutoff);
    }

    template <typename InputIt2>
    int64_t similarity(InputIt2 first2, InputIt2 last2, int64_t score_cutoff = 0) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._similarity(Range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    int64_t similarity(const Sentence2& s2, int64_t score_cutoff = 0) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._similarity(Range(s2), score_cutoff);
    }

protected:
    template <typename InputIt2>
    int64_t _distance(Range<InputIt2> s2, int64_t score_cutoff) const
    {
        const T& derived = static_cast<const T&>(*this);
        auto maximum = derived.maximum(s2);
        int64_t cutoff_similarity = std::max<int64_t>(0, maximum - score_cutoff);
        int64_t sim = derived._similarity(s2, cutoff_similarity);
        int64_t dist = maximum - sim;
        return (dist <= score_cutoff) ? dist : score_cutoff + 1;
    }

    CachedSimilarityBase(){};
    friend T;
};

} // namespace rapidfuzz::detail
