#ifndef CONVERT_QWERTY_TO_ALPHABET_HPP
#define CONVERT_QWERTY_TO_ALPHABET_HPP

#include "assemble/assemble.hpp"
#include "constants.hpp"
#include <string>
#include <vector>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace ConvertQwertyToAlphabet {

/**
 * @brief 영어 알파벳을 qwerty 자판과 매칭되는 한글 음소로 변환합니다.
 *
 * @param word 한글 음소로 변환하고자 하는 영문
 * @return 영어 알파벳이 포함되지 않은 한글 음소, 음절, 기타 문자로 이루어진 문자열
 *
 * @example
 * std::wstring result = convertQwertyToAlphabet(L"dkssudgktpdy"); // ㅇㅏㄴㄴㅕㅇㅎㅏㅅㅔㅇㅛ
 */
std::wstring convertQwertyToAlphabet(const std::wstring& word);

/**
 * @brief 영어 알파벳을 qwerty 자판과 매칭과는 한글 문자와 문장으로 변환합니다.
 *
 * @param word 한글 문장으로 변환하고자 하는 영문
 * @returns qwerty 영어 알파벳을 변환하여 한글 규칙에 맞게 합성한 문자열
 *
 * @example
 * std::wstring hangul = convertQwertyToHangul(L"dkssudgktpdy"); // 안녕하세요
 */
std::wstring convertQwertyToHangul(const std::wstring& word);

} // namespace ConvertQwertyToAlphabet
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // CONVERT_QWERTY_TO_ALPHABET_HPP
