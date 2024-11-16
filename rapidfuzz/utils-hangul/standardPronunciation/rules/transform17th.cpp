#include "transform17th.hpp"
#include "_internal/utils.hpp"
#include <stdexcept>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {
namespace Rules {

/**
 * @brief 제17항을 적용합니다.
 * @description 17항 - 받침 ‘ㄷ', 'ㅌ(ㄾ)’이 조사나 접미사의 모음 ‘ㅣ’와 결합되는 경우에는, [ㅈ, ㅊ]으로
 * 바꾸어서 뒤 음절 첫소리로 옮겨 발음한다.
 * @description [붙임] ‘ㄷ’ 뒤에 접미사 ‘히’가 결합되어 ‘티’를 이루는 것은 [치]로 발음한다.
 * @param currentSyllable 현재 음절을 입력합니다.
 * @param nextSyllable 다음 음절을 입력합니다.
 * @returns 17항이 적용되었는지의 여부를 반환합니다.
 *
 * @details
 * 17 항 - 받침 ‘ㄷ', 'ㅌ(ㄾ)’이 조사나 접미사의 모음 ‘ㅣ’와 결합되는 경우에는, [ㅈ, ㅊ]으로
 * 바꾸어서 뒤 음절 첫소리로 옮겨 발음한다.
 * [붙임] ‘ㄷ’ 뒤에 접미사 ‘히’가 결합되어 ‘티’를 이루는 것은 [치]로 발음한다.
 */
ReturnSyllables transform17th(const Syllable& currentSyllable, const Syllable& nextSyllable)
{
    // Create copies of the syllables to modify
    Syllable current = currentSyllable;
    Syllable next = nextSyllable;

    // Define the main condition: next.jungseong === 'ㅣ'
    bool mainCondition = (next.jungseong == L"ㅣ");

    if (!mainCondition) {
        return ReturnSyllables{current, next};
    }

    // Handle next syllable's choseong being 'ㅇ'
    ReturnSyllables result1 = handleChoseongIsㅇ(current, next);
    current = result1.current;
    next = result1.next;

    // Handle next syllable's choseong being 'ㅎ' and current jongseong is 'ㄷ'
    ReturnSyllables result2 = handleChoseongIsㅎAndㄷ(current, next);
    current = result2.current;
    next = result2.next;

    return ReturnSyllables{current, next};
}

ReturnSyllables handleChoseongIsㅇ(const Syllable& current, const Syllable& next)
{
    Syllable updatedCurrent = current;
    Syllable updatedNext = next;

    // Check if next.choseong is 'ㅇ' and current.jongseong exists in 음의_동화_받침
    if (updatedNext.choseong == L"ㅇ") {
        auto it = 음의_동화_받침.find(updatedCurrent.jongseong);
        if (it != 음의_동화_받침.end()) {
            // Update next.choseong with the mapped value
            updatedNext.choseong = it->second;

            // Update current.jongseong
            if (updatedCurrent.jongseong == L"ㄹㅌ") {
                updatedCurrent.jongseong = L"ㄹ";
            }
            else {
                updatedCurrent.jongseong = L"";
            }
        }
    }

    return ReturnSyllables{updatedCurrent, updatedNext};
}

ReturnSyllables handleChoseongIsㅎAndㄷ(const Syllable& current, const Syllable& next)
{
    Syllable updatedCurrent = current;
    Syllable updatedNext = next;

    // Check if next.choseong is 'ㅎ' and current.jongseong is 'ㄷ'
    if (updatedNext.choseong == L"ㅎ" && updatedCurrent.jongseong == L"ㄷ") {
        // Transform 'ㄷ' > 'ㅊ' and move to next syllable's choseong
        updatedNext.choseong = L"ㅊ";
        updatedCurrent.jongseong = L"";
    }

    return ReturnSyllables{updatedCurrent, updatedNext};
}

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
