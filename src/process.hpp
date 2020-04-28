/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */
/* Copyright © 2011 Adam Cohen */

#pragma once
#include <optional>
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
 * @brief Find the best match in a list of choices
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
 *   string_utils::default_process<wchar_t>,
 *   fuzz::ratio<std::wstring, std::wstring>
 *   90);
 * @endcode
 * 
 * @tparam Sentence
 * @tparam Iterable This can be any Iterable of Sentences that can be iterated with
 *     range based for loops like e.g. vector, list or set
 * 
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not need the same type.
 *     So it is possible to use e.g. a std::string as query and a vector of std::string_view for the
 *     choices
 * @param processor Optional argument to provide a processor callback, that is used
 *     to preprocess all strings before comparing them. Defaults to string_utils::default_process.
 * @param scorer Optional argument to provide a scorer callback, that is used to calculate
 *      the matching ratio between two strings. Defaults to fuzz::WRatio.
 * @param score_cutoff Optional argument for a score threshold between 0% and 100%.
 *      Matches with a lower score than this number will not be returned. Defaults to 0.
 * 
 * @return Optional pair of the best match. When there is no match with a score >= score_cutoff
 *     boost::nullopt is returned (using boost::optional to keep supporting C++14)
 */
template<
    typename Sentence, typename CharT = char_type<Sentence>,
    typename Iterable,
    typename = std::enable_if_t<
           std::is_same<Sentence, inner_type<Iterable>>::value
        && std::is_convertible<Sentence const&, std::basic_string_view<CharT>>::value
    >
>
std::optional<std::pair<Sentence, percent>> extractOne(
    const Sentence& query,
    const Iterable& choices,
    processor_func<Sentence> processor = string_utils::default_process<Sentence>,
    scorer_func<std::basic_string<CharT>, std::basic_string<CharT>> scorer =
        fuzz::ratio<std::basic_string<CharT>, std::basic_string<CharT>>,
    const percent score_cutoff = 0
);


/**
 * @brief Find the best match in a list of choices without any string preprocessing
 * 
 * @tparam Sentence
 * @tparam Iterable This can be any Iterable of Sentences that can be iterated with
 *     range based for loops like e.g. vector, list or set
 * 
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not need the same type.
 *     So it is possible to use e.g. a std::string as query and a vector of std::string_view for the
 *     choices
 * @param processor This overload is selected by passing nullopt for the processor.
 *     The Strings are therefore not preprocessed
 * @param scorer Optional argument to provide a scorer callback, that is used to calculate
 *      the matching ratio between two strings. Defaults to fuzz::WRatio.
 * @param score_cutoff Optional argument for a score threshold between 0% and 100%.
 *      Matches with a lower score than this number will not be returned. Defaults to 0.
 * 
 * @return Optional pair of the best match. When there is no match with a score >= score_cutoff
 *     boost::nullopt is returned (using boost::optional to keep supporting C++14)
 */
template<
    typename Sentence1, typename CharT = char_type<Sentence1>,
    typename Iterable, typename Sentence2 = inner_type<Iterable>,
    typename = std::enable_if_t<
           std::is_same<CharT, char_type<Sentence2>>::value
        && std::is_convertible<Sentence1 const&, std::basic_string_view<CharT>>::value
        && std::is_convertible<Sentence2 const&, std::basic_string_view<CharT>>::value
    >
>
std::optional<std::pair<Sentence2, percent>> extractOne(
    const Sentence1& query,
    const Iterable& choices,
    std::nullopt_t processor,
    scorer_func<Sentence1, Sentence2> scorer = fuzz::ratio<Sentence1, Sentence2>,
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
 * @tparam Sentence
 * @tparam Iterable This can be any Iterable of Sentences that can be iterated with
 *     range based for loops like e.g. vector, list or set
 * 
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not need the same type.
 *     So it is possible to use e.g. a std::string as query and a vector of std::string_view for the
 *     choices
 * @param processor Optional argument to provide a processor callback, that is used
 *     to preprocess all strings before comparing them. Defaults to string_utils::default_process.
 * @param scorer Optional argument to provide a scorer callback, that is used to calculate
 *      the matching ratio between two strings. Defaults to fuzz::WRatio.
 * @param limit maximum amount of results to return
 * @param score_cutoff Optional argument for a score threshold between 0% and 100%.
 *      Matches with a lower score than this number will not be returned. Defaults to 0.
 * 
 * @return returns a list of the best matches that have a score >= score_cutoff.
 */
template<
    typename Sentence, typename CharT = char_type<Sentence>,
    typename Iterable,
    typename = std::enable_if_t<
           std::is_same<Sentence, inner_type<Iterable>>::value
        && std::is_convertible<Sentence const&, std::basic_string_view<CharT>>::value
    >
>
std::vector<std::pair<Sentence, percent>> extract(
    const Sentence& query,
    const Iterable& choices,
    processor_func<Sentence> processor = string_utils::default_process<Sentence>,
    scorer_func<std::basic_string<CharT>, std::basic_string<CharT>> scorer =
        fuzz::ratio<std::basic_string<CharT>, std::basic_string<CharT>>,
    const std::size_t limit = 5,
    percent score_cutoff = 0
);

/**
 * @brief Find the best matches in a list of choices without any string preprocessing
 * 
 * @tparam Sentence
 * @tparam Iterable This can be any Iterable of Sentences that can be iterated with
 *     range based for loops like e.g. vector, list or set
 * 
 * @param query string we want to find
 * @param choices iterable of all strings the query should be compared with.
 *     The strings have to use the same char type used for the query, but do not need the same type.
 *     So it is possible to use e.g. a std::string as query and a vector of std::string_view for the
 *     choices
 * @param processor This overload is selected by passing nullopt for the processor.
 *     The Strings are therefore not preprocessed
 * @param scorer Optional argument to provide a scorer callback, that is used to calculate
 *      the matching ratio between two strings. Defaults to fuzz::WRatio.
 * @param limit maximum amount of results to return
 * @param score_cutoff Optional argument for a score threshold between 0% and 100%.
 *      Matches with a lower score than this number will not be returned. Defaults to 0.
 * 
 * @return returns a list of the best matches that have a score >= score_cutoff.
 */
template<
    typename Sentence1, typename CharT = char_type<Sentence1>,
    typename Iterable, typename Sentence2 = inner_type<Iterable>,
    typename = std::enable_if_t<
           std::is_same<CharT, char_type<Sentence2>>::value
        && std::is_convertible<Sentence1 const&, std::basic_string_view<CharT>>::value
        && std::is_convertible<Sentence2 const&, std::basic_string_view<CharT>>::value
    >
>
std::vector<std::pair<Sentence2, percent>> extract(
    const Sentence1& query,
    const Iterable& choices,
    std::nullopt_t processor,
    scorer_func<Sentence1, Sentence2> scorer = fuzz::ratio<Sentence1, Sentence2>,
    const std::size_t limit = 5,
    const percent score_cutoff = 0
);


} /* process */ } /* rapidfuzz */

#include "process.txx"
