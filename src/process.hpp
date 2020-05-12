/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */
/* Copyright © 2011 Adam Cohen */

#pragma once
#include <boost/optional.hpp>
#include <functional>
#include <string>
#include <utility>
#include <vector>
#include "fuzz.hpp"
#include "utils.hpp"
#include "type_traits.hpp"

namespace rapidfuzz {

/**
 * @brief 
 * @details 
 *
 * @ingroup process
 */
namespace process {

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
 *   string_utils::default_process<std::wstring>,
 *   fuzz::ratio<std::wstring, std::wstring>
 *   90);
 * @endcode
 * 
 * @tparam Sentence1 This is a string that can be explicitly converted to std::basic_string<char_type>
 *     and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to std::basic_string<char_type>
 *     and has the same `char_type` as Sentence1
 * @tparam Iterable This can be any Iterable of Sentence2 that can be iterated with
 *     range based for loops like e.g. vector, list or set
 * 
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not need the same type.
 *     (e.g. string literal as query and vector of std::string for the choices)
 * @param processor Optional argument to provide a processor callback, that is used
 *     to preprocess all strings before comparing them. Defaults to string_utils::default_process.
 *     (Has to accept one argument both the query and each choice can be implicitly converted to
 *      and return a std::basic_string of the same `char_type` as query and choices)
 * @param scorer Optional argument to provide a scorer callback, that is used to calculate
 *      the matching ratio between two strings. Defaults to fuzz::WRatio.
 *      (Requires the function signature percent(std::basic_string<CharT>, std::basic_string<CharT>, percent))
 * @param score_cutoff Optional argument for a score threshold between 0% and 100%.
 *      Matches with a lower score than this number will not be returned. Defaults to 0.
 * 
 * @return Optional pair of the best match. When there is no match with a score >= score_cutoff
 *     boost::none is returned (using boost::optional to keep supporting C++14)
 */
template<
    typename Sentence1, typename CharT = char_type<Sentence1>,
    typename Iterable, typename Sentence2 = inner_type<Iterable>,
    typename ProcessorFunc = decltype(string_utils::default_process<CharT>),
    typename ScorerFunc = decltype(fuzz::WRatio<std::basic_string<CharT>, std::basic_string<CharT>>),
    typename = std::enable_if_t<
         is_explicitly_convertible_v<Sentence1, std::basic_string<CharT>>
      && is_explicitly_convertible_v<Sentence2, std::basic_string<CharT>>
      && is_invocable_v<ProcessorFunc, std::basic_string<CharT>>
      && is_invocable_v<ScorerFunc, std::basic_string<CharT>, std::basic_string<CharT>, percent>>
>
boost::optional<std::pair<Sentence2, percent>> extractOne(
    const Sentence1& query,
    const Iterable& choices,
    ProcessorFunc&& processor = string_utils::default_process<CharT>,
    ScorerFunc&& scorer = fuzz::WRatio<std::basic_string<CharT>, std::basic_string<CharT>>,
    const percent score_cutoff = 0
);


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
 *   boost::none)
 * @endcode
 * 
 * @tparam Sentence1 This is a string that can be explicitly converted to std::basic_string<char_type>
 *     and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to std::basic_string<char_type>
 *     and has the same `char_type` as Sentence1
 * @tparam Iterable This can be any Iterable of Sentence2 that can be iterated with
 *     range based for loops like e.g. vector, list or set
 * 
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not need the same type.
 *     (e.g. string literal as query and vector of std::string for the choices)
 * @param processor This overload is selected by passing boost::none for the processor.
 *     The Strings are therefore not preprocessed
 * @param scorer Optional argument to provide a scorer callback, that is used to calculate
 *      the matching ratio between two strings. Defaults to fuzz::WRatio.
 *      (Requires the function signature percent(Sentence1, Sentence2, percent))
 * @param score_cutoff Optional argument for a score threshold between 0% and 100%.
 *      Matches with a lower score than this number will not be returned. Defaults to 0.
 * 
 * @return Optional pair of the best match. When there is no match with a score >= score_cutoff
 *     boost::none is returned (using boost::optional to keep supporting C++14)
 */
template<
    typename Sentence1, typename CharT = char_type<Sentence1>,
    typename Iterable, typename Sentence2 = inner_type<Iterable>,
    typename ScorerFunc = decltype(fuzz::WRatio<Sentence1, Sentence2>),
    typename = std::enable_if_t<is_invocable_v<ScorerFunc, Sentence1, Sentence2, percent>>
