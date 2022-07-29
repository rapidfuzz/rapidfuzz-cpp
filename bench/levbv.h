#ifdef __cplusplus
extern "C" {
#endif

#ifndef _LEVBV_H
#define _LEVBV_H

#include <stdint.h>

#ifdef __x86_64__
    #if SIZE_MAX == 0xFFFFFFFF
        typedef uint32_t bv_bits;
        #define _LEVBV_WIDTH 32
    #else
        typedef uint64_t bv_bits;
        #define _LEVBV_WIDTH 64
    #endif
#else
    typedef uint32_t bv_bits;
    #define _LEVBV_WIDTH 32
#endif

typedef struct {
    void **keys;
    uint32_t *lens;
    int capacity;
    int elements;
} Array;

int dist_bytes (const char * a, int alen, const char * b,  int blen);
int dist_uni (const uint32_t *a, int alen, const uint32_t *b, int blen);
int dist_hybrid (const uint32_t *a, int alen, const uint32_t *b, int blen);
int dist_simple (const uint32_t *a, int alen, const uint32_t *b, int blen);

int dist_array (const Array *a, const Array *b);
int dist_simple_arr ( const Array *a, const Array *b );

#endif

#ifdef __cplusplus
}
#endif
