#include "assemble.hpp"
#include "_internal/hangul.hpp"
#include "_internal/utils.hpp"
#include "disassemble/disassemble.hpp"

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace Internal {
namespace Assemble {

std::wstring assemble(const std::vector<std::wstring>& words)
{
    // Join the input words into a single string
    std::wstring joinedWords;
    for (const auto& word : words) {
        joinedWords += word;
    }

    // Disassemble the joined string into individual characters
    std::wstring disassembled = Disassemble::disassemble(joinedWords);

    // Split the disassembled string into a vector of characters
    std::vector<wchar_t> characters(disassembled.begin(), disassembled.end());

    // Reduce the vector of characters using binaryAssemble to form the final assembled string
    std::wstring assembled;
    for (const auto& ch : characters) {
        assembled = _Internal::binaryAssemble(assembled, std::wstring(1, ch));
    }

    return assembled;
}

} // namespace Assemble
} // namespace Internal
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
