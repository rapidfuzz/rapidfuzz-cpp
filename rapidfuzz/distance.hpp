/* SPDX-License-Identifier: MIT */
/* Copyright © 2022-present Max Bachmann */

#pragma once
#include <rapidfuzz/distance/Hamming.hpp>
#include <rapidfuzz/distance/Indel.hpp>
#include <rapidfuzz/distance/Levenshtein.hpp>

namespace rapidfuzz {

template <typename CharT, typename InputIt1, typename InputIt2>
std::basic_string<CharT> editops_apply(const Editops& ops, InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    
    std::basic_string<CharT> res_str;
    res_str.resize(len1 + len2);
    int64_t src_pos = 0;
    int64_t dest_pos = 0;

    for (const auto& op : ops)
    {
        /* matches between last and current editop */
        while (src_pos < op.src_pos)
        {
            res_str[dest_pos] = static_cast<CharT>(first1[src_pos]);
            src_pos++;
            dest_pos++;
        }

        switch (op.type)
        {
        case EditType::None:
        case EditType::Replace:
            res_str[dest_pos] = static_cast<CharT>(first2[op.dest_pos]);
            src_pos++;
            dest_pos++;
            break;
        case EditType::Insert:
            res_str[dest_pos] = static_cast<CharT>(first2[op.dest_pos]);
            dest_pos++;
            break;
        case EditType::Delete:
            src_pos++;
            break;
        }
    }

    /* matches after the last editop */
    while (src_pos < len1)
    {
        res_str[dest_pos] = static_cast<CharT>(first1[src_pos]);
        src_pos++;
        dest_pos++;
    }

    res_str.resize(dest_pos);
    return res_str;
}

template <typename CharT, typename Sentence1, typename Sentence2>
std::basic_string<CharT> editops_apply(const Editops& ops, const Sentence1& s1, const Sentence2& s2)
{
    return editops_apply<CharT>(ops, common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                            common::to_end(s2));
}

} // namespace rapidfuzz
