#include "getChoseong.hpp"
#include <algorithm>
#include <locale>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace GetChoseong {

/**
 * @brief Helper function to normalize a wide string to NFD (Normalization Form D).
 *
 * **Note**: C++ does not have built-in support for Unicode normalization.
 * This function requires an external library such as ICU or Boost.Locale.
 * For demonstration purposes, this function is a stub and returns the input string unchanged.
 *
 * @param input The input wide string to normalize.
 * @return A `std::wstring` normalized to NFD.
 *
 * @throws `std::runtime_error` if normalization fails.
 */
std::wstring normalizeNFD(const std::wstring& input)
{
    // TODO: Implement Unicode normalization to NFD using a library like ICU or Boost.Locale.
    // This is a stub implementation.
    // Example with Boost.Locale:
    // return boost::locale::normalize(input, boost::locale::normalize::nfd);

    // Since normalization is not implemented, return the input as-is.
    // In production code, you must implement this properly.
    return input;
}

/**
 * @brief 단어에서 초성을 추출합니다. (예: `사과` -> `'ㅅㄱ'`)
 *
 * @param word 초성을 추출할 단어
 * @return A `std::wstring` containing the extracted Choseong characters.
 *
 * @example
 * std::wstring result = getChoseong(L"사과"); // "ㅅㄱ"
 * std::wstring result = getChoseong(L"띄어 쓰기"); // "ㄸㅇ ㅆㄱ"
 */
std::wstring getChoseong(const std::wstring& word)
{
    // Step 1: Normalize the input string to NFD
    std::wstring normalized;
    try {
        normalized = normalizeNFD(word);
    }
    catch (const std::exception& e) {
        throw std::invalid_argument("Normalization to NFD failed: " + std::string(e.what()));
    }

    // Step 2: Define the regex patterns
    // EXTRACT_CHOSEOONG_REGEX: Remove all characters except Jaso Hangul Choseong, basic consonants (ㄱ-ㅎ),
    // and whitespace
    std::wregex extractChoseongRegex(L"[^\\u" + std::to_wstring(JASO_HANGUL_NFD.START_CHOSEONG) + L"-\\u" +
                                         std::to_wstring(JASO_HANGUL_NFD.END_CHOSEONG) + L"ㄱ-ㅎ\\s]+",
                                     std::regex_constants::ECMAScript | std::regex_constants::optimize);

    // CHOOSE_NFD_CHOSEONG_REGEX: Match Jaso Hangul Choseong characters
    std::wregex chooseNfdChoseongRegex(L"[\\u" + std::to_wstring(JASO_HANGUL_NFD.START_CHOSEONG) + L"-\\u" +
                                           std::to_wstring(JASO_HANGUL_NFD.END_CHOSEONG) + L"]",
                                       std::regex_constants::ECMAScript | std::regex_constants::optimize);

    // Step 3: Remove unwanted characters using EXTRACT_CHOSEOONG_REGEX
    std::wstring filtered = std::regex_replace(normalized, extractChoseongRegex, L"");

    // Step 4: Replace NFD Choseong with corresponding CHOSEONGS
    // Since C++ regex_replace does not support lambda replacements, perform manual replacement
    std::wstring result;
    result.reserve(filtered.size());

    std::wsmatch match;
    std::wstring::const_iterator searchStart(filtered.cbegin());
    while (std::regex_search(searchStart, filtered.cend(), match, chooseNfdChoseongRegex)) {
        // Append the text before the match
        result.append(match.prefix());

        // Get the matched character
        std::wstring matchedChar = match[0];

        // Calculate the index: matchedChar.codepoint - 0x1100
        // Assuming that JASO_HANGUL_NFD::START_CHOSEOONG is 0x1100
        if (matchedChar.size() != 1) {
            // Unexpected character length, skip replacement
            result.append(matchedChar);
        }
        else {
            wchar_t wc = matchedChar[0];
            int index = static_cast<int>(wc) - JASO_HANGUL_NFD.START_CHOSEONG;
            if (index >= 0 && index < static_cast<int>(_Internal::CHOSEONGS.size())) {
                result += _Internal::CHOSEONGS[index];
            }
            else {
                // Index out of range, append the original character
                result += matchedChar;
            }
        }

        // Update the search start position
        searchStart = match.suffix().first;
    }

    // Append the remaining part of the string
    result.append(searchStart, filtered.cend());

    return result;
}

} // namespace GetChoseong
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
