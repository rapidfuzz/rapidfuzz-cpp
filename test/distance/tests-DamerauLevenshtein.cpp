#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidfuzz/details/Range.hpp>
#include <rapidfuzz/details/types.hpp>
#include <string>

#include <rapidfuzz/distance/DamerauLevenshtein.hpp>

template <typename T>
std::basic_string<T> str_multiply(std::basic_string<T> a, unsigned int b)
{
    std::basic_string<T> output;
    while (b--)
        output += a;

    return output;
}

template <typename Sentence1, typename Sentence2>
int64_t damerau_levenshtein_distance(const Sentence1& s1, const Sentence2& s2,
                                     int64_t max = std::numeric_limits<int64_t>::max())
{
    int64_t res1 = rapidfuzz::experimental::damerau_levenshtein_distance(s1, s2, max);
    int64_t res2 = rapidfuzz::experimental::damerau_levenshtein_distance(s1.begin(), s1.end(), s2.begin(),
                                                                         s2.end(), max);
    rapidfuzz::experimental::CachedDamerauLevenshtein scorer(s1);
    int64_t res3 = scorer.distance(s2, max);
    int64_t res4 = scorer.distance(s2.begin(), s2.end(), max);
    REQUIRE(res1 == res2);
    REQUIRE(res1 == res3);
    REQUIRE(res1 == res4);
    return res1;
}

template <typename Sentence1, typename Sentence2>
int64_t damerau_levenshtein_similarity(const Sentence1& s1, const Sentence2& s2, int64_t max = 0)
{
    int64_t res1 = rapidfuzz::experimental::damerau_levenshtein_similarity(s1, s2, max);
    int64_t res2 = rapidfuzz::experimental::damerau_levenshtein_similarity(s1.begin(), s1.end(), s2.begin(),
                                                                           s2.end(), max);
    rapidfuzz::experimental::CachedDamerauLevenshtein scorer(s1);
    int64_t res3 = scorer.similarity(s2, max);
    int64_t res4 = scorer.similarity(s2.begin(), s2.end(), max);
    REQUIRE(res1 == res2);
    REQUIRE(res1 == res3);
    REQUIRE(res1 == res4);
    return res1;
}

template <typename Sentence1, typename Sentence2>
double damerau_levenshtein_normalized_distance(const Sentence1& s1, const Sentence2& s2,
                                               double score_cutoff = 1.0)
{
    double res1 = rapidfuzz::experimental::damerau_levenshtein_normalized_distance(s1, s2, score_cutoff);
    double res2 = rapidfuzz::experimental::damerau_levenshtein_normalized_distance(
        s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
    rapidfuzz::experimental::CachedDamerauLevenshtein scorer(s1);
    double res3 = scorer.normalized_distance(s2, score_cutoff);
    double res4 = scorer.normalized_distance(s2.begin(), s2.end(), score_cutoff);
    REQUIRE(res1 == Catch::Approx(res2).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res3).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res4).epsilon(0.0001));
    return res1;
}

template <typename Sentence1, typename Sentence2>
double damerau_levenshtein_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                                 double score_cutoff = 0.0)
{
    double res1 = rapidfuzz::experimental::damerau_levenshtein_normalized_similarity(s1, s2, score_cutoff);
    double res2 = rapidfuzz::experimental::damerau_levenshtein_normalized_similarity(
        s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
    rapidfuzz::experimental::CachedDamerauLevenshtein scorer(s1);
    double res3 = scorer.normalized_similarity(s2, score_cutoff);
    double res4 = scorer.normalized_similarity(s2.begin(), s2.end(), score_cutoff);
    REQUIRE(res1 == Catch::Approx(res2).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res3).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res4).epsilon(0.0001));
    return res1;
}

TEST_CASE("Levenshtein")
{
    std::string test = "aaaa";
    std::wstring no_suffix = L"aaa";
    std::string no_suffix2 = "aaab";
    std::string swapped1 = "abaa";
    std::string swapped2 = "baaa";
    std::string replace_all = "bbbb";

    SECTION("damerau levenshtein calculates correct distances")
    {
        REQUIRE(damerau_levenshtein_distance(test, test) == 0);
        REQUIRE(damerau_levenshtein_distance(test, no_suffix) == 1);
        REQUIRE(damerau_levenshtein_distance(swapped1, swapped2) == 1);
        REQUIRE(damerau_levenshtein_distance(test, no_suffix2) == 1);
        REQUIRE(damerau_levenshtein_distance(test, replace_all) == 4);

        {
            std::string s1 = "CA";
            std::string s2 = "ABC";
            REQUIRE(damerau_levenshtein_distance(s1, s2) == 2);
        }
    }

    SECTION("weighted levenshtein calculates correct ratios")
    {
        REQUIRE(damerau_levenshtein_normalized_similarity(test, test) == 1.0);
        REQUIRE(damerau_levenshtein_normalized_similarity(test, no_suffix) ==
                Catch::Approx(0.75).epsilon(0.0001));
        REQUIRE(damerau_levenshtein_normalized_similarity(swapped1, swapped2) ==
                Catch::Approx(0.75).epsilon(0.0001));
        REQUIRE(damerau_levenshtein_normalized_similarity(test, no_suffix2) ==
                Catch::Approx(0.75).epsilon(0.0001));
        REQUIRE(damerau_levenshtein_normalized_similarity(test, replace_all) == 0.0);

        {
            std::string s1 = "CA";
            std::string s2 = "ABC";
            REQUIRE(damerau_levenshtein_normalized_similarity(s1, s2) ==
                    Catch::Approx(0.33333).epsilon(0.0001));
        }
    }
}
