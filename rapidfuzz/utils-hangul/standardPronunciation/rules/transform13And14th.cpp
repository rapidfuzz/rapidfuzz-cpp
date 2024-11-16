#include "transform13And14th.hpp"
#include "_internal/utils.hpp"
#include <stdexcept>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {
namespace Rules {

/**
 * @brief 제13, 14항을 적용합니다.
 *
 * @param currentSyllable 현재 음절을 입력합니다.
 * @param nextSyllable 다음 음절을 입력합니다.
 * @returns 13, 14항이 적용되었는지의 여부를 반환합니다.
 *
 * @details
 * 제13항 - 홑받침이나 쌍받침이 모음으로 시작된 조사나 어미, 접미사와 결합되는 경우에는, 제 음가대로 뒤 음절
 * 첫소리로 옮겨 발음한다.
 * 제14항 - 겹받침이 모음으로 시작된 조사나 어미, 접미사와 결합되는 경우에는, 뒤엣것만을 뒤 음절 첫소리로 옮겨
 * 발음한다.
 */
ReturnSyllables transform13And14th(const Syllable& currentSyllable, const Syllable& nextSyllable)
{
    // Create copies of the syllables to modify
    Syllable current = currentSyllable;
    Syllable next = nextSyllable;

    // Define the main condition: current has jongseong and next's choseong is 'ㅇ'
    bool 제13_14항주요조건 = !current.jongseong.empty() && next.choseong == 음가가_없는_자음;

    if (!제13_14항주요조건) {
        return ReturnSyllables{current, next};
    }

    ReturnSyllables result1 = handle홑받침or쌍받침(current, next);
    current = result1.current;
    next = result1.next;

    ReturnSyllables result2 = handle겹받침(current, next);
    current = result2.current;
    next = result2.next;

    return ReturnSyllables{current, next};
}

/**
 * @brief Determines if the given syllable has a single final consonant (홑받침).
 */
bool is홑받침(const Syllable& current)
{
    return current.jongseong.length() == 받침의길이.at(L"홀받침");
}

/**
 * @brief Determines if the given syllable has a double final consonant (쌍받침).
 */
bool is쌍받침(const Syllable& current)
{
    return current.jongseong.length() == 받침의길이.at(L"쌍_겹받침") &&
           current.jongseong[0] == current.jongseong[1];
}

/**
 * @brief Determines if the given syllable has a complex final consonant (겹받침).
 */
bool is겹받침(const Syllable& current)
{
    return current.jongseong.length() == 받침의길이.at(L"쌍_겹받침") &&
           current.jongseong[0] != current.jongseong[1];
}

/**
 * @brief Handles the transformation when the syllable has a single or double final consonant.
 */
ReturnSyllables handle홑받침or쌍받침(const Syllable& current, const Syllable& next)
{
    Syllable updatedCurrent = current;
    Syllable updatedNext = next;

    // Define the target choseongs that are vowels ('ㅇ' and empty string)
    std::vector<std::wstring> targetChoseongs = {L"ㅇ", L""};

    // Check if current.jongseong is not 'ㅇ' or empty and is either single or double final consonant
    if (!_Internal::arrayIncludes<std::wstring>(targetChoseongs, updatedCurrent.jongseong) &&
        (is홑받침(updatedCurrent) || is쌍받침(updatedCurrent)))
    {

        // Move the final consonant to the next syllable's initial consonant
        updatedNext.choseong = updatedCurrent.jongseong;

        // Remove the final consonant from the current syllable
        updatedCurrent.jongseong = L"";
    }

    return ReturnSyllables{updatedCurrent, updatedNext};
}

/**
 * @brief Handles the transformation when the syllable has a complex final consonant.
 */
ReturnSyllables handle겹받침(const Syllable& current, const Syllable& next)
{
    Syllable updatedCurrent = current;
    Syllable updatedNext = next;

    if (is겹받침(updatedCurrent)) {
        // Check if the second consonant is 'ㅅ'
        if (updatedCurrent.jongseong[1] == L'ㅅ') {
            updatedNext.choseong = L"ㅆ";
        }
        else {
            // Move the second consonant to the next syllable's initial consonant
            updatedNext.choseong = std::wstring(1, updatedCurrent.jongseong[1]);
        }

        // Remove the second consonant from the current syllable's jongseong
        updatedCurrent.jongseong = replace받침ㅎ(updatedCurrent);
    }

    return ReturnSyllables{updatedCurrent, updatedNext};
}

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
