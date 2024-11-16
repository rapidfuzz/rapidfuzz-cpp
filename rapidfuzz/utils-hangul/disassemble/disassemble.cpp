#include "disassemble.hpp"
#include <stdexcept>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace Disassemble {

static std::vector<std::wstring> _splitStringToWstrings(const std::wstring& str)
{
    std::vector<std::wstring> result;
    result.reserve(str.size());
    for (wchar_t wc : str) {
        result.emplace_back(1, wc);
    }
    return result;
}

std::vector<std::vector<std::wstring>> disassembleToGroups(const std::wstring& str)
{
    std::vector<std::vector<std::wstring>> result;
    result.reserve(str.size()); // Reserve space to optimize performance

    for (wchar_t wc : str) {
        std::wstring letter(1, wc);
        auto disassembledComplete = DisassembleCompleteCharacter::disassembleCompleteCharacter(letter);

        if (disassembledComplete.has_value()) {
            std::vector<std::wstring> group;
            // Split each component into individual characters
            auto choseongParts = _splitStringToWstrings(disassembledComplete->choseong);
            auto jungseongParts = _splitStringToWstrings(disassembledComplete->jungseong);
            auto jongseongParts = _splitStringToWstrings(disassembledComplete->jongseong);

            // Append all parts to the group
            group.insert(group.end(), choseongParts.begin(), choseongParts.end());
            group.insert(group.end(), jungseongParts.begin(), jungseongParts.end());
            group.insert(group.end(), jongseongParts.begin(), jongseongParts.end());

            result.emplace_back(std::move(group));
            continue;
        }

        if (_Internal::hasProperty(_Internal::DISASSEMBLED_CONSONANTS_BY_CONSONANT, letter)) {
            const auto& disassembledConsonant = _Internal::DISASSEMBLED_CONSONANTS_BY_CONSONANT.at(letter);
            // Split the disassembled consonant into individual characters
            auto consonantParts = _splitStringToWstrings(disassembledConsonant);
            result.emplace_back(std::move(consonantParts));
            continue;
        }

        if (_Internal::hasProperty(_Internal::DISASSEMBLED_VOWELS_BY_VOWEL, letter)) {
            const auto& disassembledVowel = _Internal::DISASSEMBLED_VOWELS_BY_VOWEL.at(letter);
            // Split the disassembled vowel into individual characters
            auto vowelParts = _splitStringToWstrings(disassembledVowel);
            result.emplace_back(std::move(vowelParts));
            continue;
        }

        // If the character does not match any disassembly criteria, add it as-is
        result.emplace_back(std::vector<std::wstring>{letter});
    }

    return result;
}

/**
 * @brief Disassembles a Hangul string into a single concatenated Hangul string.
 *
 * This function takes a Hangul string, disassembles it into groups of Jamos using `disassembleToGroups`,
 * and then concatenates the disassembled Jamos back into a single Hangul string.
 *
 * @param str The input Hangul string to disassemble.
 * @return A `std::wstring` containing the concatenated disassembled Hangul string.
 *
 * @example
 * ```cpp
 * std::wstring input = L"안녕하세요";
 * std::wstring disassembled = disassemble(input);
 * // disassembled = L"ㅇㅏㄴㄴㅕㅎㅏㅅㅔㅇㅛ"
 * ```
 */
std::wstring disassemble(const std::wstring& str)
{
    std::wstring hanguls;
    hanguls.reserve(str.size() * 3); // Approximate reserve size

    auto groups = disassembleToGroups(str);
    for (const auto& disassembleds : groups) {
        for (const auto& jamo : disassembleds) {
            hanguls += jamo;
        }
    }

    return hanguls;
}

} // namespace Disassemble
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
