#ifndef DAYS_CONSTANTS_HPP
#define DAYS_CONSTANTS_HPP

#include <string>
#include <unordered_map>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace Days {

// Mapping from numbers to Hangul words for ones place (1-9)
inline const std::unordered_map<int, std::wstring> DAYS_MAP = {
    {1, L"하루"}, {2, L"이틀"},   {3, L"사흘"},   {4, L"나흘"}, {5, L"닷새"}, {6, L"엿새"},
    {7, L"이레"}, {8, L"여드레"}, {9, L"아흐레"}, {10, L"열"},  {20, L"스무"}};

// Mapping from tens place (10, 20, 30) to Hangul words
inline const std::unordered_map<int, std::wstring> DAYS_ONLY_TENS_MAP = {
    {10, L"열흘"}, {20, L"스무날"}, {30, L"서른날"}};

} // namespace Days
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // DAYS_CONSTANTS_HPP
