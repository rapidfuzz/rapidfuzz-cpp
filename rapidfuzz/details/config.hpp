/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
#    define RAPIDFUZZ_DEDUCTION_GUIDES
#    define RAPIDFUZZ_IF_CONSTEXPR_AVAILABLE 1
#    define RAPIDFUZZ_IF_CONSTEXPR if constexpr
#else
#    define RAPIDFUZZ_IF_CONSTEXPR_AVAILABLE 0
#    define RAPIDFUZZ_IF_CONSTEXPR if
#endif

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201402L) || __cplusplus >= 201402L)
#    define RAPIDFUZZ_CONSTEXPR_CXX14 constexpr
#else
#    define RAPIDFUZZ_CONSTEXPR_CXX14 inline
#endif
