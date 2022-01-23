/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */
/* Copyright © 2011 Adam Cohen */

#pragma once
#include <rapidfuzz/details/common.hpp>

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
template <typename InputIt1, typename InputIt2>
double ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
             double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// TODO documentation
template <typename CharT1>
struct CachedRatio {
    template <typename InputIt1>
    CachedRatio(InputIt1 first1, InputIt1 last1) : s1(first1, last1), PM(first1, last1)
    {}

    template <typename Sentence1>
    CachedRatio(const Sentence1& s1) : CachedRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0.0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    common::BlockPatternMatchVector PM;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedRatio(const Sentence1& s1) -> CachedRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedRatio(InputIt1 first1, InputIt1 last1) -> CachedRatio<iterator_type<InputIt1>>;
#endif

/**
 * @brief calculates the fuzz::ratio of the optimal string alignment
 *
 * @details
 * test @cite hyrro_2004 @cite wagner_fischer_1974
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
template <typename InputIt1, typename InputIt2>
double partial_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                     double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double partial_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// todo add real implementation
template <typename CharT1>
struct CachedPartialRatio {
    template <typename>
    friend struct CachedWRatio;

    template <typename InputIt1>
    CachedPartialRatio(InputIt1 first1, InputIt1 last1);

    template <typename Sentence1>
    CachedPartialRatio(const Sentence1& s1)
        : CachedPartialRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0.0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0.0) const;

private:
    std::basic_string<CharT1> s1;
    common::CharHashTable<CharT1, bool> s1_char_map;
    CachedRatio<CharT1> cached_ratio;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedPartialRatio(const Sentence1& s1) -> CachedPartialRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedPartialRatio(InputIt1 first1, InputIt1 last1) -> CachedPartialRatio<iterator_type<InputIt1>>;
#endif

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
template <typename InputIt1, typename InputIt2>
double token_sort_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                        double score_cutoff = 0);

template <typename Sentence1, typename Sentence2, typename CharT1 = char_type<Sentence1>,
          typename CharT2 = char_type<Sentence2>>
double token_sort_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// todo CachedRatio speed for equal strings vs original implementation
// TODO documentation
template <typename CharT1>
struct CachedTokenSortRatio {
    template <typename InputIt1>
    CachedTokenSortRatio(InputIt1 first1, InputIt1 last1)
        : s1_sorted(common::sorted_split(first1, last1).join()), cached_ratio(s1_sorted)
    {}

    template <typename Sentence1>
    CachedTokenSortRatio(const Sentence1& s1)
        : CachedTokenSortRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1_sorted;
    CachedRatio<CharT1> cached_ratio;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedTokenSortRatio(const Sentence1& s1) -> CachedTokenSortRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedTokenSortRatio(InputIt1 first1, InputIt1 last1)
    -> CachedTokenSortRatio<iterator_type<InputIt1>>;
#endif

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
template <typename InputIt1, typename InputIt2>
double partial_token_sort_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double partial_token_sort_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// TODO documentation
template <typename CharT1>
struct CachedPartialTokenSortRatio {
    template <typename InputIt1>
    CachedPartialTokenSortRatio(InputIt1 first1, InputIt1 last1)
        : s1_sorted(common::sorted_split(first1, last1).join()), cached_partial_ratio(s1_sorted)
    {}

    template <typename Sentence1>
    CachedPartialTokenSortRatio(const Sentence1& s1)
        : CachedPartialTokenSortRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1_sorted;
    CachedPartialRatio<CharT1> cached_partial_ratio;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedPartialTokenSortRatio(const Sentence1& s1)
    -> CachedPartialTokenSortRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedPartialTokenSortRatio(InputIt1 first1, InputIt1 last1)
    -> CachedPartialTokenSortRatio<iterator_type<InputIt1>>;
#endif

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
template <typename InputIt1, typename InputIt2>
double token_set_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                       double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double token_set_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// TODO documentation
template <typename CharT1>
struct CachedTokenSetRatio {
    template <typename InputIt1>
    CachedTokenSetRatio(InputIt1 first1, InputIt1 last1)
        : s1(first1, last1), tokens_s1(common::sorted_split(std::begin(s1), std::end(s1)))
    {}

    template <typename Sentence1>
    CachedTokenSetRatio(const Sentence1& s1)
        : CachedTokenSetRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    SplittedSentenceView<typename std::basic_string<CharT1>::iterator> tokens_s1;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedTokenSetRatio(const Sentence1& s1) -> CachedTokenSetRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedTokenSetRatio(InputIt1 first1, InputIt1 last1)
    -> CachedTokenSetRatio<iterator_type<InputIt1>>;
#endif

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
template <typename InputIt1, typename InputIt2>
double partial_token_set_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                               double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double partial_token_set_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// TODO documentation
template <typename CharT1>
struct CachedPartialTokenSetRatio {
    template <typename InputIt1>
    CachedPartialTokenSetRatio(InputIt1 first1, InputIt1 last1)
        : s1(first1, last1), tokens_s1(common::sorted_split(std::begin(s1), std::end(s1)))
    {}

    template <typename Sentence1>
    CachedPartialTokenSetRatio(const Sentence1& s1)
        : CachedPartialTokenSetRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    SplittedSentenceView<typename std::basic_string<CharT1>::iterator> tokens_s1;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedPartialTokenSetRatio(const Sentence1& s1) -> CachedPartialTokenSetRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedPartialTokenSetRatio(InputIt1 first1, InputIt1 last1)
    -> CachedPartialTokenSetRatio<iterator_type<InputIt1>>;
#endif

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
template <typename InputIt1, typename InputIt2>
double token_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                   double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double token_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// todo add real implementation
template <typename CharT1>
struct CachedTokenRatio {
    template <typename InputIt1>
    CachedTokenRatio(InputIt1 first1, InputIt1 last1)
        : s1(first1, last1),
          s1_tokens(common::sorted_split(std::begin(s1), std::end(s1))),
          s1_sorted(s1_tokens.join()),
          cached_ratio_s1_sorted(s1_sorted)
    {}

    template <typename Sentence1>
    CachedTokenRatio(const Sentence1& s1)
        : CachedTokenRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    SplittedSentenceView<typename std::basic_string<CharT1>::iterator> s1_tokens;
    std::basic_string<CharT1> s1_sorted;
    CachedRatio<CharT1> cached_ratio_s1_sorted;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedTokenRatio(const Sentence1& s1) -> CachedTokenRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedTokenRatio(InputIt1 first1, InputIt1 last1) -> CachedTokenRatio<iterator_type<InputIt1>>;
#endif

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
template <typename InputIt1, typename InputIt2>
double partial_token_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                           double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double partial_token_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// todo add real implementation
template <typename CharT1>
struct CachedPartialTokenRatio {
    template <typename InputIt1>
    CachedPartialTokenRatio(InputIt1 first1, InputIt1 last1)
        : s1(first1, last1),
          tokens_s1(common::sorted_split(std::begin(s1), std::end(s1))),
          s1_sorted(tokens_s1.join())
    {}

    template <typename Sentence1>
    CachedPartialTokenRatio(const Sentence1& s1)
        : CachedPartialTokenRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    SplittedSentenceView<typename std::basic_string<CharT1>::iterator> tokens_s1;
    std::basic_string<CharT1> s1_sorted;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedPartialTokenRatio(const Sentence1& s1) -> CachedPartialTokenRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedPartialTokenRatio(InputIt1 first1, InputIt1 last1)
    -> CachedPartialTokenRatio<iterator_type<InputIt1>>;
#endif

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
template <typename InputIt1, typename InputIt2>
double WRatio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
              double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double WRatio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// todo add real implementation
template <typename CharT1>
struct CachedWRatio {
    template <typename InputIt1>
    CachedWRatio(InputIt1 first1, InputIt1 last1);

    template <typename Sentence1>
    CachedWRatio(const Sentence1& s1) : CachedWRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    // todo somehow implement this using other ratios with creating PatternMatchVector
    // multiple times
    std::basic_string<CharT1> s1;
    CachedPartialRatio<CharT1> cached_partial_ratio;
    SplittedSentenceView<typename std::basic_string<CharT1>::iterator> tokens_s1;
    std::basic_string<CharT1> s1_sorted;
    common::BlockPatternMatchVector blockmap_s1_sorted;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedWRatio(const Sentence1& s1) -> CachedWRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedWRatio(InputIt1 first1, InputIt1 last1) -> CachedWRatio<iterator_type<InputIt1>>;
#endif

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
template <typename InputIt1, typename InputIt2>
double QRatio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
              double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double QRatio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

template <typename CharT1>
struct CachedQRatio {
    template <typename InputIt1>
    CachedQRatio(InputIt1 first1, InputIt1 last1) : s1(first1, last1), cached_ratio(first1, last1)
    {}

    template <typename Sentence1>
    CachedQRatio(const Sentence1& s1) : CachedQRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    CachedRatio<CharT1> cached_ratio;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedQRatio(const Sentence1& s1) -> CachedQRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedQRatio(InputIt1 first1, InputIt1 last1) -> CachedQRatio<iterator_type<InputIt1>>;
#endif

/**@}*/

} // namespace fuzz
} // namespace rapidfuzz

#include <rapidfuzz/fuzz.impl>
