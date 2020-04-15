#include "catch2/catch.hpp"
#include <string_view>
#include <vector>
#include <algorithm>
#include <boost/utility/string_view.hpp>

#include "../src/levenshtein.hpp"

namespace levenshtein = rapidfuzz::levenshtein;

TEST_CASE( "levenshtein works with string_views", "[string_view]" ) {
    boost::string_view test = "aaaa";
    boost::string_view no_suffix = "aaa";
    boost::string_view no_suffix2 = "aaab";
    boost::string_view swapped1 = "abaa";
    boost::string_view swapped2 = "baaa";
    boost::string_view replace_all = "bbbb";

    SECTION( "weighted levenshtein calculates correct distances" ) {
        REQUIRE( levenshtein::weighted_distance(test, test) == 0 );
        REQUIRE( levenshtein::weighted_distance(test, no_suffix) == 1 );
        REQUIRE( levenshtein::weighted_distance(swapped1, swapped2) == 2 ); 
        REQUIRE( levenshtein::weighted_distance(test, no_suffix2) == 2 );
        REQUIRE( levenshtein::weighted_distance(test, replace_all) == 8 );
    }

    SECTION( "weighted levenshtein calculates correct ratios" ) {
        REQUIRE( levenshtein::normalized_weighted_distance(test, test) == 1.0 );
        REQUIRE( levenshtein::normalized_weighted_distance(test, no_suffix) == Approx(0.857).epsilon(0.01) );
        REQUIRE( levenshtein::normalized_weighted_distance(swapped1, swapped2) == Approx(0.75).epsilon(0.01) ); 
        REQUIRE( levenshtein::normalized_weighted_distance(test, no_suffix2) == Approx(0.75).epsilon(0.01) );
        REQUIRE( levenshtein::normalized_weighted_distance(test, replace_all) == 0.0 );
    }
};