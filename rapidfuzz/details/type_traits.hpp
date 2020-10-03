/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include "types.hpp"

#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace rapidfuzz {

template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

namespace detail {
template <typename T>
auto inner_type(T const*) -> T;

template <typename T>
auto inner_type(T const&) -> typename T::value_type;
} // namespace detail

template <typename T>
using char_type = decltype(detail::inner_type(std::declval<T const&>()));

template <typename... Conds>
struct satisfies_all : std::true_type {};

template <typename Cond, typename... Conds>
struct satisfies_all<Cond, Conds...>
    : std::conditional<Cond::value, satisfies_all<Conds...>, std::false_type>::type {};

template <typename... Conds>
struct satisfies_any : std::false_type {};

template <typename Cond, typename... Conds>
struct satisfies_any<Cond, Conds...>
    : std::conditional<Cond::value, std::true_type, satisfies_any<Conds...>>::type {};

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

#define GENERATE_HAS_MEMBER(member)                                                                \
                                                                                                   \
  template <typename T>                                                                            \
  struct has_member_##member {                                                                     \
  private:                                                                                         \
    using yes = std::true_type;                                                                    \
    using no = std::false_type;                                                                    \
                                                                                                   \
    struct Fallback {                                                                              \
      int member;                                                                                  \
    };                                                                                             \
    struct Derived : T, Fallback {};                                                               \
                                                                                                   \
    template <class U>                                                                             \
    static no test(decltype(U::member)*);                                                          \
    template <typename U>                                                                          \
    static yes test(U*);                                                                           \
                                                                                                   \
    template <typename U, typename = enable_if_t<std::is_class<U>::value>>                         \
    static constexpr bool class_test(U*)                                                           \
    {                                                                                              \
      return std::is_same<decltype(test<Derived>(nullptr)), yes>::value;                           \
    }                                                                                              \
                                                                                                   \
    template <typename U, typename = enable_if_t<!std::is_class<U>::value>>                        \
    static constexpr bool class_test(const U&)                                                     \
    {                                                                                              \
      return false;                                                                                \
    }                                                                                              \
                                                                                                   \
  public:                                                                                          \
    static constexpr bool value = class_test(static_cast<T*>(nullptr));                            \
  };

GENERATE_HAS_MEMBER(data) // Creates 'has_member_data'
GENERATE_HAS_MEMBER(size) // Creates 'has_member_size'

template <typename Sentence>
using has_data_and_size = satisfies_all<has_member_data<Sentence>, has_member_size<Sentence>>;
} // namespace rapidfuzz
