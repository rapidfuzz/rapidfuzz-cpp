/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include "fuzzing.hpp"
#include <limits>
#include <rapidfuzz/distance/Indel.hpp>
#include <rapidfuzz/distance/Levenshtein.hpp>
#include <stdexcept>
#include <string>

void validate_distance(const std::basic_string<uint8_t>& s1, const std::basic_string<uint8_t>& s2,
                       int64_t score_cutoff)
{
    auto dist = rapidfuzz::indel_distance(s1, s2, score_cutoff);
    auto reference_dist = rapidfuzz::detail::generalized_levenshtein_distance(
        std::begin(s1), std::end(s1), std::begin(s2), std::end(s2), {1, 1, 2}, score_cutoff);
    if (dist != reference_dist) {
        throw std::logic_error("indel distance failed");
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::basic_string<uint8_t> s1, s2;
    if (!extract_strings(data, size, s1, s2)) {
        return 0;
    }

    validate_distance(s1, s2, 0);
    validate_distance(s1, s2, 1);
    validate_distance(s1, s2, 2);
    validate_distance(s1, s2, 3);
    validate_distance(s1, s2, 4);
    validate_distance(s1, s2, std::numeric_limits<int64_t>::max());

    return 0;
}
