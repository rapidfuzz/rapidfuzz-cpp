/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once
#include <array>
#include <cmath>
#include <cstring>
#include <limits>
#include <rapidfuzz/details/SplittedSentenceView.hpp>
#include <rapidfuzz/details/intrinsics.hpp>
#include <rapidfuzz/details/type_traits.hpp>
#include <rapidfuzz/details/types.hpp>
#include <unordered_set>
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

static inline double NormSim_to_NormDist(double score_cutoff, double imprecision = 0.00001)
{
    return std::min(1.0, 1.0 - score_cutoff + imprecision);
}

static inline void assume(bool b)
{
#if defined(__clang__)
    __builtin_assume(b);
#elif defined(__GNUC__) || defined(__GNUG__)
    if (!b) {
        __builtin_unreachable();
    }
#elif defined(_MSC_VER)
    __assume(b);
#endif
}

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
    return result_cutoff(
        (lensum > 0) ? (max - max * static_cast<double>(dist) / static_cast<double>(lensum)) : max,
        score_cutoff);
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

template <typename InputIt1, typename InputIt2>
StringAffix remove_common_affix(InputIt1& first1, InputIt1& last1, InputIt2& first2,
                                InputIt2& last2);

template <typename InputIt1, typename InputIt2>
size_t remove_common_prefix(InputIt1& first1, InputIt1 last1, InputIt2& first2, InputIt2 last2);

template <typename InputIt1, typename InputIt2>
size_t remove_common_suffix(InputIt1 first1, InputIt1& last1, InputIt2 first2, InputIt2& last2);

template <typename InputIt, typename CharT = iter_value_t<InputIt>>
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

struct BitvectorHashmap {
    BitvectorHashmap() : m_map()
    {}

    template <typename CharT>
    void insert(CharT key, int64_t pos) noexcept
    {
        insert_mask(key, UINT64_C(1) << pos);
    }

    template <typename CharT>
    void insert_mask(CharT key, uint64_t mask) noexcept
    {
        uint32_t i = lookup(static_cast<uint64_t>(key));
        m_map[i].key = static_cast<uint64_t>(key);
        m_map[i].value |= mask;
    }

    template <typename CharT>
    uint64_t get(CharT key) const noexcept
    {
        return m_map[lookup(static_cast<uint64_t>(key))].value;
    }

private:
    /**
     * lookup key inside the hashmap using a similar collision resolution
     * strategy to CPython and Ruby
     */
    uint32_t lookup(uint64_t key) const noexcept
    {
        uint32_t i = key % 128;

        if (!m_map[i].value || m_map[i].key == key) {
            return i;
        }

        uint64_t perturb = key;
        while (true) {
            i = (static_cast<uint64_t>(i) * 5 + perturb + 1) % 128;
            if (!m_map[i].value || m_map[i].key == key) {
                return i;
            }

            perturb >>= 5;
        }
    }

    struct MapElem {
        uint64_t key = 0;
        uint64_t value = 0;
    };
    std::array<MapElem, 128> m_map;
};

struct PatternMatchVector {
    PatternMatchVector() : m_extendedAscii()
    {}

    template <typename InputIt>
    PatternMatchVector(InputIt first, InputIt last) : m_extendedAscii()
    {
        insert(first, last);
    }

    template <typename InputIt>
    void insert(InputIt first, InputIt last) noexcept
    {
        uint64_t mask = 1;
        for (; first != last; ++first) {
            insert_mask(*first, mask);
            mask <<= 1;
        }
    }

    template <typename CharT>
    void insert(CharT key, int64_t pos) noexcept
    {
        insert_mask(key, UINT64_C(1) << pos);
    }

    uint64_t get(char key) const noexcept
    {
        /** treat char as value between 0 and 127 for performance reasons */
        return m_extendedAscii[static_cast<uint8_t>(key)];
    }

    template <typename CharT>
    uint64_t get(CharT key) const noexcept
    {
        /** treat char as value between 0 and 127 for performance reasons */
        if (key >= 0 && key <= 255)
            return m_extendedAscii[static_cast<uint8_t>(key)];
        else
            return m_map.get(key);
    }

    template <typename CharT>
    uint64_t get(size_t block, CharT key) const noexcept
    {
        assert(block == 0);
        (void)block;
        return get(key);
    }

    void insert_mask(char key, uint64_t mask) noexcept
    {
        /** treat char as value between 0 and 127 for performance reasons */
        m_extendedAscii[static_cast<uint8_t>(key)] |= mask;
    }

    template <typename CharT>
    void insert_mask(CharT key, uint64_t mask) noexcept
    {
        if (key >= 0 && key <= 255)
            m_extendedAscii[static_cast<uint8_t>(key)] |= mask;
        else
            m_map.insert_mask(key, mask);
    }

private:
    BitvectorHashmap m_map;
    std::array<uint64_t, 256> m_extendedAscii;
};

struct BlockPatternMatchVector {
    std::vector<PatternMatchVector> m_val;

    BlockPatternMatchVector() = default;

    template <typename InputIt>
    BlockPatternMatchVector(InputIt first, InputIt last)
    {
        insert(first, last);
    }

    size_t size() const noexcept
    {
        return m_val.size();
    }

    template <typename CharT>
    void insert(size_t block, CharT ch, int pos)
    {
        auto* be = &m_val[block];
        be->insert(ch, pos);
    }

    /**
     * @warning undefined behavior if iterator \p first is greater than \p last
     * @tparam InputIt
     * @param first
     * @param last
     */
    template <typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        auto len = std::distance(first, last);
        auto block_count = detail::ceil_div(len, 64);
        m_val.resize(static_cast<size_t>(block_count));

        for (ptrdiff_t block = 0; block < block_count; ++block) {
            if (std::distance(first + block * 64, last) > 64) {
                m_val[static_cast<size_t>(block)].insert(first + block * 64,
                                                         first + (block + 1) * 64);
            }
            else {
                m_val[static_cast<size_t>(block)].insert(first + block * 64, last);
            }
        }
    }

    template <typename CharT>
    uint64_t get(size_t block, CharT ch) const
    {
        auto* be = &m_val[block];
        return be->get(ch);
    }
};

template <typename CharT1, size_t size = sizeof(CharT1)>
struct CharSet;

template <typename CharT1>
struct CharSet<CharT1, 1> {
    using UCharT1 = typename std::make_unsigned<CharT1>::type;

    std::array<bool, std::numeric_limits<UCharT1>::max() + 1> m_val;

    CharSet() : m_val{}
    {}

    void insert(CharT1 ch)
    {
        m_val[UCharT1(ch)] = true;
    }

    template <typename CharT2>
    bool find(CharT2 ch) const
    {
        if (!CanTypeFitValue<CharT1>(ch)) {
            return false;
        }

        return m_val[UCharT1(ch)];
    }
};

template <typename CharT1, size_t size>
struct CharSet {
    std::unordered_set<CharT1> m_val;

    CharSet() : m_val{}
    {}

    void insert(CharT1 ch)
    {
        m_val.insert(ch);
    }

    template <typename CharT2>
    bool find(CharT2 ch) const
    {
        if (!CanTypeFitValue<CharT1>(ch)) {
            return false;
        }

        return m_val.find(CharT1(ch)) != m_val.end();
    }
};

/**@}*/

} // namespace common
} // namespace rapidfuzz

#include <rapidfuzz/details/common_impl.hpp>
