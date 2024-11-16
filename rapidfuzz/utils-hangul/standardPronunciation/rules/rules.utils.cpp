#include "rules.utils.hpp"
#include <algorithm> // For std::remove
#include <stdexcept> // For exception handling

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
std::wstring replaceBatchimH(const Syllable& currentSyllable)
{
    if (currentSyllable.jongseong.empty()) {
        return L"";
    }

    std::wstring modifiedJongseong = currentSyllable.jongseong;
    // Remove all occurrences of 'ㅎ'
    modifiedJongseong.erase(std::remove(modifiedJongseong.begin(), modifiedJongseong.end(), L'ㅎ'),
                            modifiedJongseong.end());

    return modifiedJongseong;
}

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
