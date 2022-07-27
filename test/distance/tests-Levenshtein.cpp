#include "rapidfuzz/details/Range.hpp"
#include "rapidfuzz/details/types.hpp"
#include "rapidfuzz/distance/Levenshtein.hpp"
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidfuzz/details/types.hpp>
#include <string>

#include <rapidfuzz/distance.hpp>

template <typename T>
std::basic_string<T> str_multiply(std::basic_string<T> a, unsigned int b) {
    std::basic_string<T> output;
    while (b--)
        output += a;

    return output;
}

template <typename Sentence1, typename Sentence2>
int64_t levenshtein_distance(const Sentence1& s1, const Sentence2& s2,
                             rapidfuzz::LevenshteinWeightTable weights = {1, 1, 1},
                             int64_t max = std::numeric_limits<int64_t>::max())
{
    int64_t res1 = rapidfuzz::levenshtein_distance(s1, s2, weights, max);
    rapidfuzz::CachedLevenshtein<typename Sentence1::value_type> scorer(s1, weights);
    int64_t res2 = scorer.distance(s2, max);
    REQUIRE(res1 == res2);
    return res1;
}

template <typename Sentence1, typename Sentence2>
double levenshtein_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                         rapidfuzz::LevenshteinWeightTable weights = {1, 1, 1},
                                         double score_cutoff = 0.0)
{
    double res1 = rapidfuzz::levenshtein_normalized_similarity(s1, s2, weights, score_cutoff);
    rapidfuzz::CachedLevenshtein<typename Sentence1::value_type> scorer(s1, weights);
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

    SECTION("weighted levenshtein calculates correct distances")
    {
        REQUIRE(levenshtein_distance(test, test, {1, 1, 2}) == 0);
        REQUIRE(levenshtein_distance(test, no_suffix, {1, 1, 2}) == 1);
        REQUIRE(levenshtein_distance(swapped1, swapped2, {1, 1, 2}) == 2);
        REQUIRE(levenshtein_distance(test, no_suffix2, {1, 1, 2}) == 2);
        REQUIRE(levenshtein_distance(test, replace_all, {1, 1, 2}) == 8);
    }

    SECTION("weighted levenshtein calculates correct ratios")
    {
        REQUIRE(levenshtein_normalized_similarity(test, test, {1, 1, 2}) == 1.0);
        REQUIRE(levenshtein_normalized_similarity(test, no_suffix, {1, 1, 2}) ==
                Catch::Approx(0.8571).epsilon(0.0001));
        REQUIRE(levenshtein_normalized_similarity(swapped1, swapped2, {1, 1, 2}) ==
                Catch::Approx(0.75).epsilon(0.0001));
        REQUIRE(levenshtein_normalized_similarity(test, no_suffix2, {1, 1, 2}) ==
                Catch::Approx(0.75).epsilon(0.0001));
        REQUIRE(levenshtein_normalized_similarity(test, replace_all, {1, 1, 2}) == 0.0);
    }

    SECTION("test mbleven implementation")
    {
        std::string a = "South Korea";
        std::string b = "North Korea";
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 4) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 3) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 2) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 1) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 0) == 1);

        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}) == 4);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 4) == 4);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 3) == 4);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 2) == 3);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 1) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 0) == 1);

        a = "aabc";
        b = "cccd";
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}) == 4);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 4) == 4);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 3) == 4);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 2) == 3);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 1) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 0) == 1);

        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}) == 6);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 6) == 6);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 5) == 6);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 4) == 5);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 3) == 4);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 2) == 3);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 1) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 0) == 1);
    }

    SECTION("test banded implementation")
    {
        {
            std::string s1 = "kkkkbbbbfkkkkkkibfkkkafakkfekgkkkkkkkkkkbdbbddddddddddafkkkekkkhkk";
            std::string s2 = "khddddddddkkkkdgkdikkccccckcckkkekkkkdddddddddddafkkhckkkkkdckkkcc";
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}) == 36);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 31) == 32);
        }

        {
            std::string s1 = "ccddcddddddddddddddddddddddddddddddddddddddddddddddddddddaaaaaaaaaaa";
            std::string s2 = "aaaaaaaaaaaaaadddddddddbddddddddddddddddddddddddddddddddddbddddddddd";
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}) == 26);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 31) == 26);
        }

        {
            std::string s1 = "accccccccccaaaaaaaccccccccccccccccccccccccccccccaccccccccccccccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaacccccccccccccccccccccc";
            std::string s2 = "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccbcccb";
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}) == 24);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 25) == 24);
        }

        {
            std::string s1 = "miiiiiiiiiiliiiiiiibghiiaaaaaaaaaaaaaaacccfccccedddaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
            std::string s2 = "aaaaaaajaaaaaaaabghiiaaaaaaaaaaaaaaacccfccccedddaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaajjdim";
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}) == 27);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 27) == 27);
        }

        {
            std::string s1 = "lllllfllllllllllllllllllllllllllllllllllllllllllllllllglllllilldcaaaaaaaaaaaaaaaaaaadbbllllllllllhllllllllllllllllllllllllllgl";
            std::string s2 = "aaaaaaaaaaaaaadbbllllllllllllllelllllllllllllllllllllllllllllllglllllilldcaaaaaaaaaaaaaaaaaaadbbllllllllllllllellllllllllllllhlllllllllill";
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}) == 23);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 27) == 23);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 28) == 23);
        }

        {
            std::string s1 = "llccacaaaaaaaaaccccccccccccccccddffaccccaccecccggggclallhcccccljif";
            std::string s2 = "bddcbllllllbcccccccccccccccccddffccccccccebcccggggclbllhcccccljifbddcccccc";
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}) == 27);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 27) == 27);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 28) == 27);
        }
    }
}

