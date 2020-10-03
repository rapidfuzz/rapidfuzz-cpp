/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once

#include <type_traits>
#include <vector>

#include "string_view.hpp"

namespace rapidfuzz {

/* 0.0% - 100.0% */
using percent = double;

template <typename CharT>
using string_view_vec = std::vector<basic_string_view<CharT>>;

struct StringAffix {
  std::size_t prefix_len;
  std::size_t suffix_len;
};

} // namespace rapidfuzz
