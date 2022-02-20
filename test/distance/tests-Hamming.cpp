#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidfuzz/distance/Hamming.hpp>
#include <string>

TEST_CASE("Hamming")
{
    std::string test = "aaaa";
    std::string diff_a = "abaa";
    std::string diff_b = "aaba";
    std::string diff_len = "aaaaa";

    SECTION("hamming calculates correct distances")
    {
        REQUIRE(rapidfuzz::hamming_distance(test, test) == 0);
        REQUIRE(rapidfuzz::hamming_distance(test, diff_a) == 1);
        REQUIRE(rapidfuzz::hamming_distance(test, diff_b) == 1);
        REQUIRE(rapidfuzz::hamming_distance(diff_a, diff_b) == 2);
    }

    SECTION("hamming raises exception for different lengths")
    {
        REQUIRE_THROWS_AS(rapidfuzz::hamming_distance(test, diff_len), std::invalid_argument);
    }
};
