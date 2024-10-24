/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include "../rapidfuzz_reference/OSA.hpp"
#include "fuzzing.hpp"
#include <rapidfuzz/details/Range.hpp>
#include <rapidfuzz/distance/OSA.hpp>
#include <stdexcept>
#include <string>

void validate_distance(size_t reference_dist, const std::vector<uint8_t>& s1, const std::vector<uint8_t>& s2,
                       size_t score_cutoff)
{
    if (reference_dist > score_cutoff) reference_dist = score_cutoff + 1;

    auto dist = rapidfuzz::osa_distance(s1, s2, score_cutoff);
    if (dist != reference_dist) {
        print_seq("s1", s1);
        print_seq("s2", s2);
        throw std::logic_error(std::string("osa distance failed (score_cutoff = ") +
                               std::to_string(score_cutoff) + std::string(", reference_score = ") +
                               std::to_string(reference_dist) + std::string(", score = ") +
                               std::to_string(dist) + ")");
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::vector<uint8_t> s1, s2;
    if (!extract_strings(data, size, s1, s2)) return 0;

    size_t reference_dist = rapidfuzz_reference::osa_distance(s1, s2);

    /* test small band */
    for (size_t i = 4; i < 32; ++i)
        validate_distance(reference_dist, s1, s2, i);

    /* unrestricted */
    validate_distance(reference_dist, s1, s2, std::numeric_limits<size_t>::max());

    /* test long sequences */
    for (unsigned int i = 2; i < 9; ++i) {
        std::vector<uint8_t> s1_ = vec_multiply(s1, pow<size_t>(2, i));
        std::vector<uint8_t> s2_ = vec_multiply(s2, pow<size_t>(2, i));

        if (s1_.size() > 10000 || s2_.size() > 10000) break;

        reference_dist = rapidfuzz_reference::osa_distance(s1_, s2_);
        validate_distance(reference_dist, s1_, s2_, std::numeric_limits<size_t>::max());
    }

    return 0;
}
