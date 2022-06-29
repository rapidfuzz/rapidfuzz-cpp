#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <rapidfuzz/fuzz.hpp>

namespace fuzz = rapidfuzz::fuzz;
using Catch::Approx;

using MetricPtr = double (*)(const char*, const char*, double);
struct Metric {
    MetricPtr call;
    const char* name;
    bool symmetric;
};

#define LIST_OF_METRICS(FUNC)                                                                                \
    /*   func                          symmetric */                                                          \
    FUNC(fuzz::ratio, true)                                                                                  \
    FUNC(fuzz::partial_ratio, false)                                                                         \
    FUNC(fuzz::token_set_ratio, true)                                                                        \
    FUNC(fuzz::token_sort_ratio, true)                                                                       \
    FUNC(fuzz::token_ratio, true)                                                                            \
    FUNC(fuzz::partial_token_set_ratio, false)                                                               \
    FUNC(fuzz::partial_token_sort_ratio, false)                                                              \
    FUNC(fuzz::partial_token_ratio, false)                                                                   \
    FUNC(fuzz::WRatio, false)                                                                                \
    FUNC(fuzz::QRatio, true)

#define CREATE_METRIC(func, symmetric)                                                                       \
    Metric{[](const char* s1, const char* s2, double score_cutoff) { return func(s1, s2, score_cutoff); },   \
           #func, symmetric},

std::vector<Metric> metrics = {LIST_OF_METRICS(CREATE_METRIC)};

/**
 * @name RatioTest
 *
 * @todo Enable 'testPartialTokenSortRatio' once the function is implemented
 */
TEST_CASE("RatioTest")
{
    const std::string s1 = "new york mets";
    const std::string s2 = "new YORK mets";
    const std::string s3 = "the wonderful new york mets";
    const std::string s4 = "new york mets vs atlanta braves";
    const std::string s5 = "atlanta braves vs new york mets";
    const std::string s6 = "new york mets - atlanta braves";
    const std::string s7 = "new york city mets - atlanta braves";
    // test silly corner cases
    const std::string s8 = "{";
    const std::string s8a = "{";
    const std::string s9 = "{a";
    const std::string s9a = "{a";
    const std::string s10 = "a{";
    const std::string s10a = "{b";

    SECTION("testEqual")
    {
        REQUIRE(100 == fuzz::ratio(s1, s1));
        REQUIRE(100 == fuzz::ratio("test", "test"));
        REQUIRE(100 == fuzz::ratio(s8, s8a));
        REQUIRE(100 == fuzz::ratio(s9, s9a));
    }

    SECTION("testPartialRatio")
    {
        REQUIRE(100 == fuzz::partial_ratio(s1, s1));
        REQUIRE(100 != fuzz::ratio(s1, s3));
        REQUIRE(100 == fuzz::partial_ratio(s1, s3));
    }

    SECTION("testTokenSortRatio")
    {
        REQUIRE(100 == fuzz::token_sort_ratio(s1, s1));
        const std::string s92 = "metss new york hello";
        const std::string s0 = "metss new york hello";
        REQUIRE(fuzz::token_sort_ratio(s92, s0) > 90);
    }

    SECTION("testTokenSetRatio")
    {
        REQUIRE(100 == fuzz::token_set_ratio(s4, s5));
        REQUIRE(100 == fuzz::token_set_ratio(s8, s8a, false));
        REQUIRE(100 == fuzz::token_set_ratio(s9, s9a, true));
        REQUIRE(100 == fuzz::token_set_ratio(s9, s9a, false));
        REQUIRE(50 == fuzz::token_set_ratio(s10, s10a, false));
    }

    SECTION("testPartialTokenSetRatio")
    {
        REQUIRE(100 == fuzz::partial_token_set_ratio(s4, s7));
    }

    SECTION("testWRatioEqual")
    {
        REQUIRE(100 == fuzz::WRatio(s1, s1));
    }

    SECTION("testWRatioPartialMatch")
    {
        // a partial match is scaled by .9
        REQUIRE(90 == fuzz::WRatio(s1, s3));
    }

    SECTION("testWRatioMisorderedMatch")
    {
        // misordered full matches are scaled by .95
        REQUIRE(95 == fuzz::WRatio(s4, s5));
    }

    SECTION("testTwoEmptyStrings")
    {
        REQUIRE(100 == fuzz::ratio("", ""));
        REQUIRE(100 == fuzz::partial_ratio("", ""));
        REQUIRE(100 == fuzz::token_sort_ratio("", ""));
        REQUIRE(0 == fuzz::token_set_ratio("", ""));
        REQUIRE(100 == fuzz::partial_token_sort_ratio("", ""));
        REQUIRE(0 == fuzz::partial_token_set_ratio("", ""));
        REQUIRE(100 == fuzz::token_ratio("", ""));
        REQUIRE(100 == fuzz::partial_token_ratio("", ""));
        REQUIRE(0 == fuzz::WRatio("", ""));
        REQUIRE(0 == fuzz::QRatio("", ""));
    }

    SECTION("testFirstStringEmpty")
    {
        for (auto& metric : metrics) {
            INFO("Score not 0 for " << metric.name);
            REQUIRE(0 == metric.call("test", "", 0));
        }
    }

    SECTION("testSecondStringEmpty")
    {
        for (auto& metric : metrics) {
            INFO("Score not 0 for " << metric.name);
            REQUIRE(0 == metric.call("", "test", 0));
        }
    }

    SECTION("testPartialRatioShortNeedle")
    {
        REQUIRE(Approx(33.3333333) == fuzz::partial_ratio("001", "220222"));
    }

    SECTION("testIssue206") /* test for https://github.com/maxbachmann/RapidFuzz/issues/206 */
    {
        const char* str1 = "South Korea";
        const char* str2 = "North Korea";

        for (auto& metric : metrics) {
            double score = metric.call(str1, str2, 0);
            INFO("score_cutoff does not work correctly for " << metric.name);
            REQUIRE(0 == metric.call(str1, str2, score + 0.0001));
            REQUIRE(score == metric.call(str1, str2, score));
        }
    }

    SECTION("testIssue210") /* test for https://github.com/maxbachmann/RapidFuzz/issues/210 */
    {
        const char* str1 = "bc";
        const char* str2 = "bca";

        for (auto& metric : metrics) {
            double score = metric.call(str1, str2, 0);
            INFO("score_cutoff does not work correctly for " << metric.name);
            REQUIRE(0 == metric.call(str1, str2, score + 0.0001));
            REQUIRE(score == metric.call(str1, str2, score));
        }
    }

    SECTION("testIssue231") /* test for https://github.com/maxbachmann/RapidFuzz/issues/231 */
    {
        const char str1[] = "er merkantilismus f/rderte handel und verkehr mit teils marktkonformen, teils "
                            "dirigistischen ma_nahmen.";
        const char str2[] = "ils marktkonformen, teils dirigistischen ma_nahmen. an der schwelle zum 19. "
                            "jahrhundert entstand ein neu";

        auto alignment = fuzz::partial_ratio_alignment(str1, str2);
        REQUIRE(alignment.src_start == 0);
        REQUIRE(alignment.src_end == 103);
        REQUIRE(alignment.dest_start == 0);
        REQUIRE(alignment.dest_end == 103);
    }
}
