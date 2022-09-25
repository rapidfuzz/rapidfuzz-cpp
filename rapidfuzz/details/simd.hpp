
/* SPDX-License-Identifier: MIT */
/* Copyright © 2022 Max Bachmann */
#pragma once

#ifndef RAPIDFUZZ_EXCLUDE_SIMD
#    if __AVX2__
#        define RAPIDFUZZ_SIMD
#        define RAPIDFUZZ_AVX2
#        include <rapidfuzz/details/simd_avx2.hpp>

#    elif (defined(_M_AMD64) || defined(_M_X64)) || defined(__SSE2__)
#        define RAPIDFUZZ_SIMD
#        define RAPIDFUZZ_SSE2
#        include <rapidfuzz/details/simd_sse2.hpp>
#    endif
#endif