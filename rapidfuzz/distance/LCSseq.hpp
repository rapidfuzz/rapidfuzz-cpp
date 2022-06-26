/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once
#include <rapidfuzz/details/PatternMatchVector.hpp>
#include <rapidfuzz/details/common.hpp>
#include <rapidfuzz/details/simd.hpp>
#include <rapidfuzz/details/span.hpp>

#include <cmath>
#include <limits>

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         int64_t max = std::numeric_limits<int64_t>::max());

template <typename Sentence1, typename Sentence2>
int64_t lcs_seq_distance(const Sentence1& s1, const Sentence2& s2,
                         int64_t max = std::numeric_limits<int64_t>::max());

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                           int64_t score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
int64_t lcs_seq_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff = 0);

template <typename InputIt1, typename InputIt2>
double lcs_seq_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff = 1.0);

template <typename Sentence1, typename Sentence2>
double lcs_seq_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0);

template <typename InputIt1, typename InputIt2>
double lcs_seq_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                     double score_cutoff = 0.0);

template <typename Sentence1, typename Sentence2>
double lcs_seq_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0);

template <typename InputIt1, typename InputIt2>
Editops lcs_seq_editops(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2);

template <typename Sentence1, typename Sentence2>
Editops lcs_seq_editops(const Sentence1& s1, const Sentence2& s2);

#ifdef RAPIDFUZZ_SIMD
/**
 * @note scores always need to be big enough to store find_result_count(count)
 */
template <int MaxLen>
struct MultiLCSseq {
private:
    constexpr static size_t get_vec_size()
    {
        switch (MaxLen) {
        case 8: return detail::native_simd<uint8_t>::size();
        case 16: return detail::native_simd<uint16_t>::size();
        case 32: return detail::native_simd<uint32_t>::size();
        case 64: return detail::native_simd<uint64_t>::size();
        }
        assert(false);
    }

    constexpr static size_t find_block_count(size_t count)
    {
        size_t vec_size = get_vec_size();
        size_t simd_vec_count = detail::ceil_div(count, vec_size);
        return detail::ceil_div(simd_vec_count * vec_size * MaxLen, 64);
    }

    constexpr static size_t find_result_count(size_t count)
    {
        size_t vec_size = get_vec_size();
        size_t simd_vec_count = detail::ceil_div(count, vec_size);
        return simd_vec_count * vec_size;
    }

public:
    MultiLCSseq(size_t count) : input_count(count), pos(0), PM(find_block_count(count) * 64)
    {}

    template <typename Sentence1>
    void insert(const Sentence1& s1_)
    {
        insert(detail::to_begin(s1_), detail::to_end(s1_));
    }

    template <typename InputIt1>
    void insert(InputIt1 first1, InputIt1 last1)
    {
        auto len = std::distance(first1, last1);
        auto block_pos = pos % 64;
        auto block = pos / 64;
        assert(len <= MaxLen);
        str_lens.push_back(static_cast<size_t>(len));

        for (; first1 != last1; ++first1) {
            PM.insert(block, *first1, block_pos);
            block_pos++;
        }
        pos += MaxLen;
    }

    template <typename InputIt2>
    void distance(tcb::span<int64_t> scores, InputIt2 first2, InputIt2 last2,
                  int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const noexcept;

    template <typename Sentence2>
    void distance(tcb::span<int64_t> scores, const Sentence2& s2,
                  int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const noexcept;

    template <typename InputIt2>
    void similarity(tcb::span<int64_t> scores, InputIt2 first2, InputIt2 last2,
                    int64_t score_cutoff = 0) const noexcept;

    template <typename Sentence2>
    void similarity(tcb::span<int64_t> scores, const Sentence2& s2, int64_t score_cutoff = 0) const noexcept;

    template <typename InputIt2>
    void normalized_distance(tcb::span<double> scores, InputIt2 first2, InputIt2 last2,
                             double score_cutoff = 1.0) const noexcept(sizeof(double) == sizeof(int64_t));

    template <typename Sentence2>
    void normalized_distance(tcb::span<double> scores, const Sentence2& s2, double score_cutoff = 1.0) const
        noexcept(sizeof(double) == sizeof(int64_t));

    template <typename InputIt2>
    void normalized_similarity(tcb::span<double> scores, InputIt2 first2, InputIt2 last2,
                               double score_cutoff = 0.0) const noexcept(sizeof(double) == sizeof(int64_t));

    template <typename Sentence2>
    void normalized_similarity(tcb::span<double> scores, const Sentence2& s2, double score_cutoff = 0.0) const
        noexcept(sizeof(double) == sizeof(int64_t));

private:
    size_t input_count;
    ptrdiff_t pos;
    detail::BlockPatternMatchVector PM;
    std::vector<size_t> str_lens;
};
#endif

template <typename CharT1>
struct CachedLCSseq {
    template <typename Sentence1>
    CachedLCSseq(const Sentence1& s1_) : CachedLCSseq(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt1>
    CachedLCSseq(InputIt1 first1, InputIt1 last1) : s1(first1, last1), PM(detail::make_range(first1, last1))
    {}

    template <typename InputIt2>
    int64_t distance(InputIt2 first2, InputIt2 last2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const;

    template <typename Sentence2>
    int64_t distance(const Sentence2& s2, int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const;

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
    detail::BlockPatternMatchVector PM;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedLCSseq(const Sentence1& s1_) -> CachedLCSseq<char_type<Sentence1>>;

template <typename InputIt1>
CachedLCSseq(InputIt1 first1, InputIt1 last1) -> CachedLCSseq<iter_value_t<InputIt1>>;
#endif

} // namespace rapidfuzz

#include <rapidfuzz/distance/LCSseq.impl>