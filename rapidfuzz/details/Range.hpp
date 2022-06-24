/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2022 Max Bachmann */

#pragma once

#include <iterator>
#include <vector>

namespace rapidfuzz {
namespace detail {

template <typename Iter>
class Range {
public:
    constexpr Range(Iter first, Iter last) : _first(first), _last(last)
    {}

    constexpr Iter begin() const
    {
        return _first;
    }
    constexpr Iter end() const
    {
        return _last;
    }
    constexpr ptrdiff_t size() const
    {
        return std::distance(_first, _last);
    }
    constexpr bool empty() const
    {
        return size() == 0;
    }
    explicit constexpr operator bool() const
    {
        return !empty();
    }
    constexpr decltype(auto) operator[](ptrdiff_t n) const
    {
        return _first[n];
    }

private:
    Iter _first;
    Iter _last;
};

template <typename CharT>
CharT* to_begin(CharT* s)
{
    return s;
}

template <typename T>
auto to_begin(T& x)
{
    using std::begin;
    return begin(x);
}

template <typename CharT>
CharT* to_end(CharT* s)
{
    while (*s != 0) {
        ++s;
    }
    return s;
}

template <typename T>
auto to_end(T& x)
{
    using std::end;
    return end(x);
}

template <typename Iter>
constexpr auto make_range(Iter first, Iter last)
{
    return Range<Iter>(first, last);
}

template <typename T>
constexpr auto make_range(T& x)
{
    auto first = to_begin(x);
    return Range<decltype(first)>(first, to_end(x));
}

template <typename InputIt1, typename InputIt2>
inline bool operator==(const Range<InputIt1>& a, const Range<InputIt2>& b)
{
    return std::equal(a.begin(), a.end(), b.begin(), b.end());
}

template <typename InputIt1, typename InputIt2>
inline bool operator!=(const Range<InputIt1>& a, const Range<InputIt2>& b)
{
    return !(a == b);
}

template <typename InputIt1, typename InputIt2>
inline bool operator<(const Range<InputIt1>& a, const Range<InputIt2>& b)
{
    return (std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end()));
}

template <typename InputIt1, typename InputIt2>
inline bool operator>(const Range<InputIt1>& a, const Range<InputIt2>& b)
{
    return b < a;
}

template <typename InputIt1, typename InputIt2>
inline bool operator<=(const Range<InputIt1>& a, const Range<InputIt2>& b)
{
    return !(b < a);
}

template <typename InputIt1, typename InputIt2>
inline bool operator>=(const Range<InputIt1>& a, const Range<InputIt2>& b)
{
    return !(a < b);
}

template <typename InputIt>
using RangeVec = std::vector<Range<InputIt>>;

} // namespace detail
} // namespace rapidfuzz