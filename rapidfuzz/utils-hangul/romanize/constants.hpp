#ifndef ROMANIZE_CONSTANTS_HPP
#define ROMANIZE_CONSTANTS_HPP

#include <string>
#include <unordered_map>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace Romanize {

// ---------------------------------------
// Middle Vowel (중성) Alphabet Pronunciations
// ---------------------------------------
inline const std::unordered_map<wchar_t, std::wstring> JUNGSEONG_ALPHABET_PRONUNCIATION = {
    // ------- 단모음
    {L'ㅏ', L"a"},
    {L'ㅓ', L"eo"},
    {L'ㅗ', L"o"},
    {L'ㅜ', L"u"},
    {L'ㅡ', L"eu"},
    {L'ㅣ', L"i"},
    {L'ㅐ', L"ae"},
    {L'ㅔ', L"e"},
    {L'ㅚ', L"oe"},
    {L'ㅟ', L"wi"},
    // ------- 이중모음
    {L'ㅑ', L"ya"},
    {L'ㅕ', L"yeo"},
    {L'ㅛ', L"yo"},
    {L'ㅠ', L"yu"},
    {L'ㅒ', L"yae"},
    {L'ㅖ', L"ye"},
    {L'ㅘ', L"wa"},
    {L'ㅙ', L"wae"},
    {L'ㅝ', L"wo"},
    {L'ㅞ', L"we"},
    {L'ㅢ', L"ui"},
};

// ---------------------------------------
// Initial Consonant (초성) Alphabet Pronunciations
// ---------------------------------------
inline const std::unordered_map<wchar_t, std::wstring> CHOSEONG_ALPHABET_PRONUNCIATION = {
    // ------- 파열음
    {L'ㄱ', L"g"},
    {L'ㄲ', L"kk"},
    {L'ㅋ', L"k"},
    {L'ㄷ', L"d"},
    {L'ㄸ', L"tt"},
    {L'ㅌ', L"t"},
    {L'ㅂ', L"b"},
    {L'ㅃ', L"pp"},
    {L'ㅍ', L"p"},
    // ------- 파찰음
    {L'ㅈ', L"j"},
    {L'ㅉ', L"jj"},
    {L'ㅊ', L"ch"},
    // ------- 마찰음
    {L'ㅅ', L"s"},
    {L'ㅆ', L"ss"},
    {L'ㅎ', L"h"},
    // ------- 비음
    {L'ㄴ', L"n"},
    {L'ㅁ', L"m"},
    {L'ㅇ', L""}, // Silent when initial
    // ------- 유음
    {L'ㄹ', L"r"},
};

// ---------------------------------------
// Final Consonant (종성) Alphabet Pronunciations
// ---------------------------------------
inline const std::unordered_map<std::wstring, std::wstring> JONGSEONG_ALPHABET_PRONUNCIATION = {
    {L"ㄱ", L"k"}, {L"ㄴ", L"n"}, {L"ㄷ", L"t"},  {L"ㄹ", L"l"},
    {L"ㅁ", L"m"}, {L"ㅂ", L"p"}, {L"ㅇ", L"ng"}, {L"", L""}, // No final consonant
};

} // namespace Romanize
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // ROMANIZE_CONSTANTS_HPP
