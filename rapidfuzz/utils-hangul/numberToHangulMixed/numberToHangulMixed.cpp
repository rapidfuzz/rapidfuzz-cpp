#include "numberToHangulMixed.hpp"
#include <algorithm>
#include <iomanip>
#include <locale>
#include <sstream>
#include <vector>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace NumberToHangulMixed {

/**
 * @brief Helper function to format a number with thousand separators based on the current locale.
 *
 * @param num The number to format.
 * @return A std::wstring with thousand separators.
 *
 * @example
 * std::wstring formatted = formatWithThousandSeparators(12345); // "12,345"
 */
std::wstring formatWithThousandSeparators(long long num)
{
    std::wstringstream ss;
    ss.imbue(std::locale("")); // Use the user's default locale

    ss << std::fixed << num;
    std::wstring numStr = ss.str();

    // Insert thousand separators
    std::wstring formatted;
    int count = 0;
    for (auto it = numStr.rbegin(); it != numStr.rend(); ++it) {
        if (count && count % 3 == 0) {
            formatted.insert(formatted.begin(), L',');
        }
        formatted.insert(formatted.begin(), *it);
        count++;
    }

    return formatted;
}

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
std::wstring numberToHangulMixed(long long input, bool spacing)
{
    if (input == 0) {
        return L"0";
    }

    std::vector<std::wstring> koreanParts;
    std::wstring remainingDigits = std::to_wstring(input);
    int placeIndex = 0;

    while (!remainingDigits.empty()) {
        std::wstring currentPart;
        if (remainingDigits.length() > 4) {
            currentPart = remainingDigits.substr(remainingDigits.length() - 4, 4);
            remainingDigits = remainingDigits.substr(0, remainingDigits.length() - 4);
        }
        else {
            currentPart = remainingDigits;
            remainingDigits = L"";
        }

        int partNumber = std::stoi(currentPart);
        if (partNumber > 0) {
            std::wstring formattedNumber = formatWithThousandSeparators(partNumber);
            std::wstring hangulPart = formattedNumber + _Internal::HANGUL_DIGITS[placeIndex];
            koreanParts.emplace_back(hangulPart);
        }

        placeIndex++;
    }

    // Remove empty parts and ensure proper ordering
    koreanParts.erase(std::remove_if(koreanParts.begin(), koreanParts.end(),
                                     [](const std::wstring& part) { return part.empty(); }),
                      koreanParts.end());

    if (spacing) {
        // Join with spaces
        std::wstring result = L"";
        for (size_t i = 0; i < koreanParts.size(); ++i) {
            result += koreanParts[i];
            if (i != koreanParts.size() - 1) {
                result += L" ";
            }
        }
        return result;
    }
    else {
        // Join without spaces
        std::wstring result = L"";
        for (const auto& part : koreanParts) {
            result += part;
        }
        return result;
    }
}

} // namespace NumberToHangulMixed
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
