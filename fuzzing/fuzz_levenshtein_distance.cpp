/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include "../rapidfuzz_reference/Levenshtein.hpp"
#include "fuzzing.hpp"
#include <rapidfuzz/details/Range.hpp>
#include <rapidfuzz/distance/Levenshtein.hpp>
#include <stdexcept>
#include <string>

template <size_t MaxLen>
void validate_simd(const std::vector<uint8_t>& s1, const std::vector<uint8_t>& s2)
{
#ifdef RAPIDFUZZ_SIMD
    size_t count = s1.size() / MaxLen + ((s1.size() % MaxLen) != 0);
    if (count == 0) return;

    rapidfuzz::experimental::MultiLevenshtein<MaxLen> scorer(count);

    std::vector<std::vector<uint8_t>> strings;

    for (auto it1 = s1.begin(); it1 != s1.end(); it1 += MaxLen) {
        if (std::distance(it1, s1.end()) < static_cast<ptrdiff_t>(MaxLen)) {
            strings.emplace_back(it1, s1.end());
            break;
        }
        else {
            strings.emplace_back(it1, it1 + MaxLen);
        }
    }

    for (const auto& s : strings)
        scorer.insert(s);

    std::vector<size_t> simd_results(scorer.result_count());
    scorer.distance(&simd_results[0], simd_results.size(), s2);

    for (size_t i = 0; i < strings.size(); ++i) {
        size_t reference_score = rapidfuzz_reference::levenshtein_distance(strings[i], s2);
        if (reference_score != simd_results[i]) {
            print_seq("s1: ", strings[i]);
            print_seq("s2: ", s2);
            throw std::logic_error(std::string("levenshtein distance using simd failed (reference_score = ") +
                                   std::to_string(reference_score) + std::string(", score = ") +
                                   std::to_string(simd_results[i]) + std::string(", i = ") +
                                   std::to_string(i) + ")");
        }
    }
#else
    (void)s1;
    (void)s2;
#endif
}

void validate_distance(size_t reference_dist, const std::vector<uint8_t>& s1, const std::vector<uint8_t>& s2,
                       size_t score_cutoff)
{
    if (reference_dist > score_cutoff) reference_dist = score_cutoff + 1;

    auto dist = rapidfuzz::levenshtein_distance(s1, s2, {1, 1, 1}, score_cutoff);
    if (dist != reference_dist) {
        print_seq("s1: ", s1);
        print_seq("s2: ", s2);
        throw std::logic_error(std::string("levenshtein distance failed (score_cutoff = ") +
                               std::to_string(score_cutoff) + std::string(", reference_score = ") +
                               std::to_string(reference_dist) + std::string(", score = ") +
                               std::to_string(dist) + ")");
    }

    validate_simd<8>(s1, s2);
    validate_simd<16>(s1, s2);
    validate_simd<32>(s1, s2);
    validate_simd<64>(s1, s2);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::vector<uint8_t> s1, s2;
    if (!extract_strings(data, size, s1, s2)) return 0;

    size_t reference_dist = rapidfuzz_reference::levenshtein_distance(s1, s2);

    /* test mbleven */
    for (size_t i = 0; i < 4; ++i)
        validate_distance(reference_dist, s1, s2, i);

    /* test small band */
    for (size_t i = 4; i < 32; ++i)
        validate_distance(reference_dist, s1, s2, i);

    /* unrestricted */
    validate_distance(reference_dist, s1, s2, std::numeric_limits<size_t>::max());

    /* score_cutoff to trigger banded implementation */
    validate_distance(reference_dist, s1, s2, s1.size() / 2);
    validate_distance(reference_dist, s1, s2, s2.size() / 2);

    return 0;
}
