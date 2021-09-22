/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once
namespace rapidfuzz {
namespace intrinsics {

#include <rapidfuzz/details/builtin.h>

static inline uint64_t addc64(uint64_t a, uint64_t b, uint64_t carryin, uint64_t* carryout)
{
    /* todo should use _addcarry_u64 when available */
    return psnip_builtin_addc64(a, b, carryin, carryout);
}

static inline std::size_t popcount64(uint64_t x)
{
    return psnip_builtin_popcount64(x);
}
    
} // namespace intrinsics
} // namespace rapidfuzz
