#include "../../rapidfuzz_reference/JaroWinkler.hpp"
#include <catch2/catch.hpp>
#include <rapidfuzz/distance/JaroWinkler.hpp>

#include "../common.hpp"

using Catch::Matchers::WithinAbs;

template <typename Sentence1, typename Sentence2>
double jaro_winkler_similarity(const Sentence1& s1, const Sentence2& s2, double prefix_weight = 0.1,
                               double score_cutoff = 0.0)
{
    double res1 = rapidfuzz::jaro_winkler_similarity(s1, s2, prefix_weight, score_cutoff);
    double res2 = rapidfuzz::jaro_winkler_similarity(s1.begin(), s1.end(), s2.begin(), s2.end(),
                                                     prefix_weight, score_cutoff);
    double res3 = rapidfuzz::jaro_winkler_normalized_similarity(s1, s2, prefix_weight, score_cutoff);
    double res4 = rapidfuzz::jaro_winkler_normalized_similarity(s1.begin(), s1.end(), s2.begin(), s2.end(),
                                                                prefix_weight, score_cutoff);
    rapidfuzz::CachedJaroWinkler scorer(s1, prefix_weight);
    double res5 = scorer.similarity(s2, score_cutoff);
    double res6 = scorer.similarity(s2.begin(), s2.end(), score_cutoff);
    double res7 = scorer.normalized_similarity(s2, score_cutoff);
    double res8 = scorer.normalized_similarity(s2.begin(), s2.end(), score_cutoff);

#ifdef RAPIDFUZZ_SIMD
    std::vector<double> results(256 / 8);
    if (s1.size() <= 8) {
        rapidfuzz::experimental::MultiJaroWinkler<8> simd_scorer(32, prefix_weight);
        for (unsigned int i = 0; i < 32; ++i)
            simd_scorer.insert(s1);
        simd_scorer.similarity(&results[0], results.size(), s2, score_cutoff);
        for (unsigned int i = 0; i < 32; ++i)
            REQUIRE_THAT(res1, WithinAbs(results[i], 0.000001));
    }
    if (s1.size() <= 16) {
        rapidfuzz::experimental::MultiJaroWinkler<16> simd_scorer(1, prefix_weight);
        simd_scorer.insert(s1);
        simd_scorer.similarity(&results[0], results.size(), s2, score_cutoff);
        REQUIRE_THAT(res1, WithinAbs(results[0], 0.000001));
    }
    if (s1.size() <= 32) {
        rapidfuzz::experimental::MultiJaroWinkler<32> simd_scorer(1, prefix_weight);
        simd_scorer.insert(s1);
        simd_scorer.similarity(&results[0], results.size(), s2, score_cutoff);
        REQUIRE_THAT(res1, WithinAbs(results[0], 0.000001));
    }
    if (s1.size() <= 64) {
        rapidfuzz::experimental::MultiJaroWinkler<64> simd_scorer(1, prefix_weight);
        simd_scorer.insert(s1);
        simd_scorer.similarity(&results[0], results.size(), s2, score_cutoff);
        REQUIRE_THAT(res1, WithinAbs(results[0], 0.000001));
    }
#endif

    REQUIRE_THAT(res1, WithinAbs(res2, 0.000001));
    REQUIRE_THAT(res1, WithinAbs(res3, 0.000001));
    REQUIRE_THAT(res1, WithinAbs(res4, 0.000001));
    REQUIRE_THAT(res1, WithinAbs(res5, 0.000001));
    REQUIRE_THAT(res1, WithinAbs(res6, 0.000001));
    REQUIRE_THAT(res1, WithinAbs(res7, 0.000001));
    REQUIRE_THAT(res1, WithinAbs(res8, 0.000001));
    return res1;
}

template <typename Sentence1, typename Sentence2>
double jaro_winkler_distance(const Sentence1& s1, const Sentence2& s2, double prefix_weight = 0.1,
                             double score_cutoff = 1.0)
{
    double res1 = rapidfuzz::jaro_winkler_distance(s1, s2, prefix_weight, score_cutoff);
    double res2 = rapidfuzz::jaro_winkler_distance(s1.begin(), s1.end(), s2.begin(), s2.end(), prefix_weight,
                                                   score_cutoff);
    double res3 = rapidfuzz::jaro_winkler_normalized_distance(s1, s2, prefix_weight, score_cutoff);
    double res4 = rapidfuzz::jaro_winkler_normalized_distance(s1.begin(), s1.end(), s2.begin(), s2.end(),
                                                              prefix_weight, score_cutoff);
    rapidfuzz::CachedJaroWinkler scorer(s1, prefix_weight);
    double res5 = scorer.distance(s2, score_cutoff);
    double res6 = scorer.distance(s2.begin(), s2.end(), score_cutoff);
    double res7 = scorer.normalized_distance(s2, score_cutoff);
    double res8 = scorer.normalized_distance(s2.begin(), s2.end(), score_cutoff);

#ifdef RAPIDFUZZ_SIMD
    std::vector<double> results(256 / 8);
    if (s1.size() <= 8) {
        rapidfuzz::experimental::MultiJaroWinkler<8> simd_scorer(1, prefix_weight);
        simd_scorer.insert(s1);
        simd_scorer.distance(&results[0], results.size(), s2, score_cutoff);
        REQUIRE_THAT(res1, WithinAbs(results[0], 0.000001));
    }
    if (s1.size() <= 16) {
        rapidfuzz::experimental::MultiJaroWinkler<16> simd_scorer(1, prefix_weight);
        simd_scorer.insert(s1);
        simd_scorer.distance(&results[0], results.size(), s2, score_cutoff);
        REQUIRE_THAT(res1, WithinAbs(results[0], 0.000001));
    }
    if (s1.size() <= 32) {
        rapidfuzz::experimental::MultiJaroWinkler<32> simd_scorer(1, prefix_weight);
        simd_scorer.insert(s1);
        simd_scorer.distance(&results[0], results.size(), s2, score_cutoff);
        REQUIRE_THAT(res1, WithinAbs(results[0], 0.000001));
    }
    if (s1.size() <= 64) {
        rapidfuzz::experimental::MultiJaroWinkler<64> simd_scorer(1, prefix_weight);
        simd_scorer.insert(s1);
        simd_scorer.distance(&results[0], results.size(), s2, score_cutoff);
        REQUIRE_THAT(res1, WithinAbs(results[0], 0.000001));
    }
#endif

    REQUIRE_THAT(res1, WithinAbs(res2, 0.000001));
    REQUIRE_THAT(res1, WithinAbs(res3, 0.000001));
    REQUIRE_THAT(res1, WithinAbs(res4, 0.000001));
    REQUIRE_THAT(res1, WithinAbs(res5, 0.000001));
    REQUIRE_THAT(res1, WithinAbs(res6, 0.000001));
    REQUIRE_THAT(res1, WithinAbs(res7, 0.000001));
    REQUIRE_THAT(res1, WithinAbs(res8, 0.000001));
    return res1;
}

