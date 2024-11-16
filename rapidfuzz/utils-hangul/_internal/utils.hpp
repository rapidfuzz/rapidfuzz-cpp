#ifndef UTILS_HPP
#define UTILS_HPP

#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace _Internal {

std::pair<std::vector<std::wstring>, std::wstring> excludeLastElement(const std::vector<std::wstring>& array);

template <typename... Args>
std::wstring joinString(const Args&... args);

bool isBlank(const std::wstring& character);

void assertCondition(bool condition, const std::string& errorMessage);

template <typename T>
bool isNotUndefined(const std::optional<T>& value);

template <typename T>
T defined(const std::optional<T>& value);

template <typename Type>
bool arrayIncludes(const std::vector<Type>& array, const Type& item, size_t fromIndex = 0);

template <typename T = std::wstring>
bool hasValueInReadOnlyStringList(const std::vector<T>& list, const T& value);

template <typename Key, typename Value>
bool hasProperty(const std::unordered_map<Key, Value>& obj, const Key& key);

} // namespace _Internal
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#include "utils_impl.hpp" // Include the implementations for template functions

#endif // UTILS_HPP
