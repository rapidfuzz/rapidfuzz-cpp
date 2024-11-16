#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <array>
#include <string>
#include <unordered_map>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace ConvertQwertyToAlphabet {

/**
 * qwerty 키보드 자판의 대소문자를 구분한 영어 알파벳을 한글 음소와 맵핑한 객체
 */
inline const std::unordered_map<std::wstring, std::wstring> QWERTY_KEYBOARD_MAP = {
    {L"q", L"ㅂ"}, {L"w", L"ㅈ"}, {L"e", L"ㄷ"}, {L"r", L"ㄱ"}, {L"t", L"ㅅ"}, {L"y", L"ㅛ"}, {L"u", L"ㅕ"},
    {L"i", L"ㅑ"}, {L"o", L"ㅐ"}, {L"p", L"ㅔ"}, {L"a", L"ㅁ"}, {L"s", L"ㄴ"}, {L"d", L"ㅇ"}, {L"f", L"ㄹ"},
    {L"g", L"ㅎ"}, {L"h", L"ㅗ"}, {L"j", L"ㅓ"}, {L"k", L"ㅏ"}, {L"l", L"ㅣ"}, {L"z", L"ㅋ"}, {L"x", L"ㅌ"},
    {L"c", L"ㅊ"}, {L"v", L"ㅍ"}, {L"b", L"ㅠ"}, {L"n", L"ㅜ"}, {L"m", L"ㅡ"}, {L"Q", L"ㅃ"}, {L"W", L"ㅉ"},
    {L"E", L"ㄸ"}, {L"R", L"ㄲ"}, {L"T", L"ㅆ"}, {L"Y", L"ㅛ"}, {L"U", L"ㅕ"}, {L"I", L"ㅑ"}, {L"O", L"ㅒ"},
    {L"P", L"ㅖ"}, {L"A", L"ㅁ"}, {L"S", L"ㄴ"}, {L"D", L"ㅇ"}, {L"F", L"ㄹ"}, {L"G", L"ㅎ"}, {L"H", L"ㅗ"},
    {L"J", L"ㅓ"}, {L"K", L"ㅏ"}, {L"L", L"ㅣ"}, {L"Z", L"ㅋ"}, {L"X", L"ㅌ"}, {L"C", L"ㅊ"}, {L"V", L"ㅍ"},
    {L"B", L"ㅠ"}, {L"N", L"ㅜ"}, {L"M", L"ㅡ"},
    // Include numbers or symbols if needed
};

} // namespace ConvertQwertyToAlphabet
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // CONSTANTS_HPP
