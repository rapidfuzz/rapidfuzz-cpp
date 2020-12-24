/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#include <algorithm>
#include <array>
#include <cctype>
#include <cwctype>

#include "details/unicode.hpp"

namespace rapidfuzz {

template <typename Sentence, typename CharT, typename>
std::basic_string<CharT> utils::default_process(Sentence&& s)
{
  /* mapping converting
   * - non alphanumeric characters to whitespace (32)
   * - A-Z to a-z
   */
  static const int ascii_mapping[128] = {
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    32, 32, 32, 32, 32, 32, 32,
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    32, 32, 32, 32, 32, 32,
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    32,  32,  32,  32,  32};


  std::basic_string<CharT> str(std::forward<Sentence>(s));

  std::transform(str.begin(), str.end(), str.begin(),
                 [](CharT ch2) {
    int ch = static_cast<int>(ch2);
    return (ch < 128) ? ascii_mapping[ch] : ch; });

  str.erase(str.begin(),
            std::find_if(str.begin(), str.end(), [](const CharT& ch) {return ch != ' '; }));

  str.erase(
      std::find_if(str.rbegin(), str.rend(), [](const CharT& ch) { return ch != ' '; }).base(),
      str.end());

  return str;
}

template <typename Sentence, typename CharT, typename>
std::basic_string<CharT> utils::default_process(Sentence s)
{
  return default_process(std::basic_string<CharT>(s.data(), s.size()));
}

} // namespace rapidfuzz
