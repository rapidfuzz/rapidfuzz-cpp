#ifndef TRANSFORM_NL_ASSIMILATION_HPP
#define TRANSFORM_NL_ASSIMILATION_HPP

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
ReturnSyllables transformNLAssimilation(const Syllable& currentSyllable, const Syllable& nextSyllable);

ReturnSyllables applyㄴㄹ덧남(const Syllable& current, const Syllable& next);

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // TRANSFORM_NL_ASSIMILATION_HPP
