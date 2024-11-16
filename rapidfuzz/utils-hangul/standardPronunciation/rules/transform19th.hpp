#ifndef TRANSFORM_19TH_HPP
#define TRANSFORM_19TH_HPP

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
 * @brief 제19항을 적용합니다.
 *
 * @param currentSyllable 현재 음절을 입력합니다.
 * @param nextSyllable 다음 음절을 입력합니다.
 * @return A ReturnNextSyllables struct containing the transformed next syllable.
 *
 * @details
 * 19항 - 받침 ‘ㅁ, ㅇ’ 뒤에 연결되는 ‘ㄹ’은 [ㄴ]으로 발음한다.
 * [붙임] 받침 ‘ㄱ, ㅂ’ 뒤에 연결되는 ‘ㄹ’도 [ㄴ]으로 발음한다.
 */
ReturnNextSyllables transform19th(const Syllable& currentSyllable, const Syllable& nextSyllable);

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // TRANSFORM_19TH_HPP
