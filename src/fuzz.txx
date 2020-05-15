/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */
/* Copyright © 2011 Adam Cohen */

#include "fuzz.hpp"
#include "levenshtein.hpp"

#include <algorithm>
#include <cmath>
#include <difflib/difflib.h>
#include <iterator>
#include <tuple>
#include <vector>
#include <iostream>

namespace rapidfuzz {

template<typename Sentence1, typename Sentence2, typename CharT>
percent fuzz::ratio(const Sentence1 &s1, const Sentence2 &s2, const percent score_cutoff)
{
    double result = levenshtein::normalized_weighted_distance(
        utils::to_string_view(s1),
        utils::to_string_view(s2),
        score_cutoff / 100);
    return result * 100;
}


template<typename Sentence1, typename Sentence2, typename CharT>
percent fuzz::partial_ratio(const Sentence1 &s1, const Sentence2 &s2, percent score_cutoff)
{
    using std::get;

    if (score_cutoff > 100) {
        return 0;
    }

    auto s1_view = utils::to_string_view(s1);
    auto s2_view = utils::to_string_view(s2);

    if (s1_view.empty() || s2_view.empty()) {
        return static_cast<double>(s1_view.empty() && s2_view.empty()) * 100.0;
    }

    // when both strings have the same length the is only one possible alignment
    if (s1_view.length() == s2_view.length()) {
        return ratio(s1_view, s2_view, score_cutoff);
    }

    if (s1_view.length() > s2_view.length()) {
        std::swap(s1_view, s2_view);
    }

    std::size_t short_len = s1_view.length();

    // TODO: This can be done based on the levenshtein distance aswell, which should
    // be faster
    auto matcher = difflib::SequenceMatcher<basic_string_view<CharT>>(s1_view, s2_view);
    auto blocks = matcher.get_matching_blocks();

    // when there is a full match exit early
    for (const auto& block : blocks) {
        if (get<2>(block) == short_len) {
            return 100;
        }
    }

    double max_ratio = 0;
    for (const auto& block : blocks) {
        std::size_t long_start = (get<1>(block) > get<0>(block)) ? get<1>(block) - get<0>(block): 0;
        auto long_substr = s2_view.substr(long_start, short_len);

        double ls_ratio = utils::result_cutoff(ratio(s1_view, long_substr), score_cutoff);

        if (ls_ratio > 99.5) {
            return 100;
        }

        if (ls_ratio > max_ratio) {
            score_cutoff = max_ratio = ls_ratio;
        }
    }

    return max_ratio;
}

template<typename Sentence1, typename Sentence2, typename CharT>
percent fuzz::token_sort_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
    if (score_cutoff > 100) {
        return 0;
    }

    string_view_vec<CharT> tokens_a = string_utils::splitSV(s1);
    std::sort(tokens_a.begin(), tokens_a.end());
    string_view_vec<CharT> tokens_b = string_utils::splitSV(s2);
    std::sort(tokens_b.begin(), tokens_b.end());

    return levenshtein::normalized_weighted_distance(
        string_utils::join(tokens_a),
        string_utils::join(tokens_b),
        score_cutoff / 100
    ) * 100;
}

template<typename Sentence1, typename Sentence2, typename CharT>
percent fuzz::partial_token_sort_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
    if (score_cutoff > 100) {
        return 0;
    }

    string_view_vec<CharT> tokens_a = string_utils::splitSV(s1);
    std::sort(tokens_a.begin(), tokens_a.end());
    string_view_vec<CharT> tokens_b = string_utils::splitSV(s2);
    std::sort(tokens_b.begin(), tokens_b.end());

    return fuzz::partial_ratio(
        string_utils::join(tokens_a),
        string_utils::join(tokens_b),
        score_cutoff);
}

