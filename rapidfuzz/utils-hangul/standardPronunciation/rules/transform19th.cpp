#include "transform19th.hpp"
#include <stdexcept>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {
namespace Rules {

/**
 * @brief Applies Rule 19 to standardize the pronunciation of Hangul syllables.
 */
ReturnNextSyllables transform19th(const Syllable& currentSyllable, const Syllable& nextSyllable)
{
    // Create a mutable copy of the next syllable to modify
    Syllable next = nextSyllable;

    // Define the main condition: current has jongseong and next's choseong is 'ㄹ'
    bool mainCondition = !currentSyllable.jongseong.empty() && next.choseong == L"ㄹ";

    if (!mainCondition) {
        return ReturnNextSyllables{next};
    }

    // Check if current.jongseong is in 자음동화_받침_ㄴ_변환
    if (_Internal::arrayIncludes<std::wstring>(자음동화_받침_ㄴ_변환, currentSyllable.jongseong)) {
        next.choseong = L"ㄴ";
    }

    return ReturnNextSyllables{next};
}

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
