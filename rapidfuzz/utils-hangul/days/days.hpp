#ifndef DAYS_HPP
#define DAYS_HPP

#include "_internal/constants.hpp"
#include <string>

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
std::wstring days(int num);

} // namespace Days
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // DAYS_HPP