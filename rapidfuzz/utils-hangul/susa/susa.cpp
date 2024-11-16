#include "susa.hpp"
#include "constants.hpp"
#include <cassert>
#include <stdexcept>
#include <unordered_map>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace Susa {

/**
 * @brief Validates that the given number is an integer between 1 and 100 inclusive.
 *
 * @param num The number to validate.
 * @throws std::invalid_argument if the number is out of range or not an integer.
 */
void validateNumber(int num)
{
    if (num < 1 || num > 100) {
        throw std::invalid_argument("지원하지 않는 숫자입니다.");
    }
}

/**
 * @brief Retrieves the Korean Susa word for a given number without classifier.
 *
 * @param num The number to convert. Assumes `num` is already validated.
 * @return The corresponding Korean Susa word.
 */
std::wstring getNumberWord(int num)
{
    using namespace RapidFuzz::Utils::Hangul::Susa;

    if (num == 100) {
        return SUSA_MAP.at(100);
    }

    int tens = (num / 10) * 10;
    int ones = num % 10;

    std::wstring tensWord = (SUSA_MAP.find(tens) != SUSA_MAP.end()) ? SUSA_MAP.at(tens) : L"";
    std::wstring onesWord = (SUSA_MAP.find(ones) != SUSA_MAP.end()) ? SUSA_MAP.at(ones) : L"";

    return tensWord + onesWord;
}

/**
 * @brief Retrieves the Korean Susa Classifier word for a given number.
 *
 * @param num The number to convert. Assumes `num` is already validated.
 * @return The corresponding Korean Susa Classifier word.
 */
std::wstring getClassifierWord(int num)
{
    using namespace RapidFuzz::Utils::Hangul::Susa;

    if (num == 20) {
        return SUSA_CLASSIFIER_MAP.at(20);
    }

    int tens = (num / 10) * 10;
    int ones = num % 10;

    std::wstring tensWord = (SUSA_MAP.find(tens) != SUSA_MAP.end()) ? SUSA_MAP.at(tens) : L"";

    if (ones == 0) {
        return tensWord;
    }

    if (SUSA_CLASSIFIER_MAP.find(ones) != SUSA_CLASSIFIER_MAP.end()) {
        std::wstring onesWord = SUSA_CLASSIFIER_MAP.at(ones);
        return tensWord + onesWord;
    }

    if (SUSA_MAP.find(ones) != SUSA_MAP.end()) {
        std::wstring onesWord = SUSA_MAP.at(ones);
        return tensWord + onesWord;
    }

    // This should not be reachable due to validation
    throw std::runtime_error("지원하지 않는 숫자입니다.");
}

/**
 * @brief Converts a given number to its corresponding Korean Susa.
 */
std::wstring susa(int num, bool classifier)
{
    validateNumber(num);
    return classifier ? getClassifierWord(num) : getNumberWord(num);
}

} // namespace Susa
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
