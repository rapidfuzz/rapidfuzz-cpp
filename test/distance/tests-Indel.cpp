#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#include <rapidfuzz/distance.hpp>
#include <rapidfuzz/distance/Indel.hpp>

#include "../common.hpp"

using Catch::Approx;

template <typename Sentence1, typename Sentence2>
size_t indel_distance(const Sentence1& s1, const Sentence2& s2,
                      size_t max = std::numeric_limits<size_t>::max())
{
    size_t res1 = rapidfuzz::indel_distance(s1, s2, max);
    size_t res2 = rapidfuzz::indel_distance(s1.begin(), s1.end(), s2.begin(), s2.end(), max);
    size_t res3 = rapidfuzz::indel_distance(
        BidirectionalIterWrapper(s1.begin()), BidirectionalIterWrapper(s1.end()),
        BidirectionalIterWrapper(s2.begin()), BidirectionalIterWrapper(s2.end()), max);
    rapidfuzz::CachedIndel scorer(s1);
    size_t res4 = scorer.distance(s2, max);
    size_t res5 = scorer.distance(s2.begin(), s2.end(), max);
#ifdef RAPIDFUZZ_SIMD
    if (s1.size() <= 64) {
        std::vector<size_t> results(256 / 8);

        if (s1.size() <= 8) {
            rapidfuzz::experimental::MultiIndel<8> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.distance(&results[0], results.size(), s2, max);
            REQUIRE(res1 == results[0]);
        }
        if (s1.size() <= 16) {
            rapidfuzz::experimental::MultiIndel<16> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.distance(&results[0], results.size(), s2, max);
            REQUIRE(res1 == results[0]);
        }
        if (s1.size() <= 32) {
            rapidfuzz::experimental::MultiIndel<32> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.distance(&results[0], results.size(), s2, max);
            REQUIRE(res1 == results[0]);
        }
        if (s1.size() <= 64) {
            rapidfuzz::experimental::MultiIndel<64> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.distance(&results[0], results.size(), s2, max);
            REQUIRE(res1 == results[0]);
        }
    }
#endif
    REQUIRE(res1 == res2);
    REQUIRE(res1 == res3);
    REQUIRE(res1 == res4);
    REQUIRE(res1 == res5);
    return res1;
}

template <typename Sentence1, typename Sentence2>
size_t indel_similarity(const Sentence1& s1, const Sentence2& s2, size_t max = 0)
{
    size_t res1 = rapidfuzz::indel_similarity(s1, s2, max);
    size_t res2 = rapidfuzz::indel_similarity(s1.begin(), s1.end(), s2.begin(), s2.end(), max);
    size_t res3 = rapidfuzz::indel_similarity(
        BidirectionalIterWrapper(s1.begin()), BidirectionalIterWrapper(s1.end()),
        BidirectionalIterWrapper(s2.begin()), BidirectionalIterWrapper(s2.end()), max);
    rapidfuzz::CachedIndel scorer(s1);
    size_t res4 = scorer.similarity(s2, max);
    size_t res5 = scorer.similarity(s2.begin(), s2.end(), max);
#ifdef RAPIDFUZZ_SIMD
    if (s1.size() <= 64) {
        std::vector<size_t> results(256 / 8);

        if (s1.size() <= 8) {
            rapidfuzz::experimental::MultiIndel<8> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.similarity(&results[0], results.size(), s2, max);
        }
        else if (s1.size() <= 16) {
            rapidfuzz::experimental::MultiIndel<16> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.similarity(&results[0], results.size(), s2, max);
        }
        else if (s1.size() <= 32) {
            rapidfuzz::experimental::MultiIndel<32> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.similarity(&results[0], results.size(), s2, max);
        }
        else {
            rapidfuzz::experimental::MultiIndel<64> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.similarity(&results[0], results.size(), s2, max);
        }

        REQUIRE(res1 == results[0]);
    }
#endif
    REQUIRE(res1 == res2);
    REQUIRE(res1 == res3);
    REQUIRE(res1 == res4);
    REQUIRE(res1 == res5);
    return res1;
}

