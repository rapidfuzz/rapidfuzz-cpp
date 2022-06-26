/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include "fuzzing.hpp"
#include "rapidfuzz/details/Range.hpp"
#include <rapidfuzz/distance/LCSseq.hpp>
#include <stdexcept>
#include <string>

template <size_t MaxLen>
void validate_simd(const std::basic_string<uint8_t>& s1, const std::basic_string<uint8_t>& s2)
{
#ifdef RAPIDFUZZ_SIMD
    size_t count = s1.size() / MaxLen + ((s1.size() % MaxLen) != 0);
    rapidfuzz::MultiLCSseq<MaxLen> scorer(count);
    auto it1 = s1.begin();
    while (true)
    {
        if (std::distance(it1, s1.end()) <= MaxLen)
        {
            scorer.insert(it1, s1.end());
            break;
        }
        scorer.insert(it1, it1 + MaxLen);
        it1 += MaxLen;
    }

    std::vector<int64_t> results(count + 100 * MaxLen);
    scorer.distance(results, s2);
#else
    (void)s1;
    (void)s2;
#endif
}


extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::basic_string<uint8_t> s1, s2;
    if (!extract_strings(data, size, s1, s2)) {
        return 0;
    }

    if (s1.size() == 0)
    {
        return 0;
    }

    validate_simd<8>(s1, s2);
    validate_simd<16>(s1, s2);
    validate_simd<32>(s1, s2);
    validate_simd<64>(s1, s2);


    return 0;
}
