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
template <bool B, class T = void>
using enable_if_t = std::enable_if_t<B, T>;
#else
template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;
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

template <typename Sentence, typename CharT = char_type<Sentence>>
using is_convertible_to_string_view =
    satisfies_any<is_explicitly_convertible<Sentence, basic_string_view<CharT>>,
                  has_data_and_size<Sentence>>;

template <typename... Sentence>
using are_convertible_to_string_view = satisfies_all<is_convertible_to_string_view<Sentence>...>;

} // namespace rapidfuzz
