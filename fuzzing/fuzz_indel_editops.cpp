/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include "../rapidfuzz_reference/Indel.hpp"
#include "fuzzing.hpp"
#include <rapidfuzz/distance.hpp>
#include <stdexcept>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::vector<uint8_t> s1, s2;
    if (!extract_strings(data, size, s1, s2)) return 0;

    size_t score = rapidfuzz_reference::indel_distance(s1, s2);
    rapidfuzz::Editops ops = rapidfuzz::indel_editops(s1, s2);

    if (ops.size() == score && s2 != rapidfuzz::editops_apply_vec<uint8_t>(ops, s1, s2))
        throw std::logic_error("levenshtein_editops failed");

    return 0;
}
