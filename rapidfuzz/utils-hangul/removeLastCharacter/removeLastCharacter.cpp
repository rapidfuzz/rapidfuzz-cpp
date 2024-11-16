#include "removeLastCharacter.hpp"

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace RemoveLastCharacter {

std::wstring removeLastCharacter(const std::wstring& words)
{
    if (words.empty()) {
        return L"";
    }

    wchar_t lastCharacter = words.back();
    std::wstring result;

    // Disassemble the last character into its Hangul components
    auto disassembleLastCharacter = Disassemble::disassembleToGroups(std::wstring(1, lastCharacter));
    std::vector<std::wstring> lastCharacterWithoutLastAlphabet =
        _Internal::excludeLastElement(disassembleLastCharacter[0]).first;

    std::wstring modifiedLastCharacter;
    if (lastCharacterWithoutLastAlphabet.size() <= 3) {
        // Handle cases with up to 3 components (choseong, jungseong, jongseong)
        std::wstring first = lastCharacterWithoutLastAlphabet[0];
        std::wstring middle =
            (lastCharacterWithoutLastAlphabet.size() > 1) ? lastCharacterWithoutLastAlphabet[1] : L"";
        std::wstring last =
            (lastCharacterWithoutLastAlphabet.size() > 2) ? lastCharacterWithoutLastAlphabet[2] : L"";

        if (!middle.empty()) {
            if (CanBe::canBeJungseong(last)) {
                // If the last component can be a jungseong, combine middle and last
                modifiedLastCharacter = CombineCharacter::combineCharacter(first, middle + last);
            }
            else {
                // Otherwise, combine all three components
                modifiedLastCharacter = CombineCharacter::combineCharacter(first, middle, last);
            }
        }
        else {
            // If only the first component exists, retain it
            modifiedLastCharacter = first;
        }
    }
    else {
        // Handle cases with more than 3 components (e.g., extended vowels)
        std::wstring first = lastCharacterWithoutLastAlphabet[0];
        std::wstring firstJungsung = lastCharacterWithoutLastAlphabet[1];
        std::wstring secondJungsung = lastCharacterWithoutLastAlphabet[2];
        std::wstring firstJongsung = lastCharacterWithoutLastAlphabet[3];

        // Combine the first, concatenated jungseongs, and first jongseong
        modifiedLastCharacter =
            CombineCharacter::combineCharacter(first, firstJungsung + secondJungsung, firstJongsung);
    }

    // Assemble the final string by replacing the last character with the modified one
    result = words.substr(0, words.length() - 1) + modifiedLastCharacter;
    return result;
}

} // namespace RemoveLastCharacter
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
