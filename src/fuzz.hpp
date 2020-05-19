/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */
/* Copyright © 2011 Adam Cohen */

#pragma once
#include "string_utils.hpp"
#include "utils.hpp"

#include <type_traits>

namespace rapidfuzz {
namespace fuzz {

/**
 * @brief calculates a simple ratio between two strings
 *
 * @details
 * @code{.cpp}
 * // score is 96.55
 * double score = ratio("this is a test", "this is a test!")
 * @endcode
 *
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence1
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2,
          typename CharT = char_type<Sentence1>>
percent ratio(const Sentence1& s1, const Sentence2& s2,
              const percent score_cutoff = 0);

/**
 * @brief calculates the fuzz::ratio of the optimal string alignment
 *
 * @details
 * @code{.cpp}
 * // score is 100
 * double score = partial_ratio("this is a test", "this is a test!")
 * @endcode
 *
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence1
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2,
          typename CharT = char_type<Sentence1>>
percent partial_ratio(const Sentence1& s1, const Sentence2& s2,
                      percent score_cutoff = 0);

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
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence1
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2,
          typename CharT = char_type<Sentence1>>
percent token_sort_ratio(const Sentence1& s1, const Sentence2& s2,
                         percent score_cutoff = 0);

/**
 * @brief Sorts the words in the strings and calculates the fuzz::partial_ratio
 * between them
 *
 *
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence1
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2,
          typename CharT = char_type<Sentence1>>
percent partial_token_sort_ratio(const Sentence1& s1, const Sentence2& s2,
                                 percent score_cutoff = 0);

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
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence1
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2,
          typename CharT = char_type<Sentence1>>
percent token_set_ratio(const Sentence1& s1, const Sentence2& s2,
                        const percent score_cutoff = 0);

/**
 * @brief Compares the words in the strings based on unique and common words
 * between them using fuzz::partial_ratio
 *
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence1
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2,
          typename CharT = char_type<Sentence1>>
percent partial_token_set_ratio(const Sentence1& s1, const Sentence2& s2,
                                percent score_cutoff = 0);

/**
 * @brief Helper method that returns the maximum of fuzz::token_set_ratio and
 * fuzz::token_sort_ratio (faster than manually executing the two functions)
 *
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence1
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2,
          typename CharT = char_type<Sentence1>>
percent token_ratio(const Sentence1& s1, const Sentence2& s2,
                    percent score_cutoff = 0);

/**
 * @brief Helper method that returns the maximum of
 * fuzz::partial_token_set_ratio and fuzz::partial_token_sort_ratio (faster than
 * manually executing the two functions)
 *
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence1
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2,
          typename CharT = char_type<Sentence1>>
percent partial_token_ratio(const Sentence1& s1, const Sentence2& s2,
                            percent score_cutoff = 0);

/**
 * @brief Calculates a quick estimation of fuzz::ratio by comparing the length
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
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence1
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2,
          typename CharT = char_type<Sentence1>>
percent length_ratio(const Sentence1& s1, const Sentence2& s2,
                     percent score_cutoff = 0);

/**
 * @brief Calculates a quick estimation of fuzz::ratio by counting uncommon
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
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence1
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2,
          typename CharT = char_type<Sentence1>>
percent quick_lev_ratio(const Sentence1& s1, const Sentence2& s2,
                        percent score_cutoff = 0);

/**
 * @brief Calculates a weighted ratio based on the other ratio algorithms
 *
 * @details
 * TODO: add a detailed description
 *
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence1
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename Sentence1, typename Sentence2,
          typename CharT = char_type<Sentence1>>
percent WRatio(const Sentence1& s1, const Sentence2& s2,
               percent score_cutoff = 0);

} // namespace fuzz
} // namespace rapidfuzz

#include "fuzz.txx"
