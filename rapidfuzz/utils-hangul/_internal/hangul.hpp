#ifndef HANGUL_HPP
#define HANGUL_HPP

#include <cassert>
#include <optional>
#include <string>
#include <vector>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace _Internal {

bool isHangulCharacter(wchar_t character);

bool isHangulAlphabet(wchar_t character);

bool isHangul(const std::wstring& actual);

void assertHangul(const std::wstring& actual, const std::string& message = "");

std::wstring parseHangul(const std::wstring& actual);

/**
 * @struct SafeParseSuccess
 * @brief Represents a successful parsing result.
 */
struct SafeParseSuccess {
    bool success = true;
    std::wstring data;
};

/**
 * @struct SafeParseError
 * @brief Represents a failed parsing result.
 */
struct SafeParseError {
    bool success = false;
    std::exception_ptr error;
};

/**
 * @brief Attempts to safely parse the input as a Hangul string.
 *
 * @param actual The input to parse.
 * @return `SafeParseSuccess` if parsing is successful, `SafeParseError` otherwise.
 */
std::variant<SafeParseSuccess, SafeParseError> safeParseHangul(const std::wstring& actual);

/**
 * @brief 두 개의 한글 자모를 합칩니다. 완성된 한글 문자는 취급하지 않습니다.
 *
 * @param source The source Hangul Jamo.
 * @param nextCharacter The next Hangul Jamo to combine.
 * @return The combined Hangul string.
 *
 * @example
 * binaryAssembleAlphabets(L"ㄱ", L"ㅏ"); // L"가"
 * binaryAssembleAlphabets(L"ㅗ", L"ㅏ"); // L"ㅘ"
 */
std::wstring binaryAssembleAlphabets(const std::wstring& source, const std::wstring& nextCharacter);

/**
 * @brief 연음 법칙을 적용하여 두 개의 한글 문자를 연결합니다.
 *
 * @param source The source Hangul syllable.
 * @param nextCharacter The next Hangul Jamo to link.
 * @return The linked Hangul string.
 */
std::wstring linkHangulCharacters(const std::wstring& source, const std::wstring& nextCharacter);

/**
 * @brief 인자로 받은 한글 문자 2개를 합성합니다.
 *
 * @param source The source Hangul character (syllable or Jamo).
 * @param nextCharacter The next Hangul Jamo to assemble.
 * @return The assembled Hangul string.
 *
 * @throws std::invalid_argument if the input characters are invalid.
 *
 * @example
 * binaryAssembleCharacters(L"가", L"ㅏ"); // L"가"
 * binaryAssembleCharacters(L"가", L"ㅇ"); // L"강"
 * binaryAssembleCharacters(L"갑", L"ㅅ"); // L"값"
 * binaryAssembleCharacters(L"깎", L"ㅏ"); // L"까까"
 */
std::wstring binaryAssembleCharacters(const std::wstring& source, const std::wstring& nextCharacter);

/**
 * @brief 인자로 받은 한글 문장과 한글 문자 하나를 합성합니다.
 *
 * @param source The source Hangul string.
 * @param nextCharacter The next Hangul character to assemble.
 * @return The assembled Hangul string.
 *
 * @example
 * binaryAssemble(L"저는 고양이를 좋아합닏", L"ㅏ"); // L"저는 고양이를 좋아합니다"
 * binaryAssemble(L"저는 고양이를 좋아합", L"ㅅ"); // L"저는 고양이를 좋아핪"
 * binaryAssemble(L"저는 고양이를 좋아하", L"ㅏ"); // L"저는 고양이를 좋아하ㅏ"
 */
std::wstring binaryAssemble(const std::wstring& source, const std::wstring& nextCharacter);

} // namespace _Internal
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // HANGUL_HPP
