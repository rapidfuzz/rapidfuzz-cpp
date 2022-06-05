#if defined(_M_X64) && !defined(__SSE2__)
#    define __SSE2__
#endif

#ifdef __x86_64__
#    ifdef __AVX2__
#        include <rapidfuzz/details/simd_avx2.hpp>
#    elif defined(__SSE2__)
#        include <rapidfuzz/details/simd_sse2.hpp>
#    endif
#endif