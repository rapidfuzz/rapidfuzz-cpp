#include "standardPronunciation.hpp"

#include <unordered_map>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {

/**
 * @brief Decomposes the Hangul phrase into Hangul syllables and non-Hangul characters.
 *
 * @param hangulPhrase The Hangul phrase to decompose.
 * @return A pair containing vectors of NotHangul and Syllable.
 */
struct DecompositionResult {
    std::vector<NotHangul> notHangulPhrase;  ///< Non-Hangul characters with their indices.
    std::vector<Syllable> disassembleHangul; ///< Hangul syllables decomposed into Syllable structs.
};

/**
 * @brief Applies transformation rules to the current and next syllables.
 *
 * @param params The parameters required for applying the rules.
 * @return A NullableReturnSyllables struct containing the transformed current and next syllables.
 */
NullableReturnSyllables applyRules(const ApplyParameters& params)
{
    Syllable current = params.currentSyllable;
    Nullable<Syllable> next = params.nextSyllable;

    // Apply Hard Conversion if enabled
    if (next.has_value() && params.options.hardConversion) {
        ReturnNextSyllables hardConversionResult = transformHardConversion(current, next.value());
        next = hardConversionResult.next;
    }

    // Apply additional transformations if next syllable exists
    if (next.has_value()) {
        // Apply transform16th
        Apply16항 apply16thParams{current, next.value(), params.phrase, params.index};
        ReturnSyllables transform16thResult = transform16th(apply16thParams);
        current = transform16thResult.current;
        next = transform16thResult.next;

        // Apply transform17th
        ReturnSyllables transform17thResult = transform17th(current, next.value());
        current = transform17thResult.current;
        next = transform17thResult.next;

        // Apply transform19th
        ReturnNextSyllables transform19thResult = transform19th(current, next.value());
        next = transform19thResult.next;

        // Apply transformNLAssimilation
        ReturnSyllables transformNLAssimilationResult = transformNLAssimilation(current, next.value());
        current = transformNLAssimilationResult.current;
        next = transformNLAssimilationResult.next;

        // Apply transform18th
        ReturnCurrentSyllables transform18thResult = transform18th(current, next.value());
        current = transform18thResult.current;

        // Apply transform20th
        ReturnSyllables transform20thResult = transform20th(current, next.value());
        current = transform20thResult.current;
        next = transform20thResult.next;
    }

    // Apply transform12th regardless of next syllable
    NullableReturnSyllables transform12thResult = transform12th(current, next);
    current = transform12thResult.current;
    next = transform12thResult.next;

    // Apply transform13And14th if next syllable exists
    if (next.has_value()) {
        ReturnSyllables transform13And14thResult = transform13And14th(current, next.value());
        current = transform13And14thResult.current;
        next = transform13And14thResult.next;
    }

    // Apply transform9And10And11th regardless of next syllable
    ReturnCurrentSyllables transform9And10And11thResult = transform9And10And11th(current, next);
    current = transform9And10And11thResult.current;

    return NullableReturnSyllables{current, next};
}

/**
 * @brief Applies transformation rules to the syllables.
 *
 * @param syllables The vector of syllables to process.
 * @param phrase The original phrase string.
 * @param options The transformation options.
 * @return The vector of transformed syllables.
 */
std::vector<Syllable> processSyllables(std::vector<Syllable>& syllables, const std::wstring& phrase,
                                       const Options& options)
{
    for (size_t index = 0; index < syllables.size(); ++index) {
        Syllable& currentSyllable = syllables[index];
        Nullable<Syllable> nextSyllable =
            (index < syllables.size() - 1) ? std::make_optional(syllables[index + 1]) : std::nullopt;

        ApplyParameters params{currentSyllable, nextSyllable, static_cast<int>(index), phrase, options};

        NullableReturnSyllables transformed = applyRules(params);

        syllables[index] = transformed.current;
        if (transformed.next.has_value()) {
            syllables[index + 1] = transformed.next.value();
        }
    }
    return syllables;
}

/**
 * @brief Decomposes a Hangul phrase into Hangul syllables and identifies non-Hangul characters.
 *
 * @param hangulPhrase The Hangul phrase to decompose.
 * @return A DecompositionResult struct containing non-Hangul characters and Hangul syllables.
 */
