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

template <size_t MaxLen>
void validate_simd(const std::vector<uint8_t>& s1, const std::vector<uint8_t>& s2)
{
#ifdef RAPIDFUZZ_SIMD
    size_t count = s1.size() / MaxLen + ((s1.size() % MaxLen) != 0);
    if (count == 0) return;

    rapidfuzz::experimental::MultiJaro<MaxLen> scorer(count);

    std::vector<std::vector<uint8_t>> strings;

    for (auto it1 = s1.begin(); it1 != s1.end(); it1 += MaxLen) {
        if (std::distance(it1, s1.end()) < static_cast<ptrdiff_t>(MaxLen)) {
            strings.emplace_back(it1, s1.end());
            break;
        }
        else {
            strings.emplace_back(it1, it1 + MaxLen);
        }
    }

    for (const auto& s : strings)
        scorer.insert(s);

    std::vector<double> simd_results(scorer.result_count());
    scorer.similarity(&simd_results[0], simd_results.size(), s2);

    for (size_t i = 0; i < strings.size(); ++i) {
        double reference_sim = rapidfuzz_reference::jaro_similarity(strings[i], s2);

        if (!is_close(simd_results[i], reference_sim, 0.0001)) {
            print_seq("s1", strings[i]);
            print_seq("s2", s2);
            throw std::logic_error(std::string("jaro similarity using simd failed (reference_score = ") +
                                   std::to_string(reference_sim) + std::string(", score = ") +
                                   std::to_string(simd_results[i]) + std::string(", i = ") +
                                   std::to_string(i) + ")");
        }
    }

#else
    (void)s1;
    (void)s2;
#endif
}

void validate_distance(const std::vector<uint8_t>& s1, const std::vector<uint8_t>& s2)
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

    validate_simd<8>(s1, s2);
    validate_simd<16>(s1, s2);
    validate_simd<32>(s1, s2);
    validate_simd<64>(s1, s2);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::vector<uint8_t> s1, s2;
    if (!extract_strings(data, size, s1, s2)) return 0;

    validate_distance(s1, s2);

    /* test long sequences */
    for (unsigned int i = 2; i < 9; ++i) {
        std::vector<uint8_t> s1_ = vec_multiply(s1, pow<size_t>(2, i));
        std::vector<uint8_t> s2_ = vec_multiply(s2, pow<size_t>(2, i));

        if (s1_.size() > 10000 || s2_.size() > 10000) break;

        validate_distance(s1_, s2_);
    }

    return 0;
}
