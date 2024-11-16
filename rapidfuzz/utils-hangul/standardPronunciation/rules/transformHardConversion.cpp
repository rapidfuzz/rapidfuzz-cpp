#include "transformHardConversion.hpp"

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {
namespace Rules {

/**
 * @brief Applies Hard Consonant Assimilation (경음화) Rules 23, 24, and 25 to standardize the pronunciation
 * of Hangul syllables.
 */
ReturnNextSyllables transformHardConversion(const Syllable& currentSyllable, const Syllable& nextSyllable)
{
    // Create a mutable copy of the next syllable to modify
    Syllable next = nextSyllable;

    // Check if next.choseong is a key in 된소리 map
    if (_Internal::hasProperty(된소리, next.choseong)) {
        bool 제23항조건 = _Internal::arrayIncludes<std::wstring>(된소리_받침, currentSyllable.jongseong);
        bool 제24_25항조건 = _Internal::arrayIncludes<std::wstring>(어간_받침, currentSyllable.jongseong) &&
                             next.choseong != L"ㅂ";

        if (제23항조건 || 제24_25항조건) {
            next.choseong = 된소리.at(next.choseong); // Using .at() for safe access
        }
    }

    return ReturnNextSyllables{next};
}

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
