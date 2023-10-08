/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once

#include <limits>
#include <rapidfuzz/details/Range.hpp>
#include <rapidfuzz/distance/Jaro_impl.hpp>

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2>
double jaro_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                     double score_cutoff = 1.0)
{
    return detail::Jaro::distance(first1, last1, first2, last2, score_cutoff, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double jaro_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    return detail::Jaro::distance(s1, s2, score_cutoff, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double jaro_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                       double score_cutoff = 0.0)
{
    return detail::Jaro::similarity(first1, last1, first2, last2, score_cutoff, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double jaro_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    return detail::Jaro::similarity(s1, s2, score_cutoff, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double jaro_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                double score_cutoff = 1.0)
{
    return detail::Jaro::normalized_distance(first1, last1, first2, last2, score_cutoff, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double jaro_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    return detail::Jaro::normalized_distance(s1, s2, score_cutoff, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double jaro_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                  double score_cutoff = 0.0)
{
    return detail::Jaro::normalized_similarity(first1, last1, first2, last2, score_cutoff, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double jaro_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    return detail::Jaro::normalized_similarity(s1, s2, score_cutoff, score_cutoff);
}

#ifdef RAPIDFUZZ_SIMD
namespace experimental {
template <int MaxLen>
struct MultiJaro : public detail::MultiSimilarityBase<MultiJaro<MaxLen>, double, 0, 1> {

private:
    friend detail::MultiSimilarityBase<MultiJaro<MaxLen>, double, 0, 1>;
    friend detail::MultiNormalizedMetricBase<MultiJaro<MaxLen>>;

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
    MultiJaro(size_t count) : input_count(count), PM(find_block_count(count) * 64)
    {
        str_lens.resize(result_count());
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

private:
    template <typename InputIt2>
    void _similarity(double* scores, size_t score_count, detail::Range<InputIt2> s2,
                     double score_cutoff = 0.0) const
    {
        if (score_count < result_count())
            throw std::invalid_argument("scores has to have >= result_count() elements");

        detail::Range scores_(scores, scores + score_count);
        if constexpr (MaxLen == 8)
            detail::jaro_similarity_simd<uint8_t>(scores_, PM, str_lens, s2, score_cutoff);
        else if constexpr (MaxLen == 16)
            detail::jaro_similarity_simd<uint16_t>(scores_, PM, str_lens, s2, score_cutoff);
        else if constexpr (MaxLen == 32)
            detail::jaro_similarity_simd<uint32_t>(scores_, PM, str_lens, s2, score_cutoff);
        else if constexpr (MaxLen == 64)
            detail::jaro_similarity_simd<uint64_t>(scores_, PM, str_lens, s2, score_cutoff);
    }

    template <typename InputIt2>
    double maximum(size_t s1_idx, detail::Range<InputIt2>) const
    {
        return 1.0;
    }

    size_t get_input_count() const noexcept
    {
        return input_count;
    }

    size_t input_count;
    size_t pos = 0;
    detail::BlockPatternMatchVector PM;
    std::vector<size_t> str_lens;
};

} /* namespace experimental */
#endif /* RAPIDFUZZ_SIMD */

template <typename CharT1>
struct CachedJaro : public detail::CachedSimilarityBase<CachedJaro<CharT1>, double, 0, 1> {
    template <typename Sentence1>
    explicit CachedJaro(const Sentence1& s1_) : CachedJaro(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt1>
    CachedJaro(InputIt1 first1, InputIt1 last1) : s1(first1, last1), PM(detail::Range(first1, last1))
    {}

private:
    friend detail::CachedSimilarityBase<CachedJaro<CharT1>, double, 0, 1>;
    friend detail::CachedNormalizedMetricBase<CachedJaro<CharT1>>;

    template <typename InputIt2>
    double maximum(detail::Range<InputIt2>) const
    {
        return 1.0;
    }

    template <typename InputIt2>
    double _similarity(detail::Range<InputIt2> s2, double score_cutoff,
                       [[maybe_unused]] double score_hint) const
    {
        return detail::jaro_similarity(PM, detail::Range(s1), s2, score_cutoff);
    }

    std::basic_string<CharT1> s1;
    detail::BlockPatternMatchVector PM;
};

template <typename Sentence1>
explicit CachedJaro(const Sentence1& s1_) -> CachedJaro<char_type<Sentence1>>;

template <typename InputIt1>
CachedJaro(InputIt1 first1, InputIt1 last1) -> CachedJaro<iter_value_t<InputIt1>>;

} // namespace rapidfuzz
