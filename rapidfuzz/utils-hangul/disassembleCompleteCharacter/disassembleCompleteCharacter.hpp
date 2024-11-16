#ifndef DISASSEMBLE_COMPLETE_CHARACTER_HPP
#define DISASSEMBLE_COMPLETE_CHARACTER_HPP

#include "_internal/constants.hpp"
#include <optional>
#include <string>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace DisassembleCompleteCharacter {

/**
 * @struct DisassembledCharacter
 * @brief Represents the disassembled components of a complete Hangul syllable.
 */
struct DisassembledCharacter {
    std::wstring choseong;
    std::wstring jungseong;
    std::wstring jongseong; // (can be empty)
};

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
std::optional<DisassembledCharacter> disassembleCompleteCharacter(const std::wstring& letter);

} // namespace DisassembleCompleteCharacter
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // DISASSEMBLE_COMPLETE_CHARACTER_HPP
