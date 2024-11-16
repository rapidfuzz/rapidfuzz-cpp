#include "transform18th.hpp"
#include "_internal/utils.hpp"
#include <stdexcept>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {
namespace Rules {

/**
 * @brief 제18항을 적용합니다.
 *
 * @param currentSyllable 현재 음절을 입력합니다.
 * @param nextSyllable 다음 음절을 입력합니다.
 * @returns 18항이 적용되었는지의 여부를 반환합니다.
 *
 * @details
 * 18항 - 받침 ‘ㄱ(ㄲ, ㅋ, ㄳ, ㄺ), ㄷ(ㅅ, ㅆ, ㅈ, ㅊ, ㅌ, ㅎ), ㅂ(ㅍ, ㄼ, ㄿ, ㅄ)’은 ‘ㄴ, ㅁ’ 앞에서 [ㅇ, ㄴ,
 * ㅁ]으로 발음한다.
 */
ReturnCurrentSyllables transform18th(const Syllable& currentSyllable, const Syllable& nextSyllable)
{
    // Create a copy of the current syllable to modify
    Syllable current = currentSyllable;

    // Define the main condition: current has jongseong and next's choseong is 'ㄴ' or 'ㅁ'
    bool 제18항주요조건 = !current.jongseong.empty() &&
                          _Internal::arrayIncludes<std::wstring>({L"ㄴ", L"ㅁ"}, nextSyllable.choseong);

    if (!제18항주요조건) {
        return ReturnCurrentSyllables{current};
    }

    if (_Internal::arrayIncludes<std::wstring>(비음화_받침_ㅇ_변환, current.jongseong)) {
        current.jongseong = L"ㅇ";
    }

    if (_Internal::arrayIncludes<std::wstring>(비음화_받침_ㄴ_변환, current.jongseong)) {
        current.jongseong = L"ㄴ";
    }

    if (_Internal::arrayIncludes<std::wstring>(비음화_받침_ㅁ_변환, current.jongseong)) {
        current.jongseong = L"ㅁ";
    }

    return ReturnCurrentSyllables{current};
}

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
