#ifndef TRANSFORM9_AND_10_AND_11TH_HPP
#define TRANSFORM9_AND_10_AND_11TH_HPP

#include "rules.types.hpp"
#include <string>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {
namespace Rules {

/**
 * @brief 제9, 10항, 11항을 적용합니다.
 *
 * @param currentSyllable 현재 음절을 입력합니다.
 * @param nextSyllable 다음 음절을 입력합니다.
 * @return A ReturnCurrentSyllables struct containing the transformed current syllable.
 *
 * @details
 * 제9항 - 받침 ‘ㄲ, ㅋ’, ‘ㅅ, ㅆ, ㅈ, ㅊ, ㅌ’, ‘ㅍ’은 어말 또는 자음 앞에서 각각 대표음 [ㄱ, ㄷ,
 * ㅂ]으로 발음한다.
 * 제10항 - 겹받침 ‘ㄳ’, ‘ㄵ’, ‘ㄼ, ㄽ, ㄾ’, ‘ㅄ’은 어말 또는 자음 앞에서 각각 [ㄱ, ㄴ, ㄹ, ㅂ]으로 발음한다.
 * 제11항 - 겹받침 ‘ㄺ, ㄻ, ㄿ’은 어말 또는 자음 앞에서 각각 [ㄱ, ㅁ, ㅂ]으로 발음한다.
 *
 */
ReturnCurrentSyllables transform9And10And11th(const Syllable& currentSyllable,
                                              const Nullable<Syllable>& nextSyllable);

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // TRANSFORM9_AND_10_AND_11TH_HPP
