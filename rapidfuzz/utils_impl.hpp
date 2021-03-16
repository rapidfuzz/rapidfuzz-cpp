/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#include <algorithm>
#include <array>
#include <cctype>
#include <cwctype>

#include <rapidfuzz/details/unicode.hpp>

namespace rapidfuzz {

//uint32_t UnicodeDefaultProcess(uint32_t ch);

template <typename Sentence, typename CharT, typename>
std::basic_string<CharT> utils::default_process(Sentence&& s)
{
  /* mapping converting
   * - non alphanumeric characters to whitespace (32)
   * - alphanumeric characters to lowercase
   *
   * generated using
   * `[ord(chr(x).lower()) if chr(x).isalnum() else 0x20 for x in range(256)]`
   * in Python
   */
  static const int extended_ascii_mapping[256] = {
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 32, 32,
    32, 32, 32, 32, 32, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 32, 32, 32, 32,
    32, 32, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 170, 32, 32, 32, 32, 32, 32, 32, 178, 179, 32, 181, 32, 32, 32, 185, 186, 32,
    188, 189, 190, 32, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236,
    237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 32, 248, 249, 250, 251, 252, 253,
    254, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238,
    239, 240, 241, 242, 243, 244, 245, 246, 32, 248, 249, 250, 251, 252, 253, 254, 255
  };


  std::basic_string<CharT> str(std::forward<Sentence>(s));

  // use direct mapping for extended Asciis
  if (sizeof(CharT) == 1) {
    std::transform(str.begin(), str.end(), str.begin(),
                   [](CharT ch2) {
      int ch = static_cast<int>(ch2);
      return extended_ascii_mapping[ch];
      });
  } else {
    std::transform(str.begin(), str.end(), str.begin(),
                 [](CharT ch2) {
      uint32_t ch = static_cast<uint32_t>(ch2);
      return (ch < 256) ? extended_ascii_mapping[ch] : Unicode::UnicodeDefaultProcess(ch2);
    });
  }


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
