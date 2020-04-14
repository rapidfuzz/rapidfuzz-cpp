/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#include "utils.hpp"

#include <algorithm>

namespace rapidfuzz {

template<typename CharT>
DecomposedSet<CharT> utils::set_decomposition(string_view_vec<CharT> a, string_view_vec<CharT> b)
{
    string_view_vec<CharT> intersection;
    string_view_vec<CharT> difference_ab;
    a.erase(std::unique(a.begin(), a.end()), a.end());
    b.erase(std::unique(b.begin(), b.end()), b.end());

    for (const auto& current_a : a) {
        auto element_b = std::find(b.begin(), b.end(), current_a);
        if (element_b != b.end()) {
            b.erase(element_b);
            intersection.push_back(current_a);
        } else {
            difference_ab.push_back(current_a);
        }
    }


    return DecomposedSet<CharT>{ intersection, difference_ab, b };
}

template<typename CharT>
uint64_t utils::bitmap_create(const basic_string_view<CharT>& sentence)
{
    uint64_t bitmap = 0;
    for (const unsigned int& letter : sentence) {
        uint8_t shift = (letter % 16) * 4;

        // make sure there is no overflow when more than 8 characters
        // with the same shift exist
        uint64_t bitmask = static_cast<uint64_t>(0b1111) << shift;
        if ((bitmap & bitmask) != bitmask) {
            bitmap += static_cast<uint64_t>(1) << shift;
        }
    }
    return bitmap;
}

template<typename CharT>
uint64_t utils::bitmap_create(const std::basic_string<CharT>& sentence)
{
    return bitmap_create(basic_string_view<CharT>(sentence));
}

inline percent utils::result_cutoff(const double result, const percent score_cutoff)
{
    return (result >= score_cutoff) ? result : 0;
}

} /* rapidfuzz */