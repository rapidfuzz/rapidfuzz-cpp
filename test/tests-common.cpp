#include "rapidfuzz/details/Range.hpp"
#include <catch2/catch.hpp>

#include <rapidfuzz/details/common.hpp>

TEST_CASE("remove affix")
{
    std::string s1 = "aabbbbaaaa";
    std::string s2 = "aaabbbbaaaaa";

    {
        auto s1_ = rapidfuzz::detail::make_range(s1);
        auto s2_ = rapidfuzz::detail::make_range(s2);
        REQUIRE(rapidfuzz::detail::remove_common_prefix(s1_, s2_) == 2);
        REQUIRE(s1_ == rapidfuzz::detail::make_range("bbbbaaaa"));
        REQUIRE(s2_ == rapidfuzz::detail::make_range("abbbbaaaaa"));
    }

    {
        auto s1_ = rapidfuzz::detail::make_range(s1);
        auto s2_ = rapidfuzz::detail::make_range(s2);
        REQUIRE(rapidfuzz::detail::remove_common_suffix(s1_, s2_) == 4);
        REQUIRE(s1_ == rapidfuzz::detail::make_range("aabbbb"));
        REQUIRE(s2_ == rapidfuzz::detail::make_range("aaabbbba"));
    }

    {
        auto s1_ = rapidfuzz::detail::make_range(s1);
        auto s2_ = rapidfuzz::detail::make_range(s2);
        auto affix = rapidfuzz::detail::remove_common_affix(s1_, s2_);
        REQUIRE(affix.prefix_len == 2);
        REQUIRE(affix.suffix_len == 4);
        REQUIRE(s1_ == rapidfuzz::detail::make_range("bbbb"));
        REQUIRE(s2_ == rapidfuzz::detail::make_range("abbbba"));
    }
}
