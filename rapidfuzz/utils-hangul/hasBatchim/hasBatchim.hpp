#ifndef HAS_BATCHIM_HPP
#define HAS_BATCHIM_HPP

#include "_internal/constants.hpp"
#include <optional>
#include <string>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace HasBatchim {

/**
 * @brief Enumeration for specifying the type of batchim to check.
 */
enum class OnlyType {
    SINGLE, ///< Check for single batchim.
    DOUBLE  ///< Check for double batchim.
};

/**
 * @brief Structure for specifying options in the hasBatchim function.
 */
struct BatchimOptions {
    /**
     * @brief The type of batchim to check.
     *
     * If not set, the function checks for the presence of any batchim.
     */
    std::optional<OnlyType> only;
};

/**
 * @brief 한글 문자열의 마지막 글자가 받침이 있는지 확인합니다.
 *
 * @param str 글자에 받침이 있는지 확인하고 싶은 문자열
 * @param options Optional structure specifying the type of batchim to check.
 *                If `options.only` is set to `OnlyType::SINGLE`, only single batchim are considered.
 *                If set to `OnlyType::DOUBLE`, only double batchim are considered.
 *                If not set, any batchim presence is checked.
 * @return `true` if the last character has a batchim matching the specified criteria; `false` otherwise.
 *
 * @example
 * bool result1 = hasBatchim(L"값"); // true
 * bool result2 = hasBatchim(L"토"); // false
 * bool result3 = hasBatchim(L"갑", BatchimOptions{OnlyType::SINGLE}); // true
 * bool result4 = hasBatchim(L"값", BatchimOptions{OnlyType::SINGLE}); // false
 * bool result5 = hasBatchim(L"값", BatchimOptions{OnlyType::DOUBLE}); // true
 * bool result6 = hasBatchim(L"토", BatchimOptions{OnlyType::DOUBLE}); // false
 */
bool hasBatchim(const std::wstring& str, const BatchimOptions& options = BatchimOptions());

} // namespace HasBatchim
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // HAS_BATCHIM_HPP
