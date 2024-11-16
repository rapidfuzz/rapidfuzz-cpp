#ifndef TRANSFORM_20TH_HPP
#define TRANSFORM_20TH_HPP

#include "../constants.hpp"
#include "_internal/utils.hpp"
#include "rules.types.hpp"
#include <string>

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
ReturnSyllables transform20th(const Syllable& currentSyllable, const Syllable& nextSyllable);

ReturnCurrentSyllables applyMainCondition(const Syllable& current, const Syllable& next);

ReturnNextSyllables applySupplementaryCondition(const Syllable& current, const Syllable& next);

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // TRANSFORM_20TH_HPP
