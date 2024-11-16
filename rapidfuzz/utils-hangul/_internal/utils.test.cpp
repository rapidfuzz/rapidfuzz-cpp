#include "utils.hpp"
#include <catch2/catch_test_macros.hpp>
#include <type_traits>
#include <unordered_map>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace _Internal {

// Test Case for excludeLastElement
TEST_CASE("excludeLastElement", "[excludeLastElement]")
{
    SECTION("마지막 요소를 제외한 모든 요소와 마지막 요소를 반환한다")
    {
        std::vector<std::wstring> input = {L"apple", L"banana", L"cherry"};
        auto result = excludeLastElement(input);
        std::vector<std::wstring> expected_all = {L"apple", L"banana"};
        std::wstring expected_last = L"cherry";
        REQUIRE(result.first == expected_all);
        REQUIRE(result.second == expected_last);
    }

    SECTION("입력 배열이 비어 있으면 빈 배열과 빈 문자열을 반환한다")
    {
        std::vector<std::wstring> input = {};
        auto result = excludeLastElement(input);
        std::vector<std::wstring> expected_all = {};
        std::wstring expected_last = L"";
        REQUIRE(result.first == expected_all);
        REQUIRE(result.second == expected_last);
    }

    SECTION("배열에 단 하나의 요소만 있는 경우, 빈배열과 그 요소를 반환한다")
    {
        std::vector<std::wstring> input = {L"apple"};
        auto result = excludeLastElement(input);
        std::vector<std::wstring> expected_all = {};
        std::wstring expected_last = L"apple";
        REQUIRE(result.first == expected_all);
        REQUIRE(result.second == expected_last);
    }
}

// Test Case for joinString
TEST_CASE("joinString", "[joinString]")
{
    SECTION("여러 문자열을 하나의 문자열로 연결한다")
    {
        std::wstring result = joinString(L"Hello", L" ", L"World");
        std::wstring expected = L"Hello World";
        REQUIRE(result == expected);
    }

    SECTION("인자가 주어지지 않았을 때 빈 문자열을 반환한다")
    {
        std::wstring result = joinString();
        std::wstring expected = L"";
        REQUIRE(result == expected);
    }
}

// Test Case for isBlank
TEST_CASE("isBlank", "[isBlank]")
{
    SECTION("문자가 공백이면 true를 반환한다")
    {
        std::wstring character = L" ";
        REQUIRE(isBlank(character) == true);
    }

    SECTION("문자가 공백이 아니면 false를 반환한다")
    {
        std::wstring character = L"a";
        REQUIRE(isBlank(character) == false);
    }
}

// Test Case for assertCondition
TEST_CASE("assertCondition", "[assertCondition]")
{
    SECTION("조건이 참이면 에러를 던지지 않는다")
    {
        REQUIRE_NOTHROW(assertCondition(true, "Invalid condition"));
    }

    SECTION("조건이 거짓이면 에러를 던진다")
    {
        REQUIRE_THROWS(assertCondition(false), "Invalid condition");
    }

    SECTION("조건이 거짓이고 에러 메시지가 제공된 경우 사용자 정의 에러 메시지를 던져야 한다")
    {
        std::string customMessage = "Custom error message";
        REQUIRE_THROWS(assertCondition(false, customMessage), customMessage);
    }
}

// Test Case for hasValueInReadOnlyStringList
TEST_CASE("hasValueInReadOnlyStringList", "[hasValueInReadOnlyStringList]")
{
    const std::vector<std::wstring> testReadonlyList = {L"ㄱ", L"ㄴ", L"ㄷ"};

    SECTION("read-only 문자열 리스트에 요소가 존재한다면 true를 반환한다.")
    {
        std::wstring testValue = L"ㄱ";
        REQUIRE(hasValueInReadOnlyStringList(testReadonlyList, testValue) == true);
    }

    SECTION("read-only 문자열 리스트에 요소가 존재하지 않으면 false를 반환한다.")
    {
        std::wstring testValue = L"ㄹ";
        REQUIRE(hasValueInReadOnlyStringList(testReadonlyList, testValue) == false);
    }

    // Note: Type narrowing is handled at compile-time in C++. Hence, runtime tests are not applicable.
}

// Test Case for hasProperty
TEST_CASE("hasProperty", "[hasProperty]")
{
    const std::unordered_map<std::wstring, std::wstring> testObj = {
        {L"ㄱ", L"ㄱ"}, {L"ㄴ", L"ㄴ"}, {L"ㄷ", L"ㄷ"}};

    SECTION("객체에 속성이 존재하면 true를 반환한다.")
    {
        std::wstring testKey = L"ㄱ";
        REQUIRE(hasProperty(testObj, testKey) == true);
    }

    SECTION("객체에 속성이 존재하지 않으면 false를 반환한다.")
    {
        std::wstring testKey = L"ㄹ";
        REQUIRE(hasProperty(testObj, testKey) == false);
    }

    // Note: Type narrowing is handled at compile-time in C++. Hence, runtime tests are not applicable.
}

} // namespace _Internal
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
