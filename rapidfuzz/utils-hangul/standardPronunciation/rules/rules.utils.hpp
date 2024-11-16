#ifndef RULES_UTILS_HPP
#define RULES_UTILS_HPP

#include "rules.types.hpp"
#include <string>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {
namespace Rules {

/**
 * @brief Replaces all occurrences of 'ㅎ' in the jongseong (final consonant) of the given syllable.
 *
 * @param currentSyllable The syllable whose jongseong is to be modified.
 * @return The modified jongseong with all 'ㅎ's removed.
 *
 * @example
 * Syllable syllable = {L"ㅇ", L"ㅏ", L"ㄹㅎ"};
 * std::wstring newJongseong = replace받침ㅎ(syllable); // newJongseong = L"ㄹ"
 */
std::wstring replace받침ㅎ(const Syllable& currentSyllable);

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // RULES_UTILS_HPP
