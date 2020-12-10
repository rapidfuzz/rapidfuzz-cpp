/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#include <algorithm>
#include <array>
#include <cctype>
#include <cwctype>

#include "details/unicode.hpp"

namespace rapidfuzz {

template <typename CharT1, typename CharT2>
bool string_view_eq(basic_string_view<CharT1> x, basic_string_view<CharT2> y)
{
  if (x.size() != y.size()) return false;

  for (std::size_t i = 0; i < x.size(); ++i) {
    if (x[i] != y[i]) return false;
  }
  return true;
}

template <typename CharT1, typename CharT2>
DecomposedSet<CharT1, CharT2, CharT1> utils::set_decomposition(SplittedSentenceView<CharT1> a,
                                                               SplittedSentenceView<CharT2> b)
{
  a.dedupe();
  b.dedupe();

  string_view_vec<CharT1> intersection;
  string_view_vec<CharT1> difference_ab;
  string_view_vec<CharT2> difference_ba = b.words();

  for (const auto& current_a : a.words()) {
    auto element_b = std::find_if(difference_ba.begin(), difference_ba.end(),
                                  [current_a](basic_string_view<CharT2> current_b) {
                                    return string_view_eq(current_a, current_b);
                                  });

    if (element_b != difference_ba.end()) {
      difference_ba.erase(element_b);
      intersection.push_back(current_a);
    }
    else {
      difference_ab.push_back(current_a);
    }
  }

  return {difference_ab, difference_ba, intersection};
}

constexpr percent utils::result_cutoff(const double result, const percent score_cutoff)
{
  return (result >= score_cutoff) ? result : 0;
}

constexpr percent utils::norm_distance(std::size_t dist, std::size_t lensum, percent score_cutoff)
{
  return result_cutoff(
    100.0 - 100 * static_cast<double>(dist) / static_cast<double>(lensum),
    score_cutoff
  );
}

template <typename T>
constexpr bool utils::is_zero(T a, T tolerance)
{
  return std::fabs(a) <= tolerance;
}

template <typename Sentence, typename CharT, typename>
basic_string_view<CharT> utils::to_string_view(Sentence&& str)
{
  return basic_string_view<CharT>(std::forward<Sentence>(str));
}

template <typename Sentence, typename CharT, typename>
basic_string_view<CharT> utils::to_string_view(const Sentence& str)
{
  return basic_string_view<CharT>(str.data(), str.size());
}

template <typename Sentence, typename CharT, typename>
std::basic_string<CharT> utils::to_string(Sentence&& str)
{
  return std::basic_string<CharT>(std::forward<Sentence>(str));
}

template <typename Sentence, typename CharT, typename>
std::basic_string<CharT> utils::to_string(const Sentence& str)
{
  return std::basic_string<CharT>(str.data(), str.size());
}

template <typename InputIterator1, typename InputIterator2>
std::pair<InputIterator1, InputIterator2>
utils::mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                InputIterator2 last2)
{
  while (first1 != last1 && first2 != last2 && *first1 == *first2) {
    ++first1;
    ++first2;
  }
  return std::pair<InputIterator1, InputIterator2>(first1, first2);
}

/**
 * Removes common prefix of two string views
 */
template <typename CharT1, typename CharT2>
std::size_t utils::remove_common_prefix(basic_string_view<CharT1>& a, basic_string_view<CharT2>& b)
{
  auto prefix =
      std::distance(a.begin(), utils::mismatch(a.begin(), a.end(), b.begin(), b.end()).first);
  a.remove_prefix(prefix);
  b.remove_prefix(prefix);
  return prefix;
}

/**
 * Removes common suffix of two string views
 */
template <typename CharT1, typename CharT2>
std::size_t utils::remove_common_suffix(basic_string_view<CharT1>& a, basic_string_view<CharT2>& b)
{
  auto suffix =
      std::distance(a.rbegin(), utils::mismatch(a.rbegin(), a.rend(), b.rbegin(), b.rend()).first);
  a.remove_suffix(suffix);
  b.remove_suffix(suffix);
  return suffix;
}

/**
 * Removes common affix of two string views
 */
template <typename CharT1, typename CharT2>
StringAffix utils::remove_common_affix(basic_string_view<CharT1>& a, basic_string_view<CharT2>& b)
{
  return StringAffix{remove_common_prefix(a, b), remove_common_suffix(a, b)};
}

template <typename Sentence1, typename Sentence2>
std::size_t utils::count_uncommon_chars(const Sentence1& s1, const Sentence2& s2)
{
  std::array<signed int, 32> char_freq{};
  for (const auto& ch : s1) {
    ++char_freq[ch % 32];
  }

  for (const auto& ch : s2) {
    --char_freq[ch % 32];
  }

  std::size_t count = 0;
  for (const auto& freq : char_freq) {
    count += std::abs(freq);
  }

  return count;
}

template <typename Sentence, typename CharT>
SplittedSentenceView<CharT> utils::sorted_split(Sentence&& sentence)
{
  auto s = to_string_view(std::forward<Sentence>(sentence));
  string_view_vec<CharT> splitted;
  const auto* first = s.data();
  const auto* second = s.data();
  const auto* last = first + s.size();

  for (; second != last && first != last; first = second + 1) {
    second = std::find_if(first, last, Unicode::is_space<CharT>);

    if (first != second) {
      splitted.emplace_back(first, second - first);
    }
  }

  std::sort(splitted.begin(), splitted.end());

  return SplittedSentenceView<CharT>(splitted);
}

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