DecompositionResult 음절분해(const std::wstring& hangulPhrase)
{
    DecompositionResult result;
    for (size_t index = 0; index < hangulPhrase.size(); ++index) {
        const wchar_t syllable = hangulPhrase[index];
        std::wstring syllableStr(1, syllable);
        if (!_Internal::isHangulCharacter(syllable) || _Internal::isHangulAlphabet(syllable)) {
            // Non-Hangul character
            result.notHangulPhrase.emplace_back(NotHangul{static_cast<int>(index), syllableStr});
        }
        else {
            // Hangul syllable
            auto disassembled = DisassembleCompleteCharacter::disassembleCompleteCharacter(syllableStr);
            if (disassembled.has_value()) {
                result.disassembleHangul.emplace_back(disassembled.value());
            }
        }
    }
    return result;
}

/**
 * @brief Assembles the transformed syllables and reinserts non-Hangul characters.
 *
 * @param processedSyllables The vector of transformed syllables.
 * @param notHangulPhrase The vector of non-Hangul characters with their indices.
 * @return The final transformed Hangul phrase as a std::wstring.
 */
std::wstring assembleChangedHangul(const std::vector<Syllable>& processedSyllables,
                                   const std::vector<NotHangul>& notHangulPhrase)
{
    std::wstring changedSyllables;
    size_t syllableIndex = 0;
    size_t totalLength = processedSyllables.size() + notHangulPhrase.size();

    // Create a map of index to non-Hangul syllable for quick lookup
    std::unordered_map<int, std::wstring> nonHangulMap;
    for (const auto& nonHangul : notHangulPhrase) {
        nonHangulMap[nonHangul.index] = nonHangul.syllable;
    }

    for (size_t i = 0; i < totalLength; ++i) {
        // Check if current index has a non-Hangul syllable
        auto it = nonHangulMap.find(static_cast<int>(i));
        if (it != nonHangulMap.end()) {
            changedSyllables += it->second;
        }
        else if (syllableIndex < processedSyllables.size()) {
            // Combine the syllable into a single character
            changedSyllables += CombineCharacter::combineCharacter(
                processedSyllables[syllableIndex].choseong, processedSyllables[syllableIndex].jungseong,
                processedSyllables[syllableIndex].jongseong);
            syllableIndex++;
        }
    }

    return changedSyllables;
}

/**
 * @brief Transforms a Hangul phrase by applying all pronunciation standardization rules.
 *
 * @param phrase The Hangul phrase to transform.
 * @param options The transformation options.
 * @return The transformed Hangul phrase as a std::wstring.
 */
std::wstring transformHangulPhrase(const std::wstring& phrase, const Options& options)
{
    DecompositionResult decomposition = 음절분해(phrase);
    std::vector<Syllable> processedSyllables =
        processSyllables(decomposition.disassembleHangul, phrase, options);

    return assembleChangedHangul(processedSyllables, decomposition.notHangulPhrase);
}

/**
 * @brief 주어진 한글 문자열을 표준 발음으로 변환합니다.
 *
 * @param hangul 한글 문자열
 * @param options 변환 옵션
 * @param options.hardConversion 경음화 등의 된소리를 적용할지 여부를 설정합니다. 기본값은 true입니다.
 * @return 변환된 표준 발음 문자열을 반환합니다.
 */
std::wstring standardizePronunciation(const std::wstring& hangul, const Options& options)
{
    if (hangul.empty()) {
        return L"";
    }

    // Split the input string by spaces into phrases
    std::vector<std::wstring> phrases;
    size_t start = 0;
    size_t pos = hangul.find(L' ', start);
    while (pos != std::wstring::npos) {
        phrases.emplace_back(hangul.substr(start, pos - start));
        start = pos + 1;
        pos = hangul.find(L' ', start);
    }
    phrases.emplace_back(hangul.substr(start));

    // Transform each phrase and concatenate the results with spaces
    std::wstring transformed;
    for (size_t i = 0; i < phrases.size(); ++i) {
        transformed += transformHangulPhrase(phrases[i], options);
        if (i != phrases.size() - 1) {
            transformed += L' ';
        }
    }

    return transformed;
}

} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
