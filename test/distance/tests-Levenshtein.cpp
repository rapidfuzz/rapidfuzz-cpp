#include "rapidfuzz/details/Range.hpp"
#include "rapidfuzz/details/types.hpp"
#include "rapidfuzz/distance/Levenshtein.hpp"
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidfuzz/details/types.hpp>
#include <string>

#include "examples/osa.hpp"
#include <rapidfuzz/distance.hpp>

template <typename T>
std::basic_string<T> str_multiply(std::basic_string<T> a, unsigned int b)
{
    std::basic_string<T> output;
    while (b--)
        output += a;

    return output;
}

template <typename Sentence1, typename Sentence2>
int64_t levenshtein_distance(const Sentence1& s1, const Sentence2& s2,
                             rapidfuzz::LevenshteinWeightTable weights = {1, 1, 1},
                             int64_t max = std::numeric_limits<int64_t>::max())
{
    int64_t res1 = rapidfuzz::levenshtein_distance(s1, s2, weights, max);
    int64_t res2 = rapidfuzz::levenshtein_distance(s1.begin(), s1.end(), s2.begin(), s2.end(), weights, max);
    rapidfuzz::CachedLevenshtein<typename Sentence1::value_type> scorer(s1, weights);
    int64_t res3 = scorer.distance(s2, max);
    int64_t res4 = scorer.distance(s2.begin(), s2.end(), max);
    REQUIRE(res1 == res2);
    REQUIRE(res1 == res3);
    REQUIRE(res1 == res4);
    return res1;
}

template <typename T>
std::basic_string<T> get_subsequence(const std::basic_string<T>& s, ptrdiff_t pos, ptrdiff_t len)
{
    return std::basic_string<T>(std::begin(s) + pos, std::begin(s) + pos + len);
}

template <typename Sentence1, typename Sentence2>
double levenshtein_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                         rapidfuzz::LevenshteinWeightTable weights = {1, 1, 1},
                                         double score_cutoff = 0.0)
{
    double res1 = rapidfuzz::levenshtein_normalized_similarity(s1, s2, weights, score_cutoff);
    double res2 = rapidfuzz::levenshtein_normalized_similarity(s1.begin(), s1.end(), s2.begin(), s2.end(),
                                                               weights, score_cutoff);
    rapidfuzz::CachedLevenshtein<typename Sentence1::value_type> scorer(s1, weights);
    double res3 = scorer.normalized_similarity(s2, score_cutoff);
    double res4 = scorer.normalized_similarity(s2.begin(), s2.end(), score_cutoff);
    REQUIRE(res1 == Catch::Approx(res2).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res3).epsilon(0.0001));
    REQUIRE(res1 == Catch::Approx(res4).epsilon(0.0001));
    return res1;
}

