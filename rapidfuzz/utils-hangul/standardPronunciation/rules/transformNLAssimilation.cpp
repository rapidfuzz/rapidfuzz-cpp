#include "transformNLAssimilation.hpp"

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {
namespace Rules {

/**
 * @brief 'ㄴ,ㄹ'이 덧나는 경우(동화작용)를 적용합니다.
 *
 * @param currentSyllable 현재 음절
 * @param nextSyllable 다음 음절
 * @return A ReturnSyllables struct containing the transformed current and next syllables.
 *
 * @details
 * 합성어에서 둘째 요소가 ‘야, 여, 요, 유, 얘, 예’ 등으로 시작되는 말이면 ‘ㄴ, ㄹ’이 덧난다
 *
 * @ref
 * - https://www.youtube.com/watch?v=Mm2JX2naqWk
 * - http://contents2.kocw.or.kr/KOCW/data/document/2020/seowon/choiyungon0805/12.pdf
 */
ReturnSyllables transformNLAssimilation(const Syllable& currentSyllable, const Syllable& nextSyllable)
{
    // Create mutable copies of the syllables to modify
    Syllable current = currentSyllable;
    Syllable next = nextSyllable;

    // Define the assimilation condition:
    // - Current syllable has jongseong
    // - Next syllable's choseong is 'ㅇ'
    // - Next syllable's jungseong is in ㄴㄹ이_덧나는_후속음절_모음
    bool ㄴㄹ이덧나는조건 =
        !current.jongseong.empty() && next.choseong == L"ㅇ" &&
        _Internal::arrayIncludes<std::wstring>(ㄴㄹ이_덧나는_후속음절_모음, next.jungseong);

    if (!ㄴㄹ이덧나는조건) {
        return ReturnSyllables{current, next};
    }

    // Apply 'ㄴ,ㄹ' Assimilation
    ReturnSyllables result = applyㄴㄹ덧남(current, next);
    current = result.current;
    next = result.next;

    return ReturnSyllables{current, next};
}

/**
 * @brief Handles the 'ㄴ,ㄹ' Assimilation condition.
 */
ReturnSyllables applyNLAssimilation(const Syllable& current, const Syllable& next)
{
    Syllable updatedCurrent = current;
    Syllable updatedNext = next;

    if (_Internal::arrayIncludes<std::wstring>(ㄴㄹ이_덧나는_모음, updatedCurrent.jungseong)) {
        if (_Internal::arrayIncludes<std::wstring>(ㄴㄹ이_덧나서_받침_ㄴ_변환, updatedCurrent.jongseong)) {
            if (updatedCurrent.jongseong == L"ㄱ") {
                updatedCurrent.jongseong = L"ㅇ";
            }
            updatedNext.choseong = L"ㄴ";
        }

        if (_Internal::arrayIncludes<std::wstring>(ㄴㄹ이_덧나서_받침_ㄹ_변환, updatedCurrent.jongseong)) {
            updatedNext.choseong = L"ㄹ";
        }
    }
    else {
        // ㄴ/ㄹ이 되기 위한 조건이지만 현재 음절의 중성의 ∙(아래아)가 하나가 아닐 경우에는 덧나지 않고
        // 연음규칙이 적용된다
        updatedNext.choseong = updatedCurrent.jongseong;
    }

    return ReturnSyllables{updatedCurrent, updatedNext};
}

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
