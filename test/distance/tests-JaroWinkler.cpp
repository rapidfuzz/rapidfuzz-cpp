#include "../../rapidfuzz_reference/JaroWinkler.hpp"
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidfuzz/distance/JaroWinkler.hpp>

#include "../common.hpp"

using Catch::Approx;

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
            REQUIRE(res1 == Approx(results[i]));
    }
    if (s1.size() <= 16) {
        rapidfuzz::experimental::MultiJaroWinkler<16> simd_scorer(1, prefix_weight);
        simd_scorer.insert(s1);
        simd_scorer.similarity(&results[0], results.size(), s2, score_cutoff);
        REQUIRE(res1 == Approx(results[0]));
    }
    if (s1.size() <= 32) {
        rapidfuzz::experimental::MultiJaroWinkler<32> simd_scorer(1, prefix_weight);
        simd_scorer.insert(s1);
        simd_scorer.similarity(&results[0], results.size(), s2, score_cutoff);
        REQUIRE(res1 == Approx(results[0]));
    }
    if (s1.size() <= 64) {
        rapidfuzz::experimental::MultiJaroWinkler<64> simd_scorer(1, prefix_weight);
        simd_scorer.insert(s1);
        simd_scorer.similarity(&results[0], results.size(), s2, score_cutoff);
        REQUIRE(res1 == Approx(results[0]));
    }
#endif

    REQUIRE(res1 == Approx(res2));
    REQUIRE(res1 == Approx(res3));
    REQUIRE(res1 == Approx(res4));
    REQUIRE(res1 == Approx(res5));
    REQUIRE(res1 == Approx(res6));
    REQUIRE(res1 == Approx(res7));
    REQUIRE(res1 == Approx(res8));
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
        REQUIRE(res1 == Approx(results[0]));
    }
    if (s1.size() <= 16) {
        rapidfuzz::experimental::MultiJaroWinkler<16> simd_scorer(1, prefix_weight);
        simd_scorer.insert(s1);
        simd_scorer.distance(&results[0], results.size(), s2, score_cutoff);
        REQUIRE(res1 == Approx(results[0]));
    }
    if (s1.size() <= 32) {
        rapidfuzz::experimental::MultiJaroWinkler<32> simd_scorer(1, prefix_weight);
        simd_scorer.insert(s1);
        simd_scorer.distance(&results[0], results.size(), s2, score_cutoff);
        REQUIRE(res1 == Approx(results[0]));
    }
    if (s1.size() <= 64) {
        rapidfuzz::experimental::MultiJaroWinkler<64> simd_scorer(1, prefix_weight);
        simd_scorer.insert(s1);
        simd_scorer.distance(&results[0], results.size(), s2, score_cutoff);
        REQUIRE(res1 == Approx(results[0]));
    }
#endif

    REQUIRE(res1 == Approx(res2));
    REQUIRE(res1 == Approx(res3));
    REQUIRE(res1 == Approx(res4));
    REQUIRE(res1 == Approx(res5));
    REQUIRE(res1 == Approx(res6));
    REQUIRE(res1 == Approx(res7));
    REQUIRE(res1 == Approx(res8));
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

    REQUIRE(Sim_original == Approx(Sim_bitparallel));
    REQUIRE((1.0 - Sim_original) == Approx(Dist_bitparallel));
    REQUIRE(Sim_original == Approx(Sim_bitparallel2));
    REQUIRE((1.0 - Sim_original) == Approx(Dist_bitparallel2));
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
        REQUIRE(jaro_winkler_sim_test(std::string(""), std::string("")) == Approx(1));
        REQUIRE(jaro_winkler_sim_test(std::string("0"), std::string("0")) == Approx(1));
        REQUIRE(jaro_winkler_sim_test(std::string("00"), std::string("00")) == Approx(1));
        REQUIRE(jaro_winkler_sim_test(std::string("0"), std::string("00")) == Approx(0.85));

        REQUIRE(jaro_winkler_sim_test(str_multiply(std::string("0"), 65),
                                      str_multiply(std::string("0"), 65)) == Approx(1));
        REQUIRE(jaro_winkler_sim_test(str_multiply(std::string("0"), 64),
                                      str_multiply(std::string("0"), 65)) == Approx(0.996923));
        REQUIRE(jaro_winkler_sim_test(str_multiply(std::string("0"), 63),
                                      str_multiply(std::string("0"), 65)) == Approx(0.993846));

        REQUIRE(jaro_winkler_sim_test(std::string("000000001"), std::string("0000010")) ==
                Approx(0.926984127));

        REQUIRE(jaro_winkler_sim_test(std::string("01"), std::string("1111100000")) == Approx(0.53333333));

        REQUIRE(jaro_winkler_sim_test(
                    std::string("10000000000000000000000000000000000000000000000000000000000000020"),
                    std::string("00000000000000000000000000000000000000000000000000000000000000000")) ==
                Approx(0.979487));
        REQUIRE(jaro_winkler_sim_test(
                    std::string("00000000000000100000000000000000000000010000000000000000000000000"),
                    std::string(
                        "0000000000000000000000000000000000000000000000000000000000000000000000000000001")) ==
                Approx(0.95334));
        REQUIRE(jaro_winkler_sim_test(
                    std::string("00000000000000000000000000000000000000000000000000000000000000000"),
                    std::string("0100000000000000000000000000000000000000000000000000000000000000000000000000"
                                "0000000000000000000000000000000000000000000000000000")) == Approx(0.852344));
    }
}