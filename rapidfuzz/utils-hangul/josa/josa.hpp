#ifndef JOSA_HPP
#define JOSA_HPP

#include "_internal/constants.hpp"
#include "_internal/utils.hpp"
#include <optional>
#include <string>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace Josa {

enum class JosaOption {
    이_가,
    을_를,
    은_는,
    으로_로,
    와_과,
    이나_나,
    이란_란,
    아_야,
    이랑_랑,
    이에요_예요,
    으로서_로서,
    으로써_로써,
    으로부터_로부터,
    이라_라
};

inline const std::array<JosaOption, 4> 로_조사 = {JosaOption::으로_로, JosaOption::으로서_로서,
                                                  JosaOption::으로써_로써, JosaOption::으로부터_로부터};

/**
 * @brief Appends the appropriate josa to the given word based on the specified JosaOption.
 *
 * @param word The input word to which the josa will be appended.
 * @param josa The JosaOption specifying which josa to use.
 * @return A new std::wstring with the josa appended.
 *
 * @example
 * std::wstring result = josa(L"사과", JosaOption::이_가); // "사과가"
 */
std::wstring josa(const std::wstring& word, JosaOption josa);

/**
 * @brief Function to pick the appropriate josa based on the word and JosaOption.
 *
 * @param word The input word to which the josa will be appended.
 * @param josa The JosaOption specifying which josa to use.
 * @return The selected josa as a std::wstring.
 *
 * @example
 * std::wstring selectedJosa = josaPicker(L"사과", JosaOption::이_가); // "가"
 */
std::wstring josaPicker(const std::wstring& word, JosaOption josa);

} // namespace Josa
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // JOSA_HPP
