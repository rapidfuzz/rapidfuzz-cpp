#ifndef GET_CHOSEONG_CONSTANTS_HPP
#define GET_CHOSEONG_CONSTANTS_HPP

#include "_internal/constants.hpp"
#include <array>
#include <string>
#include <unordered_map>

namespace RapidFuzz {
namespace Utils {
namespace Hangul {
namespace GetChoseong {

// Define the JasoHangulNFD struct
struct JasoHangulNFD {
    wchar_t START_CHOSEONG;  // 'ㄱ'
    wchar_t START_JUNGSEONG; // 'ㅏ'
    wchar_t START_JONGSEONG; // 'ㄱ'
    wchar_t END_CHOSEONG;    // 'ㅎ'
    wchar_t END_JUNGSEONG;   // 'ㅣ'
    wchar_t END_JONGSEONG;   // 'ㅎ'
};

// Initialize JASO_HANGUL_NFD struct from _JASO_HANGUL_NFD array
inline const JasoHangulNFD JASO_HANGUL_NFD = {
    _JASO_HANGUL_NFD[0], // START_CHOSEONG: 'ㄱ'
    _JASO_HANGUL_NFD[1], // START_JUNGSEONG: 'ㅏ'
    _JASO_HANGUL_NFD[2], // START_JONGSEONG: 'ㄱ'
    _JASO_HANGUL_NFD[3], // END_CHOSEONG: 'ㅎ'
    _JASO_HANGUL_NFD[4], // END_JUNGSEONG: 'ㅣ'
    _JASO_HANGUL_NFD[5]  // END_JONGSEONG: 'ㅎ'
};

} // namespace GetChoseong
} // namespace Hangul
} // namespace Utils
} // namespace RapidFuzz

#endif // GET_CHOSEONG_CONSTANTS_HPP
