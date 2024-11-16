#ifndef TRANSFORM_17TH_HPP
#define TRANSFORM_17TH_HPP

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
ReturnSyllables transform17th(const Syllable& currentSyllable, const Syllable& nextSyllable);

/**
 * @brief Handles the transformation when the next syllable's choseong is 'ㅇ' based on Rule 17.
 *
 * @param current The current syllable to be transformed.
 * @param next The next syllable to be transformed.
 * @return A ReturnSyllables struct containing the transformed current and next syllables.
 */
ReturnSyllables handleChoseongIsㅇ(const Syllable& current, const Syllable& next);

/**
 * @brief Handles the transformation when the next syllable's choseong is 'ㅎ' and current jongseong is
 * 'ㄷ' based on Rule 17.
 *
 * @param current The current syllable to be transformed.
 * @param next The next syllable to be transformed.
 * @return A ReturnSyllables struct containing the transformed current and next syllables.
 */
ReturnSyllables handleChoseongIsㅎAndㄷ(const Syllable& current, const Syllable& next);

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // TRANSFORM_17TH_HPP
