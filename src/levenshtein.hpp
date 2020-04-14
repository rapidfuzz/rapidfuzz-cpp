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

enum EditType {
    EditKeep,
    EditReplace,
    EditInsert,
    EditDelete,
};

struct Matrix {
    std::size_t prefix_len;
    std::vector<std::size_t> matrix;
    std::size_t matrix_columns;
    std::size_t matrix_rows;
};

struct MatchingBlock {
    std::size_t first_start;
    std::size_t second_start;
    std::size_t len;
    MatchingBlock(std::size_t first_start, std::size_t second_start, std::size_t len)
        : first_start(first_start)
        , second_start(second_start)
        , len(len)
    {}
};

struct EditOp {
    EditType op_type;
    std::size_t first_start;
    std::size_t second_start;
    EditOp(EditType op_type, std::size_t first_start, std::size_t second_start)
        : op_type(op_type)
        , first_start(first_start)
        , second_start(second_start)
    {}
};


template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>,
    typename = IsConvertibleToSameStringView<Sentence1, Sentence2>
>
Matrix matrix(const Sentence1& s1, const Sentence2& s2);

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>,
    typename = IsConvertibleToSameStringView<Sentence1, Sentence2>
>
std::vector<MatchingBlock> matching_blocks(const Sentence1& s1, const Sentence2& s2);

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>,
    typename = IsConvertibleToSameStringView<Sentence1, Sentence2>
>
std::size_t distance(const Sentence1& s1, const Sentence2& s2);


template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>,
    typename = IsConvertibleToSameStringView<Sentence1, Sentence2>
>
std::size_t weighted_distance(const Sentence1& s1, const Sentence2& s2);

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>,
    typename = IsConvertibleToSameStringView<Sentence1, Sentence2>
>
std::size_t generic_distance(const Sentence1& s1, const Sentence2& s2, WeightTable weights = { 1, 1, 1 });

template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>,
    typename = IsConvertibleToSameStringView<Sentence1, Sentence2>
>
double normalized_distance(const Sentence1& s1, const Sentence2& s2, const double min_ratio = 0.0);


template<
    typename Sentence1, typename Sentence2,
	typename CharT = char_type<Sentence1>,
    typename = IsConvertibleToSameStringView<Sentence1, Sentence2>
>
double normalized_weighted_distance(const Sentence1& s1, const Sentence2& s2, const double min_ratio = 0.0);

} /* levenshtein */ } /* rapidfuzz */

#include "levenshtein.txx"
