/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once

#include <cstddef>
#include <cstdint>
#include <bitset>

#if defined(_MSC_VER) && !defined(__clang__)
#    include <intrin.h>
#endif

namespace rapidfuzz {
namespace detail {

static inline uint64_t addc64(uint64_t a, uint64_t b, uint64_t carryin, uint64_t* carryout)
{
    /* todo should use _addcarry_u64 when available */
    a += carryin;
    *carryout = a < carryin;
    a += b;
    *carryout |= a < b;
    return a;
}

template <typename T, typename U>
T ceil_div(T a, U divisor)
{
    return a / divisor + static_cast<T>(a % divisor != 0);
}

static inline int64_t popcount64(uint64_t x)
{
    const uint64_t m1 = 0x5555555555555555;
    const uint64_t m2 = 0x3333333333333333;
    const uint64_t m4 = 0x0f0f0f0f0f0f0f0f;
    const uint64_t h01 = 0x0101010101010101;

    x -= (x >> 1) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;
    return static_cast<int64_t>((x * h01) >> 56);
}

static inline int popcount(uint64_t x)
{
    return static_cast<int>(std::bitset<64>(x).count());
}

static inline int popcount(uint32_t x)
{
    return static_cast<int>(std::bitset<32>(x).count());
}

static inline int popcount(uint16_t x)
{
    return static_cast<int>(std::bitset<16>(x).count());
}

static inline int popcount(uint8_t x)
{
    static constexpr int bit_count[256] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
    };
    return bit_count[x];
}


/**
 * Extract the lowest set bit from a. If no bits are set in a returns 0.
 */
template <typename T>
T blsi(T a)
{
    return a & -a;
}

/**
 * Clear the lowest set bit in a.
 */
template <typename T>
T blsr(T x)
{
    return x & (x - 1);
}

/**
 * Sets all the lower bits of the result to 1 up to and including lowest set bit (=1) in a.
 * If a is zero, blsmsk sets all bits to 1.
 */
template <typename T>
T blsmsk(T a)
{
    return a ^ (a - 1);
}

#if defined(_MSC_VER) && !defined(__clang__)
static inline int tzcnt(uint32_t x)
{
    unsigned long trailing_zero = 0;
    _BitScanForward(&trailing_zero, x);
    return trailing_zero;
}

#    if defined(_M_ARM) || defined(_M_X64)
static inline int tzcnt(uint64_t x)
{
    unsigned long trailing_zero = 0;
    _BitScanForward64(&trailing_zero, x);
    return trailing_zero;
}
#    else
static inline int tzcnt(uint64_t x)
{
    uint32_t msh = (uint32_t)(x >> 32);
    uint32_t lsh = (uint32_t)(x & 0xFFFFFFFF);
    if (lsh != 0) {
        return tzcnt(lsh);
    }
    return 32 + tzcnt(msh);
}
#    endif

#else /*  gcc / clang */
static inline int tzcnt(uint32_t x)
{
    return __builtin_ctz(x);
}

static inline int tzcnt(uint64_t x)
{
    return __builtin_ctzll(x);
}
#endif

template<class T, T... inds, class F>
constexpr void unroll(std::integer_sequence<T, inds...>, F&& f) {
  (f(std::integral_constant<T, inds>{}), ...);
}

template<class T, T count, class F>
constexpr void unroll(F&& f) {
  unroll(std::make_integer_sequence<T, count>{}, std::forward<F>(f));
}

} // namespace detail
} // namespace rapidfuzz
