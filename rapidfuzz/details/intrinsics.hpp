/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once
namespace rapidfuzz {
namespace intrinsics {

static inline uint64_t addc64(uint64_t a, uint64_t b, uint64_t carryin, uint64_t* carryout)
{
    /* todo should use _addcarry_u64 when available */
    a += carryin;
    *carryout = a < carryin;
    a += b;
    *carryout |= a < b;
    return a;
}

static inline std::size_t popcount64(uint64_t x)
{
    const uint64_t m1 = 0x5555555555555555;
    const uint64_t m2 = 0x3333333333333333;
    const uint64_t m4 = 0x0f0f0f0f0f0f0f0f;
    const uint64_t h01 = 0x0101010101010101;

    x -= (x >> 1) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;
    return static_cast<std::size_t>((x * h01) >> 56);
}
    
} // namespace intrinsics
} // namespace rapidfuzz
