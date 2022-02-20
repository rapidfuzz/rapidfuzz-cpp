/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#include <rapidfuzz/distance/Levenshtein.hpp>
#include <string>
#include <stdexcept>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size <= 4) {
    return 0;
  }
  // Extract Bytecode length:
  uint32_t codeLen = 0;
  memcpy(&codeLen, data, sizeof(codeLen));

  if (codeLen+4 > size || codeLen+4 < 4){
     return 0;
  }
  const uint8_t *code = &data[4];
  const uint8_t *input = &data[4+codeLen];

  std::basic_string<uint8_t> s1(code, codeLen);
  std::basic_string<uint8_t> s2(input,size-codeLen-4);

  rapidfuzz::levenshtein_editops(s1, s2);
  return 0;
}
