#include "combineCharacter.hpp"
#include <catch2/catch_test_macros.hpp>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace CombineCharacter {

TEST_CASE("combineCharacter", "[combineCharacter]")
{
    SECTION("종성으로 겹받침으로 구성될 수 있는 문자 두 개를 받으면 겹받침을 생성한다. (ㄱ, ㅏ, ㅂㅅ)")
    {
        REQUIRE(combineCharacter(L"ㄱ", L"ㅏ", L"ㅂㅅ") == L"값");
    }

    SECTION("종성이 입력되지 않았다면 받침이 없는 문자로 합성한다. (ㅌ, ㅗ)")
    {
        REQUIRE(combineCharacter(L"ㅌ", L"ㅗ") == L"토");
    }

    SECTION("종성이 입력되었다면 받침을 추가한다. (ㅌ, ㅗ, ㅅ)")
    {
        REQUIRE(combineCharacter(L"ㅌ", L"ㅗ", L"ㅅ") == L"톳");
    }

    SECTION("초성이 될 수 없는 문자가 초성으로 입력되면 에러를 반환한다. (ㅏ, ㅏ, ㄱ)")
    {
        REQUIRE_THROWS(combineCharacter(L"ㅏ", L"ㅏ", L"ㄱ"), "Invalid hangul Characters: ㅏ, ㅏ, ㄱ");
    }

    SECTION("중성이 될 수 없는 문자가 중성으로 입력되면 에러를 반환한다. (ㄱ, ㄴ, ㅃ)")
    {
        REQUIRE_THROWS(combineCharacter(L"ㄱ", L"ㄴ", L"ㅃ"), "Invalid hangul Characters: ㄱ, ㄴ, ㅃ");
    }

    SECTION("종성이 될 수 없는 문자가 종성으로 입력되면 에러를 반환한다. (ㄱ, ㅏ, ㅃ)")
    {
        REQUIRE_THROWS(combineCharacter(L"ㄱ", L"ㅏ", L"ㅃ"), "Invalid hangul Characters: ㄱ, ㅏ, ㅃ");
    }

    SECTION("온전한 한글 문자가 하나라도 입력되면 에러를 반환한다. (가, ㅏ, ㄱ)")
    {
        REQUIRE_THROWS(combineCharacter(L"가", L"ㅏ", L"ㄱ"), "Invalid hangul Characters: 가, ㅏ, ㄱ");
    }
}

TEST_CASE("combineVowels", "[combineVowels]")
{
    SECTION("겹모음이 될 수 있는 모음이 순서대로 입력되면 겹모음으로 합성한다.")
    {
        REQUIRE(combineVowels(L"ㅗ", L"ㅏ") == L"ㅘ");
        REQUIRE(combineVowels(L"ㅜ", L"ㅔ") == L"ㅞ");
        REQUIRE(combineVowels(L"ㅡ", L"ㅣ") == L"ㅢ");
    }

    SECTION("겹모음이 될 수 있는 모음이라고 해도 틀린 순서로 입력되면 Join한다.")
    {
        REQUIRE(combineVowels(L"ㅏ", L"ㅗ") == L"ㅏㅗ");
        REQUIRE(combineVowels(L"ㅣ", L"ㅡ") == L"ㅣㅡ");
    }

    SECTION("이미 겹모음인 문자와 모음을 합성하려고 시도하면 Join한다.")
    {
        REQUIRE(combineVowels(L"ㅘ", L"ㅏ") == L"ㅘㅏ");
        REQUIRE(combineVowels(L"ㅝ", L"ㅣ") == L"ㅝㅣ");
    }
}

} // namespace CombineCharacter
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
