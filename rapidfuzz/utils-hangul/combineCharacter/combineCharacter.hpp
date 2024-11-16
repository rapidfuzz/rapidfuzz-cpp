#ifndef COMBINE_CHARACTER_HPP
#define COMBINE_CHARACTER_HPP

#include "_internal/constants.hpp"
#include "canBe/canBe.hpp"
#include <functional>
#include <string>
#include <unordered_map>

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
                              const std::wstring& jongseong = L"");

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
curriedCombineCharacter(const std::wstring& choseong);

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
std::wstring combineVowels(const std::wstring& vowel1, const std::wstring& vowel2);

} // namespace CombineCharacter
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // COMBINE_CHARACTER_HPP
