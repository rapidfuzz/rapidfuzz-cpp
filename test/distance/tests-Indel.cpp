#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#include <rapidfuzz/distance/Indel.hpp>
using Catch::Approx;

TEST_CASE("Indel")
{
    std::string test = "aaaa";
    std::string replace_all = "bbbb";

    SECTION("similar strings")
    {
        REQUIRE(rapidfuzz::indel_distance(test, test) == 0);
        REQUIRE(rapidfuzz::indel_similarity(test, test) == 8);
        REQUIRE(rapidfuzz::indel_normalized_distance(test, test) == 0.0);
        REQUIRE(rapidfuzz::indel_normalized_similarity(test, test) == 1.0);

        REQUIRE(rapidfuzz::CachedIndel<char>(test).distance(test) == 0);
        REQUIRE(rapidfuzz::CachedIndel<char>(test).similarity(test) == 8);
        REQUIRE(rapidfuzz::CachedIndel<char>(test).normalized_distance(test) == 0.0);
        REQUIRE(rapidfuzz::CachedIndel<char>(test).normalized_similarity(test) == 1.0);
    }

    SECTION("completly different strings")
    {
        REQUIRE(rapidfuzz::indel_distance(test, replace_all) == 8);
        REQUIRE(rapidfuzz::indel_similarity(test, replace_all) == 0);
        REQUIRE(rapidfuzz::indel_normalized_distance(test, replace_all) == 1.0);
        REQUIRE(rapidfuzz::indel_normalized_similarity(test, replace_all) == 0.0);

        REQUIRE(rapidfuzz::CachedIndel<char>(test).distance(replace_all) == 8);
        REQUIRE(rapidfuzz::CachedIndel<char>(test).similarity(replace_all) == 0);
        REQUIRE(rapidfuzz::CachedIndel<char>(test).normalized_distance(replace_all) == 1.0);
        REQUIRE(rapidfuzz::CachedIndel<char>(test).normalized_similarity(replace_all) == 0.0);
    }

    SECTION("some tests for mbleven")
    {
        std::string a = "South Korea";
        std::string b = "North Korea";
        REQUIRE(rapidfuzz::indel_distance(a, b) == 4);
        REQUIRE(rapidfuzz::indel_distance(a, b, 5) == 4);
        REQUIRE(rapidfuzz::indel_distance(a, b, 4) == 4);
        REQUIRE(rapidfuzz::indel_distance(a, b, 3) == 4);
        REQUIRE(rapidfuzz::indel_distance(a, b, 2) == 3);
        REQUIRE(rapidfuzz::indel_distance(a, b, 1) == 2);
        REQUIRE(rapidfuzz::indel_distance(a, b, 0) == 1);

        REQUIRE(rapidfuzz::CachedIndel<char>(a).distance(b) == 4);
        REQUIRE(rapidfuzz::CachedIndel<char>(a).distance(b, 5) == 4);
        REQUIRE(rapidfuzz::CachedIndel<char>(a).distance(b, 4) == 4);
        REQUIRE(rapidfuzz::CachedIndel<char>(a).distance(b, 3) == 4);
        REQUIRE(rapidfuzz::CachedIndel<char>(a).distance(b, 2) == 3);
        REQUIRE(rapidfuzz::CachedIndel<char>(a).distance(b, 1) == 2);
        REQUIRE(rapidfuzz::CachedIndel<char>(a).distance(b, 0) == 1);
    }

    SECTION("testCachedImplementation")
    {
        std::string a = "001";
        std::string b = "220";
        REQUIRE(Approx(0.3333333) == rapidfuzz::indel_normalized_similarity(a, b));
        REQUIRE(Approx(0.3333333) == rapidfuzz::CachedIndel<char>(a).normalized_similarity(b));
    }
};
