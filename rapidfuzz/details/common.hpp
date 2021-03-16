/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once
#include <rapidfuzz/details/SplittedSentenceView.hpp>
#include <rapidfuzz/details/type_traits.hpp>
#include <rapidfuzz/details/types.hpp>
#include <cmath>
#include <tuple>
#include <vector>
#include <array>
#include <cstring>

namespace rapidfuzz {

template <typename CharT1, typename CharT2, typename CharT3>
struct DecomposedSet {
  SplittedSentenceView<CharT1> difference_ab;
  SplittedSentenceView<CharT2> difference_ba;
  SplittedSentenceView<CharT3> intersection;
  DecomposedSet(SplittedSentenceView<CharT1> diff_ab, SplittedSentenceView<CharT2> diff_ba,
                SplittedSentenceView<CharT3> intersect)
      : difference_ab(std::move(diff_ab)),
        difference_ba(std::move(diff_ba)),
        intersection(std::move(intersect))
  {}
};

namespace common {

/**
 * @defgroup Common Common
 * Common utilities shared among multiple functions
 * @{
 */

template <typename CharT1, typename CharT2>
DecomposedSet<CharT1, CharT2, CharT1> set_decomposition(SplittedSentenceView<CharT1> a,
                                                        SplittedSentenceView<CharT2> b);

constexpr percent result_cutoff(const double result, const percent score_cutoff);

constexpr percent norm_distance(std::size_t dist, std::size_t lensum, percent score_cutoff = 0);

static inline std::size_t score_cutoff_to_distance(percent score_cutoff, std::size_t lensum);

template <typename T>
constexpr bool is_zero(T a, T tolerance = std::numeric_limits<T>::epsilon());

/**
 * @brief Get a string view to the object passed as parameter
 *
 * @tparam Sentence This is a string that can be explicitly converted to
 * basic_string_view<char_type>
 * @tparam CharT This is the char_type of Sentence
 *
 * @param str string that should be converted to string_view (for type info check Template
 * parameters above)
 *
 * @return basic_string_view<CharT> of str
 */
template <
    typename Sentence, typename CharT = char_type<Sentence>,
    typename = enable_if_t<is_explicitly_convertible<Sentence, basic_string_view<CharT>>::value>>
basic_string_view<CharT> to_string_view(Sentence&& str);

/**
 * @brief Get a string view to the object passed as parameter
 *
 * @tparam Sentence This is a string that can not be explicitly converted to
 * basic_string_view<char_type>, but stores a sequence in a similar way (e.g. boost::string_view or
 * std::vector)
 * @tparam CharT This is the char_type of Sentence
 *
 * @param str container that should be converted to string_view (for type info check Template
 * parameters above)
 *
 * @return basic_string_view<CharT> of str
 */
template <
    typename Sentence, typename CharT = char_type<Sentence>,
    typename = enable_if_t<!is_explicitly_convertible<Sentence, basic_string_view<CharT>>::value &&
                           has_data_and_size<Sentence>::value>>
basic_string_view<CharT> to_string_view(const Sentence& str);

template <
    typename Sentence, typename CharT = char_type<Sentence>,
    typename = enable_if_t<is_explicitly_convertible<Sentence, std::basic_string<CharT>>::value>>
std::basic_string<CharT> to_string(Sentence&& str);

template <
    typename Sentence, typename CharT = char_type<Sentence>,
    typename = enable_if_t<!is_explicitly_convertible<Sentence, std::basic_string<CharT>>::value &&
                           has_data_and_size<Sentence>::value>>
std::basic_string<CharT> to_string(const Sentence& str);

/**
 * @brief Finds the first mismatching pair of elements from two ranges:
 * one defined by [first1, last1) and another defined by [first2,last2).
 * Similar implementation to std::mismatch from C++14
 *
 * @param first1, last1	-	the first range of the elements
 * @param first2, last2	-	the second range of the elements
 *
 * @return std::pair with iterators to the first two non-equal elements.
 */
template <typename InputIterator1, typename InputIterator2>
std::pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1,
                                                   InputIterator2 first2, InputIterator2 last2);

template <typename CharT1, typename CharT2>
StringAffix remove_common_affix(basic_string_view<CharT1>& a, basic_string_view<CharT2>& b);

template <typename CharT1, typename CharT2>
std::size_t remove_common_prefix(basic_string_view<CharT1>& a, basic_string_view<CharT2>& b);

template <typename CharT1, typename CharT2>
std::size_t remove_common_suffix(basic_string_view<CharT1>& a, basic_string_view<CharT2>& b);

template <typename Sentence1, typename Sentence2>
std::size_t count_uncommon_chars(const Sentence1& s1, const Sentence2& s2);


template <typename Sentence, typename CharT = char_type<Sentence>>
SplittedSentenceView<CharT> sorted_split(Sentence&& sentence);

template <std::size_t size>
struct PatternMatchVector;

template <std::size_t size>
struct PatternMatchVector {
  std::array<uint32_t, 128> m_key;
  std::array<uint64_t, 128> m_val;

