#ifndef STANDARD_PRONUNCIATION_CONSTANTS_HPP
#define STANDARD_PRONUNCIATION_CONSTANTS_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {

inline const std::wstring 음가가_없는_자음 = L"ㅇ";

inline const std::vector<std::wstring> 한글_자모 = {L"기역", L"니은", L"리을", L"미음",
                                                    L"비읍", L"시옷", L"이응"};
inline const std::vector<std::wstring> 특별한_한글_자모 = {L"디귿", L"지읒", L"치읓", L"키읔",
                                                           L"티읕", L"피읖", L"히읗"};
inline const std::unordered_map<std::wstring, std::wstring> 특별한_한글_자모의_발음 = {
    {L"ㄷ", L"ㅅ"}, {L"ㅈ", L"ㅅ"}, {L"ㅊ", L"ㅅ"}, {L"ㅌ", L"ㅅ"},
    {L"ㅎ", L"ㅅ"}, {L"ㅋ", L"ㄱ"}, {L"ㅍ", L"ㅂ"}};

// 17 항
inline const std::unordered_map<std::wstring, std::wstring> 음의_동화_받침 = {
    {L"ㄷ", L"ㅈ"}, {L"ㅌ", L"ㅊ"}, {L"ㄹㅌ", L"ㅊ"}};

// "ㄴ,ㄹ"이 덧나는 동화작용
inline const std::vector<std::wstring> ㄴㄹ이_덧나는_모음 = {L"ㅏ", L"ㅐ", L"ㅓ", L"ㅔ", L"ㅗ", L"ㅜ", L"ㅟ"};
inline const std::vector<std::wstring> ㄴㄹ이_덧나는_후속음절_모음 = {L"ㅑ", L"ㅕ", L"ㅛ", L"ㅠ",
                                                                      L"ㅣ", L"ㅒ", L"ㅖ"};
inline const std::vector<std::wstring> ㄴㄹ이_덧나서_받침_ㄴ_변환 = {L"ㄱ", L"ㄴ", L"ㄷ",
                                                                     L"ㅁ", L"ㅂ", L"ㅇ"};
inline const std::vector<std::wstring> ㄴㄹ이_덧나서_받침_ㄹ_변환 = {L"ㄹ"};

// 19 항
inline const std::vector<std::wstring> 자음동화_받침_ㄴ_변환 = {L"ㅁ", L"ㅇ", L"ㄱ", L"ㅂ"};

// 18 항
inline const std::vector<std::wstring> 비음화_받침_ㅇ_변환 = {L"ㄱ", L"ㄲ", L"ㅋ", L"ㄱㅅ", L"ㄹㄱ"};
inline const std::vector<std::wstring> 비음화_받침_ㄴ_변환 = {L"ㄷ", L"ㅅ", L"ㅆ", L"ㅈ",
                                                              L"ㅊ", L"ㅌ", L"ㅎ"};
inline const std::vector<std::wstring> 비음화_받침_ㅁ_변환 = {L"ㅂ", L"ㅍ", L"ㄹㅂ", L"ㄹㅍ", L"ㅂㅅ"};

// 12 항
inline const std::vector<std::wstring> 발음변환_받침_ㅎ = {L"ㅎ", L"ㄴㅎ", L"ㄹㅎ"};
inline const std::unordered_map<std::wstring, std::wstring> 발음변환_받침_ㅎ_발음 = {
    {L"ㄱ", L"ㅋ"}, {L"ㄷ", L"ㅌ"}, {L"ㅈ", L"ㅊ"}, {L"ㅅ", L"ㅆ"}};
inline const std::vector<std::wstring> 발음변환_첫소리_ㅎ = {L"ㄱ",   L"ㄹㄱ", L"ㄷ",  L"ㅂ",
                                                             L"ㄹㅂ", L"ㅈ",   L"ㄴㅈ"};
inline const std::unordered_map<std::wstring, std::wstring> 발음변환_첫소리_ㅎ_발음 = {
    {L"ㄱ", L"ㅋ"},   {L"ㄹㄱ", L"ㅋ"}, {L"ㄷ", L"ㅌ"},  {L"ㅂ", L"ㅍ"},
    {L"ㄹㅂ", L"ㅍ"}, {L"ㅈ", L"ㅊ"},   {L"ㄴㅈ", L"ㅊ"}};

// 9항, 10항, 11항
inline const std::unordered_map<std::wstring, std::wstring> 받침_대표음_발음 = {
    {L"ㄲ", L"ㄱ"},   {L"ㅋ", L"ㄱ"},   {L"ㄱㅅ", L"ㄱ"}, {L"ㄹㄱ", L"ㄱ"}, {L"ㅅ", L"ㄷ"},
    {L"ㅆ", L"ㄷ"},   {L"ㅈ", L"ㄷ"},   {L"ㅊ", L"ㄷ"},   {L"ㅌ", L"ㄷ"},   {L"ㅍ", L"ㅂ"},
    {L"ㅂㅅ", L"ㅂ"}, {L"ㄹㅍ", L"ㅂ"}, {L"ㄴㅈ", L"ㄴ"}, {L"ㄹㅂ", L"ㄹ"}, {L"ㄹㅅ", L"ㄹ"},
    {L"ㄹㅌ", L"ㄹ"}, {L"ㄹㅁ", L"ㅁ"}};

inline const std::unordered_map<std::wstring, std::wstring> 된소리 = {
    {L"ㄱ", L"ㄲ"}, {L"ㄷ", L"ㄸ"}, {L"ㅂ", L"ㅃ"}, {L"ㅅ", L"ㅆ"}, {L"ㅈ", L"ㅉ"}};

// 23 항
inline const std::vector<std::wstring> 된소리_받침 = {L"ㄱ", L"ㄲ",   L"ㅋ",   L"ㄱㅅ", L"ㄹㄱ", L"ㄷ",
                                                      L"ㅅ", L"ㅆ",   L"ㅈ",   L"ㅊ",   L"ㅌ",   L"ㅂ",
                                                      L"ㅍ", L"ㄹㅂ", L"ㄹㅍ", L"ㅂㅅ"};

// 24 항, 25 항
inline const std::vector<std::wstring> 어간_받침 = {L"ㄴ", L"ㄴㅈ", L"ㅁ", L"ㄹㅁ", L"ㄹㅂ", L"ㄹㅌ"};

} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // STANDARD_PRONUNCIATION_CONSTANTS_HPP
