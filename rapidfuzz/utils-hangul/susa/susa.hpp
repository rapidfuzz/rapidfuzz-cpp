#ifndef SUSA_HPP
#define SUSA_HPP

#include <string>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace Susa {

/**
 * @brief 숫자를 순 우리말 수사로 변환합니다. 주어진 숫자가 0보다 크고 100 이하일 때 유효합니다.
 *
 * @remarks
 * - **수사**란 숫자를 나타내는 우리말 단어입니다. [자세히
 * 알아보기](https://ko.dict.naver.com/#/entry/koko/d0ce2b674cae4b44b9028f648dd458b0)
 * - **수관형사**는 사물의 수나 양을 나타내는 관형사입니다. ‘두 사람’의 ‘두’, ‘세 근’의 ‘세’ 따위를 뜻 합니다.
 * [자세히 알아보기](https://ko.dict.naver.com/#/entry/koko/c513782b82554ff499c80ec616c5b611)
 *
 * @param num 숫자를 입력합니다.
 * @param classifier 수관형사를 사용할지 여부를 입력합니다. 기본값은 false입니다.
 * @return 변환된 수사를 `std::wstring` 형식으로 반환합니다.
 *
 * @throws std::invalid_argument if `num` is not within the valid range or not an integer.
 *
 * @example
 * susa(1); // L"하나"
 * susa(2); // L"둘"
 * susa(11); // L"열하나"
 * susa(21); // L"스물하나"
 * susa(99); // L"아흔아홉"
 * susa(100); // L"백"
 * susa(1, true); // L"한"
 * susa(2, true); // L"두"
 * susa(11, true); // L"열한"
 * susa(20, true); // L"스무"
 * susa(21, true); // L"스물한"
 *
 * @see https://es-hangul.slash.page/docs/api/susa
 */
std::wstring susa(int num, bool classifier = false);

} // namespace Susa
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // SUSa_HPP
