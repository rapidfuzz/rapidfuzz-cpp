#include "hangul.hpp"
#include "_internal/utils.hpp"
#include "canBe/canBe.hpp"
#include "combineCharacter/combineCharacter.hpp"
#include "disassemble/disassemble.hpp"
#include "hasBatchim/hasBatchim.hpp"
#include "removeLastCharacter/removeLastCharacter.hpp"
#include <exception>
#include <regex>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace _Internal {

bool isHangulCharacter(wchar_t character)
{
    return (character >= 0xAC00 && character <= 0xD7A3);
}

bool isHangulAlphabet(wchar_t character)
{
    return ((character >= L'ㄱ' && character <= L'ㅎ') || (character >= L'ㅏ' && character <= L'ㅣ'));
}

bool isHangul(const std::wstring& actual)
{
    // Regex to match only Hangul syllables, Jamos, and whitespace
    static const std::wregex hangulRegex(L"^[가-힣ㄱ-ㅣ\\s]+$");
    return std::regex_match(actual, hangulRegex);
}

void assertHangul(const std::wstring& actual, const std::string& message)
{
    if (!isHangul(actual)) {
        throw std::invalid_argument(message.empty() ? "Input is not a valid Hangul string." : message);
    }
}

std::wstring parseHangul(const std::wstring& actual)
{
    assertHangul(actual);
    return actual;
}

std::variant<SafeParseSuccess, SafeParseError> safeParseHangul(const std::wstring& actual)
{
    try {
        std::wstring parsedHangul = parseHangul(actual);
        SafeParseSuccess successResult;
        successResult.data = parsedHangul;
        return successResult;
    }
    catch (...) {
        SafeParseError errorResult;
        errorResult.error = std::current_exception();
        return errorResult;
    }
}

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
std::wstring binaryAssembleAlphabets(const std::wstring& source, const std::wstring& nextCharacter)
{
    std::wstring combined;

    if (CanBe::canBeJungseong(source + nextCharacter)) {
        combined = CombineCharacter::combineVowels(source, nextCharacter);
        return combined;
    }

    bool isConsonantSource = !CanBe::canBeJungseong(source);
    if (isConsonantSource && CanBe::canBeJungseong(nextCharacter)) {
        combined = CombineCharacter::combineCharacter(source, nextCharacter);
        return combined;
    }

    return joinString(source, nextCharacter);
}

/**
 * @brief 연음 법칙을 적용하여 두 개의 한글 문자를 연결합니다.
 *
 * @param source The source Hangul syllable.
 * @param nextCharacter The next Hangul Jamo to link.
 * @return The linked Hangul string.
 */
std::wstring linkHangulCharacters(const std::wstring& source, const std::wstring& nextCharacter)
{
    std::vector<std::wstring> sourceJamo = Disassemble::disassembleToGroups(source)[0];
    std::wstring lastJamo = excludeLastElement(sourceJamo).second;

    std::wstring modifiedSource = RemoveLastCharacter::removeLastCharacter(source);
    std::wstring combinedJamo = CombineCharacter::combineCharacter(lastJamo, nextCharacter);

    return joinString(modifiedSource, combinedJamo);
}

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
std::wstring binaryAssembleCharacters(const std::wstring& source, const std::wstring& nextCharacter)
{
    // Validate source and nextCharacter
    if (!(isHangulCharacter(source[0]) || isHangulAlphabet(source[0]))) {
        throw std::invalid_argument("Invalid source character: " + std::string(source.begin(), source.end()) +
                                    ". Source must be one character.");
    }
    if (!isHangulAlphabet(nextCharacter[0])) {
        throw std::invalid_argument(
            "Invalid next character: " + std::string(nextCharacter.begin(), nextCharacter.end()) +
            ". Next character must be one of the choseong, jungseong, or jongseong.");
    }

    std::vector<std::wstring> sourceJamos = Disassemble::disassembleToGroups(source)[0];

    bool isSingleCharacter = sourceJamos.size() == 1;
    if (isSingleCharacter) {
        std::wstring sourceCharacter = sourceJamos[0];
        return binaryAssembleAlphabets(sourceCharacter, nextCharacter);
    }

    std::vector<std::wstring> restJamos;
    std::wstring lastJamo;
    std::tie(restJamos, lastJamo) = excludeLastElement(sourceJamos);

    std::wstring secondaryLastJamo;
    if (restJamos.size() >= 2) {
        secondaryLastJamo = excludeLastElement(restJamos).second;
    }

    bool needLinking = CanBe::canBeChoseong(lastJamo) && CanBe::canBeJungseong(nextCharacter);
    if (needLinking) {
        return linkHangulCharacters(source, nextCharacter);
    }

    // curriedCombineCharacter functionality
    auto fixConsonant = CombineCharacter::curriedCombineCharacter;
    auto combineJungseong = fixConsonant(restJamos[0]);

    if (CanBe::canBeJungseong(lastJamo + nextCharacter)) {
        return combineJungseong(lastJamo + nextCharacter)(L"");
    }

    if (CanBe::canBeJungseong(secondaryLastJamo + lastJamo) && CanBe::canBeJongseong(nextCharacter)) {
        return combineJungseong(secondaryLastJamo + lastJamo)(nextCharacter);
    }

    if (CanBe::canBeJungseong(lastJamo) && CanBe::canBeJongseong(nextCharacter)) {
        return combineJungseong(lastJamo)(nextCharacter);
    }

    auto fixVowel = combineJungseong;

    auto combineJongseong = fixVowel(
        CanBe::canBeJungseong(restJamos[1] + restJamos[2]) ? restJamos[1] + restJamos[2] : restJamos[1]);

    std::wstring lastConsonant = lastJamo;

    if (HasBatchim::hasBatchim(source, /* options */ {}) &&
        CanBe::canBeJongseong(lastConsonant + nextCharacter))
    {
        return combineJongseong(lastConsonant + nextCharacter);
    }

    return joinString(source, nextCharacter);
}

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
std::wstring binaryAssemble(const std::wstring& source, const std::wstring& nextCharacter)
{
    std::vector<std::wstring> sourceChars;
    for (const auto& ch : source) {
        sourceChars.push_back(std::wstring(1, ch));
    }
    auto [rest, lastCharacter] = _Internal::excludeLastElement(sourceChars);

    bool needJoinString = isBlank(lastCharacter) || isBlank(nextCharacter);

    std::wstring assembled;
    for (const auto& ch : rest) {
        assembled += ch;
    }

    if (needJoinString) {
        assembled += joinString(lastCharacter, nextCharacter);
    }
    else {
        assembled += binaryAssembleCharacters(lastCharacter, nextCharacter);
    }

    return assembled;
}

} // namespace _Internal
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
