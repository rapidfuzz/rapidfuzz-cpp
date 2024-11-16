#include "days.hpp"
#include "_internal/utils.hpp"
#include <stdexcept>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace Days {

/**
 * @brief Converts a numerical day value to its corresponding Hangul word.
 *
 * This function takes an integer representing a day (1 to 30) and returns its
 * corresponding Hangul word based on predefined mappings. If the number is not
 * within the supported range or is invalid, it throws an exception.
 *
 * @param num The day number to convert (1-30).
 * @return A `std::wstring` containing the corresponding Hangul word.
 * @throws `std::invalid_argument` if the number is invalid or unsupported.
 *
 * @example
 * std::wstring word = days(5); // "닷새"
 */
std::wstring days(int num)
{
    // Validate the input number
    if (num <= 0 || num > 30) {
        throw std::invalid_argument("지원하지 않는 숫자입니다.");
    }

    // Calculate tens and ones
    int tens = (num / 10) * 10;
    int ones = num % 10;

    // If the number is exactly divisible by 10 and exists in DAYS_ONLY_TENS_MAP
    if (ones == 0 && _Internal::hasProperty(DAYS_ONLY_TENS_MAP, tens)) {
        return DAYS_ONLY_TENS_MAP.at(tens);
    }

    std::wstring tensWord = L"";
    std::wstring onesWord = L"";

    // Get the tens word if it exists in DAYS_MAP
    if (_Internal::hasProperty(DAYS_MAP, tens)) {
        tensWord = DAYS_MAP.at(tens);
    }

    // Get the ones word if it exists in DAYS_MAP
    if (_Internal::hasProperty(DAYS_MAP, ones)) {
        onesWord = DAYS_MAP.at(ones);
    }

    return tensWord + onesWord;
}

} // namespace Days
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
