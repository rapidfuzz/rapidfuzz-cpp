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

inline percent utils::result_cutoff(const double result, const percent score_cutoff)
{
    return (result >= score_cutoff) ? result : 0;
}

} /* rapidfuzz */