template <typename Sentence1, typename Sentence2>
double jaro_winkler_sim_test(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    INFO("name1: " << s1 << ", name2: " << s2 << ", score_cutoff: " << score_cutoff);
    double Sim_original = rapidfuzz_reference::jaro_winkler_similarity(s1, s2, 0.1, score_cutoff);
    double Sim_bitparallel = jaro_winkler_similarity(s1, s2, 0.1, score_cutoff);
    double Dist_bitparallel = jaro_winkler_distance(s1, s2, 0.1, 1.0 - score_cutoff);
    double Sim_bitparallel2 = jaro_winkler_similarity(s2, s1, 0.1, score_cutoff);
    double Dist_bitparallel2 = jaro_winkler_distance(s2, s1, 0.1, 1.0 - score_cutoff);

    REQUIRE_THAT(Sim_original, WithinAbs(Sim_bitparallel, 0.000001));
    REQUIRE_THAT((1.0 - Sim_original), WithinAbs(Dist_bitparallel, 0.000001));
    REQUIRE_THAT(Sim_original, WithinAbs(Sim_bitparallel2, 0.000001));
    REQUIRE_THAT((1.0 - Sim_original), WithinAbs(Dist_bitparallel2, 0.000001));
    return Sim_original;
}

TEST_CASE("JaroWinklerTest")
{
    std::array<std::string, 22> names = {"james",
                                         "robert",
                                         "john",
                                         "michael",
                                         "william",
                                         "david",
                                         "joseph",
                                         "thomas",
                                         "charles",
                                         "mary",
                                         "patricia",
                                         "jennifer",
                                         "linda",
                                         "elizabeth",
                                         "barbara",
                                         "susan",
                                         "jessica",
                                         "sarah",
                                         "karen",
                                         ""
                                         "aaaaaaaa",
                                         "aabaaab"};

    SECTION("testFullResultWithScoreCutoff")
    {
        auto score_cutoffs = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1};

        for (double score_cutoff : score_cutoffs)
            for (const auto& name1 : names)
                for (const auto& name2 : names)
                    jaro_winkler_sim_test(name1, name2, score_cutoff);
    }

    SECTION("testEdgeCaseLengths")
    {
        REQUIRE_THAT(jaro_winkler_sim_test(std::string(""), std::string("")), WithinAbs(1, 0.000001));
        REQUIRE_THAT(jaro_winkler_sim_test(std::string("0"), std::string("0")), WithinAbs(1, 0.000001));
        REQUIRE_THAT(jaro_winkler_sim_test(std::string("00"), std::string("00")), WithinAbs(1, 0.000001));
        REQUIRE_THAT(jaro_winkler_sim_test(std::string("0"), std::string("00")), WithinAbs(0.85, 0.000001));

        REQUIRE_THAT(
            jaro_winkler_sim_test(str_multiply(std::string("0"), 65), str_multiply(std::string("0"), 65)),
            WithinAbs(1, 0.000001));
        REQUIRE_THAT(
            jaro_winkler_sim_test(str_multiply(std::string("0"), 64), str_multiply(std::string("0"), 65)),
            WithinAbs(0.996923, 0.000001));
        REQUIRE_THAT(
            jaro_winkler_sim_test(str_multiply(std::string("0"), 63), str_multiply(std::string("0"), 65)),
            WithinAbs(0.993846, 0.000001));

        REQUIRE_THAT(jaro_winkler_sim_test(std::string("000000001"), std::string("0000010")),
                     WithinAbs(0.926984127, 0.000001));

        REQUIRE_THAT(jaro_winkler_sim_test(std::string("01"), std::string("1111100000")),
                     WithinAbs(0.53333333, 0.000001));

        REQUIRE_THAT(jaro_winkler_sim_test(
                         std::string("10000000000000000000000000000000000000000000000000000000000000020"),
                         std::string("00000000000000000000000000000000000000000000000000000000000000000")),
                     WithinAbs(0.979487, 0.000001));
        REQUIRE_THAT(
            jaro_winkler_sim_test(
                std::string("00000000000000100000000000000000000000010000000000000000000000000"),
                std::string(
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000001")),
            WithinAbs(0.95334, 0.000001));
        REQUIRE_THAT(
            jaro_winkler_sim_test(
                std::string("00000000000000000000000000000000000000000000000000000000000000000"),
                std::string("0100000000000000000000000000000000000000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000000000000000")),
            WithinAbs(0.852344, 0.000001));
    }
}