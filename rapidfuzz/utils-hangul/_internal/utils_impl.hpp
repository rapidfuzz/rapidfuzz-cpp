#ifndef UTILS_IMPL_HPP
#define UTILS_IMPL_HPP

#include "utils.hpp"
#include <algorithm>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace _Internal {

// Implementation of joinString using fold expressions (C++17)
template <typename... Args>
std::wstring joinString(const Args&... args)
{
    return (args + ...);
}

template <typename T>
bool isNotUndefined(const std::optional<T>& value)
{
    return value.has_value();
}

template <typename T>
T defined(const std::optional<T>& value)
{
    assertCondition(value.has_value(), L"Value is undefined");
    return value.value();
}

template <typename Type>
bool arrayIncludes(const std::vector<Type>& array, const Type& item, size_t fromIndex)
{
    if (fromIndex >= array.size()) {
        return false;
    }
    return std::find(array.begin() + fromIndex, array.end(), item) != array.end();
}

template <typename T>
bool hasValueInReadOnlyStringList(const std::vector<T>& list, const T& value)
{
    return std::any_of(list.begin(), list.end(), [&](const T& item) { return item == value; });
}

template <typename Key, typename Value>
bool hasProperty(const std::unordered_map<Key, Value>& obj, const Key& key)
{
    return obj.find(key) != obj.end();
}

} // namespace _Internal
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // UTILS_IMPL_HPP
