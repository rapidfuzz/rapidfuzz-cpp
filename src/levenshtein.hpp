/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include "utils.hpp"

#include <cmath>
#include <numeric>
#include <vector>

namespace rapidfuzz { namespace levenshtein {

struct WeightTable {
    std::size_t insert_cost;
    std::size_t delete_cost;
    std::size_t replace_cost;
};

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
std::size_t distance(const Sentence1& s1, const Sentence2& s2);


template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
std::size_t weighted_distance(const Sentence1& s1, const Sentence2& s2);

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
std::size_t generic_distance(const Sentence1& s1, const Sentence2& s2, WeightTable weights = { 1, 1, 1 });

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
double normalized_distance(const Sentence1& s1, const Sentence2& s2, const double min_ratio = 0.0);


template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>
>
double normalized_weighted_distance(const Sentence1& s1, const Sentence2& s2, const double min_ratio = 0.0);

} /* levenshtein */ } /* rapidfuzz */

#include "levenshtein.txx"
