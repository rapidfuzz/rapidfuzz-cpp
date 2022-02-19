#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <string>

#include <rapidfuzz/distance/Levenshtein.hpp>

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
    REQUIRE(rapidfuzz::levenshtein_normalized_similarity(test, replace_all, {1, 1, 2}) ==
            0.0);
  }
};

TEST_CASE("Levenshtein_editops")
{
  std::string s = "Lorem ipsum.";
  std::string d = "XYZLorem ABC iPsum";

  rapidfuzz::Editops editops = rapidfuzz::levenshtein_editops(s, d);
  rapidfuzz::Editops correct_editops(9);
  correct_editops[0] = rapidfuzz::EditOp(rapidfuzz::EditType::Insert, 0, 0);
  correct_editops[1] = rapidfuzz::EditOp(rapidfuzz::EditType::Insert, 0, 1);
  correct_editops[2] = rapidfuzz::EditOp(rapidfuzz::EditType::Insert, 0, 2);
  correct_editops[3] = rapidfuzz::EditOp(rapidfuzz::EditType::Insert, 6, 9);
  correct_editops[4] = rapidfuzz::EditOp(rapidfuzz::EditType::Insert, 6, 10);
  correct_editops[5] = rapidfuzz::EditOp(rapidfuzz::EditType::Insert, 6, 11);
  correct_editops[6] = rapidfuzz::EditOp(rapidfuzz::EditType::Insert, 6, 12);
  correct_editops[7] = rapidfuzz::EditOp(rapidfuzz::EditType::Replace, 7, 14);
  correct_editops[8] = rapidfuzz::EditOp(rapidfuzz::EditType::Delete, 11, 18);
  correct_editops.set_src_len(s.size());
  correct_editops.set_dest_len(d.size());

  REQUIRE(editops == correct_editops);
};
