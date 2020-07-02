/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#include <algorithm>
#include <array>
#include <cctype>
#include <cwctype>

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

inline percent utils::result_cutoff(const double result, const percent score_cutoff)
{
  return (result >= score_cutoff) ? result : 0;
}

percent utils::norm_distance(std::size_t dist, std::size_t lensum, percent score_cutoff)
{
  percent ratio = 100.0 - 100 * static_cast<double>(dist) / static_cast<double>(lensum);
  return result_cutoff(ratio, score_cutoff);
}

template <typename T>
bool utils::is_zero(T a, T tolerance)
{
  return std::fabs(a) <= tolerance;
}

template <typename Sentence, typename CharT = char_type<Sentence>, typename>
basic_string_view<CharT> utils::to_string_view(const Sentence& str)
{
  return basic_string_view<CharT>(str);
}

template <typename Sentence, typename CharT = char_type<Sentence>, typename>
basic_string_view<CharT> utils::to_string_view(Sentence str)
{
  return basic_string_view<CharT>(str.data(), str.size());
}

/**
 * Finds the longest common prefix between two ranges
 */
template <typename InputIterator1, typename InputIterator2>
inline auto common_prefix_length(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                                 InputIterator2 last2)
{
  return std::distance(first1, std::mismatch(first1, last1, first2, last2).first);
}

/**
 * Removes common prefix of two string views
 */
template <typename CharT1, typename CharT2>
std::size_t remove_common_prefix(basic_string_view<CharT1>& a, basic_string_view<CharT2>& b)
{
  auto prefix = common_prefix_length(a.begin(), a.end(), b.begin(), b.end());
  a.remove_prefix(prefix);
  b.remove_prefix(prefix);
  return prefix;
}

/**
 * Removes common suffix of two string views
 */
template <typename CharT1, typename CharT2>
std::size_t remove_common_suffix(basic_string_view<CharT1>& a, basic_string_view<CharT2>& b)
{
  auto suffix = common_prefix_length(a.rbegin(), a.rend(), b.rbegin(), b.rend());
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

template <typename Sentence>
std::array<unsigned int, 32> utils::char_freq(const Sentence& str)
{
  // with %32 the results for a <-> A are equal
  std::array<unsigned int, 32> char_freq{};
  for (const auto& ch : str) {
    char_freq[ch % 32]++;
  }
  return char_freq;
}

template <typename Sentence>
std::array<unsigned int, 32> utils::char_freq(const std::vector<Sentence>& str)
{
  std::array<unsigned int, 32> char_freq{};
  for (const auto& word : str) {
    for (const auto& ch : word) {
      char_freq[ch % 32]++;
    }
  }

  // count whitespaces between words
  if (str.size() > 1) {
    char_freq[0] += str.size() - 1;
  }

  return char_freq;
}

std::array<unsigned int, 32> utils::char_freq(const std::array<unsigned int, 32>& freq)
{
  return freq;
}

template <typename Sentence1, typename Sentence2>
std::size_t utils::count_uncommon_chars(const Sentence1& s1, const Sentence2& s2)
{
  auto char_freq1 = char_freq(s1);
  auto char_freq2 = char_freq(s2);

  std::size_t count = 0;

  for (std::size_t i = 0; i < 32; i++) {
    auto count1 = char_freq1[i];
    auto count2 = char_freq2[i];
    count += count1 > count2 ? count1 - count2 : count2 - count1;
  }

  return count;
}

template <typename CharT>
void utils::lower_case(std::basic_string<CharT>& s)
{
  // TODO: handle other characters like Ä <-> ä (maybe check how this is
  // implemented in cpython)
  std::transform(s.begin(), s.end(), s.begin(),
                 [](CharT ch) { return (ch >= 'A' && ch <= 'Z' ? ch + 32 : ch); });
}

template <typename CharT>
void utils::replace_non_alnum(std::basic_string<CharT>& s)
{
  // replace punctuation, control control characters, whitespaces with
  // whitespaces
  std::replace_if(
      s.begin(), s.end(),
      [](CharT ch) {
        int ascii = static_cast<int>(ch);
        return ascii <= '/' || (ascii >= ':' && ascii <= '@') || (ascii >= '[' && ascii <= '`') ||
               (ascii >= '{' && ascii <= 0x7F) /* DEL */;
      },
      static_cast<CharT>(' '));
}

template <typename CharT>
std::basic_string<CharT> utils::default_process(std::basic_string<CharT> s)
{
  replace_non_alnum(s);

  // only remove SPACE since all other space characters are already replaced
  // with SPACE
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](const CharT& ch) { return ch != ' '; }));

  s.erase(std::find_if(s.rbegin(), s.rend(), [](const CharT& ch) { return ch != ' '; }).base(),
          s.end());

  lower_case(s);
  return s;
}

template <typename Sentence, typename CharT>
std::basic_string<CharT> utils::default_process(Sentence s)
{
  return default_process(std::basic_string<CharT>(s.data(), s.size()));
}

} // namespace rapidfuzz