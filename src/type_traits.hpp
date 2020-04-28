/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include "types.hpp"

#include <iterator>
#include <type_traits>
#include <utility>
#include <functional>

namespace rapidfuzz {

namespace detail {
    template<typename T>
    auto char_type(T const*) -> T;

    template<typename T, typename U = typename T::const_iterator>
    auto char_type(T const&) -> typename std::iterator_traits<U>::value_type;
}

template<typename T>
using char_type = decltype(detail::char_type(std::declval<T const&>()));

template<typename T>
using inner_type = char_type<T>;

template<
    typename Sentence1, typename Sentence2,
	typename Char1 = char_type<Sentence1>,
	typename Char2 = char_type<Sentence2>
>
using IsConvertibleToSameStringView = std::enable_if_t<
        std::is_same<Char1, Char2>::value
     && std::is_convertible<Sentence1 const&, basic_string_view<Char1>>::value
     && std::is_convertible<Sentence2 const&, basic_string_view<Char2>>::value
    >;

template<typename Sentence1, typename Sentence2>
using scorer_func = std::function<percent(const Sentence1&, const Sentence2&, const percent)>;

template<
    typename Sentence, typename CharT = char_type<Sentence>
>
using processor_func = std::function<std::basic_string<CharT>(const Sentence&)>;


} /* rapidfuzz */