TEST_CASE("Levenshtein")
{
    std::string test = "aaaa";
    std::wstring no_suffix = L"aaa";
    std::string no_suffix2 = "aaab";
    std::string swapped1 = "abaa";
    std::string swapped2 = "baaa";
    std::string replace_all = "bbbb";

    SECTION("levenshtein calculates correct distances")
    {
        REQUIRE(levenshtein_distance(test, test) == 0);
        REQUIRE(levenshtein_distance(test, no_suffix) == 1);
        REQUIRE(levenshtein_distance(swapped1, swapped2) == 2);
        REQUIRE(levenshtein_distance(test, no_suffix2) == 1);
        REQUIRE(levenshtein_distance(test, replace_all) == 4);
    }

    SECTION("weighted levenshtein calculates correct distances")
    {
        REQUIRE(levenshtein_distance(test, test, {1, 1, 2}) == 0);
        REQUIRE(levenshtein_distance(test, no_suffix, {1, 1, 2}) == 1);
        REQUIRE(levenshtein_distance(swapped1, swapped2, {1, 1, 2}) == 2);
        REQUIRE(levenshtein_distance(test, no_suffix2, {1, 1, 2}) == 2);
        REQUIRE(levenshtein_distance(test, replace_all, {1, 1, 2}) == 8);
    }

    SECTION("weighted levenshtein calculates correct ratios")
    {
        REQUIRE(levenshtein_normalized_similarity(test, test, {1, 1, 2}) == 1.0);
        REQUIRE(levenshtein_normalized_similarity(test, no_suffix, {1, 1, 2}) ==
                Catch::Approx(0.8571).epsilon(0.0001));
        REQUIRE(levenshtein_normalized_similarity(swapped1, swapped2, {1, 1, 2}) ==
                Catch::Approx(0.75).epsilon(0.0001));
        REQUIRE(levenshtein_normalized_similarity(test, no_suffix2, {1, 1, 2}) ==
                Catch::Approx(0.75).epsilon(0.0001));
        REQUIRE(levenshtein_normalized_similarity(test, replace_all, {1, 1, 2}) == 0.0);
    }

    SECTION("test mbleven implementation")
    {
        std::string a = "South Korea";
        std::string b = "North Korea";
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 4) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 3) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 2) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 1) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 0) == 1);

        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}) == 4);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 4) == 4);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 3) == 4);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 2) == 3);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 1) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 0) == 1);

        a = "aabc";
        b = "cccd";
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}) == 4);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 4) == 4);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 3) == 4);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 2) == 3);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 1) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 1}, 0) == 1);

        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}) == 6);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 6) == 6);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 5) == 6);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 4) == 5);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 3) == 4);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 2) == 3);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 1) == 2);
        REQUIRE(levenshtein_distance(a, b, {1, 1, 2}, 0) == 1);
    }

    SECTION("test banded implementation")
    {
        {
            std::string s1 = "kkkkbbbbfkkkkkkibfkkkafakkfekgkkkkkkkkkkbdbbddddddddddafkkkekkkhkk";
            std::string s2 = "khddddddddkkkkdgkdikkccccckcckkkekkkkdddddddddddafkkhckkkkkdckkkcc";
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}) == 36);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 31) == 32);
        }

        {
            std::string s1 = "ccddcddddddddddddddddddddddddddddddddddddddddddddddddddddaaaaaaaaaaa";
            std::string s2 = "aaaaaaaaaaaaaadddddddddbddddddddddddddddddddddddddddddddddbddddddddd";
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}) == 26);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 31) == 26);
        }

        {
            std::string s1 = "accccccccccaaaaaaaccccccccccccccccccccccccccccccacccccccccccccccccccccccccccccc"
                             "ccccccccccccccccccccaaaaaaaaaaaaacccccccccccccccccccccc";
            std::string s2 = "ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
                             "ccccccccccccccccccccccccccccccccccccbcccb";
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}) == 24);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 25) == 24);
        }

        {
            std::string s1 = "miiiiiiiiiiliiiiiiibghiiaaaaaaaaaaaaaaacccfccccedddaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                             "aaaaaaaaaaaaa";
            std::string s2 =
                "aaaaaaajaaaaaaaabghiiaaaaaaaaaaaaaaacccfccccedddaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaajjdim";
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}) == 27);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 27) == 27);
        }

        {
            std::string s1 = "lllllfllllllllllllllllllllllllllllllllllllllllllllllllglllllilldcaaaaaaaaaaaaaa"
                             "aaaaadbbllllllllllhllllllllllllllllllllllllllgl";
            std::string s2 = "aaaaaaaaaaaaaadbbllllllllllllllelllllllllllllllllllllllllllllllglllllilldcaaaaa"
                             "aaaaaaaaaaaaaadbbllllllllllllllellllllllllllllhlllllllllill";
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}) == 23);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 27) == 23);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 28) == 23);
        }

        {
            std::string s1 = "llccacaaaaaaaaaccccccccccccccccddffaccccaccecccggggclallhcccccljif";
            std::string s2 = "bddcbllllllbcccccccccccccccccddffccccccccebcccggggclbllhcccccljifbddcccccc";
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}) == 27);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 27) == 27);
            REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}, 28) == 27);
        }
    }
}

TEST_CASE("Levenshtein_editops")
{
    std::string s = "Lorem ipsum.";
    std::string d = "XYZLorem ABC iPsum";

    rapidfuzz::Editops ops = rapidfuzz::levenshtein_editops(s, d);
    REQUIRE(d == rapidfuzz::editops_apply<char>(ops, s, d));
    REQUIRE(ops.get_src_len() == s.size());
    REQUIRE(ops.get_dest_len() == d.size());
}

