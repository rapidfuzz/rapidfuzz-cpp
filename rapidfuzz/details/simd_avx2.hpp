/* SPDX-License-Identifier: MIT */
/* Copyright © 2022 Max Bachmann */
#pragma once

#include <array>
#include <immintrin.h>
#include <ostream>
#include <rapidfuzz/details/intrinsics.hpp>
#include <stdint.h>

namespace rapidfuzz {
namespace detail {
namespace simd_avx2 {

template <typename T>
class native_simd;

template <>
class native_simd<uint64_t> {
public:
    using value_type = uint64_t;

    static const int _size = 4;
    __m256i xmm;

    native_simd() noexcept
    {}

    native_simd(__m256i val) noexcept : xmm(val)
    {}

    native_simd(uint64_t a) noexcept
    {
        xmm = _mm256_set1_epi64x(static_cast<int64_t>(a));
    }

    native_simd(const uint64_t* p) noexcept
    {
        load(p);
    }

    operator __m256i() const noexcept
    {
        return xmm;
    }

    constexpr static int size() noexcept
    {
        return _size;
    }

    native_simd load(const uint64_t* p) noexcept
    {
        xmm = _mm256_set_epi64x(static_cast<int64_t>(p[3]), static_cast<int64_t>(p[2]),
                                static_cast<int64_t>(p[1]), static_cast<int64_t>(p[0]));
        return *this;
    }

    void store(uint64_t* p) const noexcept
    {
        _mm256_store_si256(reinterpret_cast<__m256i*>(p), xmm);
    }

    native_simd operator+(const native_simd b) const noexcept
    {
        return _mm256_add_epi64(xmm, b);
    }

    native_simd& operator+=(const native_simd b) noexcept
    {
        xmm = _mm256_add_epi64(xmm, b);
        return *this;
    }

    native_simd operator-(const native_simd b) const noexcept
    {
        return _mm256_sub_epi64(xmm, b);
    }

    native_simd& operator-=(const native_simd b) noexcept
    {
        xmm = _mm256_sub_epi64(xmm, b);
        return *this;
    }
};

template <>
class native_simd<uint32_t> {
public:
    using value_type = uint32_t;

    static const int _size = 8;
    __m256i xmm;

    native_simd() noexcept
    {}

    native_simd(__m256i val) noexcept : xmm(val)
    {}

    native_simd(uint32_t a) noexcept
    {
        xmm = _mm256_set1_epi32(static_cast<int>(a));
    }

    native_simd(const uint64_t* p) noexcept
    {
        load(p);
    }

    operator __m256i() const
    {
        return xmm;
    }

    constexpr static int size() noexcept
    {
        return _size;
    }

    native_simd load(const uint64_t* p) noexcept
    {
        xmm = _mm256_set_epi64x(static_cast<int64_t>(p[3]), static_cast<int64_t>(p[2]),
                                static_cast<int64_t>(p[1]), static_cast<int64_t>(p[0]));
        return *this;
    }

    void store(uint32_t* p) const noexcept
    {
        _mm256_store_si256(reinterpret_cast<__m256i*>(p), xmm);
    }

    native_simd operator+(const native_simd b) const noexcept
    {
        return _mm256_add_epi32(xmm, b);
    }

    native_simd& operator+=(const native_simd b) noexcept
    {
        xmm = _mm256_add_epi32(xmm, b);
        return *this;
    }

    native_simd operator-(const native_simd b) const noexcept
    {
        return _mm256_sub_epi32(xmm, b);
    }

    native_simd& operator-=(const native_simd b) noexcept
    {
        xmm = _mm256_sub_epi32(xmm, b);
        return *this;
    }
};

template <>
class native_simd<uint16_t> {
public:
    using value_type = uint16_t;

    static const int _size = 16;
    __m256i xmm;

    native_simd() noexcept
    {}

    native_simd(__m256i val) : xmm(val)
    {}

    native_simd(uint16_t a) noexcept
    {
        xmm = _mm256_set1_epi16(static_cast<short>(a));
    }

    native_simd(const uint64_t* p) noexcept
    {
        load(p);
    }

    operator __m256i() const noexcept
    {
        return xmm;
    }