template<typename Sentence1, typename Sentence2, typename CharT>
percent fuzz::token_set_ratio(const Sentence1& s1, const Sentence2& s2, const percent score_cutoff)
{
    if (score_cutoff > 100) {
        return 0;
    }

    string_view_vec<CharT> tokens_a = string_utils::splitSV(s1);
    std::sort(tokens_a.begin(), tokens_a.end());
    string_view_vec<CharT> tokens_b = string_utils::splitSV(s2);
    std::sort(tokens_b.begin(), tokens_b.end());

    auto decomposition = utils::set_decomposition(tokens_a, tokens_b);
    auto intersect = decomposition.intersection;
    auto diff_ab = decomposition.difference_ab;
    auto diff_ba = decomposition.difference_ba;

    // one sentence is part of the other one
    if (!intersect.empty() && (diff_ab.empty() || diff_ba.empty())) {
        return 100;
    }

    std::basic_string<CharT> diff_ab_joined = string_utils::join(diff_ab);
    std::basic_string<CharT> diff_ba_joined = string_utils::join(diff_ba);

    std::size_t ab_len = diff_ab_joined.length();
    std::size_t ba_len = diff_ba_joined.length();
    std::size_t sect_len = string_utils::joined_size(intersect);

    // string length sect+ab <-> sect and sect+ba <-> sect
    std::size_t sect_ab_len = sect_len + !!sect_len + ab_len;
    std::size_t sect_ba_len = sect_len + !!sect_len + ba_len;

    auto lev_filter = levenshtein::detail::quick_lev_filter(
        basic_string_view<CharT>(diff_ab_joined),
        basic_string_view<CharT>(diff_ba_joined),
        score_cutoff / 100);

    double result = 0;
    if (lev_filter.not_zero) {
        std::size_t dist =  levenshtein::weighted_distance(lev_filter.s1_view, lev_filter.s2_view);
        double ratio = 1.0 - static_cast<double>(dist) / static_cast<double>(sect_ab_len + sect_ba_len);
        
        result = utils::result_cutoff(ratio, score_cutoff / 100);
    }

    // exit early since the other ratios are 0
    if (intersect.empty()) {
        return utils::result_cutoff(result * 100, score_cutoff);
    }

    // levenshtein distance sect+ab <-> sect and sect+ba <-> sect
    // since only sect is similar in them the distance can be calculated based on the length difference
    std::size_t sect_ab_distance = sect_ab_len - sect_len;
    std::size_t sect_ba_distance = sect_ba_len - sect_len;

    result = std::max({ result,
        1.0 - static_cast<double>(sect_ab_distance) / static_cast<double>(sect_len + sect_ab_len),
        1.0 - static_cast<double>(sect_ba_distance) / static_cast<double>(sect_len + sect_ba_len) });
    return utils::result_cutoff(result * 100, score_cutoff);
}

template<typename Sentence1, typename Sentence2, typename CharT>
percent fuzz::partial_token_set_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
    if (score_cutoff > 100) {
        return 0;
    }

    string_view_vec<CharT> tokens_a = string_utils::splitSV(s1);
    std::sort(tokens_a.begin(), tokens_a.end());
    string_view_vec<CharT> tokens_b = string_utils::splitSV(s2);
    std::sort(tokens_b.begin(), tokens_b.end());

    tokens_a.erase(std::unique(tokens_a.begin(), tokens_a.end()), tokens_a.end());
    tokens_b.erase(std::unique(tokens_b.begin(), tokens_b.end()), tokens_b.end());

    string_view_vec<CharT> difference_ab;
    std::set_difference(tokens_a.begin(), tokens_a.end(), tokens_b.begin(), tokens_b.end(),
        std::back_inserter(difference_ab));

    string_view_vec<CharT> difference_ba;
    std::set_difference(tokens_b.begin(), tokens_b.end(), tokens_a.begin(), tokens_a.end(),
        std::back_inserter(difference_ba));

    // exit early when there is a common word in both sequences
    if (difference_ab.size() < tokens_a.size()) {
        return 100;
    }

    return partial_ratio(string_utils::join(difference_ab), string_utils::join(difference_ba), score_cutoff);
}

