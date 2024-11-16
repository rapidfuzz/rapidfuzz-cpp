#include "canBe.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace CanBe {

TEST_CASE("canBeChoseong", "[canBeChoseong]")
{
    SECTION("초성이 될 수 있다고 판단되는 경우")
    {
        REQUIRE(canBeChoseong(L"ㄱ") == true);
        REQUIRE(canBeChoseong(L"ㅃ") == true);
    }

    SECTION("초성이 될 수 없다고 판단되는 경우")
    {
        REQUIRE(canBeChoseong(L"ㅏ") == false);
        REQUIRE(canBeChoseong(L"ㅘ") == false);
        REQUIRE(canBeChoseong(L"ㄱㅅ") == false);
        REQUIRE(canBeChoseong(L"가") == false);
    }
}

TEST_CASE("canBeJungseong", "[canBeJungseong]")
{
    SECTION("중성이 될 수 있다고 판단되는 경우")
    {
        REQUIRE(canBeJungseong(L"ㅗㅏ") == true);
        REQUIRE(canBeJungseong(L"ㅏ") == true);
    }

    SECTION("중성이 될 수 없다고 판단되는 경우")
    {
        REQUIRE(canBeJungseong(L"ㄱ") == false);
        REQUIRE(canBeJungseong(L"ㄱㅅ") == false);
        REQUIRE(canBeJungseong(L"가") == false);
    }
}

TEST_CASE("canBeJongseong", "[canBeJongseong]")
{
    SECTION("종성이 될 수 있다고 판단되는 경우")
    {
        REQUIRE(canBeJongseong(L"ㄱ") == true);
        REQUIRE(canBeJongseong(L"ㄱㅅ") == true);
        REQUIRE(canBeJongseong(L"ㅂㅅ") == true);
    }

    SECTION("종성이 될 수 없다고 판단되는 경우")
    {
        REQUIRE(canBeJongseong(L"ㅎㄹ") == false);
        REQUIRE(canBeJongseong(L"ㅗㅏ") == false);
        REQUIRE(canBeJongseong(L"ㅏ") == false);
        REQUIRE(canBeJongseong(L"가") == false);
    }
}

} // namespace CanBe
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
