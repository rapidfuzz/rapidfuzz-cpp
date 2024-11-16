#ifndef RULES_TYPES_HPP
#define RULES_TYPES_HPP

#include "disassembleCompleteCharacter.hpp"
#include <optional>
#include <string>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace StandardPronunciation {
namespace Rules {

/**
 * @brief Represents a Hangul syllable with its constituent Jamos.
 */
using Syllable = DisassembleCompleteCharacter::DisassembledCharacter;

/**
 * @brief Ensures that the type T is not undefined.
 *        In C++, this is implicitly handled, so this type alias serves as documentation.
 */
template <typename T>
using NonUndefined = T;

/**
 * @brief Represents a type that can be T, null, or undefined.
 *        In C++, std::optional<T> is used to represent nullable types.
 */
template <typename T>
using Nullable = std::optional<T>;

/**
 * @brief Represents a pair of current and next syllables.
 */
struct ReturnSyllables {
    Syllable current;
    Syllable next;
};

/**
 * @brief Represents the result of the following functions.
 * - transform9And10And11th
 * - transform18th
 * - applyMainCondition
 *
 * Contains the transformed current syllable only.
 */
struct ReturnCurrentSyllables {
    Syllable current; ///< The transformed current syllable.
};

/**
 * @brief Represents the result of the following functions.
 * - transform19th
 * - transformHardConversion
 * - applySupplementaryCondition
 * - transformHardConversion
 *
 * Contains the transformed next syllable only.
 */
struct ReturnNextSyllables {
    Syllable next; ///< The transformed next syllable.
};

/**
 * @brief Represents a pair of current syllable and an optional next syllable.
 */
struct NullableReturnSyllables {
    Syllable current;
    Nullable<Syllable> next;
};

} // namespace Rules
} // namespace StandardPronunciation
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // RULES_TYPES_HPP
