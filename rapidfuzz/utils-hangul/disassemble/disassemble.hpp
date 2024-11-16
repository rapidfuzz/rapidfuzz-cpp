#ifndef DISASSEMBLE_HPP
#define DISASSEMBLE_HPP

#include "_internal/constants.hpp"
#include "_internal/utils.hpp"
#include "disassembleCompleteCharacter/disassembleCompleteCharacter.hpp"
#include <string>
#include <utility>
#include <vector>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace Disassemble {

/**
 * @brief Disassembles a Hangul string into groups of Jamos.
 *
 * This function takes a Hangul string and disassembles it into groups of consonants and vowels based on
 * predefined mappings. It handles complete characters, disassembled consonants, and disassembled vowels.
 * Characters that do not match any mapping are included as-is.
 *
 * **Note**: Due to a known issue in some environments, the implementation avoids using `std::transform` or
 * similar functions that may cause performance issues or errors. Instead, it uses explicit loops to ensure
 * compatibility.
 *
 * @param str The input Hangul string to disassemble.
 * @return A `std::vector<std::vector<std::wstring>>` where each inner vector represents a group of Jamos.
 *
 * @example
 * std::wstring input = L"안녕하세요";
 * auto groups = disassembleToGroups(input);
 * // groups = { {L"ㅇ", L"ㅏ", L"ㄴ"}, {L"ㄴ", L"ㅕ", L"ㅇ"}, {L"ㅎ", L"ㅏ"}, {L"ㅅ", L"ㅔ"}, {L"ㅇ", L"ㅛ"}
 * }
 */
std::vector<std::vector<std::wstring>> disassembleToGroups(const std::wstring& str);

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
std::wstring disassemble(const std::wstring& str);

} // namespace Disassemble
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // DISASSEMBLE_HPP
