#ifndef TRANSFORM_HARD_CONVERSION_HPP
#define TRANSFORM_HARD_CONVERSION_HPP

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
 * @brief 제6장 경음화를 적용합니다.
 *
 * @param currentSyllable 현재 음절
 * @param nextSyllable 다음 음절
 * @return A ReturnNextSyllables struct containing the transformed next syllable.
 *
 * @details
 * 제23항 - 받침 ‘ㄱ(ㄲ, ㅋ, ㄳ, ㄺ), ㄷ(ㅅ, ㅆ, ㅈ, ㅊ, ㅌ), ㅂ(ㅍ, ㄼ, ㄿ, ㅄ)’ 뒤에 연결되는
 * ‘ㄱ, ㄷ, ㅂ, ㅅ, ㅈ’은 된소리로 발음한다.
 * 제24항 - 어간 받침 ‘ㄴ(ㄵ), ㅁ(ㄻ)’ 뒤에 결합되는 어미의 첫소리 ‘ㄱ, ㄷ, ㅅ, ㅈ’은 된소리로 발음한다.
 * 제25항 - 어간 받침 ‘ㄼ, ㄾ’ 뒤에 결합되는 어미의 첫소리 ‘ㄱ, ㄷ, ㅅ, ㅈ’은 된소리로 발음한다.
 */
ReturnNextSyllables transformHardConversion(const Syllable& currentSyllable, const Syllable& nextSyllable);

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // TRANSFORM_HARD_CONVERSION_HPP
