#include "transform20th.hpp"

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {
namespace Rules {

/**
 * @brief 제20항을 적용합니다.
 *
 * @param currentSyllable 현재 음절을 입력합니다.
 * @param nextSyllable 다음 음절을 입력합니다.
 * @return A ReturnSyllables struct containing the transformed current and next syllables.
 *
 * @details
 * 20항 - ‘ㄴ’은 ‘ㄹ’의 앞이나 뒤에서 [ㄹ]로 발음한다.
 * [붙임] 첫소리 ‘ㄴ’이 ‘ㅀ’, ‘ㄾ’ 뒤에 연결되는 경우에도 이에 준한다.
 */
ReturnSyllables transform20th(const Syllable& currentSyllable, const Syllable& nextSyllable)
{
    // Create mutable copies of the syllables to modify
    Syllable current = currentSyllable;
    Syllable next = nextSyllable;

    // Apply main condition: transform current.jongseong from 'ㄴ' to 'ㄹ' if next.choseong is 'ㄹ'
    ReturnCurrentSyllables resultMain = applyMainCondition(current, next);
    current = resultMain.current;

    // Apply supplementary condition: transform next.choseong from 'ㄹ' to 'ㄴ' if current.jongseong is 'ㄹ',
    // 'ㄹㅎ', or 'ㄹㅌ'
    ReturnNextSyllables resultSupplementary = applySupplementaryCondition(current, next);
    next = resultSupplementary.next;

    return ReturnSyllables{current, next};
}

ReturnCurrentSyllables applyMainCondition(const Syllable& current, const Syllable& next)
{
    Syllable updatedCurrent = current;

    if (updatedCurrent.jongseong == L"ㄴ" && next.choseong == L"ㄹ") {
        updatedCurrent.jongseong = L"ㄹ";
    }

    return ReturnCurrentSyllables{updatedCurrent};
}

ReturnNextSyllables applySupplementaryCondition(const Syllable& current, const Syllable& next)
{
    Syllable updatedNext = next;

    if (updatedNext.choseong == L"ㄴ" &&
        (current.jongseong == L"ㄹ" ||
         _Internal::arrayIncludes<std::wstring>({L"ㄹㅎ", L"ㄹㅌ"}, current.jongseong)))
    {
        updatedNext.choseong = L"ㄹ";
    }

    return ReturnNextSyllables{updatedNext};
}

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
