#ifndef REMOVE_LAST_CHARACTER_HPP
#define REMOVE_LAST_CHARACTER_HPP

#include "_internal/constants.hpp"
#include "_internal/utils.hpp"
#include "canBe/canBe.hpp"
#include "combineCharacter/combineCharacter.hpp"
#include "disassemble/disassemble.hpp"
#include <optional>
#include <string>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace RemoveLastCharacter {

/**
 * @brief Removes the last character from a given Hangul string and returns the modified string.
 * 인자로 주어진 한글 문자열에서 가장 마지막 문자 하나를 제거하여 반환합니다.
 *
 * @param words The input Hangul string from which the last character will be removed.
 * @return A new `std::wstring` with the last character removed.
 *
 * @example
 * std::wstring result = removeLastCharacter(L"안녕하세요 값"); // "안녕하세요 갑"
 * std::wstring result = removeLastCharacter(L"프론트엔드");   // "프론트엔ㄷ"
 * std::wstring result = removeLastCharacter(L"일요일");       // "일요이"
 * std::wstring result = removeLastCharacter(L"전화");         // "전호"
 * std::wstring result = removeLastCharacter(L"신세계");       // "신세ㄱ"
 */
std::wstring removeLastCharacter(const std::wstring& words);

} // namespace RemoveLastCharacter
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // REMOVE_LAST_CHARACTER_HPP
