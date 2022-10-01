#include "../../rapidfuzz_reference/JaroWinkler.hpp"
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidfuzz/distance/JaroWinkler.hpp>

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
    double res7 = scorer.similarity(s2, score_cutoff);
    double res8 = scorer.similarity(s2.begin(), s2.end(), score_cutoff);
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
    double res7 = scorer.distance(s2, score_cutoff);
    double res8 = scorer.distance(s2.begin(), s2.end(), score_cutoff);
    REQUIRE(res1 == Approx(res2));
    REQUIRE(res1 == Approx(res3));
    REQUIRE(res1 == Approx(res4));
    REQUIRE(res1 == Approx(res5));
    REQUIRE(res1 == Approx(res6));
    REQUIRE(res1 == Approx(res7));
    REQUIRE(res1 == Approx(res8));
    return res1;
}

/**
 * @name JaroWinklerFlagCharsTest
 */
TEST_CASE("JaroWinklerTest")
{
    std::array<std::string, 19> names = {"james",    "robert",   "john",   "michael",   "william",
                                         "david",    "joseph",   "thomas", "charles",   "mary",
                                         "patricia", "jennifer", "linda",  "elizabeth", "barbara",
                                         "susan",    "jessica",  "sarah",  "karen"};

    SECTION("testFullResultWithScoreCutoff")
    {
        for (double score_cutoff = 0.0; score_cutoff < 1.1; score_cutoff += 0.1)
            for (const auto& name1 : names)
                for (const auto& name2 : names) {
                    INFO("name1: " << name1 << ", name2: " << name2 << ", score_cutoff: " << score_cutoff);
                    double Sim_original =
                        rapidfuzz_reference::jaro_winkler_similarity(name1, name2, 0.1, score_cutoff);
                    double Sim_bitparallel = jaro_winkler_similarity(name1, name2, 0.1, score_cutoff);
                    double Dist_bitparallel = jaro_winkler_distance(name1, name2, 0.1, 1.0 - score_cutoff);
                    REQUIRE(Sim_original == Approx(Sim_bitparallel));
                    REQUIRE((1.0 - Sim_original) == Approx(Dist_bitparallel));
                }
    }
}