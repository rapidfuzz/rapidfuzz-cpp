/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include <vector>
#include <boost/utility/string_view.hpp>

/* 0.0% - 100.0% */
using percent = double;

template<typename CharT>
using string_view_vec = std::vector<boost::basic_string_view<CharT>>;

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

namespace utils {

template<typename CharT>
DecomposedSet<CharT> set_decomposition(string_view_vec<CharT> a, string_view_vec<CharT> b);

template<typename CharT>
uint64_t bitmap_create(const boost::basic_string_view<CharT>& sentence);

template<typename CharT>
uint64_t bitmap_create(const std::basic_string<CharT>& sentence);

percent result_cutoff(double result, percent score_cutoff);
}

#include "utils.txx"