#ifndef NUMBER_TO_HANGUL_MIXED_HPP
#define NUMBER_TO_HANGUL_MIXED_HPP

#include "_internal/constants.hpp"
#include <optional>
#include <string>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace NumberToHangulMixed {

/**
 * @brief Converts a numerical value to a mixed Hangul and numeric string representation.
 *
 * This function converts a number into its Hangul representation by splitting it into
 * 4-digit groups, converting each group to a localized string with thousand separators,
 * appending the corresponding Hangul digit (e.g., 만, 억), and assembling the parts.
 *
 * @param input The numerical value to convert.
 * @param spacing Optional parameter to specify whether to include spaces between Hangul parts.
 * @return A std::wstring representing the mixed Hangul and numeric equivalent of the input number.
 *
 * @example
 * std::wstring result = numberToHangulMixed(12345, true); // "1만 2345"
 */
std::wstring numberToHangulMixed(long long input, bool spacing = false);

} // namespace NumberToHangulMixed
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // NUMBER_TO_HANGUL_MIXED_HPP
