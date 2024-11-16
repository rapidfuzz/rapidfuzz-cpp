#include "transform12th.hpp"
#include "_internal/utils.hpp"
#include <stdexcept>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {
namespace Rules {

/**
 * @brief 제12항을 적용합니다.
 *
 * @param currentSyllable 현재 음절을 입력합니다.
 * @param nextSyllable 다음 음절을 입력합니다.
 * @return A NullableReturnSyllables struct containing the transformed current syllable and the possibly
 * modified next syllable.
 *
 * @details
 * 제12항 받침 ‘ㅎ’의 발음은 다음과 같다.
 * ‘ㅎ(ㄶ, ㅀ)’ 뒤에 ‘ㄱ, ㄷ, ㅈ’이 결합되는 경우에는, 뒤 음절 첫소리와 합쳐서 [ㅋ, ㅌ, ㅊ]으로 발음한다.
 * [붙임] 받침 ‘ㄱ(ㄺ), ㄷ, ㅂ(ㄼ), ㅈ(ㄵ)’이 뒤 음절 첫소리 ‘ㅎ’과 결합되는 경우에도, 역시 두 음을 합쳐서
 * [ㅋ, ㅌ, ㅍ, ㅊ]으로 발음한다.
 * ‘ㅎ(ㄶ, ㅀ)’ 뒤에 ‘ㅅ’이 결합되는 경우에는, ‘ㅅ’을 [ㅆ]으로 발음한다.
 * ‘ㅎ’ 뒤에 ‘ㄴ’이 결합되는 경우에는, [ㄴ]으로 발음한다.
 * [붙임] ‘ㄶ, ㅀ’ 뒤에 ‘ㄴ’이 결합되는 경우에는, ‘ㅎ’을 발음하지 않는다.
 * ‘ㅎ(ㄶ, ㅀ)’ 뒤에 모음으로 시작된 어미나 접미사가 결합되는 경우에는, ‘ㅎ’을 발음하지 않는다.
 */
NullableReturnSyllables transform12th(const Syllable& currentSyllable, const Nullable<Syllable>& nextSyllable)
{
    // Create copies of the syllables to modify
    Syllable current = currentSyllable;
    Nullable<Syllable> next =
        nextSyllable.has_value() ? Nullable<Syllable>(nextSyllable.value()) : std::nullopt;

    // If current.jongseong is empty, return as is
    if (current.jongseong.empty()) {
        return NullableReturnSyllables{current, next};
    }

    // Check if current.jongseong is in 발음변환_받침_ㅎ
    if (_Internal::arrayIncludes<std::wstring>(발음변환_받침_ㅎ, current.jongseong)) {
        if (next.has_value()) {
            // Handle next syllable's choseong being 'ㄱ', 'ㄷ', 'ㅈ', 'ㅅ'
            ReturnSyllables result1 = handleNextChoseongIsㄱㄷㅈㅅ(current, next.value());
            current = result1.current;
            next = result1.next;

            // Handle next syllable's choseong being 'ㄴ'
            ReturnSyllables result2 = handleNextChoseongIsㄴ(current, next.value());
            current = result2.current;
            next = result2.next;

            // Handle next syllable's choseong being 'ㅇ'
            ReturnSyllables result3 = handleNextChoseongIsㅇ(current, next.value());
            current = result3.current;
            next = result3.next;
        }

        if (!next.has_value()) {
            // Handle current syllable's jongseong being 'ㅇ'
            ReturnSyllables result4 = handleCurrentJongseongIsㅇ(current);
            current = result4.current;
            // next remains std::nullopt
        }
    }

    // Handle next syllable's choseong being 'ㅎ'
    NullableReturnSyllables result5 = handleNextChoseongIsㅎ(current, next);
    current = result5.current;
    next = result5.next;

    return NullableReturnSyllables{current, next};
}

ReturnSyllables handleNextChoseongIsㄱㄷㅈㅅ(const Syllable& current, const Syllable& next)
{
    Syllable updatedCurrent = current;
    Syllable updatedNext = next;

    // Define the target choseongs
    std::vector<std::wstring> targetChoseongs = {L"ㄱ", L"ㄷ", L"ㅈ", L"ㅅ"};

    if (_Internal::arrayIncludes<std::wstring>(targetChoseongs, updatedNext.choseong)) {
        // Replace next.choseong with 발음변환_받침_ㅎ_발음[next.choseong]
        auto it = 발음변환_받침_ㅎ_발음.find(updatedNext.choseong);
        if (it != 발음변환_받침_ㅎ_발음.end()) {
            updatedNext.choseong = it->second;
        }
        else {
            // If the mapping is not found, throw an error or handle accordingly
            throw std::runtime_error("발음변환_받침_ㅎ_발음 mapping not found for next.choseong.");
        }

        // Replace current.jongseong by removing 'ㅎ'
        updatedCurrent.jongseong = replace받침ㅎ(updatedCurrent);
    }

    return ReturnSyllables{updatedCurrent, updatedNext};
}

ReturnSyllables handleNextChoseongIsㄴ(const Syllable& current, const Syllable& next)
{
    Syllable updatedCurrent = current;
    Syllable updatedNext = next;

    // Check if next.choseong is 'ㄴ' and current.jongseong is 'ㄴㅎ' or 'ㄹㅎ'
    if (updatedNext.choseong == L"ㄴ" &&
        _Internal::arrayIncludes<std::wstring>(std::vector<std::wstring>{L"ㄴㅎ", L"ㄹㅎ"},
                                               updatedCurrent.jongseong))
    {
        // Replace current.jongseong by removing 'ㅎ'
        updatedCurrent.jongseong = replace받침ㅎ(updatedCurrent);
    }

    return ReturnSyllables{updatedCurrent, updatedNext};
}

ReturnSyllables handleNextChoseongIsㅇ(const Syllable& current, const Syllable& next)
{
    Syllable updatedCurrent = current;
    Syllable updatedNext = next;

    // Check if next.choseong is 'ㅇ' (음가가_없는_자음)
    if (updatedNext.choseong == 음가가_없는_자음) {
        // If current.jongseong is 'ㄴㅎ' or 'ㄹㅎ', remove 'ㅎ'
        if (_Internal::arrayIncludes<std::wstring>(std::vector<std::wstring>{L"ㄴㅎ", L"ㄹㅎ"},
                                                   updatedCurrent.jongseong))
        {
            updatedCurrent.jongseong = replace받침ㅎ(updatedCurrent);
        }
        else {
            // Remove 'ㅎ' completely
            updatedCurrent.jongseong = L"";
        }
    }
    else {
        // Replace current.jongseong by removing 'ㅎ'
        updatedCurrent.jongseong = replace받침ㅎ(updatedCurrent);
    }

    return ReturnSyllables{updatedCurrent, updatedNext};
}

ReturnSyllables handleCurrentJongseongIsㅇ(const Syllable& current)
{
    Syllable updatedCurrent = current;

    // Replace current.jongseong by removing 'ㅎ'
    updatedCurrent.jongseong = replace받침ㅎ(updatedCurrent);

    return ReturnSyllables{updatedCurrent, Syllable{}};
}

NullableReturnSyllables handleNextChoseongIsㅎ(const Syllable& current, const Nullable<Syllable>& next)
{
    Syllable updatedCurrent = current;
    Nullable<Syllable> updatedNext = next.has_value() ? Nullable<Syllable>(next.value()) : std::nullopt;

    // Check if current.jongseong is in 발음변환_첫소리_ㅎ and next.choseong is 'ㅎ'
    if (_Internal::arrayIncludes<std::wstring>(발음변환_첫소리_ㅎ, updatedCurrent.jongseong) &&
        updatedNext.has_value())
    {
        if (_Internal::arrayIncludes<std::wstring>(std::vector<std::wstring>{L"ㅎ"}, updatedNext->choseong)) {
            // Replace next.choseong with 발음변환_첫소리_ㅎ_발음[current.jongseong]
            auto it = 발음변환_첫소리_ㅎ_발음.find(updatedCurrent.jongseong);
            if (it != 발음변환_첫소리_ㅎ_발음.end()) {
                updatedNext->choseong = it->second;
            }
            else {
                throw std::runtime_error("발음변환_첫소리_ㅎ_발음 mapping not found for current.jongseong.");
            }

            // If current.jongseong has length 1, remove it
            if (updatedCurrent.jongseong.length() == 1) {
                updatedCurrent.jongseong = L"";
            }
            else if (updatedCurrent.jongseong.length() > 1) {
                // Set jongseong to the first character
                updatedCurrent.jongseong = std::wstring(1, updatedCurrent.jongseong[0]);
            }
        }
    }

    return NullableReturnSyllables{updatedCurrent, updatedNext};
}

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
