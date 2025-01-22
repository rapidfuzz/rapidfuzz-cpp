#if CATCH2_VERSION == 2
#    include <catch2/catch.hpp>
#else
#    include <catch2/catch_test_macros.hpp>
#    include <catch2/matchers/catch_matchers_floating_point.hpp>
#endif

#include <rapidfuzz/details/types.hpp>
#include <rapidfuzz/distance/OSA.hpp>
#include <string>

#include "../common.hpp"

template <typename Sentence1, typename Sentence2>
size_t osa_distance(const Sentence1& s1, const Sentence2& s2, size_t max = std::numeric_limits<size_t>::max())
{
    size_t res1 = rapidfuzz::osa_distance(s1, s2, max);
    size_t res2 = rapidfuzz::osa_distance(s1.begin(), s1.end(), s2.begin(), s2.end(), max);
    size_t res3 = rapidfuzz::osa_distance(make_bidir(s1.begin()), make_bidir(s1.end()),
                                          make_bidir(s2.begin()), make_bidir(s2.end()), max);
    rapidfuzz::CachedOSA<rapidfuzz::char_type<Sentence1>> scorer(s1);
    size_t res4 = scorer.distance(s2, max);
    size_t res5 = scorer.distance(s2.begin(), s2.end(), max);
#ifdef RAPIDFUZZ_SIMD
    if (s1.size() <= 64) {
        std::vector<size_t> results(256 / 8);

        if (s1.size() <= 8) {
            rapidfuzz::experimental::MultiOSA<8> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.distance(&results[0], results.size(), s2, max);
            REQUIRE(res1 == results[0]);
        }
        if (s1.size() <= 16) {
            rapidfuzz::experimental::MultiOSA<16> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.distance(&results[0], results.size(), s2, max);
            REQUIRE(res1 == results[0]);
        }
        if (s1.size() <= 32) {
            rapidfuzz::experimental::MultiOSA<32> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.distance(&results[0], results.size(), s2, max);
            REQUIRE(res1 == results[0]);
        }
        if (s1.size() <= 64) {
            rapidfuzz::experimental::MultiOSA<64> simd_scorer(1);
            simd_scorer.insert(s1);
            simd_scorer.distance(&results[0], results.size(), s2, max);
            REQUIRE(res1 == results[0]);
        }
    }
#endif
    REQUIRE(res1 == res2);
    REQUIRE(res1 == res3);
    REQUIRE(res1 == res4);
    REQUIRE(res1 == res5);
    return res1;
}

/* test some very simple cases of the osa distance */
TEST_CASE("osa[simple]")
{
    {
        std::string s1 = "";
        std::string s2 = "";
        REQUIRE(osa_distance(s1, s2) == 0);
    }

    {
        std::string s1 = "aaaa";
        std::string s2 = "";
        REQUIRE(osa_distance(s1, s2) == 4);
        REQUIRE(osa_distance(s2, s1) == 4);
        REQUIRE(osa_distance(s1, s2, 1) == 2);
        REQUIRE(osa_distance(s2, s1, 1) == 2);
    }

    {
        std::string s1 = "CA";
        std::string s2 = "ABC";
        REQUIRE(osa_distance(s1, s2) == 3);
    }

    {
        std::string s1 = "CA";
        std::string s2 = "AC";
        REQUIRE(osa_distance(s1, s2) == 1);
    }

    {
        std::string filler = str_multiply(std::string("a"), 64);
        std::string s1 = std::string("a") + filler + "CA" + filler + std::string("a");
        std::string s2 = std::string("b") + filler + "AC" + filler + std::string("b");
        REQUIRE(osa_distance(s1, s2) == 3);
    }
}
