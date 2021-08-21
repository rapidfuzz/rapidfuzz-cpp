/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include <rapidfuzz/details/common.hpp>

namespace rapidfuzz {
namespace string_metric {
namespace detail {

#define NOTNUM(c)   ((c>57) || (c<48))

/* For now this implementation is ported from
 * https://github.com/jamesturk/cjellyfish
 *
 * this is only a placeholder which should be replaced by a faster implementation
 * in the future
 */
template <typename CharT1, typename CharT2>
double _jaro_winkler(basic_string_view<CharT1> ying,
                     basic_string_view<CharT2> yang,
                     int winklerize, double prefix_weight = 0.1)
{
    std::size_t min_len;
    std::size_t search_range;
    std::size_t trans_count, common_chars;

    // ensure that neither string is blank
    if (!ying.size() || !yang.size()) return 0;

    if (ying.size() > yang.size()) {
        search_range = ying.size();
        min_len = yang.size();
    } else {
        search_range = yang.size();
        min_len = ying.size();
    }
  
    // Blank out the flags
    std::vector<int> ying_flag(ying.size() + 1);
    std::vector<int> yang_flag(yang.size() + 1);

    search_range = (search_range/2);
    if (search_range > 0) search_range--;


    // Looking only within the search range, count and flag the matched pairs.
    common_chars = 0;
    for (std::size_t i = 0; i < ying.size(); i++) {
        std::size_t lowlim = (i >= search_range) ? i - search_range : 0;
        std::size_t hilim = (i + search_range <= yang.size()-1) ? (i + search_range) : yang.size()-1;
        for (std::size_t j = lowlim; j <= hilim; j++)  {
            if (!yang_flag[j] && yang[j] == ying[i]) {
                yang_flag[j] = 1;
                ying_flag[i] = 1;
                common_chars++;
                break;
            }
        }
    }

    // If no characters in common - return
    if (!common_chars) {
        return 0;
    }

    // Count the number of transpositions
    std::size_t k = trans_count = 0;
    for (std::size_t i = 0; i < ying.size(); i++) {
        if (ying_flag[i]) {
            std::size_t j = k;
            for (; j < yang.size(); j++) {
                if (yang_flag[j]) {
                    k = j + 1;
                    break;
                }
            }
            if (ying[i] != yang[j]) {
                trans_count++;
            }
        }
    }
    trans_count /= 2;

    // adjust for similarities in nonmatched characters

    // Main weight computation.
    double weight = common_chars / ((double) ying.size()) + common_chars / ((double) yang.size())
        + ((double) (common_chars - trans_count)) / ((double) common_chars);
    weight /=  3.0;

    // Continue to boost the weight if the strings are similar
    if (winklerize && weight > 0.7) {
        // Adjust for having up to the first 4 characters in common
        std::size_t j = (min_len >= 4) ? 4 : min_len;
        std::size_t i = 0;
        for (i=0; ((i<j) && (ying[i] == yang[i]) && (NOTNUM(ying[i]))); i++);
        if (i) {
            weight += i * prefix_weight * (1.0 - weight);
        }
    }

    return weight;
}


template <typename CharT1, typename CharT2>
double jaro_winkler_similarity(basic_string_view<CharT1> ying, basic_string_view<CharT2> yang,
                               double prefix_weight, percent score_cutoff)
{
    return common::result_cutoff(_jaro_winkler(ying, yang, 1, prefix_weight)*100, score_cutoff);
}

template <typename CharT1, typename CharT2>
double jaro_similarity(basic_string_view<CharT1> ying, basic_string_view<CharT2> yang, percent score_cutoff)
{
    return common::result_cutoff(_jaro_winkler(ying, yang, 0)*100, score_cutoff);
}

} // namespace detail
} // namespace string_metric
} // namespace rapidfuzz
