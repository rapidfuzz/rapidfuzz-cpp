/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */
/* Copyright © 2011 Adam Cohen */

#pragma once
#include "details/type_traits.hpp"
#include "details/optional.hpp"
#include "fuzz.hpp"
#include "utils.hpp"

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace rapidfuzz {
namespace process {

/**
 * @defgroup Process Process
 * Processors to perform string matching on iterables
 * @{
 */

/**
 * @brief
 * Finds the best match in a list of choices by comparing them
 * using the provided scorer functions.
 *
 * @details
 * @code{.cpp}
 * auto match = process::extractOne(
 *   L"example",
 *   std::vector<std::wstring>{L"example", L"another example"})
 * @endcode
 *
 * A more advanced usage providing a score_cutoff and a custom scorer:
 * @code{.cpp}
 * auto match = process::extractOne(
 *   L"example",
 *   std::vector<std::wstring>{L"example", L"another example"},
 *   utils::default_process<std::wstring>,
 *   fuzz::ratio<std::wstring, std::wstring>
 *   90);
 * @endcode
 *
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence1
 * @tparam Iterable This can be any Iterable of Sentence2 that can be iterated
 * with range based for loops like e.g. vector, list or set
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param processor Optional argument to provide a processor callback, that is
 * used to preprocess all strings before comparing them. Defaults to
 * utils::default_process. (Has to accept one argument both the query and
 * each choice can be implicitly converted to and return a std::basic_string of
 * the same `char_type` as query and choices)
 * @param scorer Optional argument to provide a scorer callback, that is used to
 * calculate the matching ratio between two strings. Defaults to fuzz::WRatio.
 *      (Requires the function signature percent(std::basic_string<CharT>,
 * std::basic_string<CharT>, percent))
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return Optional pair of the best match. When there is no match with a score
 * >= score_cutoff rapidfuzz::nullopt is returned (using rapidfuzz::optional to keep
 * supporting C++11 which behavse similar to std::optional in C++17)
 */
template <
    typename Sentence1, typename CharT = char_type<Sentence1>, typename Iterable,
    typename Sentence2 = typename Iterable::value_type,
    typename ProcessorFunc = decltype(utils::default_process<std::basic_string<CharT>>),
    typename ScorerFunc =
        decltype(fuzz::WRatio<std::basic_string<CharT>, std::basic_string<CharT>>)>
optional<std::pair<Sentence2, percent>>
extractOne(const Sentence1& query, const Iterable& choices,
           ProcessorFunc&& processor = utils::default_process<std::basic_string<CharT>>,
           ScorerFunc&& scorer = fuzz::WRatio<std::basic_string<CharT>, std::basic_string<CharT>>,
           const percent score_cutoff = 0);

/**
 * @brief
 * Finds the best match in a list of choices by comparing them
 * using the provided scorer functions without any string preprocessing.
 *
 * @details
 * @code{.cpp}
 * auto match = process::extractOne(
 *   L"example",
 *   std::vector<std::wstring>{L"example", L"another example"},
 *   rapidfuzz::nullopt)
 * @endcode
 *
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * basic_string_view<char_type>, or provides data and size methods like
 * std::vector<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * basic_string_view<char_type>, or provides data and size methods like
 * std::vector<char_type> and has the same `char_type` as Sentence1
 * @tparam Iterable This can be any Iterable of Sentence2 that can be iterated
 * with range based for loops like e.g. vector, list or set
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param processor This overload is selected by passing rapidfuzz::nullopt for the
 * processor. The Strings are therefore not preprocessed
 * @param scorer Optional argument to provide a scorer callback, that is used to
 * calculate the matching ratio between two strings. Defaults to fuzz::WRatio.
 *      (Requires the function signature percent(Sentence1, Sentence2, percent))
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return Optional pair of the best match. When there is no match with a score
 * >= score_cutoff rapidfuzz::nullopt is returned (using rapidfuzz::optional to keep
 * supporting C++11 which behavse similar to std::optional in C++17)
 */
template <typename Sentence1, typename CharT = char_type<Sentence1>, typename Iterable,
          typename Sentence2 = typename Iterable::value_type,
          typename ScorerFunc = decltype(fuzz::WRatio<Sentence1, Sentence2>)>
optional<std::pair<Sentence2, percent>>
extractOne(const Sentence1& query, const Iterable& choices, nullopt_t processor,
           ScorerFunc&& scorer = fuzz::WRatio<Sentence1, Sentence2>,
           const percent score_cutoff = 0);

/**
 * @brief Find the best matches in a list of choices
 *
 * @details
 * @code{.cpp}
 * auto matches = process::extract(
 *   L"example",
 *   std::vector<std::wstring>{L"example", L"another example"})
 * @endcode
 *
 * A more advanced usage providing a score_cutoff, a custom scorer and a limit:
 * @code{.cpp}
 * auto matches = process::extract(
 *   L"example",
 *   std::vector<std::wstring>{L"Example", L"example", L"another example"},
 *   utils::default_process<wchar_t>,
 *   fuzz::ratio<std::wstring, std::wstring>
 *   2,
 *   90);
 * @endcode
 *
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * std::basic_string<char_type> and has the same `char_type` as Sentence1
 * @tparam Iterable This can be any Iterable of Sentence2 that can be iterated
 * with range based for loops like e.g. vector, list or set
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param processor Optional argument to provide a processor callback, that is
 * used to preprocess all strings before comparing them. Defaults to
 * utils::default_process. (Has to accept one argument both the query and
 * each choice can be implicitly converted to and return a std::basic_string of
 * the same `char_type` as query and choices)
 * @param scorer Optional argument to provide a scorer callback, that is used to
 * calculate the matching ratio between two strings. Defaults to fuzz::WRatio.
 *      (Requires the function signature percent(std::basic_string<CharT>,
 * std::basic_string<CharT>, percent))
 * @param limit maximum amount of results to return
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns a list of the best matches that have a score >= score_cutoff.
 */
template <
    typename Sentence1, typename CharT = char_type<Sentence1>, typename Iterable,
    typename Sentence2 = typename Iterable::value_type,
    typename ProcessorFunc = decltype(utils::default_process<std::basic_string<CharT>>),
    typename ScorerFunc =
        decltype(fuzz::WRatio<std::basic_string<CharT>, std::basic_string<CharT>>)>
std::vector<std::pair<Sentence2, percent>>
extract(const Sentence1& query, const Iterable& choices,
        ProcessorFunc&& processor = utils::default_process<std::basic_string<CharT>>,
        ScorerFunc&& scorer = fuzz::WRatio<std::basic_string<CharT>, std::basic_string<CharT>>,
        const std::size_t limit = 5, const percent score_cutoff = 0);

/**
 * @brief Find the best matches in a list of choices without any string
 * preprocessing
 *
 * @details
 * @code{.cpp}
 * auto matches = process::extract(
 *   L"example",
 *   std::vector<std::wstring>{L"example", L"another example"},
 *   rapidfuzz::nullopt)
 * @endcode
 *
 * @tparam Sentence1 This is a string that can be explicitly converted to
 * basic_string_view<char_type>, or provides data and size methods like
 * std::vector<char_type> and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to
 * basic_string_view<char_type>, or provides data and size methods like
 * std::vector<char_type> and has the same `char_type` as Sentence1
 * @tparam Iterable This can be any Iterable of Sentence2 that can be iterated
 * with range based for loops like e.g. vector, list or set
 *
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not
 * need the same type. (e.g. string literal as query and vector of std::string
 * for the choices)
 * @param processor This overload is selected by passing rapidfuzz::nullopt for the
 * processor. The Strings are therefore not preprocessed
 * @param scorer Optional argument to provide a scorer callback, that is used to
 * calculate the matching ratio between two strings. Defaults to fuzz::WRatio.
 *      (Requires the function signature percent(Sentence1, Sentence2, percent))
 * @param limit maximum amount of results to return
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns a list of the best matches that have a score >= score_cutoff.
 */
template <typename Sentence1, typename CharT = char_type<Sentence1>, typename Iterable,
          typename Sentence2 = typename Iterable::value_type,
          typename ScorerFunc = decltype(fuzz::WRatio<Sentence1, Sentence2>)>
std::vector<std::pair<Sentence2, percent>>
extract(const Sentence1& query, const Iterable& choices, nullopt_t processor,
        ScorerFunc&& scorer = fuzz::WRatio<Sentence1, Sentence2>, const std::size_t limit = 5,
        const percent score_cutoff = 0);

/**@}*/

} // namespace process
} // namespace rapidfuzz

#include "process.txx"
