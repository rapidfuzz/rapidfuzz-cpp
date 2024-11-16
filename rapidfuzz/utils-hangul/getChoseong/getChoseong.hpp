#ifndef GET_CHOSEONG_HPP
#define GET_CHOSEONG_HPP

#include "_internal/constants.hpp"
#include "utils.hpp"
#include <regex>
#include <string>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace GetChoseong {

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
std::wstring getChoseong(const std::wstring& word);

} // namespace GetChoseong
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // GET_CHOSEONG_HPP
