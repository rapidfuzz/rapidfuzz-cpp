/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2022 Max Bachmann */

#pragma once

#include <array>
#include <iterator>
#include <new>
#include <stdint.h>

namespace rapidfuzz {
namespace detail {

/* hashmap for integers which can only grow, but can't remove elements */
template <typename T_Key, typename T_Entry, T_Entry _empty_val = T_Entry()>
struct GrowingHashmap {
    using key_type = T_Key;
    using value_type = T_Entry;
    using size_type = unsigned int;

private:
    static constexpr size_type min_size = 8;
    struct MapElem {
        key_type key;
        value_type value = _empty_val;
    };

    int used;
    int fill;
    int mask;
    MapElem* m_map;

public:
    GrowingHashmap() : used(0), fill(0), mask(-1), m_map(NULL)
    {}
    ~GrowingHashmap()
    {
        delete[] m_map;
    }

    GrowingHashmap(const GrowingHashmap& other) : used(other.used), fill(other.fill), mask(other.mask)
    {
        int size = mask + 1;
        m_map = new MapElem[size];
        std::copy(other.m_map, other.m_map + size, m_map);
    }

    GrowingHashmap(GrowingHashmap&& other) noexcept : GrowingHashmap()
    {
        swap(*this, other);
    }

    GrowingHashmap& operator=(GrowingHashmap other)
    {
        swap(*this, other);
        return *this;
    }

    friend void swap(GrowingHashmap& first, GrowingHashmap& second) noexcept
    {
        std::swap(first.used, second.used);
        std::swap(first.fill, second.fill);
        std::swap(first.mask, second.mask);
        std::swap(first.m_map, second.m_map);
    }

    size_type size() const
    {
        return used;
    }
    size_type capacity() const
    {
        return mask + 1;
    }
    bool empty() const
    {
        return used == 0;
    }

    value_type get(key_type key) const noexcept
    {
        if (m_map == NULL) return _empty_val;

        return m_map[lookup(static_cast<size_t>(key))].value;
    }

    void insert(key_type key, value_type val)
    {
        if (m_map == NULL) allocate();

        size_t i = lookup(static_cast<size_t>(key));

        if (m_map[i].value == _empty_val) {
            /* resize when 2/3 full */
            if (++fill * 3 >= (mask + 1) * 2) {
                grow((used + 1) * 2);
                i = lookup(static_cast<size_t>(key));
            }

            used++;
        }

        m_map[i].key = key;
        m_map[i].value = val;
    }

private:
    void allocate()
    {
        mask = min_size - 1;
        m_map = new MapElem[min_size];
    }

    /**
     * lookup key inside the hashmap using a similar collision resolution
     * strategy to CPython and Ruby
     */
    size_t lookup(size_t key) const
    {
        size_t i = key & static_cast<size_t>(mask);

        if (m_map[i].value == _empty_val || m_map[i].key == key) return i;

        size_t perturb = key;
        while (true) {
            i = (i * 5 + perturb + 1) & static_cast<size_t>(mask);
            if (m_map[i].value == _empty_val || m_map[i].key == key) return i;

            perturb >>= 5;
        }
    }

    void grow(int minUsed)
    {
        int newSize = mask + 1;
        while (newSize <= minUsed)
            newSize <<= 1;

        MapElem* oldMap = m_map;
        m_map = new MapElem[static_cast<size_t>(newSize)];

        fill = used;
        mask = newSize - 1;

        for (int i = 0; used > 0; i++)
            if (oldMap[i].value != _empty_val) {
                size_t j = lookup(static_cast<size_t>(oldMap[i].key));

                m_map[j].key = oldMap[i].key;
                m_map[j].value = oldMap[i].value;
                used--;
            }

        used = fill;
        delete[] oldMap;
    }
};

template <typename T_Key, typename T_Entry, T_Entry _empty_val = T_Entry()>
struct HybridGrowingHashmap {
    using key_type = T_Key;
    using value_type = T_Entry;

    HybridGrowingHashmap()
    {
        m_extendedAscii.fill(_empty_val);
    }

    value_type get(char key) const noexcept
    {
        /** treat char as value between 0 and 127 for performance reasons */
        return m_extendedAscii[static_cast<uint8_t>(key)];
    }

    template <typename CharT>
    value_type get(CharT key) const noexcept
    {
        if (key >= 0 && key <= 255)
            return m_extendedAscii[static_cast<uint8_t>(key)];
        else
            return m_map.get(static_cast<key_type>(key));
    }

    value_type insert(char key, value_type val) noexcept
    {
        /** treat char as value between 0 and 127 for performance reasons */
        m_extendedAscii[static_cast<uint8_t>(key)] = val;
    }

    template <typename CharT>
    void insert(CharT key, value_type val)
    {
        if (key >= 0 && key <= 255)
            m_extendedAscii[static_cast<uint8_t>(key)] = val;
        else
            m_map.insert(static_cast<key_type>(key), val);
    }

private:
    GrowingHashmap<key_type, value_type, _empty_val> m_map;
    std::array<value_type, 256> m_extendedAscii;
};

} // namespace detail
} // namespace rapidfuzz