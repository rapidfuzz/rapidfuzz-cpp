/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include <vector>
#include <boost/utility/string_view.hpp>

namespace rapidfuzz {

/* 0.0% - 100.0% */
using percent = double;

template<typename CharT>
using basic_string_view = boost::basic_string_view<CharT>;

template<typename CharT>
using string_view_vec = std::vector<basic_string_view<CharT>>;

template<typename CharT>
struct DecomposedSet {
    string_view_vec<CharT> intersection;
    string_view_vec<CharT> difference_ab;
    string_view_vec<CharT> difference_ba;
    DecomposedSet(string_view_vec<CharT> intersection, string_view_vec<CharT> difference_ab, string_view_vec<CharT> difference_ba)
        : intersection(std::move(intersection))
        , difference_ab(std::move(difference_ab))
        , difference_ba(std::move(difference_ba))
    {}
};

} /* rapidfuzz */