template <typename Sentence1, typename Sentence2>
double indel_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    double res1 = rapidfuzz::indel_normalized_distance(s1, s2, score_cutoff);
    double res2 =
        rapidfuzz::indel_normalized_distance(s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
    double res3 = rapidfuzz::indel_normalized_distance(
        BidirectionalIterWrapper(s1.begin()), BidirectionalIterWrapper(s1.end()),
        BidirectionalIterWrapper(s2.begin()), BidirectionalIterWrapper(s2.end()), score_cutoff);
    rapidfuzz::CachedIndel scorer(s1);
    double res4 = scorer.normalized_distance(s2, score_cutoff);
    double res5 = scorer.normalized_distance(s2.begin(), s2.end(), score_cutoff);
#ifdef RAPIDFUZZ_SIMD
    if (s1.size() <= 64) {
        std::vector<double> results(256 / 8);

        if (s1.size() <= 8) {
            rapidfuzz::experimental::MultiIndel<8> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.normalized_distance(&results[0], results.size(), s2, score_cutoff);
        }
        else if (s1.size() <= 16) {
            rapidfuzz::experimental::MultiIndel<16> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.normalized_distance(&results[0], results.size(), s2, score_cutoff);
        }
        else if (s1.size() <= 32) {
            rapidfuzz::experimental::MultiIndel<32> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.normalized_distance(&results[0], results.size(), s2, score_cutoff);
        }
        else {
            rapidfuzz::experimental::MultiIndel<64> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.normalized_distance(&results[0], results.size(), s2, score_cutoff);
        }

        REQUIRE(res1 == Catch::Approx(results[0]).epsilon(0.0001));
    }
#endif
    REQUIRE(res1 == Catch::Approx(res2).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res3).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res4).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res5).epsilon(0.0001));
    return res1;
}

template <typename Sentence1, typename Sentence2>
double indel_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    double res1 = rapidfuzz::indel_normalized_similarity(s1, s2, score_cutoff);
    double res2 =
        rapidfuzz::indel_normalized_similarity(s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
    double res3 = rapidfuzz::indel_normalized_similarity(
        BidirectionalIterWrapper(s1.begin()), BidirectionalIterWrapper(s1.end()),
        BidirectionalIterWrapper(s2.begin()), BidirectionalIterWrapper(s2.end()), score_cutoff);
    rapidfuzz::CachedIndel scorer(s1);
    double res4 = scorer.normalized_similarity(s2, score_cutoff);
    double res5 = scorer.normalized_similarity(s2.begin(), s2.end(), score_cutoff);
#ifdef RAPIDFUZZ_SIMD
    if (s1.size() <= 64) {
        std::vector<double> results(256 / 8);

        if (s1.size() <= 8) {
            rapidfuzz::experimental::MultiIndel<8> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.normalized_similarity(&results[0], results.size(), s2, score_cutoff);
        }
        else if (s1.size() <= 16) {
            rapidfuzz::experimental::MultiIndel<16> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.normalized_similarity(&results[0], results.size(), s2, score_cutoff);
        }
        else if (s1.size() <= 32) {
            rapidfuzz::experimental::MultiIndel<32> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.normalized_similarity(&results[0], results.size(), s2, score_cutoff);
        }
        else {
            rapidfuzz::experimental::MultiIndel<64> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.normalized_similarity(&results[0], results.size(), s2, score_cutoff);
        }

        REQUIRE(res1 == Catch::Approx(results[0]).epsilon(0.0001));
    }
#endif
    REQUIRE(res1 == Catch::Approx(res2).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res3).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res4).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res5).epsilon(0.0001));
    return res1;
}

