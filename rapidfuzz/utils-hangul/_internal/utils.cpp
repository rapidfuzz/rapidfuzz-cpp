#include "utils.hpp"
#include <algorithm>
#include <locale>
#include <stdexcept>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace _Internal {

std::pair<std::vector<std::wstring>, std::wstring> excludeLastElement(const std::vector<std::wstring>& array)
{
    if (array.empty()) {
        return {{}, L""};
    }
    std::vector<std::wstring> newArray = array;
    std::wstring lastElement = newArray.back();
    newArray.pop_back();
    return {newArray, lastElement};
}

bool isBlank(const std::wstring& character)
{
    if (character.size() != 1) {
        return false;
    }
    // Use locale-aware whitespace check
    std::locale loc(""); // Use the global locale
    return std::iswspace(character[0]);
}

void assertCondition(bool condition, const std::string& errorMessage)
{
    if (!condition) {
        throw std::invalid_argument(errorMessage.empty() ? "Invalid condition" : errorMessage);
    }
}

} // namespace _Internal
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz
