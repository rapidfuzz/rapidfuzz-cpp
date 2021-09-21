/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once
#include <immintrin.h>

#if defined(_MSC_VER)
#  include <intrin.h>
#endif

#if defined(__i386) || defined(_M_IX86) || \
  defined(__amd64) || defined(_M_AMD64) || defined(__x86_64)
#  if defined(_MSC_VER)
#    define RAPIDFUZZ_BUILTIN__ENABLE_X86
#  elif defined(__GNUC__)
#    define RAPIDFUZZ_BUILTIN__ENABLE_X86
#    include <x86intrin.h>
#  endif
#endif

namespace intrinsics {

#include <rapidfuzz/details/builtin.h>

static inline uint64_t addc64(uint64_t a, uint64_t b, uint64_t carryin, uint64_t* carryout)
{
#if defined(RAPIDFUZZ_BUILTIN__ENABLE_X86)
    unsigned long long res;
    *carryout = _addcarry_u64(carryin, a, b, &res);
    return res;
#else
    a += carryin;
    *carryout = a < carryin;
    a += b;
    *carryout |= a < b;
    return a;
#endif
}

/*
 * count the number of bits set in a 64 bit integer
 */
static inline std::size_t popcount64(uint64_t x)
{
    return psnip_builtin_popcount64(x);
}

};
