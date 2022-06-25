/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#include <algorithm>
#include <array>
#include <cctype>
#include <cwctype>

namespace rapidfuzz {
namespace detail {

template <typename InputIt1, typename InputIt2>
DecomposedSet<InputIt1, InputIt2, InputIt1> set_decomposition(SplittedSentenceView<InputIt1> a,
                                                              SplittedSentenceView<InputIt2> b)
{
    a.dedupe();
    b.dedupe();

    RangeVec<InputIt1> intersection;
    RangeVec<InputIt1> difference_ab;
    RangeVec<InputIt2> difference_ba = b.words();

    for (const auto& current_a : a.words()) {
        auto element_b = std::find(difference_ba.begin(), difference_ba.end(), current_a);

        if (element_b != difference_ba.end()) {
            difference_ba.erase(element_b);
            intersection.push_back(current_a);
        }
        else {
            difference_ab.push_back(current_a);
        }
    }

    return {difference_ab, difference_ba, intersection};
}

template <typename Sentence, typename CharT, typename>
std::basic_string<CharT> to_string(Sentence&& str)
{
    return std::basic_string<CharT>(std::forward<Sentence>(str));
}

template <typename Sentence, typename CharT, typename>
std::basic_string<CharT> to_string(const Sentence& str)
{
    return std::basic_string<CharT>(str.data(), str.size());
}

/**
 * Removes common prefix of two string views
 */
template <typename InputIt1, typename InputIt2>
size_t remove_common_prefix(InputIt1& first1, InputIt1 last1, InputIt2& first2, InputIt2 last2)
{
    auto prefix = std::distance(first1, std::mismatch(first1, last1, first2, last2).first);
    first1 += prefix;
    first2 += prefix;
    return static_cast<size_t>(prefix);
}

/**
 * Removes common suffix of two string views
 */
template <typename InputIt1, typename InputIt2>
size_t remove_common_suffix(InputIt1 first1, InputIt1& last1, InputIt2 first2, InputIt2& last2)
{
    auto rfirst1 = std::make_reverse_iterator(last1);
    auto rlast1 = std::make_reverse_iterator(first1);
    auto rfirst2 = std::make_reverse_iterator(last2);
    auto rlast2 = std::make_reverse_iterator(first2);

    auto suffix = std::distance(rfirst1, std::mismatch(rfirst1, rlast1, rfirst2, rlast2).first);
    last1 -= suffix;
    last2 -= suffix;
    return static_cast<size_t>(suffix);
}

/**
 * Removes common affix of two string views
 */
template <typename InputIt1, typename InputIt2>
StringAffix remove_common_affix(InputIt1& first1, InputIt1& last1, InputIt2& first2, InputIt2& last2)
{
    return StringAffix{remove_common_prefix(first1, last1, first2, last2),
                       remove_common_suffix(first1, last1, first2, last2)};
}

template <typename InputIt1, typename InputIt2>
StringAffix remove_common_affix(Range<InputIt1>& s1, Range<InputIt2>& s2)
{
    InputIt1 first1 = s1.begin();
    InputIt1 last1 = s1.end();
    InputIt2 first2 = s2.begin();
    InputIt2 last2 = s2.end();
    StringAffix affix = remove_common_affix(first1, last1, first2, last2);
    s1 = make_range(first1, last1);
    s2 = make_range(first2, last2);
    return affix;
}

template <typename, typename = void>
struct is_space_dispatch_tag : std::integral_constant<int, 0> {};

template <typename CharT>
struct is_space_dispatch_tag<CharT, typename std::enable_if<sizeof(CharT) == 1>::type>
    : std::integral_constant<int, 1> {};

/*
 * Implementation of is_space for char types that are at least 2 Byte in size
 */
template <typename CharT>
bool is_space_impl(const CharT ch, std::integral_constant<int, 0>)
{
    switch (ch) {
    case 0x0009:
    case 0x000A:
    case 0x000B:
    case 0x000C:
    case 0x000D:
    case 0x001C:
    case 0x001D:
    case 0x001E:
    case 0x001F:
    case 0x0020:
    case 0x0085:
    case 0x00A0:
    case 0x1680:
    case 0x2000:
    case 0x2001:
    case 0x2002:
    case 0x2003:
    case 0x2004:
    case 0x2005:
    case 0x2006:
    case 0x2007:
    case 0x2008:
    case 0x2009:
    case 0x200A:
    case 0x2028:
    case 0x2029:
    case 0x202F:
    case 0x205F:
    case 0x3000: return true;
    }
    return false;
}

/*
 * Implementation of is_space for char types that are 1 Byte in size
 */
template <typename CharT>
bool is_space_impl(const CharT ch, std::integral_constant<int, 1>)
{
    switch (ch) {
    case 0x0009:
    case 0x000A:
    case 0x000B:
    case 0x000C:
    case 0x000D:
    case 0x001C:
    case 0x001D:
    case 0x001E:
    case 0x001F:
    case 0x0020: return true;
    }
    return false;
}

/*
 * checks whether unicode characters have the bidirectional
 * type 'WS', 'B' or 'S' or the category 'Zs'
 */
template <typename CharT>
bool is_space(const CharT ch)
{
    return is_space_impl(ch, is_space_dispatch_tag<CharT>{});
}

template <typename InputIt, typename CharT>
SplittedSentenceView<InputIt> sorted_split(InputIt first, InputIt last)
{
    RangeVec<InputIt> splitted;
    auto second = first;

    for (; first != last; first = second + 1) {
        second = std::find_if(first, last, is_space<CharT>);

        if (first != second) {
            splitted.emplace_back(first, second);
        }

        if (second == last) break;
    }

    std::sort(splitted.begin(), splitted.end());

    return SplittedSentenceView<InputIt>(splitted);
}

} // namespace detail
} // namespace rapidfuzz
