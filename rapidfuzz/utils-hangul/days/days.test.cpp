#include "days.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace Days {

TEST_CASE("days", "[days]")
{
    // helper function wstring -> string
    auto wstr_to_str = [](const std::wstring& wstr) {
        std::string str(wstr.begin(), wstr.end());
        return str;
    };

    // Define valid numbers and their corresponding Hangul words
    struct ValidNumber {
        int num;
        std::wstring word;
    };

    const std::vector<ValidNumber> validNumbers = {
        {1, L"하루"},    {2, L"이틀"},    {3, L"사흘"},      {4, L"나흘"},   {5, L"닷새"},
        {6, L"엿새"},    {7, L"이레"},    {8, L"여드레"},    {9, L"아흐레"}, {10, L"열흘"},
        {11, L"열하루"}, {20, L"스무날"}, {21, L"스무하루"}, {30, L"서른날"}};

    // Define invalid numbers
    const std::vector<int> invalidIntNumbers = {0, -1, 31};

    // Test valid numbers
    for (const ValidNumber& validNumber : validNumbers) {
        int num = validNumber.num;
        std::wstring word = validNumber.word;
        SECTION(std::to_string(num) + " - 순 우리말 날짜 " + wstr_to_str(word) + "로 바꿔 반환해야 한다.")
        {
            REQUIRE(days(num) == word);
        }
    }

    // Test invalid numbers
    for (const auto& num : invalidIntNumbers) {
        SECTION("유효하지 않은 숫자 " + std::to_string(num) + "에 대해 오류를 발생시켜야 한다.")
        {
            REQUIRE_THROWS(days(num), "지원하지 않는 숫자입니다.");
        }
    }
}

} // namespace Days
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