TEST_CASE("Levenshtein_editops")
{
    std::string s = "Lorem ipsum.";
    std::string d = "XYZLorem ABC iPsum";

    rapidfuzz::Editops ops = rapidfuzz::levenshtein_editops(s, d);
    REQUIRE(d == rapidfuzz::editops_apply<char>(ops, s, d));
    REQUIRE(ops.get_src_len() == s.size());
    REQUIRE(ops.get_dest_len() == d.size());
}

TEST_CASE("Levenshtein_find_hirschberg_pos")
{
    {
        std::string s1 = str_multiply(std::string("abb"), 2);
        std::string s2 = str_multiply(std::string("ccccca"), 2);

        auto hpos = rapidfuzz::detail::find_hirschberg_pos(
            rapidfuzz::detail::make_range(s1), rapidfuzz::detail::make_range(s2)
        );
        REQUIRE(hpos.left_score == 5);
        REQUIRE(hpos.right_score == 6);
        REQUIRE(hpos.s2_mid == 6);
        REQUIRE(hpos.s1_mid == 1);
    }

    {
        std::string s1 = str_multiply(std::string("abb"), 8*64);
        std::string s2 = str_multiply(std::string("ccccca"), 8*64);

        auto hpos = rapidfuzz::detail::find_hirschberg_pos(
            rapidfuzz::detail::make_range(s1), rapidfuzz::detail::make_range(s2)
        );
        REQUIRE(hpos.left_score == 1280);
        REQUIRE(hpos.right_score == 1281);
        REQUIRE(hpos.s2_mid == 1536);
        REQUIRE(hpos.s1_mid == 766);
    }
}

TEST_CASE("Levenshtein_editops[fuzzing_regressions]")
{
    /* Test regressions of bugs found through fuzzing */
    {
        std::string s1 = "b";
        std::string s2 = "aaaaaaaaaaaaaaaabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
        rapidfuzz::Editops ops = rapidfuzz::levenshtein_editops(s1, s2);
        REQUIRE(s2 == rapidfuzz::editops_apply<char>(ops, s1, s2));
    }

    {
        std::string s1 = "aa";
        std::string s2 = "abb";
        rapidfuzz::Editops ops = rapidfuzz::levenshtein_editops(s1, s2);
        REQUIRE(s2 == rapidfuzz::editops_apply<char>(ops, s1, s2));
    }

    {
        std::string s1 = str_multiply(std::string("abb"), 8*64);
        std::string s2 = str_multiply(std::string("ccccca"), 8*64);
        rapidfuzz::Editops ops = rapidfuzz::levenshtein_editops(s1, s2);
        REQUIRE(s2 == rapidfuzz::editops_apply<char>(ops, s1, s2));
    }

}