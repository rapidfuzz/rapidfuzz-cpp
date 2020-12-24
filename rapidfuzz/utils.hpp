/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once
#include "details/SplittedSentenceView.hpp"
#include "details/type_traits.hpp"
#include "details/types.hpp"
#include <cmath>
#include <tuple>
#include <vector>

namespace rapidfuzz {
namespace utils {

/**
 * @defgroup Utils Utils
 * Utility functions
 * @{
 */

/**
 * @brief removes any non alphanumeric characters, trim whitespaces from
 * beginning/end and lowercase the string. Currently this only supports
 * Ascii. Characters outside of the ascii spec are not changed. This
 * will be changed in the future to support full unicode. In case this has
 * has a noticable effect on the performance an additional `ascii_default_process`
 * function will be provided, that keeps this behaviour
 *
 * @tparam CharT char type of the string
 *
 * @param s string to process
 *
 * @return returns the processed string
 */

template <
    typename Sentence, typename CharT = char_type<Sentence>,
    typename = enable_if_t<is_explicitly_convertible<Sentence, std::basic_string<CharT>>::value>>
std::basic_string<CharT> default_process(Sentence&& s);

template <
    typename Sentence, typename CharT = char_type<Sentence>,
    typename = enable_if_t<!is_explicitly_convertible<Sentence, std::basic_string<CharT>>::value &&
                           has_data_and_size<Sentence>::value>>
std::basic_string<CharT> default_process(Sentence s);

/**@}*/

} // namespace utils
} // namespace rapidfuzz

#include "utils_impl.hpp"
