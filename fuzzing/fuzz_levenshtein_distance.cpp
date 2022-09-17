/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include "../rapidfuzz_reference/Levenshtein.hpp"
#include "fuzzing.hpp"
#include <rapidfuzz/details/Range.hpp>
#include <rapidfuzz/distance/Levenshtein.hpp>
#include <stdexcept>
#include <string>

void validate_distance(int64_t reference_dist, const std::basic_string<uint8_t>& s1,
                       const std::basic_string<uint8_t>& s2, int64_t score_cutoff)
{
    if (reference_dist > score_cutoff) reference_dist = score_cutoff + 1;

    auto dist = rapidfuzz::levenshtein_distance(s1, s2, {1, 1, 1}, score_cutoff);
    if (dist != reference_dist) {
        print_seq("s1: ", s1);
        print_seq("s2: ", s2);
        throw std::logic_error(std::string("levenshtein distance failed with score_cutoff: ") +
                               std::to_string(score_cutoff));
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::basic_string<uint8_t> s1, s2;
    if (!extract_strings(data, size, s1, s2)) {
        return 0;
    }

    int64_t reference_dist = rapidfuzz_reference::levenshtein_distance(s1, s2);

    /* test mbleven */
    for (int64_t i = 0; i < 4; ++i)
        validate_distance(reference_dist, s1, s2, i);

    /* test small band */
    for (int64_t i = 4; i < 32; ++i)
        validate_distance(reference_dist, s1, s2, i);

    /* unrestricted */
    validate_distance(reference_dist, s1, s2, std::numeric_limits<int64_t>::max());

    return 0;
}
