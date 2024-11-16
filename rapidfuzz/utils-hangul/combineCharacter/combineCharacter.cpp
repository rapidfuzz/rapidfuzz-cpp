#include "combineCharacter.hpp"
#include <algorithm>
#include <stdexcept>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace CombineCharacter {

/**
 * @brief 인자로 초상, 중성, 종성을 받아 하나의 한글 문자를 반환합니다.
 *
 * @param choseong 초성
 * @param jungseong 중성
 * @param jongseong 종성
 * @return A `std::wstring` representing the combined Hangul syllable.
 * @throws `std::invalid_argument` if any of the provided characters are invalid.
 *
 * @example
 * std::wstring syllable = combineCharacter(L"ㄱ", L"ㅏ", L"ㅂㅅ"); // "값"
 * std::wstring syllable = combineCharacter(L"ㅌ", L"ㅗ"); // "토"
 */
std::wstring combineCharacter(const std::wstring& choseong, const std::wstring& jungseong,
                              const std::wstring& jongseong)
{
    // Validate input characters
    if (!CanBe::canBeChoseong(choseong)) {
        throw std::invalid_argument("Invalid Choseong character: " +
                                    std::string(choseong.begin(), choseong.end()));
    }
    if (!CanBe::canBeJungseong(jungseong)) {
        throw std::invalid_argument("Invalid Jungseong character: " +
                                    std::string(jungseong.begin(), jungseong.end()));
    }
    if (!jongseong.empty() && !CanBe::canBeJongseong(jongseong)) {
        throw std::invalid_argument("Invalid Jongseong character: " +
                                    std::string(jongseong.begin(), jongseong.end()));
    }

    // Retrieve indices
    int choseongIndex = -1;
    int jungseongIndex = -1;
    int jongseongIndex = 0; // Defaults to 0 (no Jongseong)

    // Find choseong index
    auto it_choseong = std::find(_Internal::CHOSEONGS.begin(), _Internal::CHOSEONGS.end(), choseong);
    if (it_choseong != _Internal::CHOSEONGS.end()) {
        choseongIndex = std::distance(_Internal::CHOSEONGS.begin(), it_choseong);
    }
    else {
        throw std::invalid_argument("Choseong character not found: " +
                                    std::string(choseong.begin(), choseong.end()));
    }

    // Find jungseong index
    auto it_jungseong = std::find(_Internal::JUNSEONGS.begin(), _Internal::JUNSEONGS.end(), jungseong);
    if (it_jungseong != _Internal::JUNSEONGS.end()) {
        jungseongIndex = std::distance(_Internal::JUNSEONGS.begin(), it_jungseong);
    }
    else {
        throw std::invalid_argument("Jungseong character not found: " +
                                    std::string(jungseong.begin(), jungseong.end()));
    }

    // Find jongseong index if jongseong is provided
    if (!jongseong.empty()) {
        auto it_jongseong = std::find(_Internal::JONGSEONGS.begin(), _Internal::JONGSEONGS.end(), jongseong);
        if (it_jongseong != _Internal::JONGSEONGS.end()) {
            jongseongIndex = std::distance(_Internal::JONGSEONGS.begin(), it_jongseong);
        }
        else {
            throw std::invalid_argument("Jongseong character not found: " +
                                        std::string(jongseong.begin(), jongseong.end()));
        }
    }

    // Calculate Unicode code point
    // Formula: S = 0xAC00 + (Choseong_index * 21 * 28) + (Jungseong_index * 28) + Jongseong_index
    const int numOfJungseongs = _Internal::JUNSEONGS.size();  // 21
    const int numOfJongseongs = _Internal::JONGSEONGS.size(); // 28

    int unicode = _Internal::COMPLETE_HANGUL_START_CHARCODE +
                  (choseongIndex * numOfJungseongs * numOfJongseongs) + (jungseongIndex * numOfJongseongs) +
                  jongseongIndex;

    // Convert code point to wchar_t
    wchar_t syllable = static_cast<wchar_t>(unicode);

    // Return as std::wstring
    return std::wstring(1, syllable);
}

/**
 * @brief 인자로 초성, 중성, 종성을 받아 하나의 한글 문자를 반환하는 `combineCharacter` 함수의 커링된
 * 버전입니다.
 *
 * @param choseong 초성
 * @return A lambda function that takes a medial vowel (Jungseong) and returns another lambda
 *         that takes an optional final consonant (Jongseong) to form a complete Hangul syllable.
 *
 * @example
 * auto combineMiddleHangulCharacter = curriedCombineCharacter(L"ㄱ");
 * auto combineLastHangulCharacter = combineMiddleHangulCharacter(L"ㅏ");
 * std::wstring syllable = combineLastHangulCharacter(L"ㄱ"); // "각"
 */
std::function<std::function<std::wstring(const std::wstring&)>(const std::wstring&)>
curriedCombineCharacter(const std::wstring& choseong)
{
    return [choseong](const std::wstring& jungseong) -> std::function<std::wstring(const std::wstring&)> {
        return [choseong, jungseong](const std::wstring& jongseong = L"") -> std::wstring {
            return combineCharacter(choseong, jungseong, jongseong);
        };
    };
}

/**
 * @brief 인자로 두 개의 모음을 받아 합성하여 겹모음을 생성합니다. 만약 올바른 한글 규칙으로 합성할 수 없는
 * 모음들이라면 단순 Join합니다.
 *
 * @param vowel1 첫 번째 모음
 * @param vowel2 두 번째 모음
 * @return A `std::wstring` representing the combined vowel or the concatenation of the two vowels.
 *
 * @example
 * std::wstring compoundVowel1 = combineVowels(L"ㅗ", L"ㅏ"); // "ㅘ"
 * std::wstring compoundVowel2 = combineVowels(L"ㅗ", L"ㅐ"); // "ㅙ"
 * std::wstring compoundVowel3 = combineVowels(L"ㅗ", L"ㅛ"); // "ㅗㅛ"
 */
std::wstring combineVowels(const std::wstring& vowel1, const std::wstring& vowel2)
{
    // Iterate through the DISASSEMBLED_VOWELS_BY_VOWEL to find a matching combination
    for (const auto& [combinedVowel, decomposedVowels] : _Internal::DISASSEMBLED_VOWELS_BY_VOWEL) {
        if (decomposedVowels == (vowel1 + vowel2)) {
            return combinedVowel;
        }
    }

    // If no valid combination is found, return the concatenated vowels
    return vowel1 + vowel2;
}

} // namespace CombineCharacter
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
