#ifndef TRANSFORM_13_AND_14TH_HPP
#define TRANSFORM_13_AND_14TH_HPP

#include "../constants.hpp"
#include "rules.types.hpp"
#include "rules.utils.hpp"
#include <string>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {
namespace Rules {

inline const std::unordered_map<std::wstring, int> 받침의길이 = {{L"홀받침", 1}, {L"쌍_겹받침", 2}};

/**
 * @brief 제13, 14항을 적용합니다.
 *
 * @param currentSyllable 현재 음절을 입력합니다.
 * @param nextSyllable 다음 음절을 입력합니다.
 * @returns 13, 14항이 적용되었는지의 여부를 반환합니다.
 *
 * @details
 * 제13항 - 홑받침이나 쌍받침이 모음으로 시작된 조사나 어미, 접미사와 결합되는 경우에는, 제 음가대로 뒤 음절
 * 첫소리로 옮겨 발음한다.
 * 제14항 - 겹받침이 모음으로 시작된 조사나 어미, 접미사와 결합되는 경우에는, 뒤엣것만을 뒤 음절 첫소리로 옮겨
 * 발음한다.
 */
ReturnSyllables transform13And14th(const Syllable& currentSyllable, const Syllable& nextSyllable);

bool is홑받침(const Syllable& current);

bool is쌍받침(const Syllable& current);

bool is겹받침(const Syllable& current);

ReturnSyllables handle홑받침or쌍받침(const Syllable& current, const Syllable& next);

ReturnSyllables handle겹받침(const Syllable& current, const Syllable& next);

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // TRANSFORM_13_AND_14TH_HPP
