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

namespace rapidfuzz {

template<
    typename Sentence1, typename Sentence2,
	typename CharT, typename
>
percent fuzz::ratio(const Sentence1 &s1, const Sentence2 &s2, const percent score_cutoff)
{
    double result = levenshtein::normalized_weighted_distance(
        basic_string_view<CharT>(s1),
        basic_string_view<CharT>(s2),
        score_cutoff / 100);
    return result * 100;
}


template<
    typename Sentence1, typename Sentence2,
	typename CharT, typename
>
percent fuzz::partial_ratio(const Sentence1 &s1, const Sentence2 &s2, percent score_cutoff)
{
    using std::get;

    if (score_cutoff > 100) {
        return 0;
    }

    basic_string_view<CharT> s1_view(s1);
    basic_string_view<CharT> s2_view(s2);

    if (s1_view.empty() || s2_view.empty()) {
        return 0;
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
        basic_string_view<CharT> long_substr = s2_view.substr(long_start, short_len);

        double ls_ratio = ratio(s1_view, long_substr, score_cutoff);

        if (ls_ratio > 99.5) {
            return 100;
        }

        if (ls_ratio > max_ratio) {
            score_cutoff = max_ratio = ls_ratio;
        }
    }

    return max_ratio;
}

template<
    typename Sentence1, typename Sentence2,
	typename CharT, typename
>
percent fuzz::token_sort_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
    if (score_cutoff > 100) {
        return 0;
    }

    if (!length_ratio(
        Sentence<CharT>(s1),
        Sentence<CharT>(s2),
        score_cutoff))
    {
        return 0;
    }

    if (!bitmap_ratio(
        Sentence<CharT>(s1, utils::bitmap_create(s1)),
        Sentence<CharT>(s2, utils::bitmap_create(s2)),
        score_cutoff))
    {
        return 0;
    }

    string_view_vec<CharT> tokens_a = string_utils::splitSV(
        basic_string_view<CharT>(s1));
    std::sort(tokens_a.begin(), tokens_a.end());
    string_view_vec<CharT> tokens_b = string_utils::splitSV(
        basic_string_view<CharT>(s2));
    std::sort(tokens_b.begin(), tokens_b.end());

    return levenshtein::normalized_weighted_distance(
        string_utils::join(tokens_a),
        string_utils::join(tokens_b),
        score_cutoff / 100
    ) * 100;
}

template<
    typename Sentence1, typename Sentence2,
	typename CharT, typename
>
percent fuzz::partial_token_sort_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
    if (score_cutoff > 100) {
        return 0;
    }

    string_view_vec<CharT> tokens_a = string_utils::splitSV(
        basic_string_view<CharT>(s1));
    std::sort(tokens_a.begin(), tokens_a.end());
    string_view_vec<CharT> tokens_b = string_utils::splitSV(
        basic_string_view<CharT>(s2));
    std::sort(tokens_b.begin(), tokens_b.end());

    return fuzz::partial_ratio(
        string_utils::join(tokens_a),
        string_utils::join(tokens_b),
        score_cutoff);
}


template<
    typename Sentence1, typename Sentence2,
	typename CharT, typename
>
percent fuzz::token_set_ratio(const Sentence1& s1, const Sentence2& s2, const percent score_cutoff)
{
    if (score_cutoff > 100) {
        return 0;
    }

    string_view_vec<CharT> tokens_a = string_utils::splitSV(
        basic_string_view<CharT>(s1));
    std::sort(tokens_a.begin(), tokens_a.end());
    string_view_vec<CharT> tokens_b = string_utils::splitSV(
        basic_string_view<CharT>(s2));
    std::sort(tokens_b.begin(), tokens_b.end());

    auto decomposition = utils::set_decomposition(tokens_a, tokens_b);
    auto intersection = decomposition.intersection;
    auto difference_ab = decomposition.difference_ab;
    auto difference_ba = decomposition.difference_ba;

    std::basic_string<CharT> diff_ab_joined = string_utils::join(difference_ab);
    std::basic_string<CharT> diff_ba_joined = string_utils::join(difference_ba);

    std::size_t ab_len = diff_ab_joined.length();
    std::size_t ba_len = diff_ba_joined.length();
    std::size_t sect_len = string_utils::joined_size(intersection);

    // exit early since this will always result in a ratio of 1
    if (sect_len && (!ab_len || !ba_len)) {
        return 100;
    }

    // string length sect+ab <-> sect and sect+ba <-> sect
    std::size_t sect_ab_lensum = sect_len + !!sect_len + ab_len;
    std::size_t sect_ba_lensum = sect_len + !!sect_len + ba_len;

    // TODO: use len_ratio as constant time evaluation to skip this
    std::size_t sect_distance = levenshtein::weighted_distance(diff_ab_joined, diff_ba_joined);
    double result = 1.0 - sect_distance / static_cast<double>(sect_ab_lensum + sect_ba_lensum);

    // exit early since the other ratios are 0
    if (!sect_len) {
        return utils::result_cutoff(result * 100, score_cutoff);
    }

    // levenshtein distance sect+ab <-> sect and sect+ba <-> sect
    // would exit early after removing the prefix sect, so the distance can be directly calculated
    std::size_t sect_ab_distance = !!sect_len + ab_len;
    std::size_t sect_ba_distance = !!sect_len + ba_len;

    result = std::max({ result,
        1.0 - sect_ab_distance / static_cast<double>(sect_len + sect_ab_lensum),
        1.0 - sect_ba_distance / static_cast<double>(sect_len + sect_ba_lensum) });
    return utils::result_cutoff(result * 100, score_cutoff);
}


