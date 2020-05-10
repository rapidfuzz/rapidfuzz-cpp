/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include <cmath>
#include "types.hpp"
#include "type_traits.hpp"

namespace rapidfuzz { namespace utils {

template<typename CharT>
DecomposedSet<CharT> set_decomposition(string_view_vec<CharT> a, string_view_vec<CharT> b);

percent result_cutoff(const double result, const percent score_cutoff);

template<typename T>
static bool is_zero(T a, T tolerance = std::numeric_limits<T>::epsilon())
{
	return std::fabs(a) <= tolerance;
}


template<
    typename Sentence, typename CharT = char_type<Sentence>,
	typename = std::enable_if_t<is_explicitly_convertible_v<Sentence, basic_string_view<CharT>>>
>
basic_string_view<CharT> to_string_view(const Sentence& str)
{
	return basic_string_view<CharT>(str);
}

template<
    typename Sentence, typename CharT = char_type<Sentence>,
	typename = std::enable_if_t<!is_explicitly_convertible_v<Sentence, basic_string_view<CharT>>>
>
basic_string_view<CharT> to_string_view(Sentence str)
{
	return basic_string_view<CharT>(str.data(), str.size());
}



} /* utils */ } /* rapidfuzz */

#include "utils.txx"
