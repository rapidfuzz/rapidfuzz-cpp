#include "josa.hpp"
#include "disassembleCompleteCharacter.hpp"
#include "hasBatchim.hpp"
#include <algorithm> // For std::find

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace Josa {

/**
 * @brief Helper function to map JosaOption to its corresponding pair of josa strings.
 *
 * @param josa The JosaOption to map.
 * @return A pair of std::wstring where first is the josa used when batchim is present,
 *         and second is the josa used when batchim is absent.
 */
std::pair<std::wstring, std::wstring> getJosaStrings(JosaOption josa)
{
    // Define a static array mapping JosaOption enum to pair of strings
    static const std::array<std::pair<std::wstring, std::wstring>, 14> JOSA_MAP = {
        std::make_pair(L"이", L"가"),           // 이_가
        std::make_pair(L"을", L"를"),           // 을_를
        std::make_pair(L"은", L"는"),           // 은_는
        std::make_pair(L"으로", L"로"),         // 으로_로
        std::make_pair(L"와", L"과"),           // 와_과
        std::make_pair(L"이나", L"나"),         // 이나_나
        std::make_pair(L"이란", L"란"),         // 이란_란
        std::make_pair(L"아", L"야"),           // 아_야
        std::make_pair(L"이랑", L"랑"),         // 이랑_랑
        std::make_pair(L"이에요", L"예요"),     // 이에요_예요
        std::make_pair(L"으로서", L"로서"),     // 으로서_로서
        std::make_pair(L"으로써", L"로써"),     // 으로써_로써
        std::make_pair(L"으로부터", L"로부터"), // 으로부터_로부터
        std::make_pair(L"이라", L"라")          // 이라_라
    };

    int index = static_cast<int>(josa);
    if (index < 0 || index >= static_cast<int>(JOSA_MAP.size())) {
        // Handle out of range, return empty strings
        return std::make_pair(L"", L"");
    }
    return JOSA_MAP[index];
}

std::wstring josaPicker(const std::wstring& word, JosaOption josa)
{
    if (word.empty()) {
        // Return the first option if the word is empty
        return getJosaStrings(josa).first;
    }

    bool has받침 = HasBatchim::hasBatchim(word);
    int index = has받침 ? 0 : 1;

    bool is종성ㄹ = false;
    if (has받침) {
        // Get the last character's jongseong
        auto disassembled =
            DisassembleCompleteCharacter::disassembleCompleteCharacter(std::wstring(1, word.back()));
        if (disassembled.has_value()) {
            const std::wstring& jongseong = disassembled.value().jongseong;
            if (jongseong == L"ㄹ") {
                is종성ㄹ = true;
            }
        }
    }

    bool isCaseOf로 =
        has받침 && is종성ㄹ && (std::find(로_조사.begin(), 로_조사.end(), josa) != 로_조사.end());

    if (josa == JosaOption::와_과 || isCaseOf로) {
        // Toggle index
        index = (index == 0) ? 1 : 0;
    }

    bool isEndsWith이 = false;
    if (!word.empty()) {
        wchar_t last_char = word.back();
        if (last_char == L'이') {
            isEndsWith이 = true;
        }
    }

    if (josa == JosaOption::이에요_예요 && isEndsWith이) {
        index = 1;
    }

    // Get the josa strings
    auto josa_strings = getJosaStrings(josa);

    // Ensure index is within range
    if (index < 0 || index >= 2) {
        // Default to first option
        return josa_strings.first;
    }

    return index == 0 ? josa_strings.first : josa_strings.second;
}

std::wstring josa(const std::wstring& word, JosaOption josa)
{
    return word + josaPicker(word, josa);
}

} // namespace Josa
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
