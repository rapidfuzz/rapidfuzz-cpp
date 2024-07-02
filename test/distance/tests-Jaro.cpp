#include "../../rapidfuzz_reference/Jaro.hpp"
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidfuzz/distance/Jaro.hpp>

#include "../common.hpp"

using Catch::Approx;

template <typename Sentence1, typename Sentence2>
double jaro_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    double res1 = rapidfuzz::jaro_similarity(s1, s2, score_cutoff);
    double res2 = rapidfuzz::jaro_similarity(s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
    double res3 = rapidfuzz::jaro_normalized_similarity(s1, s2, score_cutoff);
    double res4 =
        rapidfuzz::jaro_normalized_similarity(s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
#if 0 // todo
    double res5 = rapidfuzz::jaro_similarity(
        BidirectionalIterWrapper(s1.begin()), BidirectionalIterWrapper(s1.end()),
        BidirectionalIterWrapper(s2.begin()), BidirectionalIterWrapper(s2.end()), score_cutoff);
#endif
    rapidfuzz::CachedJaro scorer(s1);
    double res6 = scorer.similarity(s2, score_cutoff);
    double res7 = scorer.similarity(s2.begin(), s2.end(), score_cutoff);
    double res8 = scorer.normalized_similarity(s2, score_cutoff);
    double res9 = scorer.normalized_similarity(s2.begin(), s2.end(), score_cutoff);

#ifdef RAPIDFUZZ_SIMD
    std::vector<double> results(256 / 8);
    if (s1.size() <= 8) {
        rapidfuzz::experimental::MultiJaro<8> simd_scorer(32);
        for (size_t i = 0; i < 32; ++i)
            simd_scorer.insert(s1);

        simd_scorer.similarity(&results[0], results.size(), s2, score_cutoff);
        for (size_t i = 0; i < 32; ++i)
            REQUIRE(res1 == Approx(results[i]));
    }
    if (s1.size() <= 16) {
        rapidfuzz::experimental::MultiJaro<16> simd_scorer(16);
        for (size_t i = 0; i < 16; ++i)
            simd_scorer.insert(s1);

        simd_scorer.similarity(&results[0], results.size(), s2, score_cutoff);
        for (size_t i = 0; i < 16; ++i)
            REQUIRE(res1 == Approx(results[i]));
    }
    if (s1.size() <= 32) {
        rapidfuzz::experimental::MultiJaro<32> simd_scorer(8);
        for (size_t i = 0; i < 8; ++i)
            simd_scorer.insert(s1);

        simd_scorer.similarity(&results[0], results.size(), s2, score_cutoff);
        for (size_t i = 0; i < 8; ++i)
            REQUIRE(res1 == Approx(results[i]));
    }
    if (s1.size() <= 64) {
        rapidfuzz::experimental::MultiJaro<64> simd_scorer(4);
        for (size_t i = 0; i < 4; ++i)
            simd_scorer.insert(s1);

        simd_scorer.similarity(&results[0], results.size(), s2, score_cutoff);
        for (size_t i = 0; i < 4; ++i)
            REQUIRE(res1 == Approx(results[i]));
    }
#endif

    REQUIRE(res1 == Approx(res2));
    REQUIRE(res1 == Approx(res3));
    REQUIRE(res1 == Approx(res4));
    // REQUIRE(res1 == Approx(res5));
    REQUIRE(res1 == Approx(res6));
    REQUIRE(res1 == Approx(res7));
    REQUIRE(res1 == Approx(res8));
    REQUIRE(res1 == Approx(res9));
    return res1;
}

template <typename Sentence1, typename Sentence2>
double jaro_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    double res1 = rapidfuzz::jaro_distance(s1, s2, score_cutoff);
    double res2 = rapidfuzz::jaro_distance(s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
    double res3 = rapidfuzz::jaro_normalized_distance(s1, s2, score_cutoff);
    double res4 =
        rapidfuzz::jaro_normalized_distance(s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
#if 0 // todo
    double res5 = rapidfuzz::jaro_distance(
        BidirectionalIterWrapper(s1.begin()), BidirectionalIterWrapper(s1.end()),
        BidirectionalIterWrapper(s2.begin()), BidirectionalIterWrapper(s2.end()), score_cutoff);
#endif
    rapidfuzz::CachedJaro scorer(s1);
    double res6 = scorer.distance(s2, score_cutoff);
    double res7 = scorer.distance(s2.begin(), s2.end(), score_cutoff);
    double res8 = scorer.normalized_distance(s2, score_cutoff);
    double res9 = scorer.normalized_distance(s2.begin(), s2.end(), score_cutoff);

#ifdef RAPIDFUZZ_SIMD
    std::vector<double> results(256 / 8);
    if (s1.size() <= 8) {
        rapidfuzz::experimental::MultiJaro<8> simd_scorer(32);
        for (size_t i = 0; i < 32; ++i)
            simd_scorer.insert(s1);

        simd_scorer.distance(&results[0], results.size(), s2, score_cutoff);
        for (size_t i = 0; i < 32; ++i)
            REQUIRE(res1 == Approx(results[i]));
    }
    if (s1.size() <= 16) {
        rapidfuzz::experimental::MultiJaro<16> simd_scorer(16);
        for (size_t i = 0; i < 16; ++i)
            simd_scorer.insert(s1);

        simd_scorer.distance(&results[0], results.size(), s2, score_cutoff);
        for (size_t i = 0; i < 16; ++i)
            REQUIRE(res1 == Approx(results[i]));
    }
    if (s1.size() <= 32) {
        rapidfuzz::experimental::MultiJaro<32> simd_scorer(8);
        for (size_t i = 0; i < 8; ++i)
            simd_scorer.insert(s1);

        simd_scorer.distance(&results[0], results.size(), s2, score_cutoff);
        for (size_t i = 0; i < 8; ++i)
            REQUIRE(res1 == Approx(results[i]));
    }
    if (s1.size() <= 64) {
        rapidfuzz::experimental::MultiJaro<64> simd_scorer(4);
        for (size_t i = 0; i < 4; ++i)
            simd_scorer.insert(s1);

        simd_scorer.distance(&results[0], results.size(), s2, score_cutoff);
        for (size_t i = 0; i < 4; ++i)
            REQUIRE(res1 == Approx(results[i]));
    }
#endif

    REQUIRE(res1 == Approx(res2));
    REQUIRE(res1 == Approx(res3));
    REQUIRE(res1 == Approx(res4));
    // REQUIRE(res1 == Approx(res5));
    REQUIRE(res1 == Approx(res6));
    REQUIRE(res1 == Approx(res7));
    REQUIRE(res1 == Approx(res8));
    REQUIRE(res1 == Approx(res9));
    return res1;
}

template <typename Sentence1, typename Sentence2>
double jaro_sim_test(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    INFO("name1: " << s1 << ", name2: " << s2 << ", score_cutoff: " << score_cutoff);
    double Sim_original = rapidfuzz_reference::jaro_similarity(s1, s2, score_cutoff);
    double Sim_bitparallel = jaro_similarity(s1, s2, score_cutoff);
    double Dist_bitparallel = jaro_distance(s1, s2, 1.0 - score_cutoff);
    double Sim_bitparallel2 = jaro_similarity(s2, s1, score_cutoff);
    double Dist_bitparallel2 = jaro_distance(s2, s1, 1.0 - score_cutoff);

    REQUIRE(Sim_original == Approx(Sim_bitparallel));
    REQUIRE((1.0 - Sim_original) == Approx(Dist_bitparallel));
    REQUIRE(Sim_original == Approx(Sim_bitparallel2));
    REQUIRE((1.0 - Sim_original) == Approx(Dist_bitparallel2));
    return Sim_original;
}

TEST_CASE("JaroTest")
{
    std::array<std::string, 20> names = {"james",    "robert",   "john",   "michael",   "william",
                                         "david",    "joseph",   "thomas", "charles",   "mary",
                                         "patricia", "jennifer", "linda",  "elizabeth", "barbara",
                                         "susan",    "jessica",  "sarah",  "karen",     ""};

    SECTION("testFullResultWithScoreCutoff")
    {
        auto score_cutoffs = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1};

        for (double score_cutoff : score_cutoffs)
            for (const auto& name1 : names)
                for (const auto& name2 : names)
                    jaro_sim_test(name1, name2, score_cutoff);
    }

    SECTION("testEdgeCaseLengths")
    {
        REQUIRE(jaro_sim_test(std::string(""), std::string("")) == Approx(1));
        REQUIRE(jaro_sim_test(std::string("0"), std::string("0")) == Approx(1));
        REQUIRE(jaro_sim_test(std::string("00"), std::string("00")) == Approx(1));
        REQUIRE(jaro_sim_test(std::string("0"), std::string("00")) == Approx(0.833333));

        REQUIRE(jaro_sim_test(str_multiply(std::string("0"), 65), str_multiply(std::string("0"), 65)) ==
                Approx(1));
        REQUIRE(jaro_sim_test(str_multiply(std::string("0"), 64), str_multiply(std::string("0"), 65)) ==
                Approx(0.994872));
        REQUIRE(jaro_sim_test(str_multiply(std::string("0"), 63), str_multiply(std::string("0"), 65)) ==
                Approx(0.989744));

        REQUIRE(jaro_sim_test(std::string("000000001"), std::string("0000010")) == Approx(0.8783068783));
        {
            std::string s1("01234567");
            std::string s2 = str_multiply(std::string("0"), 170) + std::string("7654321") +
                             str_multiply(std::string("0"), 200);

            REQUIRE(jaro_sim_test(s1, s2) == Approx(0.5487400531));
        }

        REQUIRE(jaro_sim_test(std::string("01"), std::string("1111100000")) == Approx(0.53333333));

        REQUIRE(
            jaro_sim_test(std::string("10000000000000000000000000000000000000000000000000000000000000020"),
                          std::string("00000000000000000000000000000000000000000000000000000000000000000")) ==
            Approx(0.979487));
        REQUIRE(jaro_sim_test(
                    std::string("00000000000000100000000000000000000000010000000000000000000000000"),
                    std::string(
                        "0000000000000000000000000000000000000000000000000000000000000000000000000000001")) ==
                Approx(0.922233));
        REQUIRE(
            jaro_sim_test(std::string("00000000000000000000000000000000000000000000000000000000000000000"),
                          std::string("0100000000000000000000000000000000000000000000000000000000000000000000"
                                      "0000000000000000000000000000000000000000000000000000000000")) ==
            Approx(0.8359375));
    }

    SECTION("testFuzzingRegressions")
    {
#ifdef RAPIDFUZZ_SIMD
        {
            std::string s2 = "010101010101010101010101010101010101010101010101010101010101010101"
                             "010101010101010101010101010101010101010101010101010101010101010101"
                             "010101010101010101010101010101010101010101010101010101010101010101"
                             "0101010101010101010101010101010101010101010101010101010101";

            std::vector<double> results(512 / 8);
            rapidfuzz::experimental::MultiJaro<8> simd_scorer(64);
            for (size_t i = 0; i < 32; ++i)
                simd_scorer.insert("10010010");

            simd_scorer.insert("00100100");
            simd_scorer.similarity(&results[0], results.size(), s2);

            for (size_t i = 0; i < 32; ++i)
                REQUIRE(results[i] == Approx(0.593750));

            REQUIRE(results[32] == Approx(0.593750));
        }
#endif
    }
}
