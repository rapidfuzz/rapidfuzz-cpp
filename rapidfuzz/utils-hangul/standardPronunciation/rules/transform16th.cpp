#include "transform16th.hpp"
#include "_internal/utils.hpp"
#include <stdexcept>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {
namespace Rules {

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
ReturnSyllables transform16th(const Apply16항& params)
{
    // Create copies of the syllables to modify
    Syllable current = params.currentSyllable;
    Syllable next = params.nextSyllable;

    // Define the main condition: current has jongseong and next's choseong is 'ㅇ'
    bool mainCondition = !current.jongseong.empty() && next.choseong == 음가가_없는_자음;

    if (!mainCondition) {
        return ReturnSyllables{current, next};
    }

    // Ensure that index is valid to access phrase[index - 1]
    if (params.index <= 0 || params.index >= params.phrase.length()) {
        throw std::out_of_range("Invalid index for combinedSyllables calculation.");
    }

    // Combine the previous syllable and the current syllable
    std::wstring combinedSyllables =
        std::wstring(1, params.phrase[params.index - 1]) + std::wstring(1, params.phrase[params.index]);

    // Handle special Hangul characters
    ReturnSyllables result1 = handleSpecialHangulCharacters(current, next, combinedSyllables);
    current = result1.current;
    next = result1.next;

    // Handle standard Hangul characters
    ReturnSyllables result2 = handleHangulCharacters(current, next, combinedSyllables);
    current = result2.current;
    next = result2.next;

    return ReturnSyllables{current, next};
}

ReturnSyllables handleSpecialHangulCharacters(const Syllable& current, const Syllable& next,
                                              const std::wstring& combinedSyllables)
{
    Syllable updatedCurrent = current;
    Syllable updatedNext = next;

    // Check if the combined syllables are in 특별한_한글_자모
    if (_Internal::arrayIncludes<std::wstring>(특별한_한글_자모, combinedSyllables)) {
        // Find the pronunciation mapping for the current jongseong
        auto it = 특별한_한글_자모의_발음.find(updatedCurrent.jongseong);
        if (it != 특별한_한글_자모의_발음.end()) {
            std::wstring 다음_음절의_초성 = it->second;

            // Update the syllables accordingly
            updatedCurrent.jongseong = L"";
            updatedNext.choseong = 다음_음절의_초성;
        }
        else {
            throw std::runtime_error("Mapping not found in 특별한_한글_자모의_발음.");
        }
    }

    return ReturnSyllables{updatedCurrent, updatedNext};
}

ReturnSyllables handleHangulCharacters(const Syllable& current, const Syllable& next,
                                       const std::wstring& combinedSyllables)
{
    Syllable updatedCurrent = current;
    Syllable updatedNext = next;

    // Check if the combined syllables are in 한글_자모
    if (_Internal::arrayIncludes<std::wstring>(한글_자모, combinedSyllables)) {
        updatedNext.choseong = updatedCurrent.jongseong;

        // If current.jongseong is not 'ㅇ', clear it
        if (updatedCurrent.jongseong != L"ㅇ") {
            updatedCurrent.jongseong = L"";
        }
    }

    return ReturnSyllables{updatedCurrent, updatedNext};
}

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
