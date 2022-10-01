/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include "../rapidfuzz_reference/Jaro.hpp"
#include "fuzzing.hpp"
#include <rapidfuzz/details/Range.hpp>
#include <rapidfuzz/distance/Jaro.hpp>
#include <stdexcept>
#include <string>

bool is_close(double a, double b, double epsilon)
{
    return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

void validate_distance(const std::basic_string<uint8_t>& s1, const std::basic_string<uint8_t>& s2)
{
    double reference_sim = rapidfuzz_reference::jaro_similarity(s1, s2);
    double sim = rapidfuzz::jaro_similarity(s1, s2);

    if (!is_close(sim, reference_sim, 0.0001)) {
        print_seq("s1", s1);
        print_seq("s2", s2);
        throw std::logic_error(std::string("jaro similarity failed (reference_score = ") +
                               std::to_string(reference_sim) + std::string(", score = ") +
                               std::to_string(sim) + ")");
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::basic_string<uint8_t> s1, s2;
    if (!extract_strings(data, size, s1, s2)) return 0;

    validate_distance(s1, s2);

    /* test long sequences */
    for (unsigned int i = 2; i < 9; ++i) {
        std::basic_string<uint8_t> s1_ = str_multiply(s1, pow<size_t>(2, i));
        std::basic_string<uint8_t> s2_ = str_multiply(s2, pow<size_t>(2, i));

        if (s1_.size() > 10000 || s2_.size() > 10000) break;

        validate_distance(s1_, s2_);
    }

    return 0;
}