template<
    typename Sentence1, typename Sentence2,
	typename CharT, typename
>
percent fuzz::partial_token_set_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
    if (score_cutoff > 100) {
        return 0;
    }

    string_view_vec<CharT> tokens_a = string_utils::splitSV(
        basic_string_view<CharT>(s1));
    std::sort(tokens_a.begin(), tokens_a.end());
    string_view_vec<CharT> tokens_b = string_utils::splitSV(
        basic_string_view<CharT>(s2));
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

template<typename CharT>
percent fuzz::token_ratio(
    const Sentence<CharT>& s1,
    const Sentence<CharT>& s2,
    percent score_cutoff)
{
    if (score_cutoff > 100) {
        return 0;
    }

    string_view_vec<CharT> tokens_a = string_utils::splitSV(s1.sentence);
    std::sort(tokens_a.begin(), tokens_a.end());
    string_view_vec<CharT> tokens_b = string_utils::splitSV(s2.sentence);
    std::sort(tokens_b.begin(), tokens_b.end());

    auto decomposition = utils::set_decomposition(tokens_a, tokens_b);
    auto intersection = decomposition.intersection;
    auto difference_ab = decomposition.difference_ab;
    auto difference_ba = decomposition.difference_ba;

    std::basic_string<CharT> diff_ab_joined = string_utils::join(difference_ab);
    std::basic_string<CharT> diff_ba_joined = string_utils::join(difference_ba);

    std::size_t ab_len = diff_ab_joined.length();
    std::size_t ba_len = diff_ba_joined.length();
    std::size_t sect_len = string_utils::joined_size(intersection);

    // exit early since this will always result in a ratio of 1
    if (sect_len && (!ab_len || !ba_len)) {
        return 100;
    }

    double result = 0;
    if (quick_lev_estimate(s1, s2, score_cutoff)) {
        result = levenshtein::normalized_weighted_distance(
            string_utils::join(tokens_a),
            string_utils::join(tokens_b),
            score_cutoff / 100);
    }

    // string length sect+ab <-> sect and sect+ba <-> sect
    std::size_t sect_ab_lensum = sect_len + !!sect_len + ab_len;
    std::size_t sect_ba_lensum = sect_len + !!sect_len + ba_len;

    Sentence<CharT> diff_ab{diff_ab_joined,  utils::bitmap_create(diff_ab_joined)};
    Sentence<CharT> diff_ba{diff_ba_joined,  utils::bitmap_create(diff_ba_joined)};
    double bm_ratio = 1.0 - bitmap_distance(diff_ab, diff_ba) / static_cast<double>(sect_ab_lensum + sect_ba_lensum);
    if (bm_ratio >= score_cutoff) {
        std::size_t sect_distance = levenshtein::weighted_distance(diff_ab_joined, diff_ba_joined);
        result = std::max(result, 1.0 - sect_distance / static_cast<double>(sect_ab_lensum + sect_ba_lensum));
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
        1.0 - sect_ab_distance / static_cast<double>(sect_len + sect_ab_lensum),
        1.0 - sect_ba_distance / static_cast<double>(sect_len + sect_ba_lensum) });
    return utils::result_cutoff(result * 100, score_cutoff);
}

template<
    typename Sentence1, typename Sentence2,
	typename CharT, typename
