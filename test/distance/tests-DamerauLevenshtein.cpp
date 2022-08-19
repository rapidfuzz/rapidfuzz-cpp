#include "rapidfuzz/details/Range.hpp"
#include "rapidfuzz/details/types.hpp"
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidfuzz/details/types.hpp>
#include <string>

#include <rapidfuzz/distance.hpp>

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
    rapidfuzz::experimental::CachedDamerauLevenshtein<typename Sentence1::value_type> scorer(s1);
    int64_t res2 = scorer.distance(s2, max);
    REQUIRE(res1 == res2);
    return res1;
}

template <typename Sentence1, typename Sentence2>
double damerau_levenshtein_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                                 double score_cutoff = 0.0)
{
    double res1 = rapidfuzz::experimental::damerau_levenshtein_normalized_similarity(s1, s2, score_cutoff);
    rapidfuzz::experimental::CachedDamerauLevenshtein<typename Sentence1::value_type> scorer(s1);
    double res2 = scorer.normalized_similarity(s2, score_cutoff);
    REQUIRE(res1 == Catch::Approx(res2).epsilon(0.0001));
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
