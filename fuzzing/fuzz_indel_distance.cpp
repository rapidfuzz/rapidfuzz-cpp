/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include "../rapidfuzz_reference/Indel.hpp"
#include "fuzzing.hpp"
#include <limits>
#include <rapidfuzz/distance/Indel.hpp>
#include <stdexcept>
#include <string>

void validate_distance(const std::vector<uint8_t>& s1, const std::vector<uint8_t>& s2, size_t score_cutoff)
{
    auto dist = rapidfuzz::indel_distance(s1, s2, score_cutoff);
    auto reference_dist = rapidfuzz_reference::indel_distance(s1, s2, score_cutoff);
    if (dist != reference_dist) {
        print_seq("s1: ", s1);
        print_seq("s2: ", s2);
        throw std::logic_error(std::string("indel distance failed (score_cutoff = ") +
                               std::to_string(score_cutoff) + std::string(", reference_score = ") +
                               std::to_string(reference_dist) + std::string(", score = ") +
                               std::to_string(dist) + ")");
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::vector<uint8_t> s1, s2;
    if (!extract_strings(data, size, s1, s2)) return 0;

    validate_distance(s1, s2, 0);
    validate_distance(s1, s2, 1);
    validate_distance(s1, s2, 2);
    validate_distance(s1, s2, 3);
    validate_distance(s1, s2, 4);
    /* score_cutoff to trigger banded implementation */
    validate_distance(s1, s2, s1.size() / 2);
    validate_distance(s1, s2, s2.size() / 2);

    /* unrestricted */
    validate_distance(s1, s2, std::numeric_limits<size_t>::max());

    return 0;
}
