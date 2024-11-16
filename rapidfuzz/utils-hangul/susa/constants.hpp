#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>
#include <unordered_map>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace Susa {

/**
 * @brief Mapping of numbers to their corresponding Korean Susa (native Korean numerals).
 */
inline const std::unordered_map<int, std::wstring> SUSA_MAP = {
    {1, L"하나"},  {2, L"둘"},    {3, L"셋"},    {4, L"넷"},    {5, L"다섯"},  {6, L"여섯"},  {7, L"일곱"},
    {8, L"여덟"},  {9, L"아홉"},  {10, L"열"},   {20, L"스물"}, {30, L"서른"}, {40, L"마흔"}, {50, L"쉰"},
    {60, L"예순"}, {70, L"일흔"}, {80, L"여든"}, {90, L"아흔"}, {100, L"백"}};

/**
 * @brief Mapping of numbers to their corresponding Korean Susa Classifier (수관형사).
 */
inline const std::unordered_map<int, std::wstring> SUSA_CLASSIFIER_MAP = {
    {1, L"한"}, {2, L"두"}, {3, L"세"}, {4, L"네"}, {20, L"스무"}};

} // namespace Susa
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // CONSTANTS_HPP
