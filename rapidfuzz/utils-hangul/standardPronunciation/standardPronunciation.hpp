#ifndef STANDARD_PRONUNCIATION_HPP
#define STANDARD_PRONUNCIATION_HPP

#include "_internal/hangul.hpp" // Includes isHangulAlphabet, isHangulCharacter
#include "_internal/utils.hpp"
#include "combineCharacter/combineCharacter.hpp"
#include "disassembleCompleteCharacter/disassembleCompleteCharacter.hpp"
#include "rules/rules.types.hpp" // Include Nullable, Syllables
#include <string>
#include <vector>

// Include transformation rule headers
#include "./rules/transform12th.hpp"
#include "./rules/transform13And14th.hpp"
#include "./rules/transform16th.hpp"
#include "./rules/transform17th.hpp"
#include "./rules/transform18th.hpp"
#include "./rules/transform19th.hpp"
#include "./rules/transform20th.hpp"
#include "./rules/transform9And10And11th.hpp"
#include "./rules/transformHardConversion.hpp"
#include "./rules/transformNLAssimilation.hpp"

using namespace RapidFuzz::Utils::Hangul::StandardPronunciation::Rules;

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {

/**
 * @brief Options for the standardizePronunciation function.
 */
struct Options {
    bool hardConversion = true; ///< Whether to apply hard consonant assimilation (경음화). Default is true.
};

/**
 * @brief Represents non-Hangul characters with their index and the character.
 */
struct NotHangul {
    int index;             ///< The position of the non-Hangul character in the original string.
    std::wstring syllable; ///< The non-Hangul character.
};

/**
 * @brief Represents the parameters required for applying transformation rules.
 */
struct ApplyParameters {
    Syllable currentSyllable;        ///< The current syllable to be transformed.
    Nullable<Syllable> nextSyllable; ///< The next syllable to be transformed, if any.
    int index;                       ///< The index of the current syllable in the syllable array.
    std::wstring phrase;             ///< The original phrase string.
    Options options;                 ///< Transformation options.
};

/**
 * @brief 주어진 한글 문자열을 표준 발음으로 변환합니다.
 *
 * @param hangul 한글 문자열
 * @param options 변환 옵션
 * @param options.hardConversion 경음화 등의 된소리를 적용할지 여부를 설정합니다. 기본값은 true입니다.
 * @return 변환된 표준 발음 문자열을 반환합니다.
 */
std::wstring standardizePronunciation(const std::wstring& hangul, const Options& options = Options());

} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // STANDARD_PRONUNCIATION_HPP
