#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <rapidfuzz/fuzz.hpp>

#include "common.hpp"

namespace fuzz = rapidfuzz::fuzz;

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

void score_test(double expected, double input)
{
    REQUIRE(input <= 100);
    REQUIRE(input >= 0);
    REQUIRE_THAT(input, Catch::Matchers::WithinAbs(expected, 0.000001));
}

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
    const std::string s9 = "{a";
    const std::string s10 = "a{";
    const std::string s10a = "{b";

    SECTION("testEqual")
    {
        score_test(100, fuzz::ratio(s1, s1));
        score_test(100, fuzz::ratio("test", "test"));
        score_test(100, fuzz::ratio(s8, s8));
        score_test(100, fuzz::ratio(s9, s9));
    }

    SECTION("testPartialRatio")
    {
        score_test(100, fuzz::partial_ratio(s1, s1));
        score_test(65, fuzz::ratio(s1, s3));
        score_test(100, fuzz::partial_ratio(s1, s3));
    }

    SECTION("testTokenSortRatio")
    {
        score_test(100, fuzz::token_sort_ratio(s1, s1));
        score_test(100, fuzz::token_sort_ratio("metss new york hello", "metss new york hello"));
    }

    SECTION("testTokenSetRatio")
    {
        score_test(100, fuzz::token_set_ratio(s4, s5));
        score_test(100, fuzz::token_set_ratio(s8, s8));
        score_test(100, fuzz::token_set_ratio(s9, s9));
        score_test(50, fuzz::token_set_ratio(s10, s10a));
    }

    SECTION("testPartialTokenSetRatio")
    {
        score_test(100, fuzz::partial_token_set_ratio(s4, s7));
    }

    SECTION("testWRatioEqual")
    {
        score_test(100, fuzz::WRatio(s1, s1));
    }

    SECTION("testWRatioPartialMatch")
    {
        // a partial match is scaled by .9
        score_test(90, fuzz::WRatio(s1, s3));
    }

    SECTION("testWRatioMisorderedMatch")
    {
        // misordered full matches are scaled by .95
        score_test(95, fuzz::WRatio(s4, s5));
    }

    SECTION("testTwoEmptyStrings")
    {
        score_test(100, fuzz::ratio("", ""));
        score_test(100, fuzz::partial_ratio("", ""));
        score_test(100, fuzz::token_sort_ratio("", ""));
        score_test(0, fuzz::token_set_ratio("", ""));
        score_test(100, fuzz::partial_token_sort_ratio("", ""));
        score_test(0, fuzz::partial_token_set_ratio("", ""));
        score_test(100, fuzz::token_ratio("", ""));
        score_test(100, fuzz::partial_token_ratio("", ""));
        score_test(0, fuzz::WRatio("", ""));
        score_test(0, fuzz::QRatio("", ""));
    }

    SECTION("testFirstStringEmpty")
    {
        for (auto& metric : metrics) {
            INFO("Score not 0 for " << metric.name);
            score_test(0, metric.call("test", "", 0));
        }
    }

    SECTION("testSecondStringEmpty")
    {
        for (auto& metric : metrics) {
            INFO("Score not 0 for " << metric.name);
            score_test(0, metric.call("", "test", 0));
        }
    }

    SECTION("testPartialRatioShortNeedle")
    {
        score_test(33.3333333, fuzz::partial_ratio("001", "220222"));
        score_test(100, fuzz::partial_ratio("physics 2 vid", "study physics physics 2 video"));
    }

    SECTION("testIssue206") /* test for https://github.com/rapidfuzz/RapidFuzz/issues/206 */
    {
        const char* str1 = "South Korea";
        const char* str2 = "North Korea";

        for (auto& metric : metrics) {
            double score = metric.call(str1, str2, 0);
            INFO("score_cutoff does not work correctly for " << metric.name);
            score_test(0, metric.call(str1, str2, score + 0.0001));
            score_test(score, metric.call(str1, str2, score - 0.0001));
        }
    }

    SECTION("testIssue210") /* test for https://github.com/rapidfuzz/RapidFuzz/issues/210 */
    {
        const char* str1 = "bc";
        const char* str2 = "bca";

        for (auto& metric : metrics) {
            double score = metric.call(str1, str2, 0);
            INFO("score_cutoff does not work correctly for " << metric.name);
            score_test(0, metric.call(str1, str2, score + 0.0001));
            score_test(score, metric.call(str1, str2, score - 0.0001));
        }
    }

    SECTION("testIssue231") /* test for https://github.com/rapidfuzz/RapidFuzz/issues/231 */
    {
        const char* str1 = "er merkantilismus f/rderte handel und verkehr mit teils marktkonformen, teils "
                           "dirigistischen ma_nahmen.";
        const char* str2 = "ils marktkonformen, teils dirigistischen ma_nahmen. an der schwelle zum 19. "
                           "jahrhundert entstand ein neu";

        auto alignment = fuzz::partial_ratio_alignment(str1, str2);
        score_test(66.2337662, alignment.score);
        REQUIRE(alignment.src_start == 0);
        REQUIRE(alignment.src_end == 103);
        REQUIRE(alignment.dest_start == 0);
        REQUIRE(alignment.dest_end == 51);
    }

    SECTION("testIssue257") /* test for https://github.com/rapidfuzz/RapidFuzz/issues/257 */
    {
        const char* str1 = "aaaaaaaaaaaaaaaaaaaaaaaabacaaaaaaaabaaabaaaaaaaababbbbbbbbbbabbcb";
        const char* str2 = "aaaaaaaaaaaaaaaaaaaaaaaababaaaaaaaabaaabaaaaaaaababbbbbbbbbbabbcb";

        score_test(98.4615385, fuzz::partial_ratio(str1, str2));
        score_test(98.4615385, fuzz::partial_ratio(str2, str1));
    }

    SECTION("testIssue257") /* test for https://github.com/rapidfuzz/RapidFuzz/issues/219 */
    {
        const char* str1 =
            "TTAGCGCTACCGGTCGCCACCATGGTTTTCTAAGGGGAGGCCGTCATCAAAAGAGTTCATGTAGCACGAAGTCCACCTTTGAAGGATCGATGAATG"
            "GCCATGAATTCGAAATCGAGGGGAGGGCGAGAGAGGGCCGGCCTTACGAGGGCACACCCAAACTGCCAAACTGAAAGTGACCAAAGGCGGCCCGTT"
            "ACCATTCTCCTGGGACATACTGTAAGTGCATGGCACCACGCTCTATTTCTTAAAAAAAGTGTAGGGTCTGGCGCCCTCGGGGGCGGCTTAGGAAAA"
            "GAGGCCTGACCAATTTTTGTCTCTTATAGGTCACCACAGTTCATGTACGGAAGCAGAGCGTTCACGAAGCACCCAGCTGACATCCCGGACTACTAT"
            "GACAGAGCTTCCCGGAAGGACTCAAGTGGGAGCGGGTCATGAACTTCGAGGACGGTGGGGCAGTGACTGTGACACAGGACACCAGCCTGAAGATGG"
            "AACTCTTATCTACAAAGTAAAGCTAAGAGGAACCAACTTCCCGCCAGATGGGCCCGTTATGCAAAAGAAAACGATGGGGTGGGAAGCTTCTGCAGA"
            "GCGCCTTTACCCCGAGGATGGCGTCCTTAAGGGGGATATCAAAATGGCGCTACGCCTTAAGGATGGAGGCAGATATTTGGCAGACTTCAAAACAAC"
            "ATTACAAGGCGAAGAAGCCAGTCCAGATGCCTGGAGCTTGCAATGGTAAGCACCTCTGCCTGCCCCGCTAGTTGGGTGTGAGTGGCCCAGGCAGCC"
            "GCCTGCATTTAGCTCTAGCCGGGGTACGGGTGCCCCTTGATGCCTGAGGCCTCTCCTGTGGCTGAGGCGACTGGCCCAGAGTCTGGGTCTCCTCGA"
            "GGGTGGCCATCTGGCGTCACCTGTCATCTGCCACCTCTGACCCCTGCCTCTCTCCTCACAGTTGACCGGAAGCTCGACATAACGAGTCACAACGAG"
            "GACTACACAGTTGTCGAGCAGTACGAACGTTCCGAGGGTCGACACTCAACTGGCAGGATGGATGAGCTTTTACAAAGGGCGGGGGCGGAGGAAGCG"
            "GAGGAGGAGGAAGTGGTGGAGGAGGCTCGAAAGGTAAGTATCAGGGTTGCAGCGTTTCTCTGACCTCATATTCCAATGGATGTGTGAGAAGCATAG"
            "TGAGATCCGTTTACCCCTTTTGCTCAATTCTCACGTGGCTGTAGTCGTGTTTATAAGTCTGATCGTAATGGCAGCTTGGTCTGCGTGCCTTGAAAT"
            "TGTGGCCCCCACATGCATAATAAACGATCCTCTAGCACTACTTTCTGTCGAGCCACCTCAGCGCCCGTACAGTAATGTCTACAGCGCGTCTAACCC"
            "GACAAATGCGTTTCTTTCTCTCCTAGAACGAAAGATTACGGATCACAGAAACGTCTCGGAAAGTCCAAATAGAAAGAACGAGAAAGAAGAAAGTGA"
            "AGGATCACAAGAGCAACTCGAAAGAAAGAGACATAAGAAGGAACTCAGAAAAGGATGACAAGTATAAAAACAAAGTGAAGAAAAGAGCGAAGAGCA"
            "GAGTAGAAGCAAGAGTAAAGAGAAGAAGAGCAAATCGAAGGAAAGGTAAGTGGCTTTCAAGAACATTGGTAAAACGTCATGTGTATTGCGGTTCCA"
            "TGCTTACACAAATTCGTTCGCTTGTTTTCAGGGACTCGAAACACAACAGAAACGAAGAGAAGAGAATGAGAAGCAGAAGCAAAGGAAGAGACCATG"
            "AAAATGTCAAGGAAAAAGAAAAAACAGTCCGATAGCAAAGGCAAAGACCAGGAGCGGTCTCGGTCGAAGGAAAAATCTAAACAACTTGAATCAAAA"
            "TCTAACGAGCATGGTAAGTTCGCGAGACACTAAGTTGATTCTTAGTGTTTAGACGTGAAACTCCCTTGGAAGGTTTAACGAATACTGTTAATATTT"
            "TCAGATCACTCAAAATCCAAAAGAACCGACGGGCACAATCCCGGAGCCGTGAATGTGATATAACCAAGGAAGCACAGTTGCAATTCGAGAACAAGA"
            "GAAAGAAGCAGAAGTAGAGAGATCGCTCGAGAAGAGTGAGAAGCAGAACACATGATAGAGACAGAAGCCGGTCGAAAGAATACCACCGCTACAGAG"
            "AACAAGGTAAGCATGACTACTTGAGTGTAAATACGTTGTGATAGAGATGAAAAACAAAACCGAACATTACTTTGGGTAATAATTAACTTTTTTTTA"
            "ATAGAATATCGGGAGAAAGGAAGGTCGAGAAGCAGAGAAAGAAGGACGCCTCAGGAAGAAGCCGTTCGAAAGACAGAAGGAGAAGGAGAAGAGATT"
            "CGAAAGTTCAGAGCGTGAAGAGTCTCAATCGCGTAATAAAGACAAGTACGGGAACCAAGAAAGTAAAAGTTCCCACAGGAAGAACTCTGAAGAGCG"
            "AGAAAAGTAAAAAAGGGTTTCCTGTTTTTTGCCTATTTTGGGTAAAGGGGTTGATGGAGAAACAGGTGTGTGGACTGCTGAGGAGTGAGTTAGAAT"
            "AAATGGTGGTATCACTTCTTCAATGCTACTACAATGGAACAACAGTCGTTACCTGTTTTAAGTTCGTGGCGTCTTATGCTCCGGACAGGGACAGAT"
            "AGGCGGTTGACAGAGAGTTAAGATCTAGTACACTGGGTTTCCTAAATGTAAGAATTGGCCCGAATCCGGCCTAATATGCGAACTTTGTGCTACCAA"
            "GCGAGCGGGAAGCTAAGGGTGGGGAATTGCGGGTTTAATGGACCATCTCATGAGTCTAGCAGTTAATGTATCCTATCTTCCAAACAGGAATGTATT"
            "CGAAAGAGTAGAGACCATAATTCGTCTAACAACTCAAGGAAAAGAAGGCGGAGTAGAGCCGATTCCGAACCCTTTGCTAGGACTAGATAGCACGTG"
            "AACCTAGACTGTCTCTGAGACTGCGCCATTACGTCTCGATCAGTAACGATTGCATCGCGAGGCTGTGGATGTAAAACCTCTGCTGACCTTGACTGA"
            "CTGAGATACAATGCCTTCAGCAATGCGTGGCAG";
        const char* str2 =
            "GTAAGGGTTTCCTGTTTTTTGCCTATTTTGGGTAAAGGGGGGTTGATGGAGAAACAGGTGTGTGGACTGCTGAGGAGTGAGTTAGAATAAATGGTG"
            "GTATCACTTCTTCAATGCTACAATGGAACAACAGTCGTTACCTGTTTTAAGTTCGTGGCGTCTTATGCTCCGGACAGGGACAGATAGGCGGTTAGA"
            "CAGAGAGTTAAGATCTAGTACACTGGGTTTCCTAAATGTAAAAATTGGCCCGAATCCGGCCTAATATGCGAACTTTGTGCTACCAAGCGAGCGGGA"
            "AGCTAAGGGTGGGGAGTGCGGGTTTAATGGACCATCTCGCAGGTCTAGCAGTTAATGTATCCTATCTTCCAAACAG";

        score_test(97.5274725, fuzz::partial_ratio(str1, str2));
        score_test(97.5274725, fuzz::partial_ratio(str2, str1));
        score_test(97.5274725, fuzz::partial_ratio(str1, str2, 97.5));
        score_test(97.5274725, fuzz::partial_ratio(str2, str1, 97.5));
    }
}