  PatternMatchVector()
    : m_key(), m_val() {}

  template<typename CharT>
  PatternMatchVector(basic_string_view<CharT> s)
    : m_key(), m_val()
  {
    for (std::size_t i = 0; i < s.size(); i++){
      insert(s[i], i);
    }
  }

  template <typename CharT>
  void insert(CharT ch, int pos) {
    uint8_t hash = ch % 128;
    uint32_t key = ch | 0x80000000U;

    // overflow starts search at 0 again.
    // Since a maximum of 64 elements is in here m_key[hash] will be false
    // after a maximum of 64 checks
    while (m_key[hash] && m_key[hash] != key) {
      if (hash == 127) hash = 0;
      else hash++;
    }

    m_key[hash] = key;
    m_val[hash] |= 1ull << pos;
  }

  template <typename CharT>
  uint64_t get(CharT ch) const {
    uint8_t hash = ch % 128;
    uint32_t key = ch | 0x80000000U;

    while (m_key[hash] && m_key[hash] != key) {
      if (hash == 127) hash = 0;
      else hash++;
    }

    return (m_key[hash] == key) ? m_val[hash] : 0;
  }
};

template <>
struct PatternMatchVector<1> {
  std::array<uint64_t, 256> m_val;

  PatternMatchVector()
    : m_val() {}

  template<typename CharT>
  PatternMatchVector(basic_string_view<CharT> s)
    : m_val()
  {
    for (std::size_t i = 0; i < s.size(); i++){
      insert(s[i], i);
    }
  }

  void insert(unsigned char ch, int pos) {
    // todo add tests for this
    m_val[ch] |= 1ull << pos;
  }

  template<typename CharT>
  uint64_t get(CharT ch) const {
    if(sizeof(CharT) == 1)
    {
      return m_val[(unsigned char)ch];
    }
    return (ch < 256) ? m_val[ch] : 0;
  }
};

template <std::size_t size>
struct BlockPatternMatchVector {
  std::vector<PatternMatchVector<size>> m_val;

  BlockPatternMatchVector() {}

  template<typename CharT>
  BlockPatternMatchVector(basic_string_view<CharT> s)
  {
    insert(s);
  }

  template<typename CharT>
  void insert(std::size_t block, CharT ch, int pos) {
    auto* be = &m_val[block];
    be->insert(ch, pos);
  }

  template<typename CharT>
  void insert(basic_string_view<CharT> s) {
    std::size_t nr = (s.size() / 64) + (std::size_t)((s.size() % 64) > 0);
    m_val.resize(nr);

    for (std::size_t i = 0; i < s.size(); i++){
      auto* be = &m_val[i/64];
      be->insert(s[i], i%64);
    }
  }

  template<typename CharT>
  uint64_t get(std::size_t block, CharT ch) const {
    auto* be = &m_val[block];
    return be->get(ch);
  }
};

/**@}*/

} // namespace common
} // namespace rapidfuzz

#include <rapidfuzz/details/common_impl.hpp>
