#ifndef NUMBER_TO_HANGUL_HPP
#define NUMBER_TO_HANGUL_HPP

#include "_internal/constants.hpp"
#include <stdexcept>
#include <string>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace NumberToHangul {

/**
 * @brief Converts a numerical value to its corresponding Hangul (Korean) representation.
 *
 * @param input The numerical value to convert. Should be between 0 and the maximum supported range.
 * @param spacing Optional parameter to specify whether to include spaces between Hangul parts.
 * @return A std::wstring representing the Hangul equivalent of the input number.
 *
 * @throws std::out_of_range If the input number exceeds the supported range.
 *
 * @example
 * std::wstring result = numberToHangul(12345, true); // "일만 이천 삼백 사십 오"
 */
std::wstring numberToHangul(long long input, bool spacing = false);

/**
 * @brief Helper function to convert a number up to 9999 into its Hangul representation.
 *
 * @param num The number to convert. Should be between 0 and 9999 inclusive.
 * @return A std::wstring representing the Hangul equivalent of the input number.
 *
 * @throws std::invalid_argument If the input number is outside the range [0, 9999].
 *
 * @example
 * std::wstring hangul = numberToKoreanUpToThousand(1234); // "일천이백삼십사"
 */
std::wstring numberToKoreanUpToThousand(int num);

} // namespace NumberToHangul
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // NUMBER_TO_HANGUL_HPP
