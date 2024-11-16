#include "romanize.hpp"

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace Romanize {

std::wstring romanizeSyllableHangul(const std::wstring& syllable,
                                    const std::vector<std::wstring>& arrayHangul, size_t index)
{
    // Check if the syllable is a Hangul character
    if (_Internal::isHangulCharacter(syllable[0])) {
        auto disassemble =
            DisassembleCompleteCharacter::disassembleCompleteCharacter(std::wstring(1, syllable[0]));
        if (!disassemble.has_value()) {
            // If disassembly fails, return the syllable as is
            return syllable;
        }

        // Extract components
        std::wstring choseong = disassemble.value().choseong;
        std::wstring jungseong = disassemble.value().jungseong;
        std::wstring jongseong = disassemble.value().jongseong;

        // Map components to Roman letters
        std::wstring romanChoseong = L"";
        std::wstring romanJungseong = L"";
        std::wstring romanJongseong = L"";

        // Map Choseong
        auto itChoseong = CHOSEONG_ALPHABET_PRONUNCIATION.find(choseong[0]);
        if (itChoseong != CHOSEONG_ALPHABET_PRONUNCIATION.end()) {
            romanChoseong = itChoseong->second;
        }

        // Map Jungseong
        auto itJungseong = JUNGSEONG_ALPHABET_PRONUNCIATION.find(jungseong[0]);
        if (itJungseong != JUNGSEONG_ALPHABET_PRONUNCIATION.end()) {
            romanJungseong = itJungseong->second;
        }

        // Map Jongseong
        if (!jongseong.empty()) {
            auto itJongseong = JONGSEONG_ALPHABET_PRONUNCIATION.find(jongseong);
            if (itJongseong != JONGSEONG_ALPHABET_PRONUNCIATION.end()) {
                romanJongseong = itJongseong->second;
            }
        }

        // Special handling for 'ㄹ'
        if (choseong == L"ㄹ") {
            if (index > 0 && _Internal::isHangulCharacter(arrayHangul[index - 1][0])) {
                auto prevDisassemble = DisassembleCompleteCharacter::disassembleCompleteCharacter(
                    std::wstring(1, arrayHangul[index - 1][0]));
                if (prevDisassemble.has_value() && prevDisassemble->jongseong == L"ㄹ") {
                    romanChoseong = L"l";
                }
            }
        }

        // Assemble the Romanized syllable
        return romanChoseong + romanJungseong + romanJongseong;
    }

    // If syllable is a vowel
    auto itVowel = JUNGSEONG_ALPHABET_PRONUNCIATION.find(syllable[0]);
    if (itVowel != JUNGSEONG_ALPHABET_PRONUNCIATION.end()) {
        return itVowel->second;
    }

    // If syllable can be a Choseong
    if (CanBe::canBeChoseong(std::wstring(1, syllable[0]))) {
        auto itChoseong = CHOSEONG_ALPHABET_PRONUNCIATION.find(syllable[0]);
        if (itChoseong != CHOSEONG_ALPHABET_PRONUNCIATION.end()) {
            return itChoseong->second;
        }
    }

    // Return the syllable as is if no mapping is found
    return syllable;
}

std::wstring romanize(const std::wstring& hangul)
{
    StandardPronunciation::Options options = {false};
    std::wstring standardizedHangul = StandardPronunciation::standardizePronunciation(hangul, options);

    std::wstring romanized = L"";

    for (size_t i = 0; i < standardizedHangul.length(); ++i) {
        std::wstring syllable = std::wstring(1, standardizedHangul[i]);
        romanized += romanizeSyllableHangul(syllable, std::vector<std::wstring>{standardizedHangul}, i);
    }

    return romanized;
}

} // namespace Romanize
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
