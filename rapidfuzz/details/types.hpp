/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once

#include <type_traits>
#include <vector>
#include "constants.hpp"



#if rapidfuzz_USES_STD_STRING_VIEW
  #include <string_view>
#else
  #include <boost/utility/string_view.hpp>
#endif


namespace rapidfuzz {

/* 0.0% - 100.0% */
using percent = double;



#if rapidfuzz_USES_STD_STRING_VIEW
  template <typename CharT>
  using basic_string_view = std::basic_string_view<CharT>;
#else
  template <typename CharT>
  using basic_string_view = boost::basic_string_view<CharT>;
#endif

template <typename CharT>
using string_view_vec = std::vector<basic_string_view<CharT>>;

struct StringAffix {
  std::size_t prefix_len;
  std::size_t suffix_len;
};

} // namespace rapidfuzz