TEST_CASE("Levenshtein_find_hirschberg_pos")
{
    {
        std::string s1 = str_multiply(std::string("abb"), 2);
        std::string s2 = str_multiply(std::string("ccccca"), 2);

        auto hpos = rapidfuzz::detail::find_hirschberg_pos(rapidfuzz::detail::make_range(s1),
                                                           rapidfuzz::detail::make_range(s2));
        REQUIRE(hpos.left_score == 5);
        REQUIRE(hpos.right_score == 6);
        REQUIRE(hpos.s2_mid == 6);
        REQUIRE(hpos.s1_mid == 1);
    }

    {
        std::string s1 = str_multiply(std::string("abb"), 8 * 64);
        std::string s2 = str_multiply(std::string("ccccca"), 8 * 64);

        auto hpos = rapidfuzz::detail::find_hirschberg_pos(rapidfuzz::detail::make_range(s1),
                                                           rapidfuzz::detail::make_range(s2));
        REQUIRE(hpos.left_score == 1280);
        REQUIRE(hpos.right_score == 1281);
        REQUIRE(hpos.s2_mid == 1536);
        REQUIRE(hpos.s1_mid == 766);
    }
}

TEST_CASE("Levenshtein_blockwise")
{
    {
        std::string s1 = str_multiply(std::string("a"), 128);
        std::string s2 = str_multiply(std::string("b"), 128);
        REQUIRE(levenshtein_distance(s1, s2, {1, 1, 1}) == 128);
    }
}

TEST_CASE("Levenshtein_editops[fuzzing_regressions]")
{
    /* Test regressions of bugs found through fuzzing */
    {
        std::string s1 = "b";
        std::string s2 = "aaaaaaaaaaaaaaaabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
        rapidfuzz::Editops ops = rapidfuzz::levenshtein_editops(s1, s2);
        REQUIRE(s2 == rapidfuzz::editops_apply<char>(ops, s1, s2));
    }

    {
        std::string s1 = "aa";
        std::string s2 = "abb";
        rapidfuzz::Editops ops = rapidfuzz::levenshtein_editops(s1, s2);
        REQUIRE(s2 == rapidfuzz::editops_apply<char>(ops, s1, s2));
    }

    {
        std::string s1 = str_multiply(std::string("abb"), 8 * 64);
        std::string s2 = str_multiply(std::string("ccccca"), 8 * 64);
        rapidfuzz::Editops ops = rapidfuzz::levenshtein_editops(s1, s2);
        REQUIRE(s2 == rapidfuzz::editops_apply<char>(ops, s1, s2));
    }
}

