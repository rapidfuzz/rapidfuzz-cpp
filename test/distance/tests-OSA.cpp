#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidfuzz/details/types.hpp>
#include <rapidfuzz/distance/OSA.hpp>
#include <string>

template <typename T>
std::basic_string<T> str_multiply(std::basic_string<T> a, unsigned int b)
{
    std::basic_string<T> output;
    while (b--)
        output += a;

    return output;
}

template <typename Sentence1, typename Sentence2>
int64_t osa_distance(const Sentence1& s1, const Sentence2& s2,
                     int64_t max = std::numeric_limits<int64_t>::max())
{
    int64_t res1 = rapidfuzz::osa_distance(s1, s2, max);
    int64_t res2 = rapidfuzz::osa_distance(s1.begin(), s1.end(), s2.begin(), s2.end(), max);
    rapidfuzz::CachedOSA scorer(s1);
    int64_t res3 = scorer.distance(s2, max);
    int64_t res4 = scorer.distance(s2.begin(), s2.end(), max);
    REQUIRE(res1 == res2);
    REQUIRE(res1 == res3);
    REQUIRE(res1 == res4);
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
