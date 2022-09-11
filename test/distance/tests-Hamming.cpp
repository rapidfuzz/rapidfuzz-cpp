#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidfuzz/distance/Hamming.hpp>
#include <string>

template <typename Sentence1, typename Sentence2>
int64_t hamming_distance(const Sentence1& s1, const Sentence2& s2,
                         int64_t max = std::numeric_limits<int64_t>::max())
{
    int64_t res1 = rapidfuzz::hamming_distance(s1, s2, max);
    int64_t res2 = rapidfuzz::hamming_distance(s1.begin(), s1.end(), s2.begin(), s2.end(), max);
    rapidfuzz::CachedHamming scorer(s1);
    int64_t res3 = scorer.distance(s2, max);
    int64_t res4 = scorer.distance(s2.begin(), s2.end(), max);
    REQUIRE(res1 == res2);
    REQUIRE(res1 == res3);
    REQUIRE(res1 == res4);
    return res1;
}

template <typename Sentence1, typename Sentence2>
int64_t hamming_similarity(const Sentence1& s1, const Sentence2& s2, int64_t max = 0)
{
    int64_t res1 = rapidfuzz::hamming_similarity(s1, s2, max);
    int64_t res2 = rapidfuzz::hamming_similarity(s1.begin(), s1.end(), s2.begin(), s2.end(), max);
    rapidfuzz::CachedHamming scorer(s1);
    int64_t res3 = scorer.similarity(s2, max);
    int64_t res4 = scorer.similarity(s2.begin(), s2.end(), max);
    REQUIRE(res1 == res2);
    REQUIRE(res1 == res3);
    REQUIRE(res1 == res4);
    return res1;
}

template <typename Sentence1, typename Sentence2>
double hamming_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    double res1 = rapidfuzz::hamming_normalized_distance(s1, s2, score_cutoff);
    double res2 =
        rapidfuzz::hamming_normalized_distance(s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
    rapidfuzz::CachedHamming scorer(s1);
    double res3 = scorer.normalized_distance(s2, score_cutoff);
    double res4 = scorer.normalized_distance(s2.begin(), s2.end(), score_cutoff);
    REQUIRE(res1 == Catch::Approx(res2).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res3).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res4).epsilon(0.0001));
    return res1;
}

template <typename Sentence1, typename Sentence2>
double hamming_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    double res1 = rapidfuzz::hamming_normalized_similarity(s1, s2, score_cutoff);
    double res2 =
        rapidfuzz::hamming_normalized_similarity(s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
    rapidfuzz::CachedHamming scorer(s1);
    double res3 = scorer.normalized_similarity(s2, score_cutoff);
    double res4 = scorer.normalized_similarity(s2.begin(), s2.end(), score_cutoff);
    REQUIRE(res1 == Catch::Approx(res2).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res3).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res4).epsilon(0.0001));
    return res1;
}

TEST_CASE("Hamming")
{
    std::string test = "aaaa";
    std::string diff_a = "abaa";
    std::string diff_b = "aaba";
    std::string diff_len = "aaaaa";

    SECTION("hamming calculates correct distances")
    {
        REQUIRE(hamming_distance(test, test) == 0);
        REQUIRE(hamming_distance(test, diff_a) == 1);
        REQUIRE(hamming_distance(test, diff_b) == 1);
        REQUIRE(hamming_distance(diff_a, diff_b) == 2);
    }

    SECTION("hamming raises exception for different lengths")
    {
        REQUIRE_THROWS_AS(rapidfuzz::hamming_distance(test, diff_len), std::invalid_argument);
    }
}