TEST_CASE("Indel")
{
    std::string test = "aaaa";
    std::string replace_all = "bbbb";

    SECTION("similar strings")
    {
        REQUIRE(indel_distance(test, test) == 0);
        REQUIRE(indel_similarity(test, test) == 8);
        REQUIRE(indel_normalized_distance(test, test) == 0.0);
        REQUIRE(indel_normalized_similarity(test, test) == 1.0);
    }

    SECTION("completly different strings")
    {
        REQUIRE(indel_distance(test, replace_all) == 8);
        REQUIRE(indel_similarity(test, replace_all) == 0);
        REQUIRE(indel_normalized_distance(test, replace_all) == 1.0);
        REQUIRE(indel_normalized_similarity(test, replace_all) == 0.0);
    }

    SECTION("some tests for mbleven")
    {
        std::string a = "South Korea";
        std::string b = "North Korea";
        REQUIRE(indel_distance(a, b) == 4);
        REQUIRE(indel_distance(a, b, 5) == 4);
        REQUIRE(indel_distance(a, b, 4) == 4);
        REQUIRE(indel_distance(a, b, 3) == 4);
        REQUIRE(indel_distance(a, b, 2) == 3);
        REQUIRE(indel_distance(a, b, 1) == 2);
        REQUIRE(indel_distance(a, b, 0) == 1);

        a = "aabc";
        b = "cccd";
        REQUIRE(indel_distance(a, b) == 6);
        REQUIRE(indel_distance(a, b, 6) == 6);
        REQUIRE(indel_distance(a, b, 5) == 6);
        REQUIRE(indel_distance(a, b, 4) == 5);
        REQUIRE(indel_distance(a, b, 3) == 4);
        REQUIRE(indel_distance(a, b, 2) == 3);
        REQUIRE(indel_distance(a, b, 1) == 2);
        REQUIRE(indel_distance(a, b, 0) == 1);
    }

    SECTION("testCachedImplementation")
    {
        std::string a = "001";
        std::string b = "220";
        REQUIRE(Approx(0.3333333) == rapidfuzz::indel_normalized_similarity(a, b));
        REQUIRE(Approx(0.3333333) == rapidfuzz::CachedIndel<char>(a).normalized_similarity(b));
    }

    SECTION("test banded implementation")
    {
        {
            std::string s1 = "ddccbccc";
            std::string s2 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                             "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaccacccaccaaaaaaaa"
                             "daaaaaaaaccccaccccccaaaaaaaccccaaacccaccccadddaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                             "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaccccccccacccaaaaaacccaaaaaacccacccaaaaaacccdccc"
                             "cccacccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccddddddaaaaaaaa"
                             "aaaaaaaaaaaaaaaaaacacccaaaaaacccddddaaaaaaaaaaaaaaaaaaaaaaaaaaaaaccccaaaaaaaaaa"
                             "ccccccaadddaaaaaaaaaaaaaaaaaaaaaacaaaaaa";
            REQUIRE(indel_distance(s1, s2) == 508);
            REQUIRE(indel_distance(s1, s2, 508) == 508);
            REQUIRE(indel_distance(s1, s2, 507) == 508);
            REQUIRE(indel_distance(s1, s2, std::numeric_limits<size_t>::max()) == 508);
        }

        {
            std::string s1 = "bbbdbbmbbbbbbbbbBbfbbbbbbbbbbbbbbbbbbbrbbbbbrbbbbbdbnbbbjbhbbbbbbbbbhbbbbbCbobb"
                             "bxbbbbbkbbbAbxbbwbbbtbcbbbbebbiblbbbbqbbbbbbpbbbbbbubbbkbbDbbbhbkbCbbgbbrbbbbbb"
                             "bbbbbkbyvbbsbAbbbbz";
            std::string s2 = "jaaagaaqyaaaanrCfwaaxaeahtaaaCzaaaspaaBkvaaaaqDaacndaaeolwiaaauaaaaaaamA";

            REQUIRE(indel_distance(s1, s2) == 231);

            rapidfuzz::Editops ops = rapidfuzz::indel_editops(s1, s2);
            REQUIRE(s2 == rapidfuzz::editops_apply_str<char>(ops, s1, s2));
        }
    }
}
