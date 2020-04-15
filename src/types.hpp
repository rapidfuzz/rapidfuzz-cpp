/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include <vector>

// use standard implementation when available
/*#if __cplusplus >= 201703L
#include <string_view>
#else
#include <boost/utility/string_view.hpp>
#endif*/

#include <boost/utility/string_view.hpp>

namespace rapidfuzz {

/* 0.0% - 100.0% */
using percent = double;

//TODO: Better implement optional support for std::string_view additionaly,
// so existing code does using boost::string_view does not break when switching
// to C++17

/*#if __cplusplus >= 201703L
template<typename CharT>
using basic_string_view = std::basic_string_view<CharT>;
#else
template<typename CharT>
using basic_string_view = boost::basic_string_view<CharT>;
#endif*/

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
