#pragma once

// C++ language version detection (C++20 is speculative):
// Note: VC14.0/1900 (VS2015) lacks too much from C++14.
#ifndef rapidfuzz_CPLUSPLUS
#  if defined(_MSVC_LANG) && !defined(__clang__)
#    define rapidfuzz_CPLUSPLUS (_MSC_VER == 1900 ? 201103L : _MSVC_LANG)
#  else
#    define rapidfuzz_CPLUSPLUS __cplusplus
#  endif
#endif

#define rapidfuzz_CPP11_OR_GREATER (rapidfuzz_CPLUSPLUS >= 201103L)
#define rapidfuzz_CPP14_OR_GREATER (rapidfuzz_CPLUSPLUS >= 201402L)
#define rapidfuzz_CPP17_OR_GREATER (rapidfuzz_CPLUSPLUS >= 201703L)

// select usage of boost::string_view or std::string_view
#if rapidfuzz_CPP17_OR_GREATER && defined(__has_include)
#  if __has_include(<string_view>)
#    define rapidfuzz_USES_STD_STRING_VIEW true
#  else
#    define rapidfuzz_USES_STD_STRING_VIEW false
#  endif
#else
#  define rapidfuzz_USES_STD_STRING_VIEW false
#endif
