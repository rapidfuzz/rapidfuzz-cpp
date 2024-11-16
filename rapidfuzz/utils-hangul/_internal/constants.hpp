#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace _Internal {

// Note: C++ does not have built-in Unicode normalization functions.
// The following array needs to be precomputed or handled with a Unicode library.
// Placeholder for _JASO_HANGUL_NFD
inline const std::array<wchar_t, 6> _JASO_HANGUL_NFD = {
    L'ㄱ', // START_CHOSEONG: 'ㄱ'
    L'ㅏ', // START_JUNGSEONG: 'ㅏ'
    L'ㄱ', // START_JONGSEONG: 'ㄱ'
    L'ㅎ', // END_CHOSEONG: 'ㅎ'
    L'ㅣ', // END_JUNGSEONG: 'ㅣ'
    L'ㅎ'  // END_JONGSEONG: 'ㅎ'
};

/**
 * @brief Complete Hangul syllables start code point ('가' - U+AC00)
 */
constexpr int COMPLETE_HANGUL_START_CHARCODE = 0xAC00;

/**
 * @brief Complete Hangul syllables end code point ('힣' - U+D7A3)
 */
constexpr int COMPLETE_HANGUL_END_CHARCODE = 0xD7A3;

constexpr int NUMBER_OF_JONGSEONG = 28;
constexpr int NUMBER_OF_JUNGSEONG = 21;

/**
 * ㄱ -> 'ㄱ'
 * ㄳ -> 'ㄱㅅ' 으로 나눈다.
 * 종성이 없는 경우 '빈' 초성으로 관리하는 것이 편리하여, 빈 문자열도 포함한다.
 */
inline const std::unordered_map<std::wstring, std::wstring> DISASSEMBLED_CONSONANTS_BY_CONSONANT = {
    {L"", L""},       {L"ㄱ", L"ㄱ"},   {L"ㄲ", L"ㄲ"},   {L"ㄳ", L"ㄱㅅ"}, {L"ㄴ", L"ㄴ"},
    {L"ㄵ", L"ㄴㅈ"}, {L"ㄶ", L"ㄴㅎ"}, {L"ㄷ", L"ㄷ"},   {L"ㄸ", L"ㄸ"},   {L"ㄹ", L"ㄹ"},
    {L"ㄺ", L"ㄹㄱ"}, {L"ㄻ", L"ㄹㅁ"}, {L"ㄼ", L"ㄹㅂ"}, {L"ㄽ", L"ㄹㅅ"}, {L"ㄾ", L"ㄹㅌ"},
    {L"ㄿ", L"ㄹㅍ"}, {L"ㅀ", L"ㄹㅎ"}, {L"ㅁ", L"ㅁ"},   {L"ㅂ", L"ㅂ"},   {L"ㅃ", L"ㅃ"},
    {L"ㅄ", L"ㅂㅅ"}, {L"ㅅ", L"ㅅ"},   {L"ㅆ", L"ㅆ"},   {L"ㅇ", L"ㅇ"},   {L"ㅈ", L"ㅈ"},
    {L"ㅉ", L"ㅉ"},   {L"ㅊ", L"ㅊ"},   {L"ㅋ", L"ㅋ"},   {L"ㅌ", L"ㅌ"},   {L"ㅍ", L"ㅍ"},
    {L"ㅎ", L"ㅎ"},
};

inline const std::unordered_map<std::wstring, std::wstring> DISASSEMBLED_VOWELS_BY_VOWEL = {
    {L"ㅏ", L"ㅏ"}, {L"ㅐ", L"ㅐ"},   {L"ㅑ", L"ㅑ"},   {L"ㅒ", L"ㅒ"},   {L"ㅓ", L"ㅓ"},   {L"ㅔ", L"ㅔ"},
    {L"ㅕ", L"ㅕ"}, {L"ㅖ", L"ㅖ"},   {L"ㅗ", L"ㅗ"},   {L"ㅘ", L"ㅗㅏ"}, {L"ㅙ", L"ㅗㅐ"}, {L"ㅚ", L"ㅗㅣ"},
    {L"ㅛ", L"ㅛ"}, {L"ㅜ", L"ㅜ"},   {L"ㅝ", L"ㅜㅓ"}, {L"ㅞ", L"ㅜㅔ"}, {L"ㅟ", L"ㅜㅣ"}, {L"ㅠ", L"ㅠ"},
    {L"ㅡ", L"ㅡ"}, {L"ㅢ", L"ㅡㅣ"}, {L"ㅣ", L"ㅣ"},
};

/**
 * 초성으로 올 수 있는 한글 글자
 */
inline const std::vector<std::wstring> CHOSEONGS = {
    L"ㄱ", L"ㄲ", L"ㄴ", L"ㄷ", L"ㄸ", L"ㄹ", L"ㅁ", L"ㅂ", L"ㅃ", L"ㅅ",
    L"ㅆ", L"ㅇ", L"ㅈ", L"ㅉ", L"ㅊ", L"ㅋ", L"ㅌ", L"ㅍ", L"ㅎ",
};

/**
 * 중성으로 올 수 있는 한글 글자
 */
inline const std::vector<std::wstring> JUNSEONGS = [] {
    std::vector<std::wstring> values;
    for (const auto& pair : DISASSEMBLED_VOWELS_BY_VOWEL) {
        values.push_back(pair.second);
    }
    return values;
}();

/**
 * 종성으로 올 수 있는 한글 글자
 */
inline const std::vector<std::wstring> JONGSEONGS = {
    L"",     L"ㄱ",   L"ㄲ",   L"ㄱㅅ", L"ㄴ",   L"ㄴㅈ", L"ㄴㅎ", L"ㄷ", L"ㄹ",   L"ㄹㄱ",
    L"ㄹㅁ", L"ㄹㅂ", L"ㄹㅅ", L"ㄹㅌ", L"ㄹㅍ", L"ㄹㅎ", L"ㅁ",   L"ㅂ", L"ㅂㅅ", L"ㅅ",
    L"ㅆ",   L"ㅇ",   L"ㅈ",   L"ㅊ",   L"ㅋ",   L"ㅌ",   L"ㅍ",   L"ㅎ",
};

inline const std::array<std::wstring, 20> HANGUL_DIGITS = {
    L"",   L"만", L"억", L"조",     L"경",     L"해",     L"자",       L"양",       L"구", L"간",
    L"정", L"재", L"극", L"항하사", L"아승기", L"나유타", L"불가사의", L"무량대수", L"겁", L"업",
};

constexpr int HANGUL_DIGITS_MAX = HANGUL_DIGITS.size() * 4; // 20 * 4 = 80

inline const std::array<std::wstring, 10> HANGUL_NUMBERS = {
    L"", L"일", L"이", L"삼", L"사", L"오", L"육", L"칠", L"팔", L"구",
};

inline const std::array<std::wstring, 10> HANGUL_NUMBERS_FOR_DECIMAL = {
    L"영", L"일", L"이", L"삼", L"사", L"오", L"육", L"칠", L"팔", L"구",
};

inline const std::array<std::wstring, 4> HANGUL_CARDINAL = {
    L"",
    L"십",
    L"백",
    L"천",
};

} // namespace _Internal
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // HANGUL_CONSTANTS_HPP
