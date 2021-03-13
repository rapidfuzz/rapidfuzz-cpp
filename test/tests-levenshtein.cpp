#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <algorithm>
#include <boost/utility/string_view.hpp>
#include <string_view>
#include <vector>

#include <rapidfuzz/string_metric.hpp>

namespace string_metric = rapidfuzz::string_metric;

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
    REQUIRE(string_metric::levenshtein(test, test, {1, 1, 2}) == 0);
    REQUIRE(string_metric::levenshtein(test, no_suffix, {1, 1, 2}) == 1);
    REQUIRE(string_metric::levenshtein(swapped1, swapped2, {1, 1, 2}) == 2);
    REQUIRE(string_metric::levenshtein(test, no_suffix2, {1, 1, 2}) == 2);
    REQUIRE(string_metric::levenshtein(test, replace_all, {1, 1, 2}) == 8);
  }

  SECTION("weighted levenshtein calculates correct ratios")
  {
    REQUIRE(string_metric::normalized_levenshtein(test, test, {1, 1, 2}) == 100.0);
    REQUIRE(string_metric::normalized_levenshtein(test, no_suffix, {1, 1, 2}) ==
            Catch::Approx(85.7).epsilon(0.01));
    REQUIRE(string_metric::normalized_levenshtein(swapped1, swapped2, {1, 1, 2}) ==
            Catch::Approx(75.0).epsilon(0.01));
    REQUIRE(string_metric::normalized_levenshtein(test, no_suffix2, {1, 1, 2}) ==
            Catch::Approx(75.0).epsilon(0.01));
    REQUIRE(string_metric::normalized_levenshtein(test, replace_all, {1, 1, 2}) ==
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
    REQUIRE(string_metric::hamming(test, test) == 0);
    REQUIRE(string_metric::hamming(test, diff_a) == 1);
    REQUIRE(string_metric::hamming(test, diff_b) == 1);
    REQUIRE(string_metric::hamming(diff_a, diff_b) == 2);
  }

  SECTION("hamming raises exception for different lengths")
  {
    REQUIRE_THROWS_AS(string_metric::hamming(test, diff_len), std::invalid_argument);
  }
};
