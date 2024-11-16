#ifndef TRANSFORM_12TH_HPP
#define TRANSFORM_12TH_HPP

#include "../constants.hpp"
#include "rules.types.hpp"
#include "rules.utils.hpp"
#include <string>

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
NullableReturnSyllables transform12th(const Syllable& currentSyllable,
                                      const Nullable<Syllable>& nextSyllable);

ReturnSyllables handleNextChoseongIsㄱㄷㅈㅅ(const Syllable& current, const Syllable& next);

ReturnSyllables handleNextChoseongIsㄴ(const Syllable& current, const Syllable& next);

ReturnSyllables handleNextChoseongIsㅇ(const Syllable& current, const Syllable& next);

ReturnSyllables handleCurrentJongseongIsㅇ(const Syllable& current);

NullableReturnSyllables handleNextChoseongIsㅎ(const Syllable& current, const Nullable<Syllable>& next);

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // TRANSFORM_12TH_HPP
