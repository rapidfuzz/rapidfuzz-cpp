#pragma once
#include <string>
#include <rapidfuzz/distance/Levenshtein.hpp>

bool extract_strings(const uint8_t* data, size_t size, std::basic_string<uint8_t>& s1,
                     std::basic_string<uint8_t>& s2)
{
    if (size <= sizeof(uint32_t)) {
        return false;
    }
    uint32_t len1 = *(uint32_t*)data;

    if (len1 + sizeof(len1) > size) {
        return false;
    }

    data += sizeof(len1);
    size -= sizeof(len1);
    s1 = std::basic_string<uint8_t>(data, len1);
    s2 = std::basic_string<uint8_t>(data + len1, size - len1);
    return true;
}
