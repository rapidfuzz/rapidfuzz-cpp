/* SPDX-License-Identifier: MIT */
/* Copyright © 2022 Max Bachmann */
#pragma once

#include <array>
#include <emmintrin.h>
#include <rapidfuzz/details/intrinsics.hpp>
#include <stdint.h>
#include <tmmintrin.h>

namespace rapidfuzz {
namespace detail {
namespace simd_sse2 {

template <typename T>
class native_simd;

template <>
class native_simd<uint64_t> {
public:
    static const int _size = 2;
    __m128i xmm;

    native_simd() noexcept
    {}

    native_simd(__m128i val) noexcept : xmm(val)
    {}

    native_simd(uint64_t a) noexcept
    {
        xmm = _mm_set1_epi64x(static_cast<int64_t>(a));
    }

    native_simd(const uint64_t* p) noexcept
    {
        load(p);
    }

    operator __m128i() const noexcept
    {
        return xmm;
    }

    constexpr static int size() noexcept
    {
        return _size;
    }

    native_simd load(const uint64_t* p) noexcept
    {
        xmm = _mm_set_epi64x(static_cast<int64_t>(p[1]), static_cast<int64_t>(p[0]));
        return *this;
    }

    void store(uint64_t* p) const noexcept
    {
        _mm_store_si128(reinterpret_cast<__m128i*>(p), xmm);
    }

    native_simd operator+(const native_simd b) const noexcept
    {
        return _mm_add_epi64(xmm, b);
    }

    native_simd& operator+=(const native_simd b) noexcept
    {
        xmm = _mm_add_epi64(xmm, b);
        return *this;
    }

    native_simd operator-(const native_simd b) const noexcept
    {
        return _mm_sub_epi64(xmm, b);
    }

    native_simd& operator-=(const native_simd b) noexcept
    {
        xmm = _mm_sub_epi64(xmm, b);
        return *this;
    }
};

template <>
class native_simd<uint32_t> {
public:
    static const int _size = 4;
    __m128i xmm;

    native_simd() noexcept
    {}

    native_simd(__m128i val) noexcept : xmm(val)
    {}

    native_simd(uint32_t a) noexcept
    {
        xmm = _mm_set1_epi32(static_cast<int>(a));
    }

    native_simd(const uint64_t* p) noexcept
    {
        load(p);
    }

    operator __m128i() const noexcept
    {
        return xmm;
    }

    constexpr static int size() noexcept
    {
        return _size;
    }

    native_simd load(const uint64_t* p) noexcept
    {
        xmm = _mm_set_epi64x(static_cast<int64_t>(p[1]), static_cast<int64_t>(p[0]));
        return *this;
    }

    void store(uint32_t* p) const noexcept
    {
        _mm_store_si128(reinterpret_cast<__m128i*>(p), xmm);
    }

    native_simd operator+(const native_simd b) const noexcept
    {
        return _mm_add_epi32(xmm, b);
    }

    native_simd& operator+=(const native_simd b) noexcept
    {
        xmm = _mm_add_epi32(xmm, b);
        return *this;
    }

    native_simd operator-(const native_simd b) const noexcept
    {
        return _mm_sub_epi32(xmm, b);
    }

    native_simd& operator-=(const native_simd b) noexcept
    {
        xmm = _mm_sub_epi32(xmm, b);
        return *this;
    }
};

template <>
class native_simd<uint16_t> {
public:
    static const int _size = 8;
    __m128i xmm;

    native_simd() noexcept
    {}

    native_simd(__m128i val) noexcept : xmm(val)
    {}

    native_simd(uint16_t a) noexcept
    {
        xmm = _mm_set1_epi16(static_cast<short>(a));
    }

    native_simd(const uint64_t* p) noexcept
    {
        load(p);
    }

    operator __m128i() const noexcept
    {
        return xmm;
    }

    constexpr static int size() noexcept
    {
        return _size;
    }

    native_simd load(const uint64_t* p) noexcept
    {
        xmm = _mm_set_epi64x(static_cast<int64_t>(p[1]), static_cast<int64_t>(p[0]));
        return *this;
    }

    void store(uint16_t* p) const noexcept
    {
        _mm_store_si128(reinterpret_cast<__m128i*>(p), xmm);
    }

    native_simd operator+(const native_simd b) const noexcept
    {
        return _mm_add_epi16(xmm, b);
    }

    native_simd& operator+=(const native_simd b) noexcept
    {
        xmm = _mm_add_epi16(xmm, b);
        return *this;
    }

    native_simd operator-(const native_simd b) const noexcept
    {
        return _mm_sub_epi16(xmm, b);
    }

    native_simd& operator-=(const native_simd b) noexcept
    {
        xmm = _mm_sub_epi16(xmm, b);
        return *this;
    }
};

template <>
class native_simd<uint8_t> {
public:
    static const int _size = 16;
    __m128i xmm;

