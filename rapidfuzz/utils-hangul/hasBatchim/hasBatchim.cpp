#include "hasBatchim.hpp"
#include <codecvt> // For codecvt_utf8_utf16 if needed
#include <locale>
#include <stdexcept>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace HasBatchim {

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
bool hasBatchim(const std::wstring& str, const BatchimOptions& options)
{
    if (str.empty()) {
        return false;
    }

    wchar_t lastChar = str.back();
    int charCode = static_cast<int>(lastChar);

    // Check if the last character is a complete Hangul syllable
    if (charCode < _Internal::COMPLETE_HANGUL_START_CHARCODE ||
        charCode > _Internal::COMPLETE_HANGUL_END_CHARCODE)
    {
        return false;
    }

    // Calculate the batchim index
    int batchimCode = (charCode - _Internal::COMPLETE_HANGUL_START_CHARCODE) % _Internal::NUMBER_OF_JONGSEONG;

    // If there is no batchim
    if (batchimCode == 0) {
        return false;
    }

    // Retrieve the corresponding batchim string
    const std::wstring& batchimStr = _Internal::JONGSEONGS[batchimCode];

    if (options.only.has_value()) {
        if (options.only.value() == OnlyType::SINGLE) {
            return batchimStr.length() == 1;
        }
        if (options.only.value() == OnlyType::DOUBLE) {
            return batchimStr.length() == 2;
        }
        // If 'only' has an unexpected value, return false
        return false;
    }

    // If 'only' is not specified, check for any batchim
    return batchimCode > 0;
}

} // namespace HasBatchim
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
