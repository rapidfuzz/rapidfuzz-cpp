/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2022 Max Bachmann */

#pragma once

#include <iterator>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace rapidfuzz::detail {

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
class Range {
    Iter _first;
    Iter _last;

public:
    using value_type = typename std::iterator_traits<Iter>::value_type;
    using iterator = Iter;
    using reverse_iterator = std::reverse_iterator<iterator>;

    constexpr Range(Iter first, Iter last) : _first(first), _last(last)
    {}

    template <typename T>
    constexpr Range(T& x) : _first(to_begin(x)), _last(to_end(x))
    {}

    constexpr iterator begin() const noexcept
    {
        return _first;
    }
    constexpr iterator end() const noexcept
    {
        return _last;
    }

    constexpr reverse_iterator rbegin() const noexcept
    {
        return reverse_iterator(end());
    }
    constexpr reverse_iterator rend() const noexcept
    {
        return reverse_iterator(begin());
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

    constexpr void remove_prefix(ptrdiff_t n)
    {
        _first += n;
    }
    constexpr void remove_suffix(ptrdiff_t n)
    {
        _last -= n;
    }

    constexpr Range subseq(ptrdiff_t pos = 0, ptrdiff_t count = std::numeric_limits<ptrdiff_t>::max())
    {
        if (pos > size()) throw std::out_of_range("Index out of range in Range::substr");

        auto start = _first + pos;
        if (std::distance(start, _last) < count) return {start, _last};
        return {start, start + count};
    }

    constexpr decltype(auto) front() const
    {
        return *(_first);
    }

    constexpr decltype(auto) back() const
    {
        return *(_last - 1);
    }

    constexpr Range<reverse_iterator> reversed() const
    {
        return {rbegin(), rend()};
    }

    friend std::ostream& operator<<(std::ostream& os, const Range& seq)
    {
        os << "[";
        for (auto x : seq)
            os << static_cast<uint64_t>(x) << ", ";
        os << "]";
        return os;
    }
};

template <typename T>
Range(T& x) -> Range<decltype(to_begin(x))>;

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

} // namespace rapidfuzz::detail
