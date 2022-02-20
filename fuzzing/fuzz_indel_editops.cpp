/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include "fuzzing.hpp"
#include <rapidfuzz/distance/Indel.hpp>
#include <stdexcept>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::basic_string<uint8_t> s1, s2;
    if (!extract_strings(data, size, s1, s2)) {
        return 0;
    }

    rapidfuzz::indel_editops(s1, s2);
    return 0;
}
