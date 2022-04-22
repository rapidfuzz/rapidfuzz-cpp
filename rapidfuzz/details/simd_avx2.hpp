#pragma once

#include <stdint.h>
#include <immintrin.h>
#include <array>
#include <rapidfuzz/details/intrinsics.hpp>

namespace rapidfuzz {
namespace detail {

template <typename T>
class native_simd;

template <>
class native_simd<uint64_t>
{
public:
    static const int _size = 4;
    __m256i xmm;

    native_simd() {}

    native_simd(__m256i val)
        : xmm(val) {}

    native_simd(uint64_t a)
    {
        xmm = _mm256_set1_epi64x(a);
    }

    operator __m256i() const {
        return xmm;
    }

    constexpr static int size() { return _size; }

    native_simd load(const uint64_t* p) {
        xmm = _mm256_set_epi64x(
            p[0],
            p[1],
            p[2],
            p[3]
        );
        return *this;
    }

    void store(const uint64_t* p) const {
        _mm256_store_si256((__m256i*)p, xmm);
    }

    native_simd operator+(const native_simd b) const {
        return _mm256_add_epi64(xmm, b);
    }

    native_simd& operator+=(const native_simd b) {
        xmm = _mm256_add_epi64(xmm, b);
        return *this;
    }

    native_simd operator-(const native_simd b) const {
        return _mm256_sub_epi64(xmm, b);
    }

    native_simd& operator-=(const native_simd b) {
        xmm = _mm256_sub_epi64(xmm, b);
        return *this;
    }

    std::array<int64_t, _size> popcount() const {
        uint64_t stored[_size];
        store(stored);

        std::array<int64_t, _size> res;
        unroll<int, _size>([&](auto i) {
            res[i] = rapidfuzz::detail::popcount(stored[i]);
        });
        return res;
    }
};

template <>
class native_simd<uint32_t>
{
public:
    static const int _size = 8;
    __m256i xmm;

    native_simd() {}

    native_simd(__m256i val)
        : xmm(val) {}

    native_simd(uint32_t a)
    {
        xmm = _mm256_set1_epi32((int)a);
    }

    operator __m256i() const {
        return xmm;
    }

    constexpr static int size() { return _size; }

    native_simd load(const uint64_t* p) {
        xmm = _mm256_set_epi64x(
            p[0],
            p[1],
            p[2],
            p[3]
        );
        return *this;
    }

    void store(const uint32_t* p) const {
        _mm256_store_si256((__m256i*)p, xmm);
    }

    native_simd operator+(const native_simd b) const {
        return _mm256_add_epi32(xmm, b);
    }

    native_simd& operator+=(const native_simd b) {
        xmm = _mm256_add_epi32(xmm, b);
        return *this;
    }

    native_simd operator-(const native_simd b) const {
        return _mm256_sub_epi32(xmm, b);
    }

    native_simd& operator-=(const native_simd b) {
        xmm = _mm256_sub_epi32(xmm, b);
        return *this;
    }

    std::array<int64_t, _size> popcount() const {
        uint32_t stored[_size];
        store(stored);

        std::array<int64_t, _size> res;
        unroll<int, _size>([&](auto i) {
            res[i] = rapidfuzz::detail::popcount(stored[i]);
        });
        return res;
    }
};

template <>
class native_simd<uint16_t>
{
public:
    static const int _size = 16;
    __m256i xmm;

    native_simd() {}

    native_simd(__m256i val)
        : xmm(val) {}

    native_simd(uint16_t a)
    {
        xmm = _mm256_set1_epi16((short)a);
    }

    operator __m256i() const {
        return xmm;
    }

    constexpr static int size() { return _size; }

    native_simd load(const uint64_t* p) {
        xmm = _mm256_set_epi64x(
            p[0], p[1], p[2], p[3]
        );
        return *this;
    }

    void store(const uint16_t* p) const {
        _mm256_store_si256((__m256i*)p, xmm);
    }

    native_simd operator+(const native_simd b) const {
        return _mm256_add_epi16(xmm, b);
    }

    native_simd& operator+=(const native_simd b) {
        xmm = _mm256_add_epi16(xmm, b);
        return *this;
    }

    native_simd operator-(const native_simd b) const {
        return _mm256_sub_epi16(xmm, b);
    }

    native_simd& operator-=(const native_simd b) {
        xmm = _mm256_sub_epi16(xmm, b);
        return *this;
    }

    std::array<int64_t, _size> popcount() const {
        uint16_t stored[_size];
        store(stored);

        std::array<int64_t, _size> res;
        unroll<int, _size>([&](auto i) {
            res[i] = rapidfuzz::detail::popcount(stored[i]);
        });
        return res;
    }
};

template <>
class native_simd<uint8_t>
{
public:
    static const int _size = 32;
    __m256i xmm;

    native_simd() {}

    native_simd(__m256i val)
        : xmm(val) {}

    native_simd(uint8_t a)
    {
        xmm = _mm256_set1_epi8((char)a);
    }

    operator __m256i() const {
        return xmm;
    }

    constexpr static int size() { return _size; }

    native_simd load(const uint64_t* p) {
        xmm = _mm256_set_epi64x(
            p[0],
            p[1],
            p[2],
            p[3]
        );
        return *this;
    }

    void store(const uint8_t* p) const {
        _mm256_store_si256((__m256i*)p, xmm);
    }

    native_simd operator+(const native_simd b) const {
        return _mm256_add_epi8(xmm, b);
    }

    native_simd& operator+=(const native_simd b) {
        xmm = _mm256_add_epi8(xmm, b);
        return *this;
    }

    native_simd operator-(const native_simd b) const {
        return _mm256_sub_epi8(xmm, b);
    }

    native_simd& operator-=(const native_simd b) {
        xmm = _mm256_sub_epi8(xmm, b);
        return *this;
    }

    std::array<int64_t, _size> popcount() const {
        uint8_t stored[_size];
        store(stored);

        std::array<int64_t, _size> res;
        unroll<int, _size>([&](auto i) {
            res[i] = rapidfuzz::detail::popcount(stored[i]);
        });
        return res;
    }
};

template <typename T>
native_simd<T> operator&(const native_simd<T>& a, const native_simd<T>& b)
{
    return _mm256_and_si256(a, b);
}

template <typename T>
native_simd<T> operator&=(native_simd<T>& a, const native_simd<T>& b)
{
    a = a & b;
    return a;
}

template <typename T>
native_simd<T> operator|(const native_simd<T>& a, const native_simd<T>& b)
{
    return _mm256_or_si256(a, b);
}

template <typename T>
native_simd<T> operator|=(native_simd<T>& a, const native_simd<T>& b)
{
    a = a | b;
    return a;
}

template <typename T>
native_simd<T> operator^(const native_simd<T>& a, const native_simd<T>& b)
{
    return _mm256_xor_si256(a, b);
}

template <typename T>
native_simd<T> operator^=(native_simd<T>& a, const native_simd<T>& b)
{
    a = a ^ b;
    return a;
}

template <typename T>
native_simd<T> operator~(const native_simd<T>& a)
{
    return _mm256_xor_si256(a, _mm256_set1_epi32(-1));
}

} // namespace detail
} // namespace rapidfuzz