    native_simd() noexcept
    {}

    native_simd(__m128i val) noexcept : xmm(val)
    {}

    native_simd(uint8_t a) noexcept
    {
        xmm = _mm_set1_epi8(static_cast<char>(a));
    }

    native_simd(const uint64_t* p) noexcept
    {
        load(p);
    }

    operator __m128i() const noexcept
    {
        return xmm;
    }

    constexpr static int size() noexcept
    {
        return _size;
    }

    native_simd load(const uint64_t* p) noexcept
    {
        xmm = _mm_set_epi64x(static_cast<int64_t>(p[1]), static_cast<int64_t>(p[0]));
        return *this;
    }

    void store(uint8_t* p) const noexcept
    {
        _mm_store_si128(reinterpret_cast<__m128i*>(p), xmm);
    }

    native_simd operator+(const native_simd b) const noexcept
    {
        return _mm_add_epi8(xmm, b);
    }

    native_simd& operator+=(const native_simd b) noexcept
    {
        xmm = _mm_add_epi8(xmm, b);
        return *this;
    }

    native_simd operator-(const native_simd b) const noexcept
    {
        return _mm_sub_epi8(xmm, b);
    }

    native_simd& operator-=(const native_simd b) noexcept
    {
        xmm = _mm_sub_epi8(xmm, b);
        return *this;
    }
};

template <typename T>
__m128i hadd_impl(const __m128i& v) noexcept;

template <>
inline __m128i hadd_impl<uint8_t>(const __m128i& v) noexcept
{
    return v;
}

template <>
inline __m128i hadd_impl<uint16_t>(const __m128i& v) noexcept
{
    __m128i mask = _mm_set_epi16(static_cast<short>(-1), static_cast<short>(0), static_cast<short>(-1),
                                 static_cast<short>(0), static_cast<short>(-1), static_cast<short>(0),
                                 static_cast<short>(-1), static_cast<short>(0));
    __m128i lo = _mm_and_si128(v, mask);
    __m128i hi = _mm_srli_epi16(v, 8);
    return _mm_add_epi16(lo, hi);
    // todo sse3
    // return _mm_maddubs_epi16(v, _mm_set1_epi8(1));
}

template <>
inline __m128i hadd_impl<uint32_t>(const __m128i& v) noexcept
{
    return _mm_madd_epi16(hadd_impl<uint16_t>(v), _mm_set1_epi16(1));
}

template <>
inline __m128i hadd_impl<uint64_t>(const __m128i& v) noexcept
{
    return _mm_sad_epu8(v, _mm_setzero_si128());
}

template <typename T>
native_simd<T> popcount_impl(const native_simd<T>& v) noexcept
{
    __m128i n, x, total;
    const __m128i popcount_mask1 = _mm_set1_epi8(0x77);
    const __m128i popcount_mask2 = _mm_set1_epi8(0x0F);

    // Count bits in each 4-bit field.
    x = v;
    n = _mm_srli_epi64(x, 1);
    n = _mm_and_si128(popcount_mask1, n);
    x = _mm_sub_epi8(x, n);
    n = _mm_srli_epi64(n, 1);
    n = _mm_and_si128(popcount_mask1, n);
    x = _mm_sub_epi8(x, n);
    n = _mm_srli_epi64(n, 1);
    n = _mm_and_si128(popcount_mask1, n);
    x = _mm_sub_epi8(x, n);
    x = _mm_add_epi8(x, _mm_srli_epi16(x, 4));
    total = _mm_and_si128(popcount_mask2, x);

    /* todo use when sse3 available
    __m128i lookup = _mm_setr_epi8(0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4);
    const __m128i low_mask = _mm_set1_epi8(0x0F);
    __m128i lo = _mm_and_si128(v, low_mask);
    __m128i hi = _mm_and_si256(_mm_srli_epi32(v, 4), low_mask);
    __m128i popcnt1 = _mm_shuffle_epi8(lookup, lo);
    __m128i popcnt2 = _mm_shuffle_epi8(lookup, hi);
    __m128i total = _mm_add_epi8(popcnt1, popcnt2);*/

    return hadd_impl<T>(total);
}

template <typename T>
std::array<T, native_simd<T>::size()> popcount(const native_simd<T>& a) noexcept
{
    alignas(16) std::array<T, native_simd<T>::size()> res;
    popcount_impl(a).store(&res[0]);
    return res;
}

template <typename T>
native_simd<T> operator&(const native_simd<T>& a, const native_simd<T>& b) noexcept
{
    return _mm_and_si128(a, b);
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
    return _mm_or_si128(a, b);
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
    return _mm_xor_si128(a, b);
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
    return _mm_xor_si128(a, _mm_set1_epi32(-1));
}

} // simd_sse2
} // namespace detail
} // namespace rapidfuzz