>
percent fuzz::partial_token_ratio(const Sentence1& s1, const Sentence2& s2, percent score_cutoff)
{
    if (score_cutoff > 100) {
        return 0;
    }

    string_view_vec<CharT> tokens_a = string_utils::splitSV(
        basic_string_view<CharT>(s1));
    std::sort(tokens_a.begin(), tokens_a.end());
    string_view_vec<CharT> tokens_b = string_utils::splitSV(
        basic_string_view<CharT>(s2));
    std::sort(tokens_b.begin(), tokens_b.end());

    auto unique_a = tokens_a;
    auto unique_b = tokens_b;
    unique_a.erase(std::unique(unique_a.begin(), unique_a.end()), unique_a.end());
    unique_b.erase(std::unique(unique_b.begin(), unique_b.end()), unique_b.end());

    string_view_vec<wchar_t> difference_ab;
    std::set_difference(unique_a.begin(), unique_a.end(), unique_b.begin(), unique_b.end(),
        std::back_inserter(difference_ab));

    string_view_vec<wchar_t> difference_ba;
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

template<typename CharT>
std::size_t fuzz::bitmap_distance(const Sentence<CharT>& s1, const Sentence<CharT>& s2)
{
    uint64_t bitmap1 = s1.bitmap;
    uint64_t bitmap2 = s2.bitmap;

    std::size_t distance = 0;
    while (bitmap1 || bitmap2) {
        uint8_t val1 = bitmap1 & 0b1111;
        uint8_t val2 = bitmap2 & 0b1111;
        distance += std::abs(val1 - val2);
        bitmap1 >>= 4;
        bitmap2 >>= 4;
    }
    return distance;
}

template<typename CharT>
percent fuzz::bitmap_ratio(const Sentence<CharT>& s1, const Sentence<CharT>& s2, percent score_cutoff)
{
    std::size_t distance = bitmap_distance(s1, s2);
    std::size_t lensum = s1.sentence.length() + s2.sentence.length();
    percent result = 1.0 - static_cast<double>(distance) / lensum;

    return utils::result_cutoff(result * 100, score_cutoff);
}


template<typename CharT>
percent fuzz::length_ratio(const Sentence<CharT>& s1, const Sentence<CharT>& s2, percent score_cutoff)
{
    std::size_t s1_len = s1.sentence.length();
    std::size_t s2_len = s2.sentence.length();
    std::size_t distance = (s1_len > s2_len)
        ? s1_len - s2_len
        : s2_len - s1_len;
    
    std::size_t lensum = s1_len + s2_len;
    double result = 1.0 - static_cast<double>(distance) / lensum;
    return utils::result_cutoff(result * 100, score_cutoff);
}

template<typename CharT>
percent fuzz::quick_lev_estimate(const Sentence<CharT>& s1, const Sentence<CharT>& s2, percent score_cutoff)
{
    if (s1.bitmap || s2.bitmap) {
        return bitmap_ratio(s1, s2, score_cutoff);
    } else {
        return length_ratio(s1, s2, score_cutoff);
    }
}

template<typename CharT>
percent fuzz::WRatio(const Sentence<CharT>& s1, const Sentence<CharT>& s2, percent score_cutoff)
{
    if (score_cutoff > 100) {
        return 0;
    }

    const double UNBASE_SCALE = 0.95;

    std::size_t len_a = s1.sentence.length();
    std::size_t len_b = s2.sentence.length();
    double len_ratio = (len_a > len_b) ? static_cast<double>(len_a) / len_b : static_cast<double>(len_b) / len_a;

    double sratio = 0;
    if (quick_lev_estimate(s1, s2, score_cutoff)) {
        sratio = ratio(s1.sentence, s2.sentence, score_cutoff);
        // increase the score_cutoff by a small step so it might be able to exit early
        score_cutoff = std::max(score_cutoff, sratio + 0.00001);
    }

    if (len_ratio < 1.5) {
        return std::max(sratio, token_ratio(s1, s2, score_cutoff / UNBASE_SCALE) * UNBASE_SCALE);
    }

    double partial_scale = (len_ratio < 8.0) ? 0.9 : 0.6;

    score_cutoff /= partial_scale;
    sratio = std::max(sratio, partial_ratio(s1.sentence, s2.sentence, score_cutoff) * partial_scale);

    // increase the score_cutoff by a small step so it might be able to exit early
    score_cutoff = std::max(score_cutoff, sratio + 0.00001) / UNBASE_SCALE;
    return std::max(sratio, partial_token_ratio(s1.sentence, s2.sentence, score_cutoff) * UNBASE_SCALE * partial_scale);
}

} /* rapidfuzz */
