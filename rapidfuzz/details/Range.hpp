/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2022 Max Bachmann */

#pragma once

#include <cstddef>
#include <iterator>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace rapidfuzz::detail {

static inline void assume(bool b)
{
#if defined(__clang__)
    __builtin_assume(b);
#elif defined(__GNUC__) || defined(__GNUG__)
    if (!b) __builtin_unreachable();
#elif defined(_MSC_VER)
    __assume(b);
#endif
}

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
    assume(s != nullptr);
    while (*s != 0)
        ++s;

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

    constexpr size_t size() const
    {
        // todo store length to prevent recalculation which could be expensive
        assert(std::distance(_first, _last) >= 0);
        return static_cast<size_t>(std::distance(_first, _last));
    }
    constexpr bool empty() const
    {
        return size() == 0;
    }
    explicit constexpr operator bool() const
    {
        return !empty();
    }

    template <
        typename... Dummy, typename IterCopy = Iter,
        typename = std::enable_if_t<std::is_base_of_v<
            std::random_access_iterator_tag, typename std::iterator_traits<IterCopy>::iterator_category>>>
    constexpr decltype(auto) operator[](size_t n) const
    {
        return _first[static_cast<ptrdiff_t>(n)];
    }

    constexpr void remove_prefix(size_t n)
    {
        if constexpr (std::is_base_of_v<std::random_access_iterator_tag,
                                        typename std::iterator_traits<Iter>::iterator_category>)
            _first += static_cast<ptrdiff_t>(n);
        else
            for (size_t i = 0; i < n; ++i)
                _first++;
    }
    constexpr void remove_suffix(size_t n)
    {
        if constexpr (std::is_base_of_v<std::random_access_iterator_tag,
                                        typename std::iterator_traits<Iter>::iterator_category>)
            _last -= static_cast<ptrdiff_t>(n);
        else
            for (size_t i = 0; i < n; ++i)
                _last--;
    }

    constexpr Range subseq(size_t pos = 0, size_t count = std::numeric_limits<size_t>::max())
    {
        if (pos > size()) throw std::out_of_range("Index out of range in Range::substr");

        // todo we should probably support non random access iterators here too
        ptrdiff_t scount = static_cast<ptrdiff_t>(count);
        auto start = _first + static_cast<ptrdiff_t>(pos);
        if (std::distance(start, _last) < scount) return {start, _last};
        return {start, start + scount};
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