TEST_CASE("Levenshtein small band")
{
    {
        std::string s1 =
            "kevZLemllleyOT1UNTKWeSOYYRKWKWeBGNWKXHK05RQKWZTMeHK2UMKTie3YKRRYKe3OINeJOKcc1OKJKWeKWNKHROINkevZ"
            "LemllleyOT1UNTKWeSOYYRKWKWeBGNWKXHK05RQKWZTMeHK2UMKTie3YKRRYKe3OINeJOKccFGNReJKWeyNK3INROK4KTJKT"
            "emumqdmumteGZLemqirniemumqdmunleGZLemuitleMKMKTemuinleOTeJKTccFGNReJKWeyNK3INROK4KTJKTemumqdmumt"
            "eGZLemqirniemumqdmunleGZLemuitleMKMKTemuinleOTeJKTccJWKOeRKY2YKTezWOKJKTXPGNWKTkexZWINeWOINYKWRO"
            "INKTeEVWZINe1ZWJKTemumqdmlmtersoeyNKTeMKjccJWKOeRKY2YKTezWOKJKTXPGNWKTkexZWINeWOINYKWROINKTeEVWZ"
            "INe1ZWJKTemumqdmlmtersoeyNKTeMKjcc3INOKJKTieJkeOkesinleGZLePKemllieOTeJKWeMRKOINKTeFKOYeTKZeKOTM"
            "KMGTMKTKeyNKTiemumueGRRKOTcc3INOKJKTieJkeOkesinleGZLePKemllieOTeJKWeMRKOINKTeFKOYeTKZeKOTMKMGTMK"
            "TKeyNKTiemumueGRRKOTccoqueaetinfeMKMKTesinfegmummdmumohkccoqueaetinfeMKMKTesinfegmummdmumohkccyO"
            "TKTeWKINYeKWNKHROINKTeD6IQMGTMeKWLZNWeJOKeFGNReJKWeAKHUWKTKTkewKYWZMe3OKccyOTKTeWKINYeKWNKHROINK"
            "TeD6IQMGTMeKWLZNWeJOKeFGNReJKWeAKHUWKTKTkewKYWZMe3OKccmumqeTUINeqsoueZTJeHROKHe3OKe3USOYeTZWeZSe"
            "mspeNOTYKWeJKWemumoeMKSKRJKYKTeFGNReJKWeAKjccmumqeTUINeqsoueZTJeHROKHe3OKe3USOYeTZWeZSemspeNOTYK"
            "WeJKWemumoeMKSKRJKYKTeFGNReJKWeAKjccHUWKTKTe2ZW6IQie3Ue3GTQe3OKegmumqheGZLeprsqiegmumrheGZLeosnm"
            "eZTJegmumsheMGWeGZLeomuoieZSeJGTTccHUWKTKTe2ZW6IQie3Ue3GTQe3OKegmumqheGZLeprsqiegmumrheGZLeosnme"
            "ZTJegmumsheMGWeGZLeomuoieZSeJGTTccxOKeCKYNUJKe2ZWeyWWKINTZTMeJOK3KWeFOLLKWeZTJeJOKeFO33KWTe3KRH3"
            "YeLOTJKTie3OINeOTeJKWeOSeCGOjccxOKeCKYNUJKe2ZWeyWWKINTZTMeJOK3KWeFOLLKWeZTJeJOKeFO33KWTe3KRH3YeL"
            "OTJKTie3OINeOTeJKWeOSeCGOjccNKLYemunmeJKW";
        std::string s2 =
            "ievZLemllleyOT1UNTKWeSOYYRKWKWeBGNWKXHK05RQKWZTMeHK2UMKTie3YKRRYKe3OINeJOKcc1OKJKWeKWNKHROINkev"
            "LemllleyOT1UNTKWeSOYYRKWKWeBGNWKXHK05RQKWZTMeHK2UMKTie3YKRRYKe3OINeJOKccNReJKWeyNK3INROK4KTJKTem"
            "umqdjmumteGZLemqirniemumqdjmunleGZLemuitleMKMKTemuinleOTeJKTccFGNReJKWeyNK3INROK4KTJKTemumqmumte"
            "GZLemqirniemumqdmunleGZLemuitleMKMKTemuinleOTeJKTccJWKOeRKY2YKTkzWOKJKTXPGNWKTkexZWINeWOINYKWROI"
            "NKTeEVWZINe1ZWJKTemumqjmlmtersoeyNKTeMKjccJWKOeRKY2YKTezWOKJKTXPGNWKTkexZWINeWOINYKWROINKTeEVWZI"
            "Ne1ZWJKTemumqmlmtersoeyNKTeMKdccINOKJKTieJkeOkesinleGZLePKemllieOTeJKWeMRKOINKTeFKOYeTKZeKOTMKMG"
            "TMKTKeyNKTiemumueGRRKOTcc3INOKJKTieJkeOkesinleGZLePKemllieOTeJKWeMRKOINKTeFKOYeTKZeKOTMKMGTMKTKe"
            "yNKTiemumueGRRKOTccoqueEetinefeMKMKTesinbegmummdmumohkccoqueEetineseMKMKTesinfegmummdjemumohkccy"
            "OTKTeWKINYebWNKHROINKTeD6IQMGTMeKWLZNWeJOKeFGNReJKWeAKHUWKTKTkewKYWZMe3OKccyOTKTeWKINYeKWNKHROIN"
            "KTeD6IQMGTMeKWLZNWeJOKeFGNReJKWeAKHUWKTKTkewKYWZMe3OKccumqeTUINeqsoueZTJeVROKHe3OKe3USOYeTZWeZSe"
            "mspeNOTYKWeJKWemumoeMKSKRJKYKTeFGNReJKWeAKdccmumqeTUINeqsoueZTJeHROKHe3OKe3USOYeTZWeZSemspeNOTYK"
            "WeJKWemumoeMKSKRJKYKTeFGNReJKWeAKdccHUWKTKTe2ZW6IQie3Ue3GTQe3OKegmuhmqheGZLeprsqiegmumrheGZLeosn"
            "meZTJegmumsheMGWeGZLeqmuoieZSeJGTTccHUWKTKTe2ZW6IQie3Ue3GTQe3OKegmumqheGZLeprsqiegmumrheGZLeosnm"
            "eZTJegmumsheMGWeGZLeomuoieZSeJGTTccxOKeCKYNUJKe2ZWeyWWKINTZTMeJOK3KWeFOLLKWeZTJeJOKeFO33KWTe3KRH"
            "3YeLOTJKTie3OINeOTeJKWeOSeCGOjccxOKeCKYNUJKe2ZWeyWWKINTZTMeJOK3KWeFOLLKWeZTJeJOKeFO33KWTe3KRH3Ye"
            "LOTJKTie3OINeOTeJKWeOSeCGOdccNKLYemunmeJKWk";

        rapidfuzz::Editops ops1;
        rapidfuzz::detail::levenshtein_align(ops1, rapidfuzz::detail::make_range(s1),
                                             rapidfuzz::detail::make_range(s2));
        REQUIRE(s2 == rapidfuzz::editops_apply<char>(ops1, s1, s2));
        rapidfuzz::Editops ops2;
        rapidfuzz::detail::levenshtein_align(ops2, rapidfuzz::detail::make_range(s1),
                                             rapidfuzz::detail::make_range(s2),
                                             static_cast<int64_t>(ops1.size()));
        REQUIRE(ops1 == ops2);
    }

    {
        std::string s1 =
            "GdFGRdyKGTGRfdVPNdkmhdwUMKdjpjnccXUdGRTGKMGOhdsUREJdFKGdrUOFGSRCTSVGRPRFOUOIdeXUNdzEJUTXGdFGRdyK"
            "GTGRfdVPNdkmhdwUMKdjpjnccKOdAGRDKOFUOIdNKTdFGNdtRMCZdFGSdyKOKYTGRSdFGSdvOOGROdVPNdlihdqUIUYTdjpj"
            "ndUOFdFGRdyKGTGRaccKOdAGRDKOFUOIdNKTdFGNdtRMCZdFGSdyKOKYTGRSdFGSdvOOGROdVPNdlihdqUIUYTdjpjndUOFd"
            "FGRdyKGTGRaccYEJUTXVGRPRFOUOIdeVPNdklhdzGQTGNDGRdjpjofdWURFGdFCSdtKOKIUOISCNTdHGROGRdGRN0EJTKITg"
            "dCUHccYEJUTXVGRPRFOUOIdeVPNdklhdzGQTGNDGRdjpjofdWURFGdFCSdtKOKIUOISCNTdHGROGRdGRN0EJTKITgdCUHccq"
            "ORUHGOdGKOGSdyKGTGRSd2DGRdFKGdBKRLYCNLGKTdGKOGRdx2OFKIUOIdFGSdAGRNKGTGRSgd2DGRdFKGccqORUHGOdGKOG"
            "SdyKGTGRSd2DGRdFKGdBKRLYCNLGKTdGKOGRdx2OFKIUOIdFGSdAGRNKGTGRSgd2DGRdFKGccuPRTYGTXUOIdFGSdyKGTVGR"
            "J0MTOKYYGSgdCUEJdWGOOdLGKOGdx2OFKIUOIdVPRMKGITgdDKSdXURdsCUGRdGKOGSccuPRTYGTXUOIdFGSdyKGTVGRJ0MT"
            "OKYYGSgdCUEJdWGOOdLGKOGdx2OFKIUOIdVPRMKGITgdDKSdXURdsCUGRdGKOGSccwCJRGSdYPWKGd2DGRdGKOGdtRJ1JUOI"
            "dFGSdyKGTXKOYGSdeKNduCMMGdFGRduPRTYGTXUOIfdXUdDGSTKNNGOgccwCJRGSdYPWKGd2DGRdGKOGdtRJ1JUOIdFGSdyK"
            "GTXKOYGSdeKNduCMMGdFGRduPRTYGTXUOIfdXUdDGSTKNNGOgccCUHdqORUHGOdGKOGSdAGRNKGTGRSdGKOGOdNKTdGKOGNd"
            "OGUGOdyKGTGRdCDIGYEJMPYYGOGOdyKGTVGRTRCIgccCUHdqORUHGOdGKOGSdAGRNKGTGRSdGKOGOdNKTdGKOGNdOGUGOdyK"
            "GTGRdCDIGYEJMPYYGOGOdyKGTVGRTRCIgccFGYYGOdtRH2MMUOIdVPOdGKOGRdtOTYEJGKFUOId2DGRdFKGdFRGKdGDGOdIG"
            "OCOOTGOdu0MMGdPFGRdVPRdGKOGNccFGYYGOdtRH2MMUOIdVPOdGKOGRdtOTYEJGKFUOId2DGRdFKGdFRGKdGDGOdIGOCOOT"
            "GOdu0MMGdPFGRdVPRdGKOGNccAGRIMGKEJdVPRdFGNdyKGTGKOKIUOISCNTdIGTRPHHGOdWKRFgdNKTdR2ELWKRLGOFGRdxR"
            "CHTdCUHXUJGDGOhccAGRIMGKEJdVPRdFGNdyKGTGKOKIUOISCNTdIGTRPHHGOdWKRFgdNKTdR2ELWKRLGOFGRdxRCHTdCUHX"
            "UJGDGOhccu";
        std::string s2 =
            "SdFGRdyKGTGRfdFPNdkmhdwUMKdjpjndVccXUdGRTGKMGOhdsUREJdFKGdrUOFGSRCTSVGRPRFOUOIdeXUNdzEJUTXGdFGRd"
            "yKGTGRfdVPNdkmhdwUMKdjpjnccbzGRDKOFUOIdNKTdFGNdtRMCZdFGSdyKOKYTGRSdFGSdvOOGROdVPNdlihdqUIUYTdjpj"
            "ndUOFdFGRdyKGTGRbccKOdAGRDKOFUOIdNKTdFGNdtRMCZdFGSdyKOKYTGRSdFGSdvOOGROdVPNdlihdqUIUYTdjpjndUOFd"
            "FGRdyKGTGRbccYEJUTXVGRPRFOUOIdeVPNdklhdzGQTGNDGRdjpjofdWURFGdFCSdtKOKIUOISCNTdHGROGRdGRN0EJTKITg"
            "dCUHccYEJUTXVGRPRFOUOIdeVPNdklhdzGQTGNDGRdjpjofdWURFGdFCSdtKOKIUOISCNTdHGROGRdGRN0EJTKITgdCUHccq"
            "ORUHGOhdGKOGSdyKGTGRSd2DGRdFKGdBKRLYCNLGKTdGKOGRdx2OFKIUOIdFGSdAGRNKGTGRSgd2DGRdFKGccqORUHGOdGKO"
            "GSdyKGTGRSd2DGRdFKGdBKRLYCNLGKTdGKOGRdx2OFKIUOIdFGSdAGRNKGTGRSgd2DGRdFKGccVPRTYGTXUOIdFGSdyKGTVG"
            "RJ0MTOKYYGSgdCUEJdWGOOdLGKOGdx2OFKIUOIdVPRMKGITgdDKSdXURdsCUGRdGKOGSccuPRTYGTXUOIdFGSdyKGTVGRJ0M"
            "TOKYYGSgdCUEJdWGOOdLGKOGdx2OFKIUOIdVPRMKGITgdDKSdXURdsCUGRdGKOGSccwCJRGSdYPWKGd2DGRdGKOGdtRJ1JUO"
            "IdFGSdyKGTXKOYGSdeKNduCMMGdFGRduPRTYGTXUOIfdXUDDGSTKNNGOgccwCJRGSdYPWKGd2DGRdGKOGdtRJ1JUOIdFGSdy"
            "KGTXKOYGSdeKNduCMMGdFGRduPRTYGTXUOIfdXUdDGSTKNNGOgccCUHdqORUHGOdGKOGSdAGRNKGTGRSdGKOGOdNKTdGKOGN"
            "dOGUGOdyKGTGRdCDIGYEJMPYYGOGOdyKGTVGRTRCIgccCUHdqORUHGOdGKOGSdAGRNKGTGRSdGKOGOdNKTdGKOGNdOGUGOdy"
            "KGTGRdCDIGYEJMPYYGOGOdyKGTVGRTRCIgccbFGYYGOdtRH2MMUOIdVPOdGKOGRdtOTYEJGKFUOId2DGTdFKGdFRGKdGDGOd"
            "IGOCOOTGOdu0MMGdPFGRdVPRdGKOGNccFGYYGOdtRH2MMUOIdVPOdGKOGRdtOTYEJGKFUOId2DGRdFKGdFRGKdGDGOdIGOCO"
            "OTGOdu0MMGdPFGRdVPRdGKOGNccAGRIMGKEJdVPRdFGNdyKGTGKOKIUOISCNTdIGTRPHHGOdWKRFgdNKTdR2ELWKRLGOFGRd"
            "xRCHTdCUHXUJGDGOhccAGRIMGKEJdVPRdFGNdyKGTGKOKIUOISCNTdIGTRPHHGOdWKRFgdNKTdR2ELWKRLGOFGRdxRCHTdCU"
            "HXUJGDGOhccZ";

        rapidfuzz::Editops ops1;
        rapidfuzz::detail::levenshtein_align(ops1, rapidfuzz::detail::make_range(s1),
                                             rapidfuzz::detail::make_range(s2));
        REQUIRE(s2 == rapidfuzz::editops_apply<char>(ops1, s1, s2));
        rapidfuzz::Editops ops2;
        rapidfuzz::detail::levenshtein_align(ops2, rapidfuzz::detail::make_range(s1),
                                             rapidfuzz::detail::make_range(s2),
                                             static_cast<int64_t>(ops1.size()));
        REQUIRE(ops1 == ops2);
    }
}

