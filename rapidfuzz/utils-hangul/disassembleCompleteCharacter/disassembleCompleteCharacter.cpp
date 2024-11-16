#include "disassembleCompleteCharacter.hpp"

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace DisassembleCompleteCharacter {

/**
 * @brief 완전한 한글 문자열을 초성, 중성, 종성으로 분리합니다.

 * @param letter 분리하고자 하는 완전한 한글 문자열
 * @return `std::optional<DisassembledCharacter>` containing the disassembled components
 *         if the input is a valid Hangul syllable; otherwise, `std::nullopt`.
 *
 * @example
 * auto result = disassembleCompleteCharacter(L"값");
 *  // result->choseong == L"ㄱ"
 *  // result->jungseong == L"ㅏ"
 *  // result->jongseong == L"ㅂㅅ"
 * auto result = disassembleCompleteCharacter(L"리");
 *  // result->choseong == L"ㄹ"
 *  // result->jungseong == L"ㅣ"
 *  // result->jongseong == L""
 * auto result = disassembleCompleteCharacter(L"빚");
 *  // result->choseong == L"ㅂ"
 *  // result->jungseong == L"ㅣ"
 *  // result->jongseong == L"ㅈ"
 * auto result = disassembleCompleteCharacter(L"박");
 *  // result->choseong == L"ㅂ"
 *  // result->jungseong == L"ㅏ"
 *  // result->jongseong == L"ㄱ"
 */
std::optional<DisassembledCharacter> disassembleCompleteCharacter(const std::wstring& letter)
{
    if (letter.empty()) {
        return std::nullopt;
    }

    wchar_t wc = letter[0];
    int charCode = static_cast<int>(wc);

    // Check if the character is within the range of complete Hangul syllables
    if (charCode < _Internal::COMPLETE_HANGUL_START_CHARCODE ||
        charCode > _Internal::COMPLETE_HANGUL_END_CHARCODE)
    {
        return std::nullopt;
    }

    // Calculate the offset from the start of Hangul syllables
    int hangulCode = charCode - _Internal::COMPLETE_HANGUL_START_CHARCODE;

    // Calculate indices for choseong, jungseong, and jongseong
    int jongseongIndex = hangulCode % _Internal::NUMBER_OF_JONGSEONG;
    int jungseongIndex =
        ((hangulCode - jongseongIndex) / _Internal::NUMBER_OF_JONGSEONG) % _Internal::NUMBER_OF_JUNGSEONG;
    int choseongIndex =
        (hangulCode - jongseongIndex) / (_Internal::NUMBER_OF_JONGSEONG * _Internal::NUMBER_OF_JUNGSEONG);

    // Retrieve the corresponding Hangul Jamos
    // Ensure that indices are within the bounds of their respective arrays
    if (choseongIndex < 0 || choseongIndex >= static_cast<int>(_Internal::CHOSEONGS.size()) ||
        jungseongIndex < 0 || jungseongIndex >= static_cast<int>(_Internal::JUNSEONGS.size()) ||
        jongseongIndex < 0 || jongseongIndex >= static_cast<int>(_Internal::JONGSEONGS.size()))
    {
        return std::nullopt;
    }

    DisassembledCharacter disassembled;
    disassembled.choseong = _Internal::CHOSEONGS[choseongIndex];
    disassembled.jungseong = _Internal::JUNSEONGS[jungseongIndex];
    disassembled.jongseong = _Internal::JONGSEONGS[jongseongIndex];

    return disassembled;
}

} // namespace DisassembleCompleteCharacter
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
