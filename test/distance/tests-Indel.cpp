#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#include <rapidfuzz/distance/Indel.hpp>
using Catch::Approx;

template <typename Sentence1, typename Sentence2>
int64_t indel_distance(const Sentence1& s1, const Sentence2& s2,
                       int64_t max = std::numeric_limits<int64_t>::max())
{
    int64_t res1 = rapidfuzz::indel_distance(s1, s2, max);
    int64_t res2 = rapidfuzz::indel_distance(s1.begin(), s1.end(), s2.begin(), s2.end(), max);
    rapidfuzz::CachedIndel<typename Sentence1::value_type> scorer(s1);
    int64_t res3 = scorer.distance(s2, max);
    int64_t res4 = scorer.distance(s2.begin(), s2.end(), max);
    REQUIRE(res1 == res2);
    REQUIRE(res1 == res3);
    REQUIRE(res1 == res4);
    return res1;
}

template <typename Sentence1, typename Sentence2>
int64_t indel_similarity(const Sentence1& s1, const Sentence2& s2, int64_t max = 0)
{
    int64_t res1 = rapidfuzz::indel_similarity(s1, s2, max);
    int64_t res2 = rapidfuzz::indel_similarity(s1.begin(), s1.end(), s2.begin(), s2.end(), max);
    rapidfuzz::CachedIndel<typename Sentence1::value_type> scorer(s1);
    int64_t res3 = scorer.similarity(s2, max);
    int64_t res4 = scorer.similarity(s2.begin(), s2.end(), max);
    REQUIRE(res1 == res2);
    REQUIRE(res1 == res3);
    REQUIRE(res1 == res4);
    return res1;
}

template <typename Sentence1, typename Sentence2>
double indel_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    double res1 = rapidfuzz::indel_normalized_distance(s1, s2, score_cutoff);
    double res2 =
        rapidfuzz::indel_normalized_distance(s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
    rapidfuzz::CachedIndel<typename Sentence1::value_type> scorer(s1);
    double res3 = scorer.normalized_distance(s2, score_cutoff);
    double res4 = scorer.normalized_distance(s2.begin(), s2.end(), score_cutoff);
    REQUIRE(res1 == Catch::Approx(res2).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res3).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res4).epsilon(0.0001));
    return res1;
}

template <typename Sentence1, typename Sentence2>
double indel_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    double res1 = rapidfuzz::indel_normalized_similarity(s1, s2, score_cutoff);
    double res2 =
        rapidfuzz::indel_normalized_similarity(s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
    rapidfuzz::CachedIndel<typename Sentence1::value_type> scorer(s1);
    double res3 = scorer.normalized_similarity(s2, score_cutoff);
    double res4 = scorer.normalized_similarity(s2.begin(), s2.end(), score_cutoff);
    REQUIRE(res1 == Catch::Approx(res2).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res3).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res4).epsilon(0.0001));
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
}