    constexpr static int size() noexcept
    {
        return _size;
    }

    native_simd load(const uint64_t* p) noexcept
    {
        xmm = _mm256_set_epi64x(static_cast<int64_t>(p[3]), static_cast<int64_t>(p[2]),
                                static_cast<int64_t>(p[1]), static_cast<int64_t>(p[0]));
        return *this;
    }

    void store(uint16_t* p) const noexcept
    {
        _mm256_store_si256(reinterpret_cast<__m256i*>(p), xmm);
    }

    native_simd operator+(const native_simd b) const noexcept
    {
        return _mm256_add_epi16(xmm, b);
    }

    native_simd& operator+=(const native_simd b) noexcept
    {
        xmm = _mm256_add_epi16(xmm, b);
        return *this;
    }

    native_simd operator-(const native_simd b) const noexcept
    {
        return _mm256_sub_epi16(xmm, b);
    }

    native_simd& operator-=(const native_simd b) noexcept
    {
        xmm = _mm256_sub_epi16(xmm, b);
        return *this;
    }
};

template <>
class native_simd<uint8_t> {
public:
    using value_type = uint8_t;

    static const int _size = 32;
    __m256i xmm;

    native_simd() noexcept
    {}

    native_simd(__m256i val) noexcept : xmm(val)
    {}

    native_simd(uint8_t a) noexcept
    {
        xmm = _mm256_set1_epi8(static_cast<char>(a));
    }

    native_simd(const uint64_t* p) noexcept
    {
        load(p);
    }

    operator __m256i() const noexcept
    {
        return xmm;
    }

    constexpr static int size() noexcept
    {
        return _size;
    }

    native_simd load(const uint64_t* p) noexcept
    {
        xmm = _mm256_set_epi64x(static_cast<int64_t>(p[3]), static_cast<int64_t>(p[2]),
                                static_cast<int64_t>(p[1]), static_cast<int64_t>(p[0]));
        return *this;
    }

    void store(uint8_t* p) const noexcept
    {
        _mm256_store_si256(reinterpret_cast<__m256i*>(p), xmm);
    }

    native_simd operator+(const native_simd b) const noexcept
    {
        return _mm256_add_epi8(xmm, b);
    }

    native_simd& operator+=(const native_simd b) noexcept
    {
        xmm = _mm256_add_epi8(xmm, b);
        return *this;
    }

    native_simd operator-(const native_simd b) const noexcept
    {
        return _mm256_sub_epi8(xmm, b);
    }

