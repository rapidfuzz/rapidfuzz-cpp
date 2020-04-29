/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#include "levenshtein.hpp"
#include "string_utils.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

namespace rapidfuzz {

template<
    typename Sentence1, typename Sentence2,
    typename CharT, typename
>
std::size_t levenshtein::distance(const Sentence1& s1, const Sentence2& s2)
{
    basic_string_view<CharT> sentence1(s1);
    basic_string_view<CharT> sentence2(s2);
    string_utils::remove_common_affix(sentence1, sentence2);

    if (sentence2.size() > sentence1.size())
        std::swap(sentence1, sentence2);

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

template<
    typename Sentence1, typename Sentence2,
    typename CharT, typename
>
std::size_t levenshtein::weighted_distance(const Sentence1& s1, const Sentence2& s2)
{
    basic_string_view<CharT> sentence1(s1);
    basic_string_view<CharT> sentence2(s2);
    string_utils::remove_common_affix(sentence1, sentence2);

    if (sentence2.size() > sentence1.size()) {
        std::swap(sentence1, sentence2);
    }

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

template<
    typename Sentence1, typename Sentence2,
    typename CharT, typename
>
std::size_t levenshtein::generic_distance(const Sentence1& s1, const Sentence2& s2, WeightTable weights)
{
    basic_string_view<CharT> sentence1(s1);
    basic_string_view<CharT> sentence2(s2);
    string_utils::remove_common_affix(sentence1, sentence2);
    if (sentence1.size() > sentence2.size()) {
        std::swap(sentence1, sentence2);
        std::swap(weights.insert_cost, weights.delete_cost);
    }

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

template<
    typename Sentence1, typename Sentence2,
    typename CharT, typename
>
double levenshtein::normalized_distance(const Sentence1& s1, const Sentence2& s2, const double min_ratio)
{
    basic_string_view<CharT> sentence1(s1);
    basic_string_view<CharT> sentence2(s2);

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

template<
    typename Sentence1, typename Sentence2,
    typename CharT, typename
>
double levenshtein::normalized_weighted_distance(const Sentence1& s1, const Sentence2& s2, const double min_ratio)
{
    basic_string_view<CharT> sentence1(s1);
    basic_string_view<CharT> sentence2(s2);

    if (sentence1.empty() || sentence2.empty()) {
        return sentence1.empty() && sentence2.empty();
    }

    std::size_t sentence1_len = sentence1.length();
    std::size_t sentence2_len = sentence2.length();
    std::size_t lensum = sentence1_len + sentence2_len;

    // constant time calculation to find a string ratio based on the string length
    // so it can exit early without running any levenshtein calculations
    std::size_t min_distance = (sentence1_len > sentence2_len)
        ? sentence1_len - sentence2_len
        : sentence2_len - sentence1_len;

    double len_ratio = 1.0 - static_cast<double>(min_distance) / lensum;
    if (len_ratio < min_ratio) {
        return 0.0;
    }

    std::size_t dist =  weighted_distance(sentence1, sentence2);

    double ratio = 1.0 - static_cast<double>(dist) / lensum;
    return (ratio >= min_ratio) ? ratio : 0.0;
}

} /* rapidfuzz */
