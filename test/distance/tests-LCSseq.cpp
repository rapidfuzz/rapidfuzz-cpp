#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidfuzz/distance/LCSseq.hpp>
#include <string>

#include <rapidfuzz/distance/Indel.hpp>

#include "../common.hpp"

template <typename Sentence1, typename Sentence2>
size_t lcs_seq_distance(const Sentence1& s1, const Sentence2& s2,
                        size_t max = std::numeric_limits<size_t>::max())
{
    size_t res1 = rapidfuzz::lcs_seq_distance(s1, s2, max);
    size_t res2 = rapidfuzz::lcs_seq_distance(s1.begin(), s1.end(), s2.begin(), s2.end(), max);
    size_t res3 = rapidfuzz::lcs_seq_distance(
        BidirectionalIterWrapper(s1.begin()), BidirectionalIterWrapper(s1.end()),
        BidirectionalIterWrapper(s2.begin()), BidirectionalIterWrapper(s2.end()), max);
    rapidfuzz::CachedLCSseq scorer(s1);
    size_t res4 = scorer.distance(s2, max);
    size_t res5 = scorer.distance(s2.begin(), s2.end(), max);
#ifdef RAPIDFUZZ_SIMD
    if (s1.size() <= 64) {
        std::vector<size_t> results(256 / 8);

        if (s1.size() <= 8) {
            rapidfuzz::experimental::MultiLCSseq<8> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.distance(&results[0], results.size(), s2, max);
            REQUIRE(res1 == results[0]);
        }
        if (s1.size() <= 16) {
            rapidfuzz::experimental::MultiLCSseq<16> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.distance(&results[0], results.size(), s2, max);
            REQUIRE(res1 == results[0]);
        }
        if (s1.size() <= 32) {
            rapidfuzz::experimental::MultiLCSseq<32> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.distance(&results[0], results.size(), s2, max);
            REQUIRE(res1 == results[0]);
        }
        if (s1.size() <= 64) {
            rapidfuzz::experimental::MultiLCSseq<64> simd_scorer(1);
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
size_t lcs_seq_similarity(const Sentence1& s1, const Sentence2& s2, size_t max = 0)
{
    size_t res1 = rapidfuzz::lcs_seq_similarity(s1, s2, max);
    size_t res2 = rapidfuzz::lcs_seq_similarity(s1.begin(), s1.end(), s2.begin(), s2.end(), max);
    size_t res3 = rapidfuzz::lcs_seq_similarity(
        BidirectionalIterWrapper(s1.begin()), BidirectionalIterWrapper(s1.end()),
        BidirectionalIterWrapper(s2.begin()), BidirectionalIterWrapper(s2.end()), max);
    rapidfuzz::CachedLCSseq scorer(s1);
    size_t res4 = scorer.similarity(s2, max);
    size_t res5 = scorer.similarity(s2.begin(), s2.end(), max);
#ifdef RAPIDFUZZ_SIMD
    if (s1.size() <= 64) {
        std::vector<size_t> results(256 / 8);

        if (s1.size() <= 8) {
            rapidfuzz::experimental::MultiLCSseq<8> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.similarity(&results[0], results.size(), s2, max);
        }
        else if (s1.size() <= 16) {
            rapidfuzz::experimental::MultiLCSseq<16> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.similarity(&results[0], results.size(), s2, max);
        }
        else if (s1.size() <= 32) {
            rapidfuzz::experimental::MultiLCSseq<32> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.similarity(&results[0], results.size(), s2, max);
        }
        else {
            rapidfuzz::experimental::MultiLCSseq<64> simd_scorer(1);
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
double lcs_seq_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    double res1 = rapidfuzz::lcs_seq_normalized_distance(s1, s2, score_cutoff);
    double res2 =
        rapidfuzz::lcs_seq_normalized_distance(s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
    double res3 = rapidfuzz::lcs_seq_normalized_distance(
        BidirectionalIterWrapper(s1.begin()), BidirectionalIterWrapper(s1.end()),
        BidirectionalIterWrapper(s2.begin()), BidirectionalIterWrapper(s2.end()), score_cutoff);
    rapidfuzz::CachedLCSseq scorer(s1);
    double res4 = scorer.normalized_distance(s2, score_cutoff);
    double res5 = scorer.normalized_distance(s2.begin(), s2.end(), score_cutoff);
    REQUIRE(res1 == Catch::Approx(res2).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res3).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res4).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res5).epsilon(0.0001));
    return res1;
}

template <typename Sentence1, typename Sentence2>
double lcs_seq_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    double res1 = rapidfuzz::lcs_seq_normalized_similarity(s1, s2, score_cutoff);
    double res2 =
        rapidfuzz::lcs_seq_normalized_similarity(s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
    double res3 = rapidfuzz::lcs_seq_normalized_similarity(
        BidirectionalIterWrapper(s1.begin()), BidirectionalIterWrapper(s1.end()),
        BidirectionalIterWrapper(s2.begin()), BidirectionalIterWrapper(s2.end()), score_cutoff);
    rapidfuzz::CachedLCSseq scorer(s1);
    double res4 = scorer.normalized_similarity(s2, score_cutoff);
    double res5 = scorer.normalized_similarity(s2.begin(), s2.end(), score_cutoff);
    REQUIRE(res1 == Catch::Approx(res2).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res3).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res4).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res5).epsilon(0.0001));
    return res1;
}