    native_simd& operator-=(const native_simd b) noexcept
    {
        xmm = _mm256_sub_epi8(xmm, b);
        return *this;
    }
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const native_simd<T>& a)
{
    alignas(32) std::array<T, native_simd<T>::size()> res;
    a.store(&res[0]);

    for (size_t i = res.size() - 1; i != 0; i--)
        os << std::bitset<std::numeric_limits<T>::digits>(res[i]) << "|";

    os << std::bitset<std::numeric_limits<T>::digits>(res[0]);
    return os;
}

template <typename T>
__m256i hadd_impl(__m256i x) noexcept;

template <>
inline __m256i hadd_impl<uint8_t>(__m256i x) noexcept
{
    return x;
}

template <>
inline __m256i hadd_impl<uint16_t>(__m256i x) noexcept
{
    const __m256i mask = _mm256_set1_epi16(0x001f);
    __m256i y = _mm256_srli_si256(x, 1);
    x = _mm256_add_epi16(x, y);
    return _mm256_and_si256(x, mask);
}

template <>
inline __m256i hadd_impl<uint32_t>(__m256i x) noexcept
{
    const __m256i mask = _mm256_set1_epi32(0x0000003F);
    x = hadd_impl<uint16_t>(x);
    __m256i y = _mm256_srli_si256(x, 2);
    x = _mm256_add_epi32(x, y);
    return _mm256_and_si256(x, mask);
}

template <>
inline __m256i hadd_impl<uint64_t>(__m256i x) noexcept
{
    return _mm256_sad_epu8(x, _mm256_setzero_si256());
}

/* based on the paper `Faster Population Counts Using AVX2 Instructions` */
template <typename T>
native_simd<T> popcount_impl(const native_simd<T>& v) noexcept
{
    __m256i lookup = _mm256_setr_epi8(0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 0, 1, 1, 2, 1, 2, 2, 3,
                                      1, 2, 2, 3, 2, 3, 3, 4);
    const __m256i low_mask = _mm256_set1_epi8(0x0F);
    __m256i lo = _mm256_and_si256(v, low_mask);
    __m256i hi = _mm256_and_si256(_mm256_srli_epi32(v, 4), low_mask);
    __m256i popcnt1 = _mm256_shuffle_epi8(lookup, lo);
    __m256i popcnt2 = _mm256_shuffle_epi8(lookup, hi);
    __m256i total = _mm256_add_epi8(popcnt1, popcnt2);
    return hadd_impl<T>(total);
}

template <typename T>
std::array<T, native_simd<T>::size()> popcount(const native_simd<T>& a) noexcept
{
    alignas(32) std::array<T, native_simd<T>::size()> res;
    popcount_impl(a).store(&res[0]);
    return res;
}

// function andnot: a & ~ b
template <typename T>
native_simd<T> andnot(const native_simd<T>& a, const native_simd<T>& b)
{
    return _mm256_andnot_si256(b, a);
}

static inline native_simd<uint8_t> operator==(const native_simd<uint8_t>& a,
                                              const native_simd<uint8_t>& b) noexcept
{
    return _mm256_cmpeq_epi8(a, b);
}

static inline native_simd<uint16_t> operator==(const native_simd<uint16_t>& a,
                                               const native_simd<uint16_t>& b) noexcept
{
    return _mm256_cmpeq_epi16(a, b);
}

static inline native_simd<uint32_t> operator==(const native_simd<uint32_t>& a,
                                               const native_simd<uint32_t>& b) noexcept
{
    return _mm256_cmpeq_epi32(a, b);
}

static inline native_simd<uint64_t> operator==(const native_simd<uint64_t>& a,
                                               const native_simd<uint64_t>& b) noexcept
{
    return _mm256_cmpeq_epi64(a, b);
}

static inline native_simd<uint8_t> operator<<(const native_simd<uint8_t>& a, int b) noexcept
{
    return _mm256_and_si256(_mm256_slli_epi16(a, b),
                            _mm256_set1_epi8(static_cast<char>(0xFF << (b & 0b1111))));
}

static inline native_simd<uint16_t> operator<<(const native_simd<uint16_t>& a, int b) noexcept
{
    return _mm256_slli_epi16(a, b);
}

static inline native_simd<uint32_t> operator<<(const native_simd<uint32_t>& a, int b) noexcept
{
    return _mm256_slli_epi32(a, b);
}

static inline native_simd<uint64_t> operator<<(const native_simd<uint64_t>& a, int b) noexcept
{
    return _mm256_slli_epi64(a, b);
}

template <typename T>
native_simd<T> operator&(const native_simd<T>& a, const native_simd<T>& b) noexcept
{
    return _mm256_and_si256(a, b);
}

template <typename T>
native_simd<T> operator&=(native_simd<T>& a, const native_simd<T>& b) noexcept
{
    a = a & b;
    return a;
}

template <typename T>
native_simd<T> operator|(const native_simd<T>& a, const native_simd<T>& b) noexcept
{
    return _mm256_or_si256(a, b);
}

template <typename T>
native_simd<T> operator|=(native_simd<T>& a, const native_simd<T>& b) noexcept
{
    a = a | b;
    return a;
}

template <typename T>
native_simd<T> operator^(const native_simd<T>& a, const native_simd<T>& b) noexcept
{
    return _mm256_xor_si256(a, b);
}

template <typename T>
native_simd<T> operator^=(native_simd<T>& a, const native_simd<T>& b) noexcept
{
    a = a ^ b;
    return a;
}

template <typename T>
native_simd<T> operator~(const native_simd<T>& a) noexcept
{
    return _mm256_xor_si256(a, _mm256_set1_epi32(-1));
}

} // namespace simd_avx2
} // namespace detail
} // namespace rapidfuzz