>
boost::optional<std::pair<Sentence2, percent>> extractOne(
    const Sentence1& query,
    const Iterable& choices,
    boost::none_t processor,
    ScorerFunc&& scorer = fuzz::WRatio<Sentence1, Sentence2>,
    const percent score_cutoff = 0
);


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
 *   string_utils::default_process<wchar_t>,
 *   fuzz::ratio<std::wstring, std::wstring>
 *   2,
 *   90);
 * @endcode
 * 
 * @tparam Sentence1 This is a string that can be explicitly converted to std::basic_string<char_type>
 *     and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to std::basic_string<char_type>
 *     and has the same `char_type` as Sentence1
 * @tparam Iterable This can be any Iterable of Sentence2 that can be iterated with
 *     range based for loops like e.g. vector, list or set
 * 
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not need the same type.
 *     (e.g. string literal as query and vector of std::string for the choices)
 * @param processor Optional argument to provide a processor callback, that is used
 *     to preprocess all strings before comparing them. Defaults to string_utils::default_process.
 *     (Has to accept one argument both the query and each choice can be implicitly converted to
 *      and return a std::basic_string of the same `char_type` as query and choices)
 * @param scorer Optional argument to provide a scorer callback, that is used to calculate
 *      the matching ratio between two strings. Defaults to fuzz::WRatio.
 *      (Requires the function signature percent(std::basic_string<CharT>, std::basic_string<CharT>, percent))
 * @param limit maximum amount of results to return
 * @param score_cutoff Optional argument for a score threshold between 0% and 100%.
 *      Matches with a lower score than this number will not be returned. Defaults to 0.
 *
 * @return returns a list of the best matches that have a score >= score_cutoff.
 */
template<
    typename Sentence1, typename CharT = char_type<Sentence1>,
    typename Iterable, typename Sentence2 = inner_type<Iterable>,
    typename ProcessorFunc = decltype(string_utils::default_process<CharT>),
    typename ScorerFunc = decltype(fuzz::WRatio<std::basic_string<CharT>, std::basic_string<CharT>>),
    typename = std::enable_if_t<
         is_explicitly_convertible_v<Sentence1, std::basic_string<CharT>>
      && is_explicitly_convertible_v<Sentence2, std::basic_string<CharT>>
      && is_invocable_v<ProcessorFunc, std::basic_string<CharT>>
      && is_invocable_v<ScorerFunc, std::basic_string<CharT>, std::basic_string<CharT>, percent>>
>
std::vector<std::pair<Sentence2, percent>> extract(
    const Sentence1& query,
    const Iterable& choices,
    ProcessorFunc&& processor = string_utils::default_process<CharT>,
    ScorerFunc&& scorer = fuzz::WRatio<std::basic_string<CharT>, std::basic_string<CharT>>,
    const std::size_t limit = 5,
    const percent score_cutoff = 0
);


/**
 * @brief Find the best matches in a list of choices without any string preprocessing
 * 
 * @details
 * @code{.cpp}
 * auto matches = process::extract(
 *   L"example",
 *   std::vector<std::wstring>{L"example", L"another example"},
 *   boost::none)
 * @endcode
 * 
 * @tparam Sentence1 This is a string that can be explicitly converted to std::basic_string<char_type>
 *     and has the same `char_type` as Sentence2
 * @tparam Sentence2 This is a string that can be explicitly converted to std::basic_string<char_type>
 *     and has the same `char_type` as Sentence1
 * @tparam Iterable This can be any Iterable of Sentence2 that can be iterated with
 *     range based for loops like e.g. vector, list or set
 * 
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not need the same type.
 *     (e.g. string literal as query and vector of std::string for the choices)
 * @param processor This overload is selected by passing boost::none for the processor.
 *     The Strings are therefore not preprocessed
 * @param scorer Optional argument to provide a scorer callback, that is used to calculate
 *      the matching ratio between two strings. Defaults to fuzz::WRatio.
 *      (Requires the function signature percent(Sentence1, Sentence2, percent))
 * @param limit maximum amount of results to return
 * @param score_cutoff Optional argument for a score threshold between 0% and 100%.
 *      Matches with a lower score than this number will not be returned. Defaults to 0.
 * 
 * @return returns a list of the best matches that have a score >= score_cutoff.
 */
template<
    typename Sentence1, typename CharT = char_type<Sentence1>,
    typename Iterable, typename Sentence2 = inner_type<Iterable>,
    typename ScorerFunc = decltype(fuzz::WRatio<Sentence1, Sentence2>),
    typename = std::enable_if_t<is_invocable_v<ScorerFunc, Sentence1, Sentence2, percent>>
>
std::vector<std::pair<Sentence2, percent>> extract(
    const Sentence1& query,
    const Iterable& choices,
    boost::none_t processor,
    ScorerFunc&& scorer = fuzz::WRatio<Sentence1, Sentence2>,
    const std::size_t limit = 5,
    const percent score_cutoff = 0
);


} /* process */ } /* rapidfuzz */

#include "process.txx"
