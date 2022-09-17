#include <catch2/catch_test_macros.hpp>

#include <rapidfuzz/details/common.hpp>

TEST_CASE("remove affix")
{
    std::string s1 = "aabbbbaaaa";
    std::string s2 = "aaabbbbaaaaa";

    {
        rapidfuzz::detail::Range s1_(s1);
        rapidfuzz::detail::Range s2_(s2);
        REQUIRE(rapidfuzz::detail::remove_common_prefix(s1_, s2_) == 2);
        REQUIRE(s1_ == rapidfuzz::detail::Range("bbbbaaaa"));
        REQUIRE(s2_ == rapidfuzz::detail::Range("abbbbaaaaa"));
    }

    {
        rapidfuzz::detail::Range s1_(s1);
        rapidfuzz::detail::Range s2_(s2);
        REQUIRE(rapidfuzz::detail::remove_common_suffix(s1_, s2_) == 4);
        REQUIRE(s1_ == rapidfuzz::detail::Range("aabbbb"));
        REQUIRE(s2_ == rapidfuzz::detail::Range("aaabbbba"));
    }

    {
        rapidfuzz::detail::Range s1_(s1);
        rapidfuzz::detail::Range s2_(s2);
        auto affix = rapidfuzz::detail::remove_common_affix(s1_, s2_);
        REQUIRE(affix.prefix_len == 2);
        REQUIRE(affix.suffix_len == 4);
        REQUIRE(s1_ == rapidfuzz::detail::Range("bbbb"));
        REQUIRE(s2_ == rapidfuzz::detail::Range("abbbba"));
    }
}
