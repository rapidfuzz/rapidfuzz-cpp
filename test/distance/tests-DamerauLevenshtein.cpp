#include <catch2/catch.hpp>
#include <rapidfuzz/details/Range.hpp>
#include <rapidfuzz/details/types.hpp>
#include <string>

#include <rapidfuzz/distance/DamerauLevenshtein.hpp>

#include "../common.hpp"

using Catch::Matchers::WithinAbs;

template <typename Sentence1, typename Sentence2>
size_t damerau_levenshtein_distance(const Sentence1& s1, const Sentence2& s2,
                                    size_t max = std::numeric_limits<size_t>::max())
{
    size_t res1 = rapidfuzz::experimental::damerau_levenshtein_distance(s1, s2, max);
    size_t res2 = rapidfuzz::experimental::damerau_levenshtein_distance(s1.begin(), s1.end(), s2.begin(),
                                                                        s2.end(), max);
    size_t res3 = rapidfuzz::experimental::damerau_levenshtein_distance(
        make_bidir(s1.begin()), make_bidir(s1.end()), make_bidir(s2.begin()), make_bidir(s2.end()), max);
    rapidfuzz::experimental::CachedDamerauLevenshtein<rapidfuzz::char_type<Sentence1>> scorer(s1);
    size_t res4 = scorer.distance(s2, max);
    size_t res5 = scorer.distance(s2.begin(), s2.end(), max);
    REQUIRE(res1 == res2);
    REQUIRE(res1 == res3);
    REQUIRE(res1 == res4);
    REQUIRE(res1 == res5);
    return res1;
}

template <typename Sentence1, typename Sentence2>
size_t damerau_levenshtein_similarity(const Sentence1& s1, const Sentence2& s2, size_t max = 0)
{
    size_t res1 = rapidfuzz::experimental::damerau_levenshtein_similarity(s1, s2, max);
    size_t res2 = rapidfuzz::experimental::damerau_levenshtein_similarity(s1.begin(), s1.end(), s2.begin(),
                                                                          s2.end(), max);
    size_t res3 = rapidfuzz::experimental::damerau_levenshtein_similarity(
        make_bidir(s1.begin()), make_bidir(s1.end()), make_bidir(s2.begin()), make_bidir(s2.end()), max);
    rapidfuzz::experimental::CachedDamerauLevenshtein<rapidfuzz::char_type<Sentence1>> scorer(s1);
    size_t res4 = scorer.similarity(s2, max);
    size_t res5 = scorer.similarity(s2.begin(), s2.end(), max);
    REQUIRE(res1 == res2);
    REQUIRE(res1 == res3);
    REQUIRE(res1 == res4);
    REQUIRE(res1 == res5);
    return res1;
}

template <typename Sentence1, typename Sentence2>
double damerau_levenshtein_normalized_distance(const Sentence1& s1, const Sentence2& s2,
                                               double score_cutoff = 1.0)
{
    double res1 = rapidfuzz::experimental::damerau_levenshtein_normalized_distance(s1, s2, score_cutoff);
    double res2 = rapidfuzz::experimental::damerau_levenshtein_normalized_distance(
        s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
    double res3 = rapidfuzz::experimental::damerau_levenshtein_normalized_distance(
        make_bidir(s1.begin()), make_bidir(s1.end()), make_bidir(s2.begin()), make_bidir(s2.end()),
        score_cutoff);
    rapidfuzz::experimental::CachedDamerauLevenshtein<rapidfuzz::char_type<Sentence1>> scorer(s1);
    double res4 = scorer.normalized_distance(s2, score_cutoff);
    double res5 = scorer.normalized_distance(s2.begin(), s2.end(), score_cutoff);
    REQUIRE_THAT(res1, WithinAbs(res2, 0.0001));
    REQUIRE_THAT(res1, WithinAbs(res3, 0.0001));
    REQUIRE_THAT(res1, WithinAbs(res4, 0.0001));
    REQUIRE_THAT(res1, WithinAbs(res5, 0.0001));
    return res1;
}

template <typename Sentence1, typename Sentence2>
double damerau_levenshtein_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                                 double score_cutoff = 0.0)
{
    double res1 = rapidfuzz::experimental::damerau_levenshtein_normalized_similarity(s1, s2, score_cutoff);
    double res2 = rapidfuzz::experimental::damerau_levenshtein_normalized_similarity(
        s1.begin(), s1.end(), s2.begin(), s2.end(), score_cutoff);
    double res3 = rapidfuzz::experimental::damerau_levenshtein_normalized_similarity(
        make_bidir(s1.begin()), make_bidir(s1.end()), make_bidir(s2.begin()), make_bidir(s2.end()),
        score_cutoff);
    rapidfuzz::experimental::CachedDamerauLevenshtein<rapidfuzz::char_type<Sentence1>> scorer(s1);
    double res4 = scorer.normalized_similarity(s2, score_cutoff);
    double res5 = scorer.normalized_similarity(s2.begin(), s2.end(), score_cutoff);
    REQUIRE_THAT(res1, WithinAbs(res2, 0.0001));
    REQUIRE_THAT(res1, WithinAbs(res3, 0.0001));
    REQUIRE_THAT(res1, WithinAbs(res4, 0.0001));
    REQUIRE_THAT(res1, WithinAbs(res5, 0.0001));
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
        REQUIRE_THAT(damerau_levenshtein_normalized_similarity(test, no_suffix), WithinAbs(0.75, 0.0001));
        REQUIRE_THAT(damerau_levenshtein_normalized_similarity(swapped1, swapped2), WithinAbs(0.75, 0.0001));
        REQUIRE_THAT(damerau_levenshtein_normalized_similarity(test, no_suffix2), WithinAbs(0.75, 0.0001));
        REQUIRE(damerau_levenshtein_normalized_similarity(test, replace_all) == 0.0);

        {
            std::string s1 = "CA";
            std::string s2 = "ABC";
            REQUIRE_THAT(damerau_levenshtein_normalized_similarity(s1, s2), WithinAbs(0.33333, 0.0001));
        }
    }
}
