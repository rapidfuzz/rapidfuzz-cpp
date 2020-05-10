/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include "types.hpp"

#include <boost/callable_traits.hpp>
#include <iterator>
#include <type_traits>
#include <utility>
#include <functional>

namespace rapidfuzz {

using boost::callable_traits::is_invocable;

// the version used by boost currently has a bug https://github.com/boostorg/callable_traits/issues/184
template <class Fn, class... ArgTypes>
constexpr bool is_invocable_v = is_invocable<Fn, ArgTypes...>::value;

namespace detail {
    template<typename T>
    auto inner_type(T const*) -> T;

    template<typename T, typename U = typename T::const_iterator>
    auto inner_type(T const&) -> typename std::iterator_traits<U>::value_type;
}

template<typename T>
using inner_type = decltype(detail::inner_type(std::declval<T const&>()));

template<typename T>
using char_type = inner_type<T>;


// taken from https://stackoverflow.com/questions/16893992/check-if-type-can-be-explicitly-converted
template<typename From, typename To>
struct is_explicitly_convertible
{
    template<typename T>
    static void f(T);

    template<typename F, typename T>
    static constexpr auto test(int) -> 
    decltype(f(static_cast<T>(std::declval<F>())),true) {
        return true;
    }

    template<typename F, typename T>
    static constexpr auto test(...) -> bool {
        return false;
    }

    static bool const value = test<From,To>(0);
};

template<typename From, typename To>
constexpr bool is_explicitly_convertible_v = is_explicitly_convertible<From, To>::value;

} /* rapidfuzz */
