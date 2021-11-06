
/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2021 Max Bachmann */

/* This is heavily inspired by the implementation used inside CPython
 * Original License: PSFL
 *
 * Written by Marc-Andre Lemburg (mal@lemburg.com).
 * Modified for Python 2.0 by Fredrik Lundh (fredrik@pythonware.com)
 * Copyright (c) Corporation for National Research Initiatives.
 */

#pragma once

#include <cstdint>
#include <type_traits>
#include <limits>

namespace rapidfuzz {
namespace Unicode {
namespace detail {

const unsigned short ALPHA_MASK          = 0x01;
const unsigned short DECIMAL_MASK        = 0x02;
const unsigned short DIGIT_MASK          = 0x04;
const unsigned short LOWER_MASK          = 0x08;
const unsigned short LINEBREAK_MASK      = 0x10;
const unsigned short SPACE_MASK          = 0x20;
const unsigned short TITLE_MASK          = 0x40;
const unsigned short UPPER_MASK          = 0x80;
const unsigned short XID_START_MASK      = 0x100;
const unsigned short XID_CONTINUE_MASK   = 0x200;
const unsigned short PRINTABLE_MASK      = 0x400;
const unsigned short NUMERIC_MASK        = 0x800;
const unsigned short CASE_IGNORABLE_MASK = 0x1000;
const unsigned short CASED_MASK          = 0x2000;
const unsigned short EXTENDED_CASE_MASK  = 0x4000;

struct TypeRecord {
    /*
       These are either deltas to the character or offsets in
       ExtendedCase.
    */
    const int upper;
    const int lower;
    const int title;

    /* Note if more flag space is needed, decimal and digit could be unified. */
    const unsigned char decimal;
    const unsigned char digit;
    const unsigned short flags;
};

extern const uint32_t SHIFT;
extern const TypeRecord TypeRecords[];
extern const uint32_t ExtendedCase[];
extern const unsigned short index1[];
extern const unsigned short index2[];

constexpr static bool is_alnum(const unsigned short flags) {
    return ((flags & ALPHA_MASK)
      || (flags & DECIMAL_MASK)
      || (flags & DIGIT_MASK)
      || (flags & NUMERIC_MASK));
}

static inline const TypeRecord* gettyperecord(uint32_t code)
{
    unsigned int index;
    if (code >= 0x110000)
        index = 0;
    else
    {
        index = index1[(code>>SHIFT)];
        index = index2[(index<<SHIFT)+(code&((1<<SHIFT)-1))];
    }

    return &TypeRecords[index];
}

bool IsWhitespace_u32(uint32_t ch);
bool IsWhitespace_u8(uint8_t ch);
}


/*
 * checks whether unicode characters have the bidirectional
 * type 'WS', 'B' or 'S' or the category 'Zs'
 */
template <typename CharT>
bool is_space(const CharT ch)
{
    if (ch < 0 || ch > std::numeric_limits<uint32_t>::max())
    {
        return false;
    }

    if (sizeof(CharT) == 1)
    {
        return detail::IsWhitespace_u8(static_cast<uint8_t>(ch));
    }
    else
    {
        return detail::IsWhitespace_u32(static_cast<uint32_t>(ch));
    }
}

static inline uint32_t UnicodeDefaultProcess(uint32_t ch)
{
    /* todo capital sigma not handled
     * see Python implementation
     */
    const detail::TypeRecord *ctype = detail::gettyperecord(ch);

    /* non alphanumeric characyers are replaces with whitespaces */
    if (!detail::is_alnum(ctype->flags)) {
        return ' ';
    }

    if (ctype->flags & detail::EXTENDED_CASE_MASK) {
        int index = ctype->lower & 0xFFFF;
        /*int n = ctype->lower >> 24;
        int i;
        for (i = 0; i < n; i++)
            res[i] = detail::ExtendedCase[index + i];*/
        /* for now ignore extended cases. The only exisiting
         * on is U+0130 anyways */
        return detail::ExtendedCase[index];
    }
    return ch + static_cast<uint32_t>(ctype->lower);
}

} // namespace Unicode
} // namespace rapidfuzz
