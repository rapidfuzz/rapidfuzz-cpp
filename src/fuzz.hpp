/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */
/* Copyright © 2011 Adam Cohen */

#pragma once
#include "string_utils.hpp"
#include "utils.hpp"

#include <type_traits>

namespace rapidfuzz { namespace fuzz {

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
percent ratio(const Sentence1 &s1, const Sentence2 &s2, const percent score_cutoff = 0);

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
percent partial_ratio(const Sentence1 &s1, const Sentence2 &s2, percent score_cutoff = 0);

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
percent token_sort_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
percent partial_token_sort_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);


template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
percent token_set_ratio(const Sentence1& s1, const Sentence2& s2, const percent score_cutoff = 0);

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
percent partial_token_set_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
percent token_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);


template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
percent partial_token_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
percent length_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
percent quick_lev_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
percent WRatio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

} /* fuzz */ } /* rapidfuzz */

#include "fuzz.txx"
