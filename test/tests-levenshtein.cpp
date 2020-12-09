#include <catch2/catch.hpp>
#include <algorithm>
#include <boost/utility/string_view.hpp>
#include <string_view>
#include <vector>

#include <rapidfuzz/levenshtein.hpp>

namespace levenshtein = rapidfuzz::levenshtein;

TEST_CASE("levenshtein works with string_views", "[string_view]")
{
  rapidfuzz::string_view test = "aaaa";
  rapidfuzz::string_view no_suffix = "aaa";
  rapidfuzz::string_view no_suffix2 = "aaab";
  rapidfuzz::string_view swapped1 = "abaa";
  rapidfuzz::string_view swapped2 = "baaa";
  rapidfuzz::string_view replace_all = "bbbb";

  SECTION("weighted levenshtein calculates correct distances")
  {
    REQUIRE(levenshtein::weighted_distance(test, test) == 0);
    REQUIRE(levenshtein::weighted_distance(test, no_suffix) == 1);
    REQUIRE(levenshtein::weighted_distance(swapped1, swapped2) == 2);
    REQUIRE(levenshtein::weighted_distance(test, no_suffix2) == 2);
    REQUIRE(levenshtein::weighted_distance(test, replace_all) == 8);
  }

  SECTION("weighted levenshtein calculates correct ratios")
  {
    REQUIRE(levenshtein::normalized_weighted_distance(test, test) == 1.0);
    REQUIRE(levenshtein::normalized_weighted_distance(test, no_suffix) ==
            Approx(0.857).epsilon(0.01));
    REQUIRE(levenshtein::normalized_weighted_distance(swapped1, swapped2) ==
            Approx(0.75).epsilon(0.01));
    REQUIRE(levenshtein::normalized_weighted_distance(test, no_suffix2) ==
            Approx(0.75).epsilon(0.01));
    REQUIRE(levenshtein::normalized_weighted_distance(test, replace_all) ==
            0.0);
  }
};

TEST_CASE("hamming", "[string_view]")
{
  rapidfuzz::string_view test = "aaaa";
  rapidfuzz::string_view diff_a = "abaa";
  rapidfuzz::string_view diff_b = "aaba";
  rapidfuzz::string_view diff_len = "aaaaa";

  SECTION("hamming calculates correct distances")
  {
    REQUIRE(levenshtein::hamming(test, test) == 0);
    REQUIRE(levenshtein::hamming(test, diff_a) == 1);
    REQUIRE(levenshtein::hamming(test, diff_b) == 1);
    REQUIRE(levenshtein::hamming(diff_a, diff_b) == 2);
  }

  SECTION("hamming raises exception for different lengths")
  {
    REQUIRE_THROWS_AS(levenshtein::hamming(test, diff_len), std::invalid_argument);
  }
};
