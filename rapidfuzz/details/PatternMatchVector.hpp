/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2022 Max Bachmann */

#pragma once
#include <array>
#include <limits>
#include <stdint.h>
#include <stdio.h>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include <rapidfuzz/details/intrinsics.hpp>

namespace rapidfuzz {
namespace detail {

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
        auto block_count = ceil_div(len, 64);
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

} // namespace detail
} // namespace rapidfuzz