template<typename Sentence1, typename Sentence2, typename CharT>
percent fuzz::token_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
    if (score_cutoff > 100) {
        return 0;
    }

    string_view_vec<CharT> tokens_a = string_utils::splitSV(s1);
    std::sort(tokens_a.begin(), tokens_a.end());
    string_view_vec<CharT> tokens_b = string_utils::splitSV(s2);
    std::sort(tokens_b.begin(), tokens_b.end());

    auto decomposition = utils::set_decomposition(tokens_a, tokens_b);
    auto intersect = decomposition.intersection;
    auto diff_ab = decomposition.difference_ab;
    auto diff_ba = decomposition.difference_ba;

    if (!intersect.empty() && (diff_ab.empty() || diff_ba.empty())) {
        return 100;
    }

    std::basic_string<CharT> diff_ab_joined = string_utils::join(diff_ab);
    std::basic_string<CharT> diff_ba_joined = string_utils::join(diff_ba);

    std::size_t ab_len = diff_ab_joined.length();
    std::size_t ba_len = diff_ba_joined.length();
    std::size_t sect_len = string_utils::joined_size(intersect);

    double result = levenshtein::normalized_weighted_distance(
        string_utils::join(tokens_a),
        string_utils::join(tokens_b),
        score_cutoff / 100);

    // string length sect+ab <-> sect and sect+ba <-> sect
    std::size_t sect_ab_lensum = sect_len + !!sect_len + ab_len;
    std::size_t sect_ba_lensum = sect_len + !!sect_len + ba_len;

    auto lev_filter = levenshtein::detail::quick_lev_filter(
        basic_string_view<CharT>(diff_ab_joined),
        basic_string_view<CharT>(diff_ba_joined),
        score_cutoff / 100);

    if (lev_filter.not_zero) {
        std::size_t dist =  levenshtein::weighted_distance(lev_filter.s1_view, lev_filter.s2_view);
        double ratio = 1.0 - static_cast<double>(dist) / static_cast<double>(sect_ab_lensum + sect_ba_lensum);
        
        result = std::max(result, utils::result_cutoff(ratio, score_cutoff / 100));
    }

    // exit early since the other ratios are 0
    if (!sect_len) {
        return utils::result_cutoff(result * 100, score_cutoff);
    }

    // levenshtein distance sect+ab <-> sect and sect+ba <-> sect
    // would exit early after removing the prefix sect, so the distance can be directly calculated
    std::size_t sect_ab_distance = !!sect_len + ab_len;
    std::size_t sect_ba_distance = !!sect_len + ba_len;

    result = std::max({ result,
        // levenshtein distances sect+ab <-> sect and sect+ba <-> sect
        // would exit early after removing the prefix sect, so the distance can be directly calculated
        1.0 - static_cast<double>(sect_ab_distance) / static_cast<double>(sect_len + sect_ab_lensum),
        1.0 - static_cast<double>(sect_ba_distance) / static_cast<double>(sect_len + sect_ba_lensum) });
    return utils::result_cutoff(result * 100, score_cutoff);
}

template<typename Sentence1, typename Sentence2, typename CharT>
percent fuzz::partial_token_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
    if (score_cutoff > 100) {
        return 0;
    }

    string_view_vec<CharT> tokens_a = string_utils::splitSV(s1);
    std::sort(tokens_a.begin(), tokens_a.end());
    string_view_vec<CharT> tokens_b = string_utils::splitSV(s2);
    std::sort(tokens_b.begin(), tokens_b.end());

    auto unique_a = tokens_a;
    auto unique_b = tokens_b;
    unique_a.erase(std::unique(unique_a.begin(), unique_a.end()), unique_a.end());
    unique_b.erase(std::unique(unique_b.begin(), unique_b.end()), unique_b.end());

    string_view_vec<CharT> difference_ab;
    std::set_difference(unique_a.begin(), unique_a.end(), unique_b.begin(), unique_b.end(),
        std::back_inserter(difference_ab));

    string_view_vec<CharT> difference_ba;
    std::set_difference(unique_b.begin(), unique_b.end(), unique_a.begin(), unique_a.end(),
        std::back_inserter(difference_ba));

    // exit early when there is a common word in both sequences
    if (difference_ab.size() < unique_a.size()) {
        return 100;
    }

    percent result = partial_ratio(string_utils::join(tokens_a), string_utils::join(tokens_b), score_cutoff);
    // do not calculate the same partial_ratio twice
    if (tokens_a.size() == unique_a.size() && tokens_b.size() == unique_b.size()) {
        return result;
    }

    score_cutoff = std::max(score_cutoff, result);
    return std::max(
        result,
        partial_ratio(string_utils::join(difference_ab), string_utils::join(difference_ba), score_cutoff));
}

template<typename Sentence1, typename Sentence2, typename CharT>
percent fuzz::quick_lev_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
    if (utils::is_zero(length_ratio(s1, s2, score_cutoff))) {
        return 0;
    }
    std::size_t distance = string_utils::count_uncommon_chars(s1, s2);
    std::size_t lensum = s1.length() + s2.length();
    percent result = 1.0 - static_cast<double>(distance) / static_cast<double>(lensum);

    return utils::result_cutoff(result * 100, score_cutoff);
}


template<typename Sentence1, typename Sentence2, typename CharT>
percent fuzz::length_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
    std::size_t s1_len = s1.length();
    std::size_t s2_len = s2.length();
    std::size_t distance = (s1_len > s2_len)
        ? s1_len - s2_len
        : s2_len - s1_len;
    
    std::size_t lensum = s1_len + s2_len;
    double result = 1.0 - static_cast<double>(distance) / static_cast<double>(lensum);
    return utils::result_cutoff(result * 100, score_cutoff);
}

template<typename Sentence1, typename Sentence2, typename CharT>
percent fuzz::WRatio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
    if (score_cutoff > 100) {
        return 0;
    }

    constexpr double UNBASE_SCALE = 0.95;

    auto s1_view = utils::to_string_view(s1);
    auto s2_view = utils::to_string_view(s2);

    std::size_t len_a = s1_view.length();
    std::size_t len_b = s2_view.length();
    double len_ratio = (len_a > len_b)
        ? static_cast<double>(len_a) / static_cast<double>(len_b)
        : static_cast<double>(len_b) / static_cast<double>(len_a);

    if (len_ratio < 1.5) {
        auto lev_filter = levenshtein::detail::quick_lev_filter(s1_view, s2_view, score_cutoff / 100);
        
        // ratio and token_sort ratio are not required so token_set_ratio / partial_token_set_ratio is enough
        if (!lev_filter.not_zero) {
            return token_set_ratio(s1, s2, score_cutoff / UNBASE_SCALE) * UNBASE_SCALE;
        }

        std::size_t dist = levenshtein::weighted_distance(lev_filter.s1_view, lev_filter.s2_view);
        percent end_ratio = utils::result_cutoff(
            100.0 - 100.0 * static_cast<double>(dist) / static_cast<double>(s1_view.length() + s2_view.length()),
            score_cutoff);

        score_cutoff = std::max(score_cutoff, end_ratio + 0.00001) / UNBASE_SCALE;
        return std::max(end_ratio, token_ratio(s1_view, s2_view, score_cutoff) * UNBASE_SCALE);
    }

    percent end_ratio = ratio(s1, s2, score_cutoff);

    const double PARTIAL_SCALE = (len_ratio < 8.0) ? 0.9 : 0.6;

    // increase the score_cutoff by a small step so it might be able to exit early
    score_cutoff = std::max(score_cutoff, end_ratio + 0.00001) / PARTIAL_SCALE;
    end_ratio = std::max(end_ratio, partial_ratio(s1, s2, score_cutoff) * PARTIAL_SCALE);

    score_cutoff = std::max(score_cutoff, end_ratio + 0.00001) / UNBASE_SCALE;
    return std::max(end_ratio, partial_token_ratio(s1, s2, score_cutoff) * UNBASE_SCALE * PARTIAL_SCALE);
}

} /* rapidfuzz */
