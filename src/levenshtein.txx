/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#include "levenshtein.hpp"
#include "string_utils.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

namespace rapidfuzz {

template<typename Sentence1, typename Sentence2>
std::size_t levenshtein::distance(const Sentence1& s1, const Sentence2& s2)
{
    auto sentence1 = utils::to_string_view(s1);
    auto sentence2 = utils::to_string_view(s2);
    
    if (sentence2.size() > sentence1.size()) {
        return distance(sentence2, sentence1);
    }

    string_utils::remove_common_affix(sentence1, sentence2);

    if (sentence2.empty()) {
        return sentence1.length();
    }

    std::vector<std::size_t> cache(sentence2.length() + 1);
    std::iota(cache.begin(), cache.end(), 0);

    for (const auto& char1 : sentence1) {
        auto cache_iter = cache.begin();
        std::size_t temp = *cache_iter;
        *cache_iter += 1;

        for (const auto& char2 : sentence2) {
            if (char1 != char2) {
                ++temp;
            }

            temp = std::min({
                *cache_iter + 1,
                *(++cache_iter) + 1,
                temp
            });
            std::swap(*cache_iter, temp);
        }
    }
    return cache.back();
}

template<typename Sentence1, typename Sentence2>
std::size_t levenshtein::weighted_distance(const Sentence1& s1, const Sentence2& s2)
{
    auto sentence1 = utils::to_string_view(s1);
    auto sentence2 = utils::to_string_view(s2);

    if (sentence2.size() > sentence1.size()) {
        return weighted_distance(sentence2, sentence1);
    }

    string_utils::remove_common_affix(sentence1, sentence2);

    if (sentence2.empty()) {
        return sentence1.length();
    }

    std::vector<std::size_t> cache(sentence2.length());
    std::iota(cache.begin(), cache.end(), 1);

    std::size_t sentence1_pos = 0;
    for (const auto& char1 : sentence1) {
        auto cache_iter = cache.begin();
        std::size_t current_cache = sentence1_pos;
        std::size_t result = sentence1_pos + 1;
        for (const auto& char2 : sentence2) {
            if (char1 == char2) {
                result = current_cache;
            } else {
                ++result;
            }
            current_cache = *cache_iter;
            if (result > current_cache + 1) {
                result = current_cache + 1;
            }

            *cache_iter = result;
            ++cache_iter;
        }

        ++sentence1_pos;
    }
    return cache.back();
}

template<typename Sentence1, typename Sentence2>
std::size_t levenshtein::generic_distance(const Sentence1& s1, const Sentence2& s2, WeightTable weights)
{
    auto sentence1 = utils::to_string_view(s1);
    auto sentence2 = utils::to_string_view(s2);

    if (sentence1.size() > sentence2.size()) {
        std::swap(weights.insert_cost, weights.delete_cost);
        return generic_distance(sentence2, sentence1, weights);   
    }

    string_utils::remove_common_affix(sentence1, sentence2);

    std::vector<std::size_t> cache(sentence1.size() + 1);

    cache[0] = 0;
    for (std::size_t i = 1; i < cache.size(); ++i) {
        cache[i] = cache[i - 1] + weights.delete_cost;
    }

    for (const auto& char2 : sentence2) {
        auto cache_iter = cache.begin();
        std::size_t temp = *cache_iter;
        *cache_iter += weights.insert_cost;

        for (const auto& char1 : sentence1) {
            if (char1 != char2) {
                temp = std::min({ *cache_iter + weights.delete_cost,
                    *(cache_iter + 1) + weights.insert_cost,
                    temp + weights.replace_cost });
            }
            ++cache_iter;
            std::swap(*cache_iter, temp);
        }
    }

    return cache.back();
}

template<typename Sentence1, typename Sentence2>
double levenshtein::normalized_distance(const Sentence1& s1, const Sentence2& s2, const double min_ratio)
{
    auto sentence1 = utils::to_string_view(s1);
    auto sentence2 = utils::to_string_view(s2);

    if (sentence1.empty() || sentence2.empty()) {
        return sentence1.empty() && sentence2.empty();
    }

    std::size_t sentence1_len = sentence1.length();
    std::size_t sentence2_len = sentence2.length();
    std::size_t max_len = std::max(sentence1_len, sentence2_len);

    // constant time calculation to find a string ratio based on the string length
    // so it can exit early without running any levenshtein calculations
    std::size_t min_distance = (sentence1_len > sentence2_len)
        ? sentence1_len - sentence2_len
        : sentence2_len - sentence1_len;

    double len_ratio = 1.0 - static_cast<double>(min_distance) / max_len;
    if (len_ratio < min_ratio) {
        return 0.0;
    }

    std::size_t dist = distance(sentence1, sentence2);

    double ratio = 1.0 - static_cast<double>(dist) / max_len;
    return (ratio >= min_ratio) ? ratio : 0.0;
}

template<typename Sentence1, typename Sentence2>
double levenshtein::normalized_weighted_distance(const Sentence1& s1, const Sentence2& s2, const double min_ratio)
{
    auto sentence1 = utils::to_string_view(s1);
    auto sentence2 = utils::to_string_view(s2);

    if (sentence1.empty() || sentence2.empty()) {
        return sentence1.empty() && sentence2.empty();
    }

    std::size_t sentence1_len = sentence1.length();
    std::size_t sentence2_len = sentence2.length();
    std::size_t lensum = sentence1_len + sentence2_len;

    auto lev_filter = detail::quick_lev_filter(sentence1, sentence2, min_ratio);

    if (!lev_filter.not_zero) {
        return 0.0;
    }

    // calculate the levenshtein distance in quadratic time
    std::size_t dist =  weighted_distance(lev_filter.s1_view, lev_filter.s2_view);
    double ratio = 1.0 - static_cast<double>(dist) / static_cast<double>(lensum);
    return utils::result_cutoff(ratio, min_ratio);
}

template<typename CharT1, typename CharT2>
levenshtein::detail::LevFilter<CharT1, CharT2>
levenshtein::detail::quick_lev_filter(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2, const double min_ratio)
{
    if (utils::is_zero(min_ratio)) {
        return {true, s1, s2};
    }

    std::size_t s1_len = s1.length();
    std::size_t s2_len = s2.length();
    std::size_t lensum = s1_len + s2_len;
    
    std::size_t cutoff_distance = static_cast<double>(lensum) * (1.0 - min_ratio);

    // constant time calculation to find a string ratio based on the string length
    // so it can exit early without running any levenshtein calculations
    std::size_t length_distance = (s1_len > s2_len) ? s1_len - s2_len : s2_len - s1_len;

    if (length_distance > cutoff_distance) {
        return {false, s1, s2};
    }

    // remove common prefix + suffix in constant time
    string_utils::remove_common_affix(s1, s2);

    if (s1.empty()) {
        double ratio = 1.0 - static_cast<double>(s2.length()) / static_cast<double>(lensum);
        return {ratio >= min_ratio, s1, s2};
    }

    if (s2.empty()) {
        double ratio = 1.0 - static_cast<double>(s1.length()) / static_cast<double>(lensum);
        return {ratio >= min_ratio, s1, s2};
    }

    // find uncommon chars in the two sequences to exit early in many cases in linear time
    std::size_t uncommon_char_distance = string_utils::count_uncommon_chars(s1, s2);
    return {uncommon_char_distance <= cutoff_distance, s1, s2};
}


} /* rapidfuzz */
