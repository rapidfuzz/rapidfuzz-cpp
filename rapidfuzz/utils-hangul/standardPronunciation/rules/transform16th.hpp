#ifndef TRANSFORM_16TH_HPP
#define TRANSFORM_16TH_HPP

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
 * @brief Struct representing the parameters required to apply Rule 16.
 */
struct Apply16항 {
    Syllable currentSyllable; ///< 현재 음절
    Syllable nextSyllable;    ///< 다음 음절
    std::wstring phrase;      ///< 분리되지 않은 한글 구절
    int index;                ///< 현재 음절의 순서
};

/**
 * @brief 제16항을 적용합니다.
 *
 * @param params The parameters required to apply Rule 16, encapsulated in an Apply16항 struct.
 * @return 16항이 적용되었는지의 여부를 반환합니다.
 *
 * @details
 * 제16항 - 한글 자모의 이름은 그 받침소리를 연음하되, ‘ㄷ, ㅈ, ㅊ, ㅋ, ㅌ, ㅍ, ㅎ’의 경우에는
 * 특별히 다음과 같이 발음한다.
 * - ㄷ, ㅈ, ㅊ, ㅌ, ㅎ > ㅅ (디귿이:디그시, 지읒이:지으시, 치읓이:치으시, 티읕이:티으시, 히읗이:히으시)
 * - ㅋ > ㄱ (키읔이:키으기)
 * - ㅍ > ㅂ (피읖이:피으비)
 */
ReturnSyllables transform16th(const Apply16항& params);

ReturnSyllables handleSpecialHangulCharacters(const Syllable& current, const Syllable& next,
                                              const std::wstring& combinedSyllables);

ReturnSyllables handleHangulCharacters(const Syllable& current, const Syllable& next,
                                       const std::wstring& combinedSyllables);

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // TRANSFORM_16TH_HPP