TEST_CASE("Levenshtein large band")
{
    REQUIRE(osa_example1.size() == 106514);
    REQUIRE(osa_example2.size() == 107244);

    {
        std::basic_string<uint8_t> s1 = get_subsequence(osa_example1, 51, 6541);
        std::basic_string<uint8_t> s2 = get_subsequence(osa_example2, 51, 6516);

        rapidfuzz::Editops ops1;
        rapidfuzz::detail::levenshtein_align(ops1, rapidfuzz::detail::make_range(s1),
                                             rapidfuzz::detail::make_range(s2));
        REQUIRE(s2 == rapidfuzz::editops_apply<uint8_t>(ops1, s1, s2));
        rapidfuzz::Editops ops2;
        rapidfuzz::detail::levenshtein_align(ops2, rapidfuzz::detail::make_range(s1),
                                             rapidfuzz::detail::make_range(s2),
                                             static_cast<int64_t>(ops1.size()));
        REQUIRE(ops1 == ops2);
    }

    {
        auto dist = rapidfuzz::levenshtein_distance(osa_example1, osa_example2, {1, 1, 1});
        REQUIRE(dist == 5278);
    }
    {
        auto dist = rapidfuzz::levenshtein_distance(osa_example1, osa_example2, {1, 1, 1}, 2500);
        REQUIRE(dist == 2501);
    }
    {
        rapidfuzz::Editops ops1 = rapidfuzz::levenshtein_editops(osa_example1, osa_example2);
        REQUIRE(ops1.size() == 5278);
        REQUIRE(osa_example2 == rapidfuzz::editops_apply<uint8_t>(ops1, osa_example1, osa_example2));
    }
    {
        rapidfuzz::Editops ops1 = rapidfuzz::levenshtein_editops(osa_example1, osa_example2, 5278);
        REQUIRE(ops1.size() == 5278);
        REQUIRE(osa_example2 == rapidfuzz::editops_apply<uint8_t>(ops1, osa_example1, osa_example2));
    }
    {
        rapidfuzz::Editops ops1 = rapidfuzz::levenshtein_editops(osa_example1, osa_example2, 2000);
        REQUIRE(ops1.size() == 5278);
        REQUIRE(osa_example2 == rapidfuzz::editops_apply<uint8_t>(ops1, osa_example1, osa_example2));
    }
}
