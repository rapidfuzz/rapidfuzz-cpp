#include "transform9And10And11th.hpp"
#include "../constants.hpp"
#include "_internal/utils.hpp"

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
                                              const Nullable<Syllable>& nextSyllable)
{
    // Create a copy of the current syllable to modify
    Syllable modifiedCurrent = currentSyllable;

    // Determine if the current syllable is at the end of a word (no next syllable)
    bool is어말 = !currentSyllable.jongseong.empty() && !nextSyllable.has_value();

    // Determine if the current syllable is before a syllable starting with a consonant that has pronunciation
    bool is음가있는자음앞 = false;
    if (!currentSyllable.jongseong.empty() && nextSyllable.has_value()) {
        // Check if the next syllable's choseong is not in 음가가_없는_자음
        const std::wstring& nextChoseong = nextSyllable->choseong;
        if (nextChoseong != 음가가_없는_자음) {
            is음가있는자음앞 = true;
        }
    }

    // Main condition: either end of word or before consonant with pronunciation
    bool 제9_10_11항주요조건 =
        (is어말 || is음가있는자음앞) && _Internal::hasProperty(받침_대표음_발음, currentSyllable.jongseong);

    if (제9_10_11항주요조건) {
        // Replace the jongseong with its representative pronunciation
        auto it = 받침_대표음_발음.find(currentSyllable.jongseong);
        if (it != 받침_대표음_발음.end()) {
            modifiedCurrent.jongseong = it->second;
        }
        else {
            // This should not happen as we checked hasProperty earlier
            throw std::runtime_error("Unsupported jongseong encountered in transform9And10And11th.");
        }
    }

    return ReturnCurrentSyllables{modifiedCurrent};
}

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
