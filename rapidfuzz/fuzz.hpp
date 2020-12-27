/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */
/* Copyright © 2011 Adam Cohen */

#pragma once
#include "details/common.hpp"

#include <type_traits>

namespace rapidfuzz {
namespace fuzz {

/**
 * @defgroup Fuzz Fuzz
 * A collection of string matching algorithms from FuzzyWuzzy
 * @{
 */

/**
 * @brief calculates a simple ratio between two strings
 *
 * @details
 * @code{.cpp}
 * // score is 96.55
 * double score = ratio("this is a test", "this is a test!")
 * @endcode
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2>
percent ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

// TODO documentation
template<typename Sentence1>
struct CachedRatio {
  using CharT1 = char_type<Sentence1>;

  CachedRatio(const Sentence1& s1);

  template<typename Sentence2>
  double ratio(const Sentence2& s2, percent score_cutoff = 0) const;

private:
  rapidfuzz::basic_string_view<CharT1> s1_view;
  common::blockmap_entry<sizeof(CharT1)> blockmap_s1;
};


/**
 * @brief Calculates a quick upper bound on fuzz::ratio by counting uncommon
 * letters between the two sentences. Guaranteed to be equal or higher than
 * fuzz::ratio. (internally used by fuzz::ratio already when providing it with a
 * score_cutoff to speed up the matching)
 *
 * @details
 * Since it only counts the uncommon characters it runs in linear time (O(N)),
 * while most other algorithms use a weighted levenshtein distance and therefore
 * have a quadratic runtime (O(N*M)). The result is equal to the weighted
 * levenshtein ratio of the two sentences with the letters in a sorted order and
 * is therefore always equal or higher than fuzz::ratio
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2>
percent quick_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

// todo add real implementation
template<typename Sentence1>
struct CachedQuickRatio {
  using CharT1 = char_type<Sentence1>;

  CachedQuickRatio(const Sentence1& s1)
    : s1_view(common::to_string_view(s1)) {}

  template<typename Sentence2>
  double ratio(const Sentence2& s2, percent score_cutoff = 0) const {
    return quick_ratio(s1_view, s2, score_cutoff);
  }

private:
  rapidfuzz::basic_string_view<CharT1> s1_view;
};


/**
 * @brief Calculates a quick upper bound on fuzz::ratio by comparing the length
 * of the two sentences. Guaranteed to be equal or higher than fuzz::ratio
 * (internally used by fuzz::ratio already when providing it with a score_cutoff
 * to speed up the matching)
 *
 * @details
 * Since it only compares the known lengths of the strings it runs in constant
 * time (O(1)), while most other algorithms use a weighted levenshtein distance
 * and therefore have a quadratic runtime (O(N*M)). It returns the same result
 * as the weighted levenshtein ratio for two strings where one string is part of
 * the other string. The main purpose is to have a really quick ratio to remove
 * some very bad matches early on
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2>
percent real_quick_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

template<typename Sentence1>
struct CachedRealQuickRatio {
  using CharT1 = char_type<Sentence1>;

  CachedRealQuickRatio(const Sentence1& s1)
    : s1_view(common::to_string_view(s1)) {}

  template<typename Sentence2>
  double ratio(const Sentence2& s2, percent score_cutoff = 0) const {
    return real_quick_ratio(s1_view, s2, score_cutoff);
  }

private:
  rapidfuzz::basic_string_view<CharT1> s1_view;
};


/**
 * @brief calculates the fuzz::ratio of the optimal string alignment
 *
 * @details
 * @code{.cpp}
 * // score is 100
 * double score = partial_ratio("this is a test", "this is a test!")
 * @endcode
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2, typename CharT1 = char_type<Sentence1>,
          typename CharT2 = char_type<Sentence2>>
percent partial_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

// todo add real implementation
template<typename Sentence1>
struct CachedPartialRatio {
  using CharT1 = char_type<Sentence1>;

  CachedPartialRatio(const Sentence1& s1)
    : s1_view(common::to_string_view(s1)) {}

  template<typename Sentence2>
  double ratio(const Sentence2& s2, percent score_cutoff = 0) const {
    return partial_ratio(s1_view, s2, score_cutoff);
  }

private:
  rapidfuzz::basic_string_view<CharT1> s1_view;
};


/**
 * @brief Sorts the words in the strings and calculates the fuzz::ratio between
 * them
 *
 * @details
 * @code{.cpp}
 * // score is 100
 * double score = token_sort_ratio("fuzzy wuzzy was a bear", "wuzzy fuzzy was a
 * bear")
 * @endcode
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2, typename CharT1 = char_type<Sentence1>,
          typename CharT2 = char_type<Sentence2>>
percent token_sort_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

// TODO documentation
template<typename Sentence1>
struct CachedTokenSortRatio {
  using CharT1 = char_type<Sentence1>;

  CachedTokenSortRatio(const Sentence1& s1);

  template<typename Sentence2>
  double ratio(const Sentence2& s2, percent score_cutoff = 0) const;

private:
  std::basic_string<CharT1> s1_sorted;
  common::blockmap_entry<sizeof(CharT1)> blockmap_s1_sorted;
};


/**
 * @brief Sorts the words in the strings and calculates the fuzz::partial_ratio
 * between them
 *
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2, typename CharT1 = char_type<Sentence1>,
          typename CharT2 = char_type<Sentence2>>
percent partial_token_sort_ratio(const Sentence1& s1, const Sentence2& s2,
                                 percent score_cutoff = 0);

// TODO documentation
template<typename Sentence1>
struct CachedPartialTokenSortRatio {
  using CharT1 = char_type<Sentence1>;

  CachedPartialTokenSortRatio(const Sentence1& s1);

  template<typename Sentence2>
  double ratio(const Sentence2& s2, percent score_cutoff = 0) const;

private:
  std::basic_string<CharT1> s1_sorted;
};

/**
 * @brief Compares the words in the strings based on unique and common words
 * between them using fuzz::ratio
 *
 * @details
 * @code{.cpp}
 * // score1 is 83.87
 * double score1 = token_sort_ratio("fuzzy was a bear", "fuzzy fuzzy was a
 * bear")
 * // score2 is 100
 * double score2 = token_set_ratio("fuzzy was a bear", "fuzzy fuzzy was a bear")
 * @endcode
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2>
percent token_set_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

// TODO documentation
template<typename Sentence1>
struct CachedTokenSetRatio {
  using CharT1 = char_type<Sentence1>;

  CachedTokenSetRatio(const Sentence1& s1);

  template<typename Sentence2>
  double ratio(const Sentence2& s2, percent score_cutoff = 0) const;

private:
  SplittedSentenceView<CharT1> tokens_s1;
};

/**
 * @brief Compares the words in the strings based on unique and common words
 * between them using fuzz::partial_ratio
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2, typename CharT1 = char_type<Sentence1>,
          typename CharT2 = char_type<Sentence2>>
percent partial_token_set_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

// TODO documentation
template<typename Sentence1>
struct CachedPartialTokenSetRatio {
  using CharT1 = char_type<Sentence1>;

  CachedPartialTokenSetRatio(const Sentence1& s1);

  template<typename Sentence2>
  double ratio(const Sentence2& s2, percent score_cutoff = 0) const;

private:
  SplittedSentenceView<CharT1> tokens_s1;
};

/**
 * @brief Helper method that returns the maximum of fuzz::token_set_ratio and
 * fuzz::token_sort_ratio (faster than manually executing the two functions)
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2>
percent token_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

// todo add real implementation
template<typename Sentence1>
struct CachedTokenRatio {
  using CharT1 = char_type<Sentence1>;

  CachedTokenRatio(const Sentence1& s1);

  template<typename Sentence2>
  double ratio(const Sentence2& s2, percent score_cutoff = 0) const;

private:
  SplittedSentenceView<CharT1> tokens_s1;
  std::basic_string<CharT1> s1_sorted;
};


/**
 * @brief Helper method that returns the maximum of
 * fuzz::partial_token_set_ratio and fuzz::partial_token_sort_ratio (faster than
 * manually executing the two functions)
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2, typename CharT1 = char_type<Sentence1>,
          typename CharT2 = char_type<Sentence2>>
percent partial_token_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

// todo add real implementation
template<typename Sentence1>
struct CachedPartialTokenRatio {
  using CharT1 = char_type<Sentence1>;

  CachedPartialTokenRatio(const Sentence1& s1);

  template<typename Sentence2>
  double ratio(const Sentence2& s2, percent score_cutoff = 0) const;

private:
  SplittedSentenceView<CharT1> tokens_s1;
  std::basic_string<CharT1> s1_sorted;
};


/**
 * @brief Calculates a weighted ratio based on the other ratio algorithms
 *
 * @details
 * @todo add a detailed description
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2>
percent WRatio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);

// todo add real implementation
template<typename Sentence1>
struct CachedWRatio {
  using CharT1 = char_type<Sentence1>;

  CachedWRatio(const Sentence1& s1);

  template<typename Sentence2>
  double ratio(const Sentence2& s2, percent score_cutoff = 0) const;

private:
  rapidfuzz::basic_string_view<CharT1> s1_view;
  common::blockmap_entry<sizeof(CharT1)> blockmap_s1;
};

/**
 * @brief Calculates a quick ratio between two strings using fuzz.ratio
 *
 * @details
 * @todo add a detailed description
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2>
percent QRatio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff = 0);


template<typename Sentence1>
struct CachedQRatio {
  using CharT1 = char_type<Sentence1>;

  CachedQRatio(const Sentence1& s1);

  template<typename Sentence2>
  double ratio(const Sentence2& s2, percent score_cutoff = 0) const;

private:
  rapidfuzz::basic_string_view<CharT1> s1_view;
  common::blockmap_entry<sizeof(CharT1)> blockmap_s1;
};

/**@}*/

} // namespace fuzz
} // namespace rapidfuzz

#include "fuzz_impl.hpp"
