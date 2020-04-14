/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include "types.hpp"
#include "type_traits.hpp"

#include <string>

namespace rapidfuzz { namespace utils {

template<typename CharT>
DecomposedSet<CharT> set_decomposition(string_view_vec<CharT> a, string_view_vec<CharT> b);

template<typename CharT>
uint64_t bitmap_create(const basic_string_view<CharT>& sentence);

template<typename CharT>
uint64_t bitmap_create(const std::basic_string<CharT>& sentence);

percent result_cutoff(const double result, const percent score_cutoff);

} /* utils */ } /* rapidfuzz */

#include "utils.txx"
