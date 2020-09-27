/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include "types.hpp"

#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace rapidfuzz {

#if rapidfuzz_CPP14_OR_GREATER
template< bool B, class T = void >
using enable_if_t = std::enable_if_t<B,T>;
#else
template< bool B, class T = void >
using enable_if_t = typename std::enable_if<B,T>::type;
#endif

namespace detail {
template <typename T>
auto inner_type(T const*) -> T;

template <typename T, typename U = typename T::const_iterator>
auto inner_type(T const&) -> typename std::iterator_traits<U>::value_type;
} // namespace detail

template <typename T>
using inner_type = decltype(detail::inner_type(std::declval<T const&>()));

template <typename T>
using char_type = inner_type<T>;

// taken from
// https://stackoverflow.com/questions/16893992/check-if-type-can-be-explicitly-converted
template <typename From, typename To>
struct is_explicitly_convertible {
  template <typename T>
  static void f(T);

  template <typename F, typename T>
  static constexpr auto test(int) -> decltype(f(static_cast<T>(std::declval<F>())), true)
  {
    return true;
  }

  template <typename F, typename T>
  static constexpr auto test(...) -> bool
  {
    return false;
  }

  static bool const value = test<From, To>(0);
};

// taken from
// https://stackoverflow.com/questions/16803814/how-do-i-return-the-largest-type-in-a-list-of-types
template <typename... Ts>
struct largest_type;

template <typename T>
struct largest_type<T> {
  using type = T;
};

template <typename T, typename U, typename... Ts>
struct largest_type<T, U, Ts...> {
  using type =
      typename largest_type<typename std::conditional<(sizeof(U) <= sizeof(T)), T, U>::type,
                            Ts...>::type;
};


// taken from https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Member_Detector
#define GENERATE_HAS_MEMBER(member)                                                    \
                                                                                       \
template < class T >                                                                   \
class HasMember_##member                                                               \
{                                                                                      \
private:                                                                               \
    using Yes = char[2];                                                               \
    using  No = char[1];                                                               \
                                                                                       \
    struct Fallback { int member; };                                                   \
    struct Derived : T, Fallback { };                                                  \
                                                                                       \
    template < class U >                                                               \
    static No& test ( decltype(U::member)* );                                          \
    template < typename U >                                                            \
    static Yes& test ( U* );                                                           \
                                                                                       \
public:                                                                                \
    static constexpr bool RESULT = sizeof(test<Derived>(nullptr)) == sizeof(Yes);      \
};                                                                                     \
/* compililation would fail when T is no class, so it should be disabled */            \
template < class T, typename = typename std::enable_if<std::is_class<T>::value>::type> \
struct has_member_##member                                                             \
: public std::integral_constant<bool, HasMember_##member<T>::RESULT>                   \
{                                                                                      \
};


GENERATE_HAS_MEMBER(data)  // Creates 'has_member_data'
GENERATE_HAS_MEMBER(size)  // Creates 'has_member_size'

/*
template<typename T>
struct has_size_method
{
private:
	typedef std::true_type yes;
	typedef std::false_type no;
 
	template<typename U> static auto test(int) -> decltype(std::declval<U>().size() == 1, yes());
 
	template<typename> static no test(...);
 
public:
	static constexpr bool value = std::is_same<decltype(test<T>(0)),yes>::value;
};*/

} // namespace rapidfuzz
