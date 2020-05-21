/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include <nonstd/string_view.hpp>
#include <vector>

namespace rapidfuzz {

/* 0.0% - 100.0% */
using percent = double;

template <typename CharT>
using basic_string_view = nonstd::basic_string_view<CharT>;

template <typename CharT>
using string_view_vec = std::vector<basic_string_view<CharT>>;

template <typename CharT> struct DecomposedSet {
  string_view_vec<CharT> intersection;
  string_view_vec<CharT> difference_ab;
  string_view_vec<CharT> difference_ba;
  DecomposedSet(string_view_vec<CharT> intersect,
                string_view_vec<CharT> diff_ab, string_view_vec<CharT> diff_ba)
      : intersection(std::move(intersect)),
        difference_ab(std::move(diff_ab)),
        difference_ba(std::move(diff_ba))
  {}
};

struct StringAffix {
  std::size_t prefix_len;
  std::size_t suffix_len;
};

} // namespace rapidfuzz