TEST_CASE("LCSseq")
{
    std::string test = "aaaa";
    std::string replace_all = "bbbb";

    SECTION("similar strings")
    {
        REQUIRE(lcs_seq_distance(test, test) == 0);
        REQUIRE(lcs_seq_similarity(test, test) == 4);
        REQUIRE(lcs_seq_normalized_distance(test, test) == 0.0);
        REQUIRE(lcs_seq_normalized_similarity(test, test) == 1.0);
    }

    SECTION("completly different strings")
    {
        REQUIRE(rapidfuzz::lcs_seq_distance(test, replace_all) == 4);
        REQUIRE(rapidfuzz::lcs_seq_similarity(test, replace_all) == 0);
        REQUIRE(rapidfuzz::lcs_seq_normalized_distance(test, replace_all) == 1.0);
        REQUIRE(rapidfuzz::lcs_seq_normalized_similarity(test, replace_all) == 0.0);
    }

    SECTION("some tests for mbleven")
    {
        std::string a = "South Korea";
        std::string b = "North Korea";
        REQUIRE(lcs_seq_similarity(a, b) == 9);
        REQUIRE(lcs_seq_similarity(a, b, 9) == 9);
        REQUIRE(lcs_seq_similarity(a, b, 10) == 0);

        REQUIRE(lcs_seq_distance(a, b) == 2);
        REQUIRE(lcs_seq_distance(a, b, 4) == 2);
        REQUIRE(lcs_seq_distance(a, b, 3) == 2);
        REQUIRE(lcs_seq_distance(a, b, 2) == 2);
        REQUIRE(lcs_seq_distance(a, b, 1) == 2);
        REQUIRE(lcs_seq_distance(a, b, 0) == 1);

        a = "aabc";
        b = "cccd";
        REQUIRE(lcs_seq_similarity(a, b) == 1);
        REQUIRE(lcs_seq_similarity(a, b, 1) == 1);
        REQUIRE(lcs_seq_similarity(a, b, 2) == 0);

        REQUIRE(lcs_seq_distance(a, b) == 3);
        REQUIRE(lcs_seq_distance(a, b, 4) == 3);
        REQUIRE(lcs_seq_distance(a, b, 3) == 3);
        REQUIRE(lcs_seq_distance(a, b, 2) == 3);
        REQUIRE(lcs_seq_distance(a, b, 1) == 2);
        REQUIRE(lcs_seq_distance(a, b, 0) == 1);
    }

    SECTION("testCachedImplementation")
    {
        std::string a = "001";
        std::string b = "220";
        REQUIRE(1 == rapidfuzz::lcs_seq_similarity(a, b));
        REQUIRE(1 == rapidfuzz::CachedLCSseq<char>(a).similarity(b));
    }
}

#ifdef RAPIDFUZZ_SIMD
TEST_CASE("SIMD wraparound")
{
    rapidfuzz::experimental::MultiLCSseq<8> scorer(4);
    scorer.insert(std::string("a"));
    scorer.insert(std::string("b"));
    scorer.insert(std::string("aa"));
    scorer.insert(std::string("bb"));
    std::vector<size_t> results(scorer.result_count());

    {
        std::string s2 = str_multiply(std::string("b"), 256);
        scorer.distance(&results[0], results.size(), s2);
        REQUIRE(results[0] == 256);
        REQUIRE(results[1] == 255);
        REQUIRE(results[2] == 256);
        REQUIRE(results[3] == 254);
    }

    {
        std::string s2 = str_multiply(std::string("b"), 300);
        scorer.distance(&results[0], results.size(), s2);
        REQUIRE(results[0] == 300);
        REQUIRE(results[1] == 299);
        REQUIRE(results[2] == 300);
        REQUIRE(results[3] == 298);
    }

    {
        std::string s2 = str_multiply(std::string("b"), 512);
        scorer.distance(&results[0], results.size(), s2);
        REQUIRE(results[0] == 512);
        REQUIRE(results[1] == 511);
        REQUIRE(results[2] == 512);
        REQUIRE(results[3] == 510);
    }
}
#endif
