#ifndef ASSEMBLE_HPP
#define ASSEMBLE_HPP

#include <string>
#include <vector>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace Assemble {

/**
 * @brief 인자로 받은 배열에 담긴 한글 문장과 문자를 한글 규칙에 맞게 합성합니다.
 *
 * @param words A vector of `std::wstring` containing Hangul words and characters.
 * @return A `std::wstring` representing the assembled Hangul string.
 *
 * @example
 * std::vector<std::wstring> words = {L"아버지가", L" ", L"방ㅇ", L"ㅔ ", L"들ㅇ", L"ㅓ갑니다"};
 * std::wstring assembled = assemble(words); // "아버지가 방에 들어갑니다"
 *
 * std::vector<std::wstring> words = {L"아버지가", L" ", L"방에 ", L"들어갑니다"};
 * std::wstring assembled = assemble(words); // "아버지가 방에 들어갑니다"
 *
 * std::vector<std::wstring> words = {L"ㅇ", L"ㅏ", L"ㅂ", L"ㅓ", L"ㅈ", L"ㅣ"};
 * std::wstring assembled = assemble(words); // "아버지"
 */
std::wstring assemble(const std::vector<std::wstring>& words);

} // namespace Assemble
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // ASSEMBLE_HPP
