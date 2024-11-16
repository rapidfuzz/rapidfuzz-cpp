#ifndef TRANSFORM_18TH_HPP
#define TRANSFORM_18TH_HPP

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
 * @brief 제18항을 적용합니다.
 *
 * @param currentSyllable 현재 음절을 입력합니다.
 * @param nextSyllable 다음 음절을 입력합니다.
 * @returns 18항이 적용되었는지의 여부를 반환합니다.
 *
 * @details
 * 18항 - 받침 ‘ㄱ(ㄲ, ㅋ, ㄳ, ㄺ), ㄷ(ㅅ, ㅆ, ㅈ, ㅊ, ㅌ, ㅎ), ㅂ(ㅍ, ㄼ, ㄿ, ㅄ)’은 ‘ㄴ, ㅁ’ 앞에서 [ㅇ, ㄴ,
 * ㅁ]으로 발음한다.
 */
ReturnCurrentSyllables transform18th(const Syllable& currentSyllable, const Syllable& nextSyllable);

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // TRANSFORM_18TH_HPP
