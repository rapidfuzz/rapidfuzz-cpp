#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#include <rapidfuzz/distance.hpp>

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
        REQUIRE(rapidfuzz::levenshtein_distance(test, test, {1, 1, 2}) == 0);
        REQUIRE(rapidfuzz::levenshtein_distance(test, no_suffix, {1, 1, 2}) == 1);
        REQUIRE(rapidfuzz::levenshtein_distance(swapped1, swapped2, {1, 1, 2}) == 2);
        REQUIRE(rapidfuzz::levenshtein_distance(test, no_suffix2, {1, 1, 2}) == 2);
        REQUIRE(rapidfuzz::levenshtein_distance(test, replace_all, {1, 1, 2}) == 8);
    }

    SECTION("weighted levenshtein calculates correct ratios")
    {
        REQUIRE(rapidfuzz::levenshtein_normalized_similarity(test, test, {1, 1, 2}) == 1.0);
        REQUIRE(rapidfuzz::levenshtein_normalized_similarity(test, no_suffix, {1, 1, 2}) ==
                Catch::Approx(0.8571).epsilon(0.0001));
        REQUIRE(rapidfuzz::levenshtein_normalized_similarity(swapped1, swapped2, {1, 1, 2}) ==
                Catch::Approx(0.75).epsilon(0.0001));
        REQUIRE(rapidfuzz::levenshtein_normalized_similarity(test, no_suffix2, {1, 1, 2}) ==
                Catch::Approx(0.75).epsilon(0.0001));
        REQUIRE(rapidfuzz::levenshtein_normalized_similarity(test, replace_all, {1, 1, 2}) == 0.0);
    }
};

TEST_CASE("Levenshtein_editops")
{
    std::string s = "Lorem ipsum.";
    std::string d = "XYZLorem ABC iPsum";

    rapidfuzz::Editops ops = rapidfuzz::levenshtein_editops(s, d);
    REQUIRE(d == rapidfuzz::editops_apply<char>(ops, s, d));
    REQUIRE(ops.get_src_len() == s.size());
    REQUIRE(ops.get_dest_len() == d.size());
};

TEST_CASE("Levenshtein_editops[fuzzing_regressions]")
{
    /* Test regressions of bugs found through fuzzing */
    uint8_t string1[] = {0x95};
    uint32_t len1 = 1;
    std::basic_string<uint8_t> s1(string1, len1);

    uint8_t string2[] = {0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x95, 0x95, 0x00, 0x00, 0xff, 0xff,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint32_t len2 = 65;
    std::basic_string<uint8_t> s2(string2, len2);

    rapidfuzz::Editops ops = rapidfuzz::levenshtein_editops(s1, s2);
    REQUIRE(s2 == rapidfuzz::editops_apply<uint8_t>(ops, s1, s2));
};
