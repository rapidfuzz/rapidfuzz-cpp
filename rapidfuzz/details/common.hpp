/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once
#include <array>
#include <cmath>
#include <cstring>
#include <rapidfuzz/details/SplittedSentenceView.hpp>
#include <rapidfuzz/details/type_traits.hpp>
#include <rapidfuzz/details/types.hpp>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2, typename InputIt3>
struct DecomposedSet {
    SplittedSentenceView<InputIt1> difference_ab;
    SplittedSentenceView<InputIt2> difference_ba;
    SplittedSentenceView<InputIt3> intersection;
    DecomposedSet(SplittedSentenceView<InputIt1> diff_ab, SplittedSentenceView<InputIt2> diff_ba,
                  SplittedSentenceView<InputIt3> intersect)
        : difference_ab(std::move(diff_ab)),
          difference_ba(std::move(diff_ba)),
          intersection(std::move(intersect))
    {}
};

namespace common {

/**
 * @defgroup Common Common
 * Common utilities shared among multiple functions
 * @{
 */

template <typename InputIt1, typename InputIt2>
DecomposedSet<InputIt1, InputIt2, InputIt1> set_decomposition(SplittedSentenceView<InputIt1> a,
                                                              SplittedSentenceView<InputIt2> b);

constexpr double result_cutoff(double result, double score_cutoff)
{
    return (result >= score_cutoff) ? result : 0;
}

template <int Max = 1>
constexpr double norm_distance(int64_t dist, int64_t lensum, double score_cutoff = 0)
{
    double max = static_cast<double>(Max);
    return result_cutoff((lensum > 0) ? (max - max * dist / lensum) : max, score_cutoff);
}

template <int Max = 1>
static inline int64_t score_cutoff_to_distance(double score_cutoff, int64_t lensum)
{
    return static_cast<int64_t>(
        std::ceil(static_cast<double>(lensum) * (1.0 - score_cutoff / Max)));
}

template <typename T>
constexpr bool is_zero(T a, T tolerance = std::numeric_limits<T>::epsilon())
{
    return std::fabs(a) <= tolerance;
}

template <typename Sentence, typename CharT = char_type<Sentence>,
          typename = std::enable_if_t<
              is_explicitly_convertible<Sentence, std::basic_string<CharT>>::value>>
std::basic_string<CharT> to_string(Sentence&& str);

template <typename Sentence, typename CharT = char_type<Sentence>,
          typename = std::enable_if_t<
              !is_explicitly_convertible<Sentence, std::basic_string<CharT>>::value &&
              has_data_and_size<Sentence>::value>>
std::basic_string<CharT> to_string(const Sentence& str);

template <typename CharT>
CharT* to_begin(CharT* s)
{
    return s;
}

template <typename T>
auto to_begin(T& x)
{
    using std::begin;
    return begin(x);
}

template <typename CharT>
CharT* to_end(CharT* s)
{
    while (*s != 0) {
        ++s;
    }
    return s;
}

template <typename T>
auto to_end(T& x)
{
    using std::end;
    return end(x);
}

/**
 * @brief Finds the first mismatching pair of elements from two ranges:
 * one defined by [first1, last1) and another defined by [first2,last2).
 * Similar implementation to std::mismatch from C++14
 *
 * @param first1, last1	-	the first range of the elements
 * @param first2, last2	-	the second range of the elements
 *
 * @return std::pair with iterators to the first two non-equal elements.
 */
template <typename InputIterator1, typename InputIterator2>
std::pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1,
                                                   InputIterator2 first2, InputIterator2 last2);

template <typename InputIt1, typename InputIt2>
StringAffix remove_common_affix(InputIt1& first1, InputIt1& last1, InputIt2& first2,
                                InputIt2& last2);

template <typename InputIt1, typename InputIt2>
int64_t remove_common_prefix(InputIt1& first1, InputIt1 last1, InputIt2& first2, InputIt2 last2);

template <typename InputIt1, typename InputIt2>
int64_t remove_common_suffix(InputIt1 first1, InputIt1& last1, InputIt2 first2, InputIt2& last2);

template <typename InputIt, typename CharT = iterator_type<InputIt>>
SplittedSentenceView<InputIt> sorted_split(InputIt first, InputIt last);

template <typename T>
constexpr auto to_unsigned(T value) -> typename std::make_unsigned<T>::type
{
    return typename std::make_unsigned<T>::type(value);
}

template <typename T>
constexpr auto to_signed(T value) -> typename std::make_unsigned<T>::type
{
    return typename std::make_signed<T>::type(value);
}

/*
 * taken from https://stackoverflow.com/a/17251989/11335032
 */
template <typename T, typename U>
bool CanTypeFitValue(const U value)
{
    const intmax_t botT = intmax_t(std::numeric_limits<T>::min());
    const intmax_t botU = intmax_t(std::numeric_limits<U>::min());
    const uintmax_t topT = uintmax_t(std::numeric_limits<T>::max());
    const uintmax_t topU = uintmax_t(std::numeric_limits<U>::max());
    return !((botT > botU && value < static_cast<U>(botT)) ||
             (topT < topU && value > static_cast<U>(topT)));
}

struct PatternMatchVector {
    struct MapElem {
        uint64_t key = 0;
        uint64_t value = 0;
    };
    std::array<MapElem, 128> m_map;
    std::array<uint64_t, 256> m_extendedAscii;

    PatternMatchVector() : m_map(), m_extendedAscii()
    {}

    template <typename InputIt>
    PatternMatchVector(InputIt first, InputIt last) : m_map(), m_extendedAscii()
    {
        insert(first, last);
    }

    template <typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        uint64_t mask = 1;
        for (; first != last; ++first) {
            insert_mask(*first, mask);
            mask <<= 1;
        }
    }

    template <typename CharT>
    void insert(CharT key, int64_t pos)
    {
        insert_mask(key, 1ull << pos);
    }

    template <typename CharT>
    uint64_t get(CharT key) const
    {
        if (key >= 0 && key <= 255) {
            return m_extendedAscii[(uint8_t)key];
        }
        else {
            return m_map[lookup((uint64_t)key)].value;
        }
    }

    template <typename CharT>
    uint64_t get(int64_t block, CharT key) const
    {
        assert(block == 0);
        (void)block;
        return get(key);
    }

private:
    template <typename CharT>
    void insert_mask(CharT key, uint64_t mask)
    {
        if (key >= 0 && key <= 255) {
            m_extendedAscii[(uint8_t)key] |= mask;
        }
        else {
            int64_t i = lookup((uint64_t)key);
            m_map[i].key = key;
            m_map[i].value |= mask;
        }
    }

    /**
     * lookup key inside the hasmap using a similar collision resolution
     * strategy to CPython and Ruby
     */
    int64_t lookup(uint64_t key) const
    {
        int64_t i = key % 128;

        if (!m_map[i].value || m_map[i].key == key) {
            return i;
        }

        int64_t perturb = key;
        while (true) {
            i = ((i * 5) + perturb + 1) % 128;
            if (!m_map[i].value || m_map[i].key == key) {
                return i;
            }

            perturb >>= 5;
        }
    }
};

struct BlockPatternMatchVector {
    std::vector<PatternMatchVector> m_val;

    BlockPatternMatchVector()
    {}

    template <typename InputIt>
    BlockPatternMatchVector(InputIt first, InputIt last)
    {
        insert(first, last);
    }

    template <typename CharT>
    void insert(int64_t block, CharT ch, int pos)
    {
        auto* be = &m_val[block];
        be->insert(ch, pos);
    }

    template <typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        int64_t len = std::distance(first, last);
        int64_t block_count = (len / 64) + bool(len % 64);
        m_val.resize(block_count);

        for (int64_t block = 0; block < block_count; ++block) {
            if (std::distance(first + block * 64, last) > 64) {
                m_val[block].insert(first + block * 64, first + (block + 1) * 64);
            }
            else {
                m_val[block].insert(first + block * 64, last);
            }
        }
    }

    template <typename CharT>
    uint64_t get(int64_t block, CharT ch) const
    {
        auto* be = &m_val[block];
        return be->get(ch);
    }
};

template <typename CharT1, typename ValueType, int64_t size = sizeof(CharT1)>
struct CharHashTable;

template <typename CharT1, typename ValueType>
struct CharHashTable<CharT1, ValueType, 1> {
    using UCharT1 = typename std::make_unsigned<CharT1>::type;

    std::array<ValueType, std::numeric_limits<UCharT1>::max() + 1> m_val;
    ValueType m_default;

    CharHashTable() : m_val{}, m_default{}
    {}

    ValueType& create(CharT1 ch)
    {
        return m_val[UCharT1(ch)];
    }

    template <typename CharT2>
    const ValueType& operator[](CharT2 ch) const
    {
        if (!CanTypeFitValue<CharT1>(ch)) {
            return m_default;
        }

        return m_val[UCharT1(ch)];
    }
};

template <typename CharT1, typename ValueType, int64_t size>
struct CharHashTable {
    std::unordered_map<CharT1, ValueType> m_val;
    ValueType m_default;

    CharHashTable() : m_val{}, m_default{}
    {}

    ValueType& create(CharT1 ch)
    {
        return m_val[ch];
    }

    template <typename CharT2>
    const ValueType& operator[](CharT2 ch) const
    {
        if (!CanTypeFitValue<CharT1>(ch)) {
            return m_default;
        }

        auto search = m_val.find(CharT1(ch));
        if (search == m_val.end()) {
            return m_default;
        }

        return search->second;
    }
};

template <typename T>
struct MatrixVectorView {
    explicit MatrixVectorView(T* vector, int64_t cols) : m_vector(vector), m_cols(cols)
    {}

    T& operator[](uint64_t col)
    {
        assert(col < m_cols);
        return m_vector[col];
    }

private:
    T* m_vector;
    int64_t m_cols;
};

template <typename T>
struct ConstMatrixVectorView {
    explicit ConstMatrixVectorView(const T* vector, int64_t cols) : m_vector(vector), m_cols(cols)
    {}

    ConstMatrixVectorView(const MatrixVectorView<T>& other) : m_vector(other.m_vector)
    {}

    const T& operator[](uint64_t col)
    {
        assert(col < m_cols);
        return m_vector[col];
    }

private:
    const T* m_vector;
    int64_t m_cols;
};

template <typename T>
struct Matrix {
    Matrix(uint64_t rows, uint64_t cols, uint64_t val)
    {
        m_rows = rows;
        m_cols = cols;
        if (rows * cols > 0) {
            m_matrix = new T[rows * cols];
            std::fill_n(m_matrix, rows * cols, val);
        }
        else {
            m_matrix = nullptr;
        }
    }

    ~Matrix()
    {
        delete[] m_matrix;
    }

    MatrixVectorView<uint64_t> operator[](uint64_t row)
    {
        assert(row < m_rows);
        return MatrixVectorView<uint64_t>(&m_matrix[row * m_cols], m_rows);
    }

    ConstMatrixVectorView<uint64_t> operator[](uint64_t row) const
    {
        assert(row < m_rows);
        return ConstMatrixVectorView<uint64_t>(&m_matrix[row * m_cols], m_rows);
    }

private:
    uint64_t m_rows;
    uint64_t m_cols;
    T* m_matrix;
};

/**@}*/

} // namespace common
} // namespace rapidfuzz

#include <rapidfuzz/details/common_impl.hpp>
