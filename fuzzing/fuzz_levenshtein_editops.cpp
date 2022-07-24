/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include "fuzzing.hpp"
#include <rapidfuzz/distance.hpp>
#include <stdexcept>
#include <string>

template <typename T>
std::basic_string<T> str_multiply(std::basic_string<T> a, unsigned int b) {
    std::basic_string<T> output;
    while (b--)
        output += a;

    return output;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::basic_string<uint8_t> s1, s2;
    if (!extract_strings(data, size, s1, s2))
        return 0;

    /* hirschbergs algorithm is only used for very long sequences which are apparently not generated a lot by the fuzzer */
    for (int i = 0; i < 10; i++)
    {
        rapidfuzz::Editops ops = rapidfuzz::levenshtein_editops(s1, s2);

        if (s2 != rapidfuzz::editops_apply<uint8_t>(ops, s1, s2))
        {
            throw std::logic_error("levenshtein_editops failed");
        }

        s1 = str_multiply(s1, 2);
        s2 = str_multiply(s2, 2);
    }


    return 0;
}
