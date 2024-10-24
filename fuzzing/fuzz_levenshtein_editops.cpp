/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include "../rapidfuzz_reference/Levenshtein.hpp"
#include "fuzzing.hpp"
#include <rapidfuzz/distance.hpp>
#include <stdexcept>
#include <string>

void validate_editops(const std::vector<uint8_t>& s1, const std::vector<uint8_t>& s2, size_t score,
                      size_t score_hint = std::numeric_limits<size_t>::max())
{
    rapidfuzz::Editops ops = rapidfuzz::levenshtein_editops(s1, s2, score_hint);
    if (ops.size() == score && s2 != rapidfuzz::editops_apply_vec<uint8_t>(ops, s1, s2))
        throw std::logic_error("levenshtein_editops failed");
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::vector<uint8_t> s1, s2;
    if (!extract_strings(data, size, s1, s2)) return 0;

    /* hirschbergs algorithm is only used for very long sequences which are apparently not generated a lot by
     * the fuzzer */
    for (int i = 0; i < 10; i++) {
        size_t score = rapidfuzz_reference::levenshtein_distance(s1, s2);
        validate_editops(s1, s2, score);
        validate_editops(s1, s2, score, 64);
        validate_editops(s1, s2, score, score != 0 ? score - 1 : 0);
        validate_editops(s1, s2, score, score);

        if (s1.size() > 1 && s2.size() > 1) {
            auto hpos = rapidfuzz::detail::find_hirschberg_pos(rapidfuzz::detail::Range(s1),
                                                               rapidfuzz::detail::Range(s2));
            if (hpos.left_score + hpos.right_score != score)
                throw std::logic_error("find_hirschberg_pos failed");
        }

        s1 = vec_multiply(s1, 2);
        s2 = vec_multiply(s2, 2);
    }

    return 0;
}
