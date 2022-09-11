//  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
//  SPDX-License-Identifier: MIT
//  RapidFuzz v1.0.2
//  Generated: 2022-09-11 17:50:38.322175
//  ----------------------------------------------------------
//  This file is an amalgamation of multiple different files.
//  You probably shouldn't edit it directly.
//  ----------------------------------------------------------
#ifndef RAPIDFUZZ_AMALGAMATED_HPP_INCLUDED
#define RAPIDFUZZ_AMALGAMATED_HPP_INCLUDED

#include <algorithm>
#include <cmath>

#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>

#include <array>
#include <iterator>
#include <new>
#include <stdint.h>

namespace rapidfuzz {
namespace detail {

/* hashmap for integers which can only grow, but can't remove elements */
template <typename T_Key, typename T_Entry>
struct GrowingHashmap {
    using key_type = T_Key;
    using value_type = T_Entry;
    using size_type = unsigned int;

private:
    static constexpr size_type min_size = 8;
    struct MapElem {
        key_type key;
        value_type value = value_type();
    };

    int used;
    int fill;
    int mask;
    MapElem* m_map;

public:
    GrowingHashmap() : used(0), fill(0), mask(-1), m_map(NULL)
    {}
    ~GrowingHashmap()
    {
        delete[] m_map;
    }

    GrowingHashmap(const GrowingHashmap& other) : used(other.used), fill(other.fill), mask(other.mask)
    {
        int size = mask + 1;
        m_map = new MapElem[size];
        std::copy(other.m_map, other.m_map + size, m_map);
    }

    GrowingHashmap(GrowingHashmap&& other) noexcept : GrowingHashmap()
    {
        swap(*this, other);
    }

    GrowingHashmap& operator=(GrowingHashmap other)
    {
        swap(*this, other);
        return *this;
    }

    friend void swap(GrowingHashmap& first, GrowingHashmap& second) noexcept
    {
        std::swap(first.used, second.used);
        std::swap(first.fill, second.fill);
        std::swap(first.mask, second.mask);
        std::swap(first.m_map, second.m_map);
    }

    size_type size() const
    {
        return used;
    }
    size_type capacity() const
    {
        return mask + 1;
    }
    bool empty() const
    {
        return used == 0;
    }

    value_type get(key_type key) const noexcept
    {
        if (m_map == NULL) return value_type();

        return m_map[lookup(key)].value;
    }

    value_type& operator[](key_type key) noexcept
    {
        if (m_map == NULL) allocate();

        size_t i = lookup(key);

        if (m_map[i].value == value_type()) {
            /* resize when 2/3 full */
            if (++fill * 3 >= (mask + 1) * 2) {
                grow((used + 1) * 2);
                i = lookup(key);
            }

            used++;
        }

        m_map[i].key = key;
        return m_map[i].value;
    }

private:
    void allocate()
    {
        mask = min_size - 1;
        m_map = new MapElem[min_size];
    }

    /**
     * lookup key inside the hashmap using a similar collision resolution
     * strategy to CPython and Ruby
     */
    size_t lookup(key_type key) const
    {
        size_t hash = static_cast<size_t>(key);
        size_t i = hash & static_cast<size_t>(mask);

        if (m_map[i].value == value_type() || m_map[i].key == key) return i;

        size_t perturb = hash;
        while (true) {
            i = (i * 5 + perturb + 1) & static_cast<size_t>(mask);
            if (m_map[i].value == value_type() || m_map[i].key == key) return i;

            perturb >>= 5;
        }
    }

    void grow(int minUsed)
    {
        int newSize = mask + 1;
        while (newSize <= minUsed)
            newSize <<= 1;

        MapElem* oldMap = m_map;
        m_map = new MapElem[static_cast<size_t>(newSize)];

        fill = used;
        mask = newSize - 1;

        for (int i = 0; used > 0; i++)
            if (oldMap[i].value != value_type()) {
                size_t j = lookup(oldMap[i].key);

                m_map[j].key = oldMap[i].key;
                m_map[j].value = oldMap[i].value;
                used--;
            }

        used = fill;
        delete[] oldMap;
    }
};

template <typename T_Key, typename T_Entry>
struct HybridGrowingHashmap {
    using key_type = T_Key;
    using value_type = T_Entry;

    HybridGrowingHashmap()
    {
        m_extendedAscii.fill(value_type());
    }

    value_type get(char key) const noexcept
    {
        /** treat char as value between 0 and 127 for performance reasons */
        return m_extendedAscii[static_cast<uint8_t>(key)];
    }

    template <typename CharT>
    value_type get(CharT key) const noexcept
    {
        if (key >= 0 && key <= 255)
            return m_extendedAscii[static_cast<uint8_t>(key)];
        else
            return m_map.get(static_cast<key_type>(key));
    }

    value_type& operator[](char key) noexcept
    {
        /** treat char as value between 0 and 127 for performance reasons */
        return m_extendedAscii[static_cast<uint8_t>(key)];
    }

    template <typename CharT>
    value_type& operator[](CharT key)
    {
        if (key >= 0 && key <= 255)
            return m_extendedAscii[static_cast<uint8_t>(key)];
        else
            return m_map[static_cast<key_type>(key)];
    }

private:
    GrowingHashmap<key_type, value_type> m_map;
    std::array<value_type, 256> m_extendedAscii;
};

} // namespace detail
} // namespace rapidfuzz

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <stdint.h>
#include <stdio.h>
#include <vector>

namespace rapidfuzz {
namespace detail {

template <typename T, bool IsConst>
struct BitMatrixView {

    using value_type = T;
    using size_type = size_t;
    using pointer = std::conditional_t<IsConst, const value_type*, value_type*>;
    using reference = std::conditional_t<IsConst, const value_type&, value_type&>;

    BitMatrixView(pointer vector, size_type cols) noexcept : m_vector(vector), m_cols(cols)
    {}

    reference operator[](size_type col) noexcept
    {
        assert(col < m_cols);
        return m_vector[col];
    }

    size_type size() const noexcept
    {
        return m_cols;
    }

private:
    pointer m_vector;
    size_type m_cols;
};

template <typename T>
struct BitMatrix {

    using value_type = T;

    BitMatrix() : m_rows(0), m_cols(0), m_matrix(nullptr)
    {}

    BitMatrix(size_t rows, size_t cols, T val) : m_rows(rows), m_cols(cols), m_matrix(nullptr)
    {
        if (m_rows && m_cols) m_matrix = new T[m_rows * m_cols];
        std::fill_n(m_matrix, m_rows * m_cols, val);
    }

    BitMatrix(const BitMatrix& other) : m_rows(other.m_rows), m_cols(other.m_cols), m_matrix(nullptr)
    {
        if (m_rows && m_cols) m_matrix = new T[m_rows * m_cols];
        std::copy(other.m_matrix, other.m_matrix + m_rows * m_cols, m_matrix);
    }

    BitMatrix(BitMatrix&& other) noexcept : m_rows(0), m_cols(0), m_matrix(nullptr)
    {
        other.swap(*this);
    }

    BitMatrix& operator=(BitMatrix&& other) noexcept
    {
        other.swap(*this);
        return *this;
    }

    BitMatrix& operator=(const BitMatrix& other)
    {
        BitMatrix temp = other;
        temp.swap(*this);
        return *this;
    }

    void swap(BitMatrix& rhs) noexcept
    {
        using std::swap;
        swap(m_rows, rhs.m_rows);
        swap(m_cols, rhs.m_cols);
        swap(m_matrix, rhs.m_matrix);
    }

    ~BitMatrix()
    {
        delete[] m_matrix;
    }

    BitMatrixView<value_type, false> operator[](size_t row) noexcept
    {
        assert(row < m_rows);
        return {&m_matrix[row * m_cols], m_cols};
    }

    BitMatrixView<value_type, true> operator[](size_t row) const noexcept
    {
        assert(row < m_rows);
        return {&m_matrix[row * m_cols], m_cols};
    }

    size_t rows() const noexcept
    {
        return m_rows;
    }

    size_t cols() const noexcept
    {
        return m_cols;
    }

private:
    size_t m_rows;
    size_t m_cols;
    T* m_matrix;
};

template <typename T>
struct ShiftedBitMatrix {
    using value_type = T;

    ShiftedBitMatrix()
    {}

    ShiftedBitMatrix(size_t rows, size_t cols, T val) : m_matrix(rows, cols, val), m_offsets(rows)
    {}

    ShiftedBitMatrix(const ShiftedBitMatrix& other) : m_matrix(other.m_matrix), m_offsets(other.m_offsets)
    {}

    ShiftedBitMatrix(ShiftedBitMatrix&& other) noexcept
    {
        other.swap(*this);
    }

    ShiftedBitMatrix& operator=(ShiftedBitMatrix&& other) noexcept
    {
        other.swap(*this);
        return *this;
    }

    ShiftedBitMatrix& operator=(const ShiftedBitMatrix& other)
    {
        ShiftedBitMatrix temp = other;
        temp.swap(*this);
        return *this;
    }

    void swap(ShiftedBitMatrix& rhs) noexcept
    {
        using std::swap;
        swap(m_matrix, rhs.m_matrix);
        swap(m_offsets, rhs.m_offsets);
    }

    bool test_bit(size_t row, size_t col, bool default_ = false) const noexcept
    {
        ptrdiff_t offset = static_cast<ptrdiff_t>(m_offsets[row]);

        if (offset < 0) {
            col += static_cast<size_t>(-offset);
        }
        else if (col >= static_cast<size_t>(offset)) {
            col -= static_cast<size_t>(offset);
        }
        /* bit on the left of the band */
        else {
            return default_;
        }

        size_t word_size = sizeof(value_type) * 8;
        size_t col_word = col / word_size;
        uint64_t col_mask = value_type(1) << (col % word_size);

        return bool(m_matrix[row][col_word] & col_mask);
    }

    auto operator[](size_t row) noexcept
    {
        return m_matrix[row];
    }

    auto operator[](size_t row) const noexcept
    {
        return m_matrix[row];
    }

    void set_offset(size_t row, ptrdiff_t offset)
    {
        m_offsets[row] = offset;
    }

private:
    BitMatrix<value_type> m_matrix;
    std::vector<ptrdiff_t> m_offsets;
};

} // namespace detail
} // namespace rapidfuzz

#include <iterator>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace rapidfuzz {
namespace detail {

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
        os << "{";
        for (auto x : seq)
            os << static_cast<uint64_t>(x) << ", ";
        os << "]";
        return os;
    }
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

#include <array>
#include <cmath>
#include <cstring>
#include <limits>

#include <algorithm>

#include <algorithm>
#include <cassert>
#include <stddef.h>
#include <stdexcept>
#include <stdint.h>
#include <type_traits>
#include <vector>

namespace rapidfuzz {

struct StringAffix {
    size_t prefix_len;
    size_t suffix_len;
};

struct LevenshteinWeightTable {
    int64_t insert_cost;
    int64_t delete_cost;
    int64_t replace_cost;
};

/**
 * @brief Edit operation types used by the Levenshtein distance
 */
enum class EditType {
    None = 0,    /**< No Operation required */
    Replace = 1, /**< Replace a character if a string by another character */
    Insert = 2,  /**< Insert a character into a string */
    Delete = 3   /**< Delete a character from a string */
};

/**
 * @brief Edit operations used by the Levenshtein distance
 *
 * This represents an edit operation of type type which is applied to
 * the source string
 *
 * Replace: replace character at src_pos with character at dest_pos
 * Insert:  insert character from dest_pos at src_pos
 * Delete:  delete character at src_pos
 */
struct EditOp {
    EditType type;   /**< type of the edit operation */
    size_t src_pos;  /**< index into the source string */
    size_t dest_pos; /**< index into the destination string */

    EditOp() : type(EditType::None), src_pos(0), dest_pos(0)
    {}

    EditOp(EditType type_, size_t src_pos_, size_t dest_pos_)
        : type(type_), src_pos(src_pos_), dest_pos(dest_pos_)
    {}
};

inline bool operator==(EditOp a, EditOp b)
{
    return (a.type == b.type) && (a.src_pos == b.src_pos) && (a.dest_pos == b.dest_pos);
}

inline bool operator!=(EditOp a, EditOp b)
{
    return !(a == b);
}

/**
 * @brief Edit operations used by the Levenshtein distance
 *
 * This represents an edit operation of type type which is applied to
 * the source string
 *
 * None:    s1[src_begin:src_end] == s1[dest_begin:dest_end]
 * Replace: s1[i1:i2] should be replaced by s2[dest_begin:dest_end]
 * Insert:  s2[dest_begin:dest_end] should be inserted at s1[src_begin:src_begin].
 *          Note that src_begin==src_end in this case.
 * Delete:  s1[src_begin:src_end] should be deleted.
 *          Note that dest_begin==dest_end in this case.
 */
struct Opcode {
    EditType type;     /**< type of the edit operation */
    size_t src_begin;  /**< index into the source string */
    size_t src_end;    /**< index into the source string */
    size_t dest_begin; /**< index into the destination string */
    size_t dest_end;   /**< index into the destination string */

    Opcode() : type(EditType::None), src_begin(0), src_end(0), dest_begin(0), dest_end(0)
    {}

    Opcode(EditType type_, size_t src_begin_, size_t src_end_, size_t dest_begin_, size_t dest_end_)
        : type(type_), src_begin(src_begin_), src_end(src_end_), dest_begin(dest_begin_), dest_end(dest_end_)
    {}
};

inline bool operator==(Opcode a, Opcode b)
{
    return (a.type == b.type) && (a.src_begin == b.src_begin) && (a.src_end == b.src_end) &&
           (a.dest_begin == b.dest_begin) && (a.dest_end == b.dest_end);
}

inline bool operator!=(Opcode a, Opcode b)
{
    return !(a == b);
}

namespace detail {
template <typename Vec>
auto vector_slice(const Vec& vec, int start, int stop, int step) -> Vec
{
    Vec new_vec;

    if (step == 0) throw std::invalid_argument("slice step cannot be zero");
    if (step < 0) throw std::invalid_argument("step sizes below 0 lead to an invalid order of editops");

    if (start < 0)
        start = std::max<int>(start + static_cast<int>(vec.size()), 0);
    else if (start > static_cast<int>(vec.size()))
        start = static_cast<int>(vec.size());

    if (stop < 0)
        stop = std::max<int>(stop + static_cast<int>(vec.size()), 0);
    else if (stop > static_cast<int>(vec.size()))
        stop = static_cast<int>(vec.size());

    if (start >= stop) return new_vec;

    int count = (stop - 1 - start) / step + 1;
    new_vec.reserve(static_cast<size_t>(count));

    for (int i = start; i < stop; i += step)
        new_vec.push_back(vec[static_cast<size_t>(i)]);

    return new_vec;
}

template <typename Vec>
void vector_remove_slice(Vec& vec, int start, int stop, int step)
{
    if (step == 0) throw std::invalid_argument("slice step cannot be zero");
    if (step < 0) throw std::invalid_argument("step sizes below 0 lead to an invalid order of editops");

    if (start < 0)
        start = std::max<int>(start + static_cast<int>(vec.size()), 0);
    else if (start > static_cast<int>(vec.size()))
        start = static_cast<int>(vec.size());

    if (stop < 0)
        stop = std::max<int>(stop + static_cast<int>(vec.size()), 0);
    else if (stop > static_cast<int>(vec.size()))
        stop = static_cast<int>(vec.size());

    if (start >= stop) return;

    auto iter = vec.begin() + start;
    for (int i = start; i < static_cast<int>(vec.size()); i++)
        if (i >= stop || ((i - start) % step != 0)) *(iter++) = vec[static_cast<size_t>(i)];

    vec.resize(static_cast<size_t>(std::distance(vec.begin(), iter)));
    vec.shrink_to_fit();
}

} // namespace detail

class Opcodes;

class Editops : private std::vector<EditOp> {
public:
    using std::vector<EditOp>::size_type;

    Editops() noexcept : src_len(0), dest_len(0)
    {}

    Editops(size_type count, const EditOp& value) : std::vector<EditOp>(count, value), src_len(0), dest_len(0)
    {}

    explicit Editops(size_type count) : std::vector<EditOp>(count), src_len(0), dest_len(0)
    {}

    Editops(const Editops& other)
        : std::vector<EditOp>(other), src_len(other.src_len), dest_len(other.dest_len)
    {}

    Editops(const Opcodes& other);

    Editops(Editops&& other) noexcept
    {
        swap(other);
    }

    Editops& operator=(Editops other) noexcept
    {
        swap(other);
        return *this;
    }

    /* Element access */
    using std::vector<EditOp>::at;
    using std::vector<EditOp>::operator[];
    using std::vector<EditOp>::front;
    using std::vector<EditOp>::back;
    using std::vector<EditOp>::data;

    /* Iterators */
    using std::vector<EditOp>::begin;
    using std::vector<EditOp>::cbegin;
    using std::vector<EditOp>::end;
    using std::vector<EditOp>::cend;
    using std::vector<EditOp>::rbegin;
    using std::vector<EditOp>::crbegin;
    using std::vector<EditOp>::rend;
    using std::vector<EditOp>::crend;

    /* Capacity */
    using std::vector<EditOp>::empty;
    using std::vector<EditOp>::size;
    using std::vector<EditOp>::max_size;
    using std::vector<EditOp>::reserve;
    using std::vector<EditOp>::capacity;
    using std::vector<EditOp>::shrink_to_fit;

    /* Modifiers */
    using std::vector<EditOp>::clear;
    using std::vector<EditOp>::insert;
    using std::vector<EditOp>::emplace;
    using std::vector<EditOp>::erase;
    using std::vector<EditOp>::push_back;
    using std::vector<EditOp>::emplace_back;
    using std::vector<EditOp>::pop_back;
    using std::vector<EditOp>::resize;

    void swap(Editops& rhs) noexcept
    {
        std::swap(src_len, rhs.src_len);
        std::swap(dest_len, rhs.dest_len);
        std::vector<EditOp>::swap(rhs);
    }

    Editops slice(int start, int stop, int step = 1) const
    {
        Editops ed_slice = detail::vector_slice(*this, start, stop, step);
        ed_slice.src_len = src_len;
        ed_slice.dest_len = dest_len;
        return ed_slice;
    }

    void remove_slice(int start, int stop, int step = 1)
    {
        detail::vector_remove_slice(*this, start, stop, step);
    }

    Editops reverse() const
    {
        Editops reversed = *this;
        std::reverse(reversed.begin(), reversed.end());
        return reversed;
    }

    size_t get_src_len() const noexcept
    {
        return src_len;
    }
    void set_src_len(size_t len) noexcept
    {
        src_len = len;
    }
    size_t get_dest_len() const noexcept
    {
        return dest_len;
    }
    void set_dest_len(size_t len) noexcept
    {
        dest_len = len;
    }

    Editops inverse() const
    {
        Editops inv_ops = *this;
        std::swap(inv_ops.src_len, inv_ops.dest_len);
        for (auto& op : inv_ops) {
            std::swap(op.src_pos, op.dest_pos);
            if (op.type == EditType::Delete)
                op.type = EditType::Insert;
            else if (op.type == EditType::Insert)
                op.type = EditType::Delete;
        }
        return inv_ops;
    }

    Editops remove_subsequence(const Editops& subsequence) const
    {
        Editops result;
        result.set_src_len(src_len);
        result.set_dest_len(dest_len);

        if (subsequence.size() > size()) throw std::invalid_argument("subsequence is not a subsequence");

        result.resize(size() - subsequence.size());

        /* offset to correct removed edit operations */
        int offset = 0;
        auto op_iter = begin();
        auto op_end = end();
        size_t result_pos = 0;
        for (const auto& sop : subsequence) {
            for (; op_iter != op_end && sop != *op_iter; op_iter++) {
                result[result_pos] = *op_iter;
                result[result_pos].src_pos =
                    static_cast<size_t>(static_cast<ptrdiff_t>(result[result_pos].src_pos) + offset);
                result_pos++;
            }
            /* element of subsequence not part of the sequence */
            if (op_iter == op_end) throw std::invalid_argument("subsequence is not a subsequence");

            if (sop.type == EditType::Insert)
                offset++;
            else if (sop.type == EditType::Delete)
                offset--;
            op_iter++;
        }

        /* add remaining elements */
        for (; op_iter != op_end; op_iter++) {
            result[result_pos] = *op_iter;
            result[result_pos].src_pos =
                static_cast<size_t>(static_cast<ptrdiff_t>(result[result_pos].src_pos) + offset);
            result_pos++;
        }

        return result;
    }

private:
    size_t src_len;
    size_t dest_len;
};

inline bool operator==(const Editops& lhs, const Editops& rhs)
{
    if (lhs.get_src_len() != rhs.get_src_len() || lhs.get_dest_len() != rhs.get_dest_len()) {
        return false;
    }

    if (lhs.size() != rhs.size()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

inline bool operator!=(const Editops& lhs, const Editops& rhs)
{
    return !(lhs == rhs);
}

inline void swap(Editops& lhs, Editops& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

class Opcodes : private std::vector<Opcode> {
public:
    using std::vector<Opcode>::size_type;

    Opcodes() noexcept : src_len(0), dest_len(0)
    {}

    Opcodes(size_type count, const Opcode& value) : std::vector<Opcode>(count, value), src_len(0), dest_len(0)
    {}

    explicit Opcodes(size_type count) : std::vector<Opcode>(count), src_len(0), dest_len(0)
    {}

    Opcodes(const Opcodes& other)
        : std::vector<Opcode>(other), src_len(other.src_len), dest_len(other.dest_len)
    {}

    Opcodes(const Editops& other);

    Opcodes(Opcodes&& other) noexcept
    {
        swap(other);
    }

    Opcodes& operator=(Opcodes other) noexcept
    {
        swap(other);
        return *this;
    }

    /* Element access */
    using std::vector<Opcode>::at;
    using std::vector<Opcode>::operator[];
    using std::vector<Opcode>::front;
    using std::vector<Opcode>::back;
    using std::vector<Opcode>::data;

    /* Iterators */
    using std::vector<Opcode>::begin;
    using std::vector<Opcode>::cbegin;
    using std::vector<Opcode>::end;
    using std::vector<Opcode>::cend;
    using std::vector<Opcode>::rbegin;
    using std::vector<Opcode>::crbegin;
    using std::vector<Opcode>::rend;
    using std::vector<Opcode>::crend;

    /* Capacity */
    using std::vector<Opcode>::empty;
    using std::vector<Opcode>::size;
    using std::vector<Opcode>::max_size;
    using std::vector<Opcode>::reserve;
    using std::vector<Opcode>::capacity;
    using std::vector<Opcode>::shrink_to_fit;

    /* Modifiers */
    using std::vector<Opcode>::clear;
    using std::vector<Opcode>::insert;
    using std::vector<Opcode>::emplace;
    using std::vector<Opcode>::erase;
    using std::vector<Opcode>::push_back;
    using std::vector<Opcode>::emplace_back;
    using std::vector<Opcode>::pop_back;
    using std::vector<Opcode>::resize;

    void swap(Opcodes& rhs) noexcept
    {
        std::swap(src_len, rhs.src_len);
        std::swap(dest_len, rhs.dest_len);
        std::vector<Opcode>::swap(rhs);
    }

    Opcodes slice(int start, int stop, int step = 1) const
    {
        Opcodes ed_slice = detail::vector_slice(*this, start, stop, step);
        ed_slice.src_len = src_len;
        ed_slice.dest_len = dest_len;
        return ed_slice;
    }

    Opcodes reverse() const
    {
        Opcodes reversed = *this;
        std::reverse(reversed.begin(), reversed.end());
        return reversed;
    }

    size_t get_src_len() const noexcept
    {
        return src_len;
    }
    void set_src_len(size_t len) noexcept
    {
        src_len = len;
    }
    size_t get_dest_len() const noexcept
    {
        return dest_len;
    }
    void set_dest_len(size_t len) noexcept
    {
        dest_len = len;
    }

    Opcodes inverse() const
    {
        Opcodes inv_ops = *this;
        std::swap(inv_ops.src_len, inv_ops.dest_len);
        for (auto& op : inv_ops) {
            std::swap(op.src_begin, op.dest_begin);
            std::swap(op.src_end, op.dest_end);
            if (op.type == EditType::Delete)
                op.type = EditType::Insert;
            else if (op.type == EditType::Insert)
                op.type = EditType::Delete;
        }
        return inv_ops;
    }

private:
    size_t src_len;
    size_t dest_len;
};

inline bool operator==(const Opcodes& lhs, const Opcodes& rhs)
{
    if (lhs.get_src_len() != rhs.get_src_len() || lhs.get_dest_len() != rhs.get_dest_len()) return false;

    if (lhs.size() != rhs.size()) return false;

    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

inline bool operator!=(const Opcodes& lhs, const Opcodes& rhs)
{
    return !(lhs == rhs);
}

inline void swap(Opcodes& lhs, Opcodes& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

inline Editops::Editops(const Opcodes& other)
{
    src_len = other.get_src_len();
    dest_len = other.get_dest_len();
    for (const auto& op : other) {
        switch (op.type) {
        case EditType::None: break;

        case EditType::Replace:
            for (size_t j = 0; j < op.src_end - op.src_begin; j++)
                push_back({EditType::Replace, op.src_begin + j, op.dest_begin + j});
            break;

        case EditType::Insert:
            for (size_t j = 0; j < op.dest_end - op.dest_begin; j++)
                push_back({EditType::Insert, op.src_begin, op.dest_begin + j});
            break;

        case EditType::Delete:
            for (size_t j = 0; j < op.src_end - op.src_begin; j++)
                push_back({EditType::Delete, op.src_begin + j, op.dest_begin});
            break;
        }
    }
}

inline Opcodes::Opcodes(const Editops& other)
{
    src_len = other.get_src_len();
    dest_len = other.get_dest_len();
    size_t src_pos = 0;
    size_t dest_pos = 0;
    for (size_t i = 0; i < other.size();) {
        if (src_pos < other[i].src_pos || dest_pos < other[i].dest_pos) {
            push_back({EditType::None, src_pos, other[i].src_pos, dest_pos, other[i].dest_pos});
            src_pos = other[i].src_pos;
            dest_pos = other[i].dest_pos;
        }

        size_t src_begin = src_pos;
        size_t dest_begin = dest_pos;
        EditType type = other[i].type;
        do {
            switch (type) {
            case EditType::None: break;

            case EditType::Replace:
                src_pos++;
                dest_pos++;
                break;

            case EditType::Insert: dest_pos++; break;

            case EditType::Delete: src_pos++; break;
            }
            i++;
        } while (i < other.size() && other[i].type == type && src_pos == other[i].src_pos &&
                 dest_pos == other[i].dest_pos);

        push_back({type, src_begin, src_pos, dest_begin, dest_pos});
    }

    if (src_pos < other.get_src_len() || dest_pos < other.get_dest_len()) {
        push_back({EditType::None, src_pos, other.get_src_len(), dest_pos, other.get_dest_len()});
    }
}

template <typename T>
struct ScoreAlignment {
    T score;           /**< resulting score of the algorithm */
    size_t src_start;  /**< index into the source string */
    size_t src_end;    /**< index into the source string */
    size_t dest_start; /**< index into the destination string */
    size_t dest_end;   /**< index into the destination string */

    ScoreAlignment() : score(T()), src_start(0), src_end(0), dest_start(0), dest_end(0)
    {}

    ScoreAlignment(T score_, size_t src_start_, size_t src_end_, size_t dest_start_, size_t dest_end_)
        : score(score_),
          src_start(src_start_),
          src_end(src_end_),
          dest_start(dest_start_),
          dest_end(dest_end_)
    {}
};

template <typename T>
inline bool operator==(const ScoreAlignment<T>& a, const ScoreAlignment<T>& b)
{
    return (a.score == b.score) && (a.src_start == b.src_start) && (a.src_end == b.src_end) &&
           (a.dest_start == b.dest_start) && (a.dest_end == b.dest_end);
}

} // namespace rapidfuzz

#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace rapidfuzz {

namespace detail {
template <typename T>
auto inner_type(T const*) -> T;

template <typename T>
auto inner_type(T const&) -> typename T::value_type;
} // namespace detail

template <typename T>
using char_type = decltype(detail::inner_type(std::declval<T const&>()));

/* backport of std::iter_value_t from C++20
 * This does not cover the complete functionality, but should be enough for
 * the use cases in this library
 */
template <typename T>
using iter_value_t = typename std::iterator_traits<T>::value_type;

template <typename... Conds>
struct satisfies_all : std::true_type {};

template <typename Cond, typename... Conds>
struct satisfies_all<Cond, Conds...>
    : std::conditional<Cond::value, satisfies_all<Conds...>, std::false_type>::type {};

template <typename... Conds>
struct satisfies_any : std::false_type {};

template <typename Cond, typename... Conds>
struct satisfies_any<Cond, Conds...>
    : std::conditional<Cond::value, std::true_type, satisfies_any<Conds...>>::type {};

// taken from
// https://stackoverflow.com/questions/16893992/check-if-type-can-be-explicitly-converted
template <typename From, typename To>
struct is_explicitly_convertible {
    template <typename T>
    static void f(T);

    template <typename F, typename T>
    static constexpr auto test(int /*unused*/) -> decltype(f(static_cast<T>(std::declval<F>())), true)
    {
        return true;
    }

    template <typename F, typename T>
    static constexpr auto test(...) -> bool
    {
        return false;
    }

    static bool const value = test<From, To>(0);
};

#define GENERATE_HAS_MEMBER(member)                                                                          \
                                                                                                             \
    template <typename T>                                                                                    \
    struct has_member_##member {                                                                             \
    private:                                                                                                 \
        using yes = std::true_type;                                                                          \
        using no = std::false_type;                                                                          \
                                                                                                             \
        struct Fallback {                                                                                    \
            int member;                                                                                      \
        };                                                                                                   \
        struct Derived : T, Fallback {};                                                                     \
                                                                                                             \
        template <class U>                                                                                   \
        static no test(decltype(U::member)*);                                                                \
        template <typename U>                                                                                \
        static yes test(U*);                                                                                 \
                                                                                                             \
        template <typename U, typename = std::enable_if_t<std::is_class<U>::value>>                          \
        static constexpr bool class_test(U*)                                                                 \
        {                                                                                                    \
            return std::is_same<decltype(test<Derived>(nullptr)), yes>::value;                               \
        }                                                                                                    \
                                                                                                             \
        template <typename U, typename = std::enable_if_t<!std::is_class<U>::value>>                         \
        static constexpr bool class_test(const U&)                                                           \
        {                                                                                                    \
            return false;                                                                                    \
        }                                                                                                    \
                                                                                                             \
    public:                                                                                                  \
        static constexpr bool value = class_test(static_cast<T*>(nullptr));                                  \
    };

GENERATE_HAS_MEMBER(data) // Creates 'has_member_data'
GENERATE_HAS_MEMBER(size) // Creates 'has_member_size'

template <typename Sentence>
using has_data_and_size = satisfies_all<has_member_data<Sentence>, has_member_size<Sentence>>;

// This trait checks if a given type is a standard collection of hashable types
// SFINAE ftw
template <class T>
class is_hashable_sequence {
    is_hashable_sequence() = delete;
    using hashable = char;
    struct not_hashable {
        char t[2];
    }; // Ensured to work on any platform
    template <typename C>
    static hashable matcher(decltype(&std::hash<typename C::value_type>::operator()));
    template <typename C>
    static not_hashable matcher(...);

public:
    static bool const value = (sizeof(matcher<T>(nullptr)) == sizeof(hashable));
};

template <class T>
class is_standard_iterable {
    is_standard_iterable() = delete;
    using iterable = char;
    struct not_iterable {
        char t[2];
    }; // Ensured to work on any platform
    template <typename C>
    static iterable matcher(typename C::const_iterator*);
    template <typename C>
    static not_iterable matcher(...);

public:
    static bool const value = (sizeof(matcher<T>(nullptr)) == sizeof(iterable));
};

template <typename C>
void* sub_matcher(typename C::value_type const& (C::*)(int64_t) const);

// TODO: Not a real SFINAE, because of the ambiguity between
// value_type const& operator[](int64_t) const;
// and value_type& operator[](int64_t);
// Not really important
template <class T>
class has_bracket_operator {
    has_bracket_operator() = delete;
    using has_op = char;
    struct hasnt_op {
        char t[2];
    }; // Ensured to work on any platform
    template <typename C>
    static has_op matcher(decltype(sub_matcher<T>(&T::at)));
    template <typename C>
    static hasnt_op matcher(...);

public:
    static bool const value = (sizeof(matcher<T>(nullptr)) == sizeof(has_op));
};

namespace static_if_detail {

struct identity {
    template <typename T>
    T operator()(T&& x) const
    {
        return std::forward<T>(x);
    }
};

template <bool Cond>
struct statement {
    template <typename F>
    void then(const F& f)
    {
        f(identity());
    }

    template <typename F>
    void else_(const F&)
    {}
};

template <>
struct statement<false> {
    template <typename F>
    void then(const F&)
    {}

    template <typename F>
    void else_(const F& f)
    {
        f(identity());
    }
};

} // end of namespace static_if_detail

template <bool Cond, typename F>
static_if_detail::statement<Cond> static_if(F const& f)
{
    static_if_detail::statement<Cond> if_;
    if_.then(f);
    return if_;
}

} // namespace rapidfuzz

#include <string>

namespace rapidfuzz {

template <typename InputIt>
class SplittedSentenceView {
public:
    using CharT = iter_value_t<InputIt>;

    SplittedSentenceView(detail::RangeVec<InputIt> sentence) noexcept(
        std::is_nothrow_move_constructible<detail::RangeVec<InputIt>>::value)
        : m_sentence(std::move(sentence))
    {}

    size_t dedupe();
    size_t size() const;

    size_t length() const
    {
        return size();
    }

    bool empty() const
    {
        return m_sentence.empty();
    }

    size_t word_count() const
    {
        return m_sentence.size();
    }

    std::basic_string<CharT> join() const;

    const detail::RangeVec<InputIt>& words() const
    {
        return m_sentence;
    }

private:
    detail::RangeVec<InputIt> m_sentence;
};

template <typename InputIt>
size_t SplittedSentenceView<InputIt>::dedupe()
{
    size_t old_word_count = word_count();
    m_sentence.erase(std::unique(m_sentence.begin(), m_sentence.end()), m_sentence.end());
    return old_word_count - word_count();
}

template <typename InputIt>
size_t SplittedSentenceView<InputIt>::size() const
{
    if (m_sentence.empty()) return 0;

    // there is a whitespace between each word
    size_t result = m_sentence.size() - 1;
    for (const auto& word : m_sentence) {
        result += static_cast<size_t>(std::distance(word.begin(), word.end()));
    }

    return result;
}

template <typename InputIt>
auto SplittedSentenceView<InputIt>::join() const -> std::basic_string<CharT>
{
    if (m_sentence.empty()) {
        return std::basic_string<CharT>();
    }

    auto sentence_iter = m_sentence.begin();
    std::basic_string<CharT> joined(sentence_iter->begin(), sentence_iter->end());
    const std::basic_string<CharT> whitespace{0x20};
    ++sentence_iter;
    for (; sentence_iter != m_sentence.end(); ++sentence_iter) {
        joined.append(whitespace)
            .append(std::basic_string<CharT>(sentence_iter->begin(), sentence_iter->end()));
    }
    return joined;
}

} // namespace rapidfuzz

#include <bitset>
#include <cassert>
#include <cstddef>
#include <limits>
#include <stdint.h>
#include <type_traits>

#if defined(_MSC_VER) && !defined(__clang__)
#    include <intrin.h>
#endif

namespace rapidfuzz {
namespace detail {

/*
 * shift right without undefined behavior for shifts > bit width
 */
template <typename U>
constexpr uint64_t shr64(uint64_t a, U shift)
{
    return (shift < 64) ? a >> shift : 0;
}

constexpr uint64_t addc64(uint64_t a, uint64_t b, uint64_t carryin, uint64_t* carryout)
{
    /* todo should use _addcarry_u64 when available */
    a += carryin;
    *carryout = a < carryin;
    a += b;
    *carryout |= a < b;
    return a;
}

template <typename T, typename U>
constexpr T ceil_div(T a, U divisor)
{
    T _div = static_cast<T>(divisor);
    return a / _div + static_cast<T>(a % _div != 0);
}

static inline int popcount(uint64_t x)
{
    return static_cast<int>(std::bitset<64>(x).count());
}

static inline int popcount(uint32_t x)
{
    return static_cast<int>(std::bitset<32>(x).count());
}

static inline int popcount(uint16_t x)
{
    return static_cast<int>(std::bitset<16>(x).count());
}

static inline int popcount(uint8_t x)
{
    static constexpr int bit_count[256] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};
    return bit_count[x];
}

template <typename T>
constexpr T rotl(T x, unsigned int n)
{
    unsigned int num_bits = std::numeric_limits<T>::digits;
    assert(n < num_bits);
    unsigned int count_mask = num_bits - 1;

#if _MSC_VER && !defined(__clang__)
#    pragma warning(push)
/* unary minus operator applied to unsigned type, result still unsigned */
#    pragma warning(disable : 4146)
#endif
    return (x << n) | (x >> (-n & count_mask));
#if _MSC_VER && !defined(__clang__)
#    pragma warning(pop)
#endif
}

/**
 * Extract the lowest set bit from a. If no bits are set in a returns 0.
 */
template <typename T>
constexpr T blsi(T a)
{
#if _MSC_VER && !defined(__clang__)
#    pragma warning(push)
/* unary minus operator applied to unsigned type, result still unsigned */
#    pragma warning(disable : 4146)
#endif
    return a & -a;
#if _MSC_VER && !defined(__clang__)
#    pragma warning(pop)
#endif
}

/**
 * Clear the lowest set bit in a.
 */
template <typename T>
constexpr T blsr(T x)
{
    return x & (x - 1);
}

/**
 * Sets all the lower bits of the result to 1 up to and including lowest set bit (=1) in a.
 * If a is zero, blsmsk sets all bits to 1.
 */
template <typename T>
constexpr T blsmsk(T a)
{
    return a ^ (a - 1);
}

#if defined(_MSC_VER) && !defined(__clang__)
static inline int countr_zero(uint32_t x)
{
    unsigned long trailing_zero = 0;
    _BitScanForward(&trailing_zero, x);
    return trailing_zero;
}

#    if defined(_M_ARM) || defined(_M_X64)
static inline int countr_zero(uint64_t x)
{
    unsigned long trailing_zero = 0;
    _BitScanForward64(&trailing_zero, x);
    return trailing_zero;
}
#    else
static inline int countr_zero(uint64_t x)
{
    uint32_t msh = (uint32_t)(x >> 32);
    uint32_t lsh = (uint32_t)(x & 0xFFFFFFFF);
    if (lsh != 0) return countr_zero(lsh);
    return 32 + countr_zero(msh);
}
#    endif

#else /*  gcc / clang */
static inline int countr_zero(uint32_t x)
{
    return __builtin_ctz(x);
}

static inline int countr_zero(uint64_t x)
{
    return __builtin_ctzll(x);
}
#endif

template <typename T, T N, T Pos = 0, bool IsEmpty = (N == 0)>
struct UnrollImpl;

template <typename T, T N, T Pos>
struct UnrollImpl<T, N, Pos, false> {
    template <typename F>
    static void call(F&& f)
    {
        f(Pos);
        UnrollImpl<T, N - 1, Pos + 1>::call(std::forward<F>(f));
    }
};

template <typename T, T N, T Pos>
struct UnrollImpl<T, N, Pos, true> {
    template <typename F>
    static void call(F&&)
    {}
};

template <typename T, int N, class F>
constexpr void unroll(F&& f)
{
    UnrollImpl<T, N>::call(f);
}

} // namespace detail
} // namespace rapidfuzz

#include <vector>

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2, typename InputIt3>
struct DecomposedSet {
    SplittedSentenceView<InputIt1> difference_ab;
    SplittedSentenceView<InputIt2> difference_ba;
    SplittedSentenceView<InputIt3> intersection;
    DecomposedSet(SplittedSentenceView<InputIt1> diff_ab, SplittedSentenceView<InputIt2> diff_ba,
                  SplittedSentenceView<InputIt3> intersect)
        : difference_ab(std::move(diff_ab)),
          difference_ba(std::move(diff_ba)),
          intersection(std::move(intersect))
    {}
};

namespace detail {

/**
 * @defgroup Common Common
 * Common utilities shared among multiple functions
 * @{
 */

static inline double NormSim_to_NormDist(double score_cutoff, double imprecision = 0.00001)
{
    return std::min(1.0, 1.0 - score_cutoff + imprecision);
}

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

template <typename InputIt1, typename InputIt2>
DecomposedSet<InputIt1, InputIt2, InputIt1> set_decomposition(SplittedSentenceView<InputIt1> a,
                                                              SplittedSentenceView<InputIt2> b);

constexpr double result_cutoff(double result, double score_cutoff)
{
    return (result >= score_cutoff) ? result : 0;
}

template <int Max = 1>
constexpr double norm_distance(int64_t dist, int64_t lensum, double score_cutoff = 0)
{
    double max = static_cast<double>(Max);
    return result_cutoff((lensum > 0) ? (max - max * static_cast<double>(dist) / static_cast<double>(lensum))
                                      : max,
                         score_cutoff);
}

template <int Max = 1>
static inline int64_t score_cutoff_to_distance(double score_cutoff, int64_t lensum)
{
    return static_cast<int64_t>(std::ceil(static_cast<double>(lensum) * (1.0 - score_cutoff / Max)));
}

template <typename T>
constexpr bool is_zero(T a, T tolerance = std::numeric_limits<T>::epsilon())
{
    return std::fabs(a) <= tolerance;
}

template <typename Sentence, typename CharT = char_type<Sentence>,
          typename = std::enable_if_t<is_explicitly_convertible<Sentence, std::basic_string<CharT>>::value>>
std::basic_string<CharT> to_string(Sentence&& str);

template <typename Sentence, typename CharT = char_type<Sentence>,
          typename = std::enable_if_t<!is_explicitly_convertible<Sentence, std::basic_string<CharT>>::value &&
                                      has_data_and_size<Sentence>::value>>
std::basic_string<CharT> to_string(const Sentence& str);

template <typename InputIt1, typename InputIt2>
StringAffix remove_common_affix(detail::Range<InputIt1>& s1, detail::Range<InputIt2>& s2);

template <typename InputIt1, typename InputIt2>
StringAffix remove_common_affix(InputIt1& first1, InputIt1& last1, InputIt2& first2, InputIt2& last2);

template <typename InputIt1, typename InputIt2>
size_t remove_common_prefix(InputIt1& first1, InputIt1 last1, InputIt2& first2, InputIt2 last2);

template <typename InputIt1, typename InputIt2>
size_t remove_common_suffix(InputIt1 first1, InputIt1& last1, InputIt2 first2, InputIt2& last2);

template <typename InputIt, typename CharT = iter_value_t<InputIt>>
SplittedSentenceView<InputIt> sorted_split(InputIt first, InputIt last);

/**@}*/

} // namespace detail
} // namespace rapidfuzz

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

#include <cmath>

namespace rapidfuzz {
namespace detail {

template <typename T, typename... Args>
struct NormalizedMetricBase {
    template <typename InputIt1, typename InputIt2>
    static double normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                      Args... args, double score_cutoff)
    {
        return _normalized_distance(detail::make_range(first1, last1), detail::make_range(first2, last2),
                                    std::forward<Args>(args)..., score_cutoff);
    }

    template <typename Sentence1, typename Sentence2>
    static double normalized_distance(const Sentence1& s1, const Sentence2& s2, Args... args,
                                      double score_cutoff)
    {
        return _normalized_distance(detail::make_range(s1), detail::make_range(s2),
                                    std::forward<Args>(args)..., score_cutoff);
    }

    template <typename InputIt1, typename InputIt2>
    static double normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                        Args... args, double score_cutoff)
    {
        return _normalized_similarity(detail::make_range(first1, last1), detail::make_range(first2, last2),
                                      std::forward<Args>(args)..., score_cutoff);
    }

    template <typename Sentence1, typename Sentence2>
    static double normalized_similarity(const Sentence1& s1, const Sentence2& s2, Args... args,
                                        double score_cutoff)
    {
        return _normalized_similarity(detail::make_range(s1), detail::make_range(s2),
                                      std::forward<Args>(args)..., score_cutoff);
    }

protected:
    template <typename InputIt1, typename InputIt2>
    static double _normalized_distance(Range<InputIt1> s1, Range<InputIt2> s2, Args... args,
                                       double score_cutoff)
    {
        auto maximum = T::maximum(s1, s2, args...);
        int64_t cutoff_distance =
            static_cast<int64_t>(std::ceil(static_cast<double>(maximum) * score_cutoff));
        int64_t dist = T::_distance(s1, s2, std::forward<Args>(args)..., cutoff_distance);
        double norm_dist = (maximum) ? static_cast<double>(dist) / static_cast<double>(maximum) : 0.0;
        return (norm_dist <= score_cutoff) ? norm_dist : 1.0;
    }

    template <typename InputIt1, typename InputIt2>
    static double _normalized_similarity(Range<InputIt1> s1, Range<InputIt2> s2, Args... args,
                                         double score_cutoff)
    {
        double cutoff_score = NormSim_to_NormDist(score_cutoff);
        double norm_dist = _normalized_distance(s1, s2, std::forward<Args>(args)..., cutoff_score);
        double norm_sim = 1.0 - norm_dist;
        return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
    }

    NormalizedMetricBase(){};
    friend T;
};

template <typename T, typename... Args>
struct DistanceBase : public NormalizedMetricBase<T, Args...> {
    template <typename InputIt1, typename InputIt2>
    static int64_t distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, Args... args,
                            int64_t score_cutoff)
    {
        return T::_distance(detail::make_range(first1, last1), detail::make_range(first2, last2),
                            std::forward<Args>(args)..., score_cutoff);
    }

    template <typename Sentence1, typename Sentence2>
    static int64_t distance(const Sentence1& s1, const Sentence2& s2, Args... args, int64_t score_cutoff)
    {
        return T::_distance(detail::make_range(s1), detail::make_range(s2), std::forward<Args>(args)...,
                            score_cutoff);
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, Args... args,
                              int64_t score_cutoff)
    {
        return _similarity(detail::make_range(first1, last1), detail::make_range(first2, last2),
                           std::forward<Args>(args)..., score_cutoff);
    }

    template <typename Sentence1, typename Sentence2>
    static int64_t similarity(const Sentence1& s1, const Sentence2& s2, Args... args, int64_t score_cutoff)
    {
        return _similarity(detail::make_range(s1), detail::make_range(s2), std::forward<Args>(args)...,
                           score_cutoff);
    }

protected:
    template <typename InputIt1, typename InputIt2>
    static int64_t _similarity(Range<InputIt1> s1, Range<InputIt2> s2, Args... args, int64_t score_cutoff)
    {
        auto maximum = T::maximum(s1, s2, args...);
        int64_t cutoff_distance = maximum - score_cutoff;
        int64_t dist = T::_distance(s1, s2, std::forward<Args>(args)..., cutoff_distance);
        int64_t sim = maximum - dist;
        return (sim >= score_cutoff) ? sim : 0;
    }

    DistanceBase(){};
    friend T;
};

template <typename T, typename... Args>
struct SimilarityBase : public NormalizedMetricBase<T, Args...> {
    template <typename InputIt1, typename InputIt2>
    static int64_t distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, Args... args,
                            int64_t score_cutoff)
    {
        return _distance(detail::make_range(first1, last1), detail::make_range(first2, last2),
                         std::forward<Args>(args)..., score_cutoff);
    }

    template <typename Sentence1, typename Sentence2>
    static int64_t distance(const Sentence1& s1, const Sentence2& s2, Args... args, int64_t score_cutoff)
    {
        return _distance(detail::make_range(s1), detail::make_range(s2), std::forward<Args>(args)...,
                         score_cutoff);
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, Args... args,
                              int64_t score_cutoff)
    {
        return T::_similarity(detail::make_range(first1, last1), detail::make_range(first2, last2),
                              std::forward<Args>(args)..., score_cutoff);
    }

    template <typename Sentence1, typename Sentence2>
    static int64_t similarity(const Sentence1& s1, const Sentence2& s2, Args... args, int64_t score_cutoff)
    {
        return T::_similarity(detail::make_range(s1), detail::make_range(s2), std::forward<Args>(args)...,
                              score_cutoff);
    }

protected:
    template <typename InputIt1, typename InputIt2>
    static int64_t _distance(Range<InputIt1> s1, Range<InputIt2> s2, Args... args, int64_t score_cutoff)
    {
        auto maximum = T::maximum(s1, s2, args...);
        int64_t cutoff_similarity = std::max<int64_t>(0, maximum - score_cutoff);
        int64_t sim = T::_similarity(s1, s2, std::forward<Args>(args)..., cutoff_similarity);
        int64_t dist = maximum - sim;
        return (dist <= score_cutoff) ? dist : score_cutoff + 1;
    }

    SimilarityBase(){};
    friend T;
};

template <typename T>
struct CachedNormalizedMetricBase {
    template <typename InputIt2>
    double normalized_distance(InputIt2 first2, InputIt2 last2, double score_cutoff = 1.0) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._normalized_distance(detail::make_range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    double normalized_distance(const Sentence2& s2, double score_cutoff = 1.0) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._normalized_distance(detail::make_range(s2), score_cutoff);
    }

    template <typename InputIt2>
    double normalized_similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0.0) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._normalized_similarity(detail::make_range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    double normalized_similarity(const Sentence2& s2, double score_cutoff = 0.0) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._normalized_similarity(detail::make_range(s2), score_cutoff);
    }

protected:
    template <typename InputIt2>
    double _normalized_distance(Range<InputIt2> s2, double score_cutoff) const
    {
        const T& derived = static_cast<const T&>(*this);
        auto maximum = derived.maximum(s2);
        int64_t cutoff_distance =
            static_cast<int64_t>(std::ceil(static_cast<double>(maximum) * score_cutoff));
        int64_t dist = derived._distance(s2, cutoff_distance);
        double norm_dist = (maximum) ? static_cast<double>(dist) / static_cast<double>(maximum) : 0.0;
        return (norm_dist <= score_cutoff) ? norm_dist : 1.0;
    }

    template <typename InputIt2>
    double _normalized_similarity(Range<InputIt2> s2, double score_cutoff) const
    {
        double cutoff_score = NormSim_to_NormDist(score_cutoff);
        double norm_dist = _normalized_distance(s2, cutoff_score);
        double norm_sim = 1.0 - norm_dist;
        return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
    }

    CachedNormalizedMetricBase(){};
    friend T;
};

template <typename T>
struct CachedDistanceBase : public CachedNormalizedMetricBase<T> {
    template <typename InputIt2>
    int64_t distance(InputIt2 first2, InputIt2 last2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._distance(detail::make_range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    int64_t distance(const Sentence2& s2, int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._distance(detail::make_range(s2), score_cutoff);
    }

    template <typename InputIt2>
    int64_t similarity(InputIt2 first2, InputIt2 last2, int64_t score_cutoff = 0) const
    {
        return _similarity(detail::make_range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    int64_t similarity(const Sentence2& s2, int64_t score_cutoff = 0) const
    {
        return _similarity(detail::make_range(s2), score_cutoff);
    }

protected:
    template <typename InputIt2>
    int64_t _similarity(Range<InputIt2> s2, int64_t score_cutoff) const
    {
        const T& derived = static_cast<const T&>(*this);
        auto maximum = derived.maximum(s2);
        int64_t cutoff_distance = maximum - score_cutoff;
        int64_t dist = derived._distance(s2, cutoff_distance);
        int64_t sim = maximum - dist;
        return (sim >= score_cutoff) ? sim : 0;
    }

    CachedDistanceBase(){};
    friend T;
};

template <typename T>
struct CachedSimilarityBase : public CachedNormalizedMetricBase<T> {
    template <typename InputIt2>
    int64_t distance(InputIt2 first2, InputIt2 last2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const
    {
        return _distance(detail::make_range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    int64_t distance(const Sentence2& s2, int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const
    {
        return _distance(detail::make_range(s2), score_cutoff);
    }

    template <typename InputIt2>
    int64_t similarity(InputIt2 first2, InputIt2 last2, int64_t score_cutoff = 0) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._similarity(detail::make_range(first2, last2), score_cutoff);
    }

    template <typename Sentence2>
    int64_t similarity(const Sentence2& s2, int64_t score_cutoff = 0) const
    {
        const T& derived = static_cast<const T&>(*this);
        return derived._similarity(detail::make_range(s2), score_cutoff);
    }

protected:
    template <typename InputIt2>
    int64_t _distance(Range<InputIt2> s2, int64_t score_cutoff) const
    {
        const T& derived = static_cast<const T&>(*this);
        auto maximum = derived.maximum(s2);
        int64_t cutoff_similarity = std::max<int64_t>(0, maximum - score_cutoff);
        int64_t sim = derived._similarity(s2, cutoff_similarity);
        int64_t dist = maximum - sim;
        return (dist <= score_cutoff) ? dist : score_cutoff + 1;
    }

    CachedSimilarityBase(){};
    friend T;
};

} // namespace detail
} // namespace rapidfuzz

namespace rapidfuzz {
namespace detail {

template <typename IntType>
struct RowId {
    IntType val = -1;
    friend bool operator==(const RowId& lhs, const RowId& rhs)
    {
        return lhs.val == rhs.val;
    }

    friend bool operator!=(const RowId& lhs, const RowId& rhs)
    {
        return !(lhs == rhs);
    }
};

/*
 * based on the paper
 * "Linear space string correction algorithm using the Damerau-Levenshtein distance"
 * from Chunchun Zhao and Sartaj Sahni
 */
template <typename IntType, typename InputIt1, typename InputIt2>
int64_t damerau_levenshtein_distance_zhao(Range<InputIt1> s1, Range<InputIt2> s2, int64_t max)
{
    IntType len1 = static_cast<IntType>(s1.size());
    IntType len2 = static_cast<IntType>(s2.size());
    IntType maxVal = static_cast<IntType>(std::max(len1, len2) + 1);
    assert(std::numeric_limits<IntType>::max() > maxVal);

    HybridGrowingHashmap<typename Range<InputIt1>::value_type, RowId<IntType>> last_row_id;
    size_t size = static_cast<size_t>(s2.size() + 2);
    assume(size != 0);
    std::vector<IntType> FR_arr(size, maxVal);
    std::vector<IntType> R1_arr(size, maxVal);
    std::vector<IntType> R_arr(size);
    R_arr[0] = maxVal;
    std::iota(R_arr.begin() + 1, R_arr.end(), IntType(0));

    IntType* R = &R_arr[1];
    IntType* R1 = &R1_arr[1];
    IntType* FR = &FR_arr[1];

    for (IntType i = 1; i <= len1; i++) {
        std::swap(R, R1);
        IntType last_col_id = -1;
        IntType last_i2l1 = R[0];
        R[0] = i;
        IntType T = maxVal;

        for (IntType j = 1; j <= len2; j++) {
            ptrdiff_t diag = R1[j - 1] + static_cast<IntType>(s1[i - 1] != s2[j - 1]);
            ptrdiff_t left = R[j - 1] + 1;
            ptrdiff_t up = R1[j] + 1;
            ptrdiff_t temp = std::min({diag, left, up});

            if (s1[i - 1] == s2[j - 1]) {
                last_col_id = j;   // last occurence of s1_i
                FR[j] = R1[j - 2]; // save H_k-1,j-2
                T = last_i2l1;     // save H_i-2,l-1
            }
            else {
                ptrdiff_t k = last_row_id.get(static_cast<uint64_t>(s2[j - 1])).val;
                ptrdiff_t l = last_col_id;

                if ((j - l) == 1) {
                    ptrdiff_t transpose = FR[j] + (i - k);
                    temp = std::min(temp, transpose);
                }
                else if ((i - k) == 1) {
                    ptrdiff_t transpose = T + (j - l);
                    temp = std::min(temp, transpose);
                }
            }

            last_i2l1 = R[j];
            R[j] = static_cast<IntType>(temp);
        }
        last_row_id[s1[i - 1]].val = i;
    }

    int64_t dist = R[s2.size()];
    return (dist <= max) ? dist : max + 1;
}

template <typename InputIt1, typename InputIt2>
int64_t damerau_levenshtein_distance(Range<InputIt1> s1, Range<InputIt2> s2, int64_t max)
{
    int64_t min_edits = std::abs(s1.size() - s2.size());
    if (min_edits > max) return max + 1;

    /* common affix does not effect Levenshtein distance */
    remove_common_affix(s1, s2);

    ptrdiff_t maxVal = std::max(s1.size(), s2.size()) + 1;
    if (std::numeric_limits<int16_t>::max() > maxVal)
        return damerau_levenshtein_distance_zhao<int16_t>(s1, s2, max);
    else if (std::numeric_limits<int32_t>::max() > maxVal)
        return damerau_levenshtein_distance_zhao<int32_t>(s1, s2, max);
    else
        return damerau_levenshtein_distance_zhao<int64_t>(s1, s2, max);
}

class DamerauLevenshtein : public DistanceBase<DamerauLevenshtein> {
    friend DistanceBase<DamerauLevenshtein>;
    friend NormalizedMetricBase<DamerauLevenshtein>;

    template <typename InputIt1, typename InputIt2>
    static int64_t maximum(Range<InputIt1> s1, Range<InputIt2> s2)
    {
        return std::max(s1.size(), s2.size());
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t _distance(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_cutoff)
    {
        return damerau_levenshtein_distance(s1, s2, score_cutoff);
    }
};

} // namespace detail
} // namespace rapidfuzz

namespace rapidfuzz {
/* the API will require a change when adding custom weights */
namespace experimental {
/**
 * @brief Calculates the Damerau Levenshtein distance between two strings.
 *
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 * @param max
 *   Maximum Damerau Levenshtein distance between s1 and s2, that is
 *   considered as a result. If the distance is bigger than max,
 *   max + 1 is returned instead. Default is std::numeric_limits<size_t>::max(),
 *   which deactivates this behaviour.
 *
 * @return Damerau Levenshtein distance between s1 and s2
 */
template <typename InputIt1, typename InputIt2>
int64_t damerau_levenshtein_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                     int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::DamerauLevenshtein::distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t damerau_levenshtein_distance(const Sentence1& s1, const Sentence2& s2,
                                     int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::DamerauLevenshtein::distance(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
int64_t damerau_levenshtein_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                       int64_t score_cutoff = 0)
{
    return detail::DamerauLevenshtein::similarity(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t damerau_levenshtein_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff = 0)
{
    return detail::DamerauLevenshtein::similarity(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double damerau_levenshtein_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                               InputIt2 last2, double score_cutoff = 1.0)
{
    return detail::DamerauLevenshtein::normalized_distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double damerau_levenshtein_normalized_distance(const Sentence1& s1, const Sentence2& s2,
                                               double score_cutoff = 1.0)
{
    return detail::DamerauLevenshtein::normalized_distance(s1, s2, score_cutoff);
}

/**
 * @brief Calculates a normalized Damerau Levenshtein similarity
 *
 * @details
 * Both string require a similar length
 *
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 * @param score_cutoff
 *   Optional argument for a score threshold as a float between 0 and 1.0.
 *   For ratio < score_cutoff 0 is returned instead. Default is 0,
 *   which deactivates this behaviour.
 *
 * @return Normalized Damerau Levenshtein distance between s1 and s2
 *   as a float between 0 and 1.0
 */
template <typename InputIt1, typename InputIt2>
double damerau_levenshtein_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                                 InputIt2 last2, double score_cutoff = 0.0)
{
    return detail::DamerauLevenshtein::normalized_similarity(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double damerau_levenshtein_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                                 double score_cutoff = 0.0)
{
    return detail::DamerauLevenshtein::normalized_similarity(s1, s2, score_cutoff);
}

template <typename CharT1>
struct CachedDamerauLevenshtein : public detail::CachedDistanceBase<CachedDamerauLevenshtein<CharT1>> {
    template <typename Sentence1>
    CachedDamerauLevenshtein(const Sentence1& s1_)
        : CachedDamerauLevenshtein(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt1>
    CachedDamerauLevenshtein(InputIt1 first1, InputIt1 last1) : s1(first1, last1)
    {}

private:
    friend detail::CachedDistanceBase<CachedDamerauLevenshtein<CharT1>>;
    friend detail::CachedNormalizedMetricBase<CachedDamerauLevenshtein<CharT1>>;

    template <typename InputIt2>
    int64_t maximum(detail::Range<InputIt2> s2) const
    {
        return std::max(static_cast<ptrdiff_t>(s1.size()), s2.size());
    }

    template <typename InputIt2>
    int64_t _distance(detail::Range<InputIt2> s2, int64_t score_cutoff) const
    {
        return damerau_levenshtein_distance(s1, s2, score_cutoff);
    }

    std::basic_string<CharT1> s1;
};

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename Sentence1>
CachedDamerauLevenshtein(const Sentence1& s1_) -> CachedDamerauLevenshtein<char_type<Sentence1>>;

template <typename InputIt1>
CachedDamerauLevenshtein(InputIt1 first1, InputIt1 last1) -> CachedDamerauLevenshtein<iter_value_t<InputIt1>>;
#endif

} // namespace experimental
} // namespace rapidfuzz

#include <cmath>
#include <numeric>

#include <stdexcept>

namespace rapidfuzz {
namespace detail {

class Hamming : public DistanceBase<Hamming> {
    friend DistanceBase<Hamming>;
    friend NormalizedMetricBase<Hamming>;

    template <typename InputIt1, typename InputIt2>
    static int64_t maximum(Range<InputIt1> s1, Range<InputIt2>)
    {
        return s1.size();
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t _distance(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_cutoff)
    {
        if (s1.size() != s2.size()) throw std::invalid_argument("Sequences are not the same length.");

        int64_t dist = 0;
        for (ptrdiff_t i = 0; i < s1.size(); ++i)
            dist += bool(s1[i] != s2[i]);

        return (dist <= score_cutoff) ? dist : score_cutoff + 1;
    }
};
} // namespace detail
} // namespace rapidfuzz

namespace rapidfuzz {

/**
 * @brief Calculates the Hamming distance between two strings.
 *
 * @details
 * Both strings require a similar length
 *
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 * @param max
 *   Maximum Hamming distance between s1 and s2, that is
 *   considered as a result. If the distance is bigger than max,
 *   max + 1 is returned instead. Default is std::numeric_limits<size_t>::max(),
 *   which deactivates this behaviour.
 *
 * @return Hamming distance between s1 and s2
 */
template <typename InputIt1, typename InputIt2>
int64_t hamming_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::Hamming::distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t hamming_distance(const Sentence1& s1, const Sentence2& s2,
                         int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::Hamming::distance(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
int64_t hamming_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                           int64_t score_cutoff = 0)
{
    return detail::Hamming::similarity(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t hamming_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff = 0)
{
    return detail::Hamming::similarity(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double hamming_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff = 1.0)
{
    return detail::Hamming::normalized_distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double hamming_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    return detail::Hamming::normalized_distance(s1, s2, score_cutoff);
}

/**
 * @brief Calculates a normalized hamming similarity
 *
 * @details
 * Both string require a similar length
 *
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 * @param score_cutoff
 *   Optional argument for a score threshold as a float between 0 and 1.0.
 *   For ratio < score_cutoff 0 is returned instead. Default is 0,
 *   which deactivates this behaviour.
 *
 * @return Normalized hamming distance between s1 and s2
 *   as a float between 0 and 1.0
 */
template <typename InputIt1, typename InputIt2>
double hamming_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                     double score_cutoff = 0.0)
{
    return detail::Hamming::normalized_similarity(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double hamming_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    return detail::Hamming::normalized_similarity(s1, s2, score_cutoff);
}

template <typename CharT1>
struct CachedHamming : public detail::CachedDistanceBase<CachedHamming<CharT1>> {
    template <typename Sentence1>
    CachedHamming(const Sentence1& s1_) : CachedHamming(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt1>
    CachedHamming(InputIt1 first1, InputIt1 last1) : s1(first1, last1)
    {}

private:
    friend detail::CachedDistanceBase<CachedHamming<CharT1>>;
    friend detail::CachedNormalizedMetricBase<CachedHamming<CharT1>>;

    template <typename InputIt2>
    int64_t maximum(detail::Range<InputIt2> s2) const
    {
        return s2.size();
    }

    template <typename InputIt2>
    int64_t _distance(detail::Range<InputIt2> s2, int64_t score_cutoff) const
    {
        return detail::Hamming::distance(s1, s2, score_cutoff);
    }

    std::basic_string<CharT1> s1;
};

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename Sentence1>
CachedHamming(const Sentence1& s1_) -> CachedHamming<char_type<Sentence1>>;

template <typename InputIt1>
CachedHamming(InputIt1 first1, InputIt1 last1) -> CachedHamming<iter_value_t<InputIt1>>;
#endif

/**@}*/

} // namespace rapidfuzz

#include <limits>

#include <array>
#include <limits>
#include <stdint.h>
#include <stdio.h>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace rapidfuzz {
namespace detail {

struct BitvectorHashmap {
    BitvectorHashmap() : m_map()
    {}

    template <typename CharT>
    uint64_t get(CharT key) const noexcept
    {
        return m_map[lookup(static_cast<uint64_t>(key))].value;
    }

    template <typename CharT>
    uint64_t& operator[](CharT key) noexcept
    {
        uint32_t i = lookup(static_cast<uint64_t>(key));
        m_map[i].key = static_cast<uint64_t>(key);
        return m_map[i].value;
    }

private:
    /**
     * lookup key inside the hashmap using a similar collision resolution
     * strategy to CPython and Ruby
     */
    uint32_t lookup(uint64_t key) const noexcept
    {
        uint32_t i = key % 128;

        if (!m_map[i].value || m_map[i].key == key) return i;

        uint64_t perturb = key;
        while (true) {
            i = (static_cast<uint64_t>(i) * 5 + perturb + 1) % 128;
            if (!m_map[i].value || m_map[i].key == key) return i;

            perturb >>= 5;
        }
    }

    struct MapElem {
        uint64_t key = 0;
        uint64_t value = 0;
    };
    std::array<MapElem, 128> m_map;
};

struct PatternMatchVector {
    PatternMatchVector() : m_extendedAscii()
    {}

    template <typename InputIt>
    PatternMatchVector(Range<InputIt> s) : m_extendedAscii()
    {
        insert(s);
    }

    size_t size() const noexcept
    {
        return 1;
    }

    template <typename InputIt>
    void insert(Range<InputIt> s) noexcept
    {
        uint64_t mask = 1;
        for (const auto& ch : s) {
            insert_mask(ch, mask);
            mask <<= 1;
        }
    }

    template <typename CharT>
    void insert(CharT key, int64_t pos) noexcept
    {
        insert_mask(key, UINT64_C(1) << pos);
    }

    uint64_t get(char key) const noexcept
    {
        /** treat char as value between 0 and 127 for performance reasons */
        return m_extendedAscii[static_cast<uint8_t>(key)];
    }

    template <typename CharT>
    uint64_t get(CharT key) const noexcept
    {
        if (key >= 0 && key <= 255)
            return m_extendedAscii[static_cast<uint8_t>(key)];
        else
            return m_map.get(key);
    }

    template <typename CharT>
    uint64_t get(size_t block, CharT key) const noexcept
    {
        assert(block == 0);
        (void)block;
        return get(key);
    }

    void insert_mask(char key, uint64_t mask) noexcept
    {
        /** treat char as value between 0 and 127 for performance reasons */
        m_extendedAscii[static_cast<uint8_t>(key)] |= mask;
    }

    template <typename CharT>
    void insert_mask(CharT key, uint64_t mask) noexcept
    {
        if (key >= 0 && key <= 255)
            m_extendedAscii[static_cast<uint8_t>(key)] |= mask;
        else
            m_map[key] |= mask;
    }

private:
    BitvectorHashmap m_map;
    std::array<uint64_t, 256> m_extendedAscii;
};

struct BlockPatternMatchVector {
    BlockPatternMatchVector() = delete;

    BlockPatternMatchVector(size_t str_len)
        : m_block_count(ceil_div(str_len, 64)), m_map(nullptr), m_extendedAscii(256, m_block_count, 0)
    {}

    template <typename InputIt>
    BlockPatternMatchVector(Range<InputIt> s) : BlockPatternMatchVector(static_cast<size_t>(s.size()))
    {
        insert(s);
    }

    ~BlockPatternMatchVector()
    {
        delete[] m_map;
    }

    size_t size() const noexcept
    {
        return m_block_count;
    }

    template <typename CharT>
    void insert(size_t block, CharT ch, int pos) noexcept
    {
        uint64_t mask = UINT64_C(1) << pos;
        insert_mask(block, ch, mask);
    }

    /**
     * @warning undefined behavior if iterator \p first is greater than \p last
     * @tparam InputIt
     * @param first
     * @param last
     */
    template <typename InputIt>
    void insert(Range<InputIt> s) noexcept
    {
        auto len = s.size();
        uint64_t mask = 1;
        for (ptrdiff_t i = 0; i < len; ++i) {
            size_t block = static_cast<size_t>(i) / 64;
            insert_mask(block, s[i], mask);
            mask = rotl(mask, 1);
        }
    }

    template <typename CharT>
    void insert_mask(size_t block, CharT key, uint64_t mask) noexcept
    {
        assert(block < size());
        if (key >= 0 && key <= 255)
            m_extendedAscii[static_cast<uint8_t>(key)][block] |= mask;
        else {
            if (!m_map) m_map = new BitvectorHashmap[m_block_count];
            m_map[block][key] |= mask;
        }
    }

    void insert_mask(size_t block, char key, uint64_t mask) noexcept
    {
        insert_mask(block, static_cast<uint8_t>(key), mask);
    }

    template <typename CharT>
    uint64_t get(size_t block, CharT key) const noexcept
    {
        if (key >= 0 && key <= 255)
            return m_extendedAscii[static_cast<uint8_t>(key)][block];
        else if (m_map)
            return m_map[block].get(key);
        else
            return 0;
    }

    uint64_t get(size_t block, char ch) const noexcept
    {
        return get(block, static_cast<uint8_t>(ch));
    }

private:
    size_t m_block_count;
    BitvectorHashmap* m_map;
    BitMatrix<uint64_t> m_extendedAscii;
};

} // namespace detail
} // namespace rapidfuzz

#include <limits>

#include <algorithm>
#include <array>

namespace rapidfuzz {
namespace detail {

template <bool RecordMatrix>
struct LCSseqResult;

template <>
struct LCSseqResult<true> {
    ShiftedBitMatrix<uint64_t> S;

    int64_t sim;
};

template <>
struct LCSseqResult<false> {
    int64_t sim;
};

/*
 * An encoded mbleven model table.
 *
 * Each 8-bit integer represents an edit sequence, with using two
 * bits for a single operation.
 *
 * Each Row of 8 integers represent all possible combinations
 * of edit sequences for a gived maximum edit distance and length
 * difference between the two strings, that is below the maximum
 * edit distance
 *
 *   0x1 = 01 = DELETE,
 *   0x2 = 10 = INSERT
 *
 * 0x5 -> DEL + DEL
 * 0x6 -> DEL + INS
 * 0x9 -> INS + DEL
 * 0xA -> INS + INS
 */
static constexpr std::array<std::array<uint8_t, 7>, 14> lcs_seq_mbleven2018_matrix = {{
    /* max edit distance 1 */
    {0},
    /* case does not occur */ /* len_diff 0 */
    {0x01},                   /* len_diff 1 */
    /* max edit distance 2 */
    {0x09, 0x06}, /* len_diff 0 */
    {0x01},       /* len_diff 1 */
    {0x05},       /* len_diff 2 */
    /* max edit distance 3 */
    {0x09, 0x06},       /* len_diff 0 */
    {0x25, 0x19, 0x16}, /* len_diff 1 */
    {0x05},             /* len_diff 2 */
    {0x15},             /* len_diff 3 */
    /* max edit distance 4 */
    {0x96, 0x66, 0x5A, 0x99, 0x69, 0xA5}, /* len_diff 0 */
    {0x25, 0x19, 0x16},                   /* len_diff 1 */
    {0x65, 0x56, 0x95, 0x59},             /* len_diff 2 */
    {0x15},                               /* len_diff 3 */
    {0x55},                               /* len_diff 4 */
}};

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_mbleven2018(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_cutoff)
{
    auto len1 = s1.size();
    auto len2 = s2.size();

    if (len1 < len2) return lcs_seq_mbleven2018(s2, s1, score_cutoff);

    auto len_diff = len1 - len2;
    int64_t max_misses = static_cast<ptrdiff_t>(len1) - score_cutoff;
    auto ops_index = (max_misses + max_misses * max_misses) / 2 + len_diff - 1;
    auto& possible_ops = lcs_seq_mbleven2018_matrix[static_cast<size_t>(ops_index)];
    int64_t max_len = 0;

    for (uint8_t ops : possible_ops) {
        ptrdiff_t s1_pos = 0;
        ptrdiff_t s2_pos = 0;
        int64_t cur_len = 0;

        while (s1_pos < len1 && s2_pos < len2) {
            if (s1[s1_pos] != s2[s2_pos]) {
                if (!ops) break;
                if (ops & 1)
                    s1_pos++;
                else if (ops & 2)
                    s2_pos++;
#if defined(__GNUC__) && !defined(__clang__) && !defined(__ICC) && __GNUC__ < 10
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wconversion"
#endif
                ops >>= 2;
#if defined(__GNUC__) && !defined(__clang__) && !defined(__ICC) && __GNUC__ < 10
#    pragma GCC diagnostic pop
#endif
            }
            else {
                cur_len++;
                s1_pos++;
                s2_pos++;
            }
        }

        max_len = std::max(max_len, cur_len);
    }

    return (max_len >= score_cutoff) ? max_len : 0;
}

template <bool RecordMatrix>
struct LCSseqResult;

template <size_t N, bool RecordMatrix, typename PMV, typename InputIt1, typename InputIt2>
auto lcs_unroll(const PMV& block, Range<InputIt1>, Range<InputIt2> s2, int64_t score_cutoff = 0)
    -> LCSseqResult<RecordMatrix>
{
    uint64_t S[N];
    unroll<size_t, N>([&](size_t i) { S[i] = ~UINT64_C(0); });

    LCSseqResult<RecordMatrix> res;
    static_if<RecordMatrix>(
        [&](auto f) { f(res).S = ShiftedBitMatrix<uint64_t>(s2.size(), N, ~UINT64_C(0)); });

    for (ptrdiff_t i = 0; i < s2.size(); ++i) {
        uint64_t carry = 0;
        unroll<size_t, N>([&](size_t word) {
            uint64_t Matches = block.get(word, s2[i]);
            uint64_t u = S[word] & Matches;
            uint64_t x = addc64(S[word], u, carry, &carry);
            S[word] = x | (S[word] - u);

            static_if<RecordMatrix>([&](auto f) { f(res).S[i][word] = S[word]; });
        });
    }

    res.sim = 0;
    unroll<size_t, N>([&](size_t i) { res.sim += popcount(~S[i]); });

    if (res.sim < score_cutoff) res.sim = 0;

    return res;
}

template <bool RecordMatrix, typename PMV, typename InputIt1, typename InputIt2>
auto lcs_blockwise(const PMV& block, Range<InputIt1>, Range<InputIt2> s2, int64_t score_cutoff = 0)
    -> LCSseqResult<RecordMatrix>
{
    auto words = block.size();
    std::vector<uint64_t> S(words, ~UINT64_C(0));

    LCSseqResult<RecordMatrix> res;
    static_if<RecordMatrix>(
        [&](auto f) { f(res).S = ShiftedBitMatrix<uint64_t>(s2.size(), words, ~UINT64_C(0)); });

    for (ptrdiff_t i = 0; i < s2.size(); ++i) {
        uint64_t carry = 0;
        for (size_t word = 0; word < words; ++word) {
            const uint64_t Matches = block.get(word, s2[i]);
            uint64_t Stemp = S[word];

            uint64_t u = Stemp & Matches;

            uint64_t x = addc64(Stemp, u, carry, &carry);
            S[word] = x | (Stemp - u);

            static_if<RecordMatrix>([&](auto f) { f(res).S[i][word] = S[word]; });
        }
    }

    res.sim = 0;
    for (uint64_t Stemp : S)
        res.sim += popcount(~Stemp);

    if (res.sim < score_cutoff) res.sim = 0;

    return res;
}

template <typename PMV, typename InputIt1, typename InputIt2>
int64_t longest_common_subsequence(const PMV& block, Range<InputIt1> s1, Range<InputIt2> s2,
                                   int64_t score_cutoff)
{
    auto nr = ceil_div(s1.size(), 64);
    switch (nr) {
    case 0: return 0;
    case 1: return lcs_unroll<1, false>(block, s1, s2, score_cutoff).sim;
    case 2: return lcs_unroll<2, false>(block, s1, s2, score_cutoff).sim;
    case 3: return lcs_unroll<3, false>(block, s1, s2, score_cutoff).sim;
    case 4: return lcs_unroll<4, false>(block, s1, s2, score_cutoff).sim;
    case 5: return lcs_unroll<5, false>(block, s1, s2, score_cutoff).sim;
    case 6: return lcs_unroll<6, false>(block, s1, s2, score_cutoff).sim;
    case 7: return lcs_unroll<7, false>(block, s1, s2, score_cutoff).sim;
    case 8: return lcs_unroll<8, false>(block, s1, s2, score_cutoff).sim;
    default: return lcs_blockwise<false>(block, s1, s2, score_cutoff).sim;
    }
}

template <typename InputIt1, typename InputIt2>
int64_t longest_common_subsequence(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_cutoff)
{
    if (s1.empty()) return 0;
    if (s1.size() <= 64) return longest_common_subsequence(PatternMatchVector(s1), s1, s2, score_cutoff);

    return longest_common_subsequence(BlockPatternMatchVector(s1), s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_similarity(const BlockPatternMatchVector& block, Range<InputIt1> s1, Range<InputIt2> s2,
                           int64_t score_cutoff)
{
    auto len1 = s1.size();
    auto len2 = s2.size();
    int64_t max_misses = static_cast<int64_t>(len1) + len2 - 2 * score_cutoff;

    /* no edits are allowed */
    if (max_misses == 0 || (max_misses == 1 && len1 == len2))
        return std::equal(s1.begin(), s1.end(), s2.begin(), s2.end()) ? len1 : 0;

    if (max_misses < std::abs(len1 - len2)) return 0;

    // do this first, since we can not remove any affix in encoded form
    if (max_misses >= 5) return longest_common_subsequence(block, s1, s2, score_cutoff);

    /* common affix does not effect Levenshtein distance */
    auto affix = remove_common_affix(s1, s2);
    int64_t lcs_sim = static_cast<int64_t>(affix.prefix_len + affix.suffix_len);
    if (!s1.empty() && !s2.empty()) lcs_sim += lcs_seq_mbleven2018(s1, s2, score_cutoff - lcs_sim);

    return lcs_sim;
}

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_similarity(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_cutoff)
{
    auto len1 = s1.size();
    auto len2 = s2.size();

    // Swapping the strings so the second string is shorter
    if (len1 < len2) return lcs_seq_similarity(s2, s1, score_cutoff);

    int64_t max_misses = static_cast<int64_t>(len1) + len2 - 2 * score_cutoff;

    /* no edits are allowed */
    if (max_misses == 0 || (max_misses == 1 && len1 == len2))
        return std::equal(s1.begin(), s1.end(), s2.begin(), s2.end()) ? len1 : 0;

    if (max_misses < std::abs(len1 - len2)) return 0;

    /* common affix does not effect Levenshtein distance */
    auto affix = remove_common_affix(s1, s2);
    int64_t lcs_sim = static_cast<int64_t>(affix.prefix_len + affix.suffix_len);
    if (s1.size() && s2.size()) {
        if (max_misses < 5)
            lcs_sim += lcs_seq_mbleven2018(s1, s2, score_cutoff - lcs_sim);
        else
            lcs_sim += longest_common_subsequence(s1, s2, score_cutoff - lcs_sim);
    }

    return lcs_sim;
}

/**
 * @brief recover alignment from bitparallel Levenshtein matrix
 */
template <typename InputIt1, typename InputIt2>
Editops recover_alignment(Range<InputIt1> s1, Range<InputIt2> s2, const LCSseqResult<true>& matrix,
                          StringAffix affix)
{
    auto len1 = s1.size();
    auto len2 = s2.size();
    size_t dist = static_cast<size_t>(static_cast<int64_t>(len1) + len2 - 2 * matrix.sim);
    Editops editops(dist);
    editops.set_src_len(len1 + affix.prefix_len + affix.suffix_len);
    editops.set_dest_len(len2 + affix.prefix_len + affix.suffix_len);

    if (dist == 0) return editops;

    auto col = len1;
    auto row = len2;

    while (row && col) {
        /* Deletion */
        if (matrix.S.test_bit(row - 1, col - 1)) {
            assert(dist > 0);
            dist--;
            col--;
            editops[dist].type = EditType::Delete;
            editops[dist].src_pos = col + affix.prefix_len;
            editops[dist].dest_pos = row + affix.prefix_len;
        }
        else {
            row--;

            /* Insertion */
            if (row && !(matrix.S.test_bit(row - 1, col - 1))) {
                assert(dist > 0);
                dist--;
                editops[dist].type = EditType::Insert;
                editops[dist].src_pos = col + affix.prefix_len;
                editops[dist].dest_pos = row + affix.prefix_len;
            }
            /* Match */
            else {
                col--;
                assert(s1[col] == s2[row]);
            }
        }
    }

    while (col) {
        dist--;
        col--;
        editops[dist].type = EditType::Delete;
        editops[dist].src_pos = col + affix.prefix_len;
        editops[dist].dest_pos = row + affix.prefix_len;
    }

    while (row) {
        dist--;
        row--;
        editops[dist].type = EditType::Insert;
        editops[dist].src_pos = col + affix.prefix_len;
        editops[dist].dest_pos = row + affix.prefix_len;
    }

    return editops;
}

template <typename InputIt1, typename InputIt2>
LCSseqResult<true> lcs_matrix(Range<InputIt1> s1, Range<InputIt2> s2)
{
    auto nr = ceil_div(s1.size(), 64);
    switch (nr) {
    case 0:
    {
        LCSseqResult<true> res;
        res.sim = 0;
        return res;
    }
    case 1: return lcs_unroll<1, true>(PatternMatchVector(s1), s1, s2);
    case 2: return lcs_unroll<2, true>(BlockPatternMatchVector(s1), s1, s2);
    case 3: return lcs_unroll<3, true>(BlockPatternMatchVector(s1), s1, s2);
    case 4: return lcs_unroll<4, true>(BlockPatternMatchVector(s1), s1, s2);
    case 5: return lcs_unroll<5, true>(BlockPatternMatchVector(s1), s1, s2);
    case 6: return lcs_unroll<6, true>(BlockPatternMatchVector(s1), s1, s2);
    case 7: return lcs_unroll<7, true>(BlockPatternMatchVector(s1), s1, s2);
    case 8: return lcs_unroll<8, true>(BlockPatternMatchVector(s1), s1, s2);
    default: return lcs_blockwise<true>(BlockPatternMatchVector(s1), s1, s2);
    }
}

template <typename InputIt1, typename InputIt2>
Editops lcs_seq_editops(Range<InputIt1> s1, Range<InputIt2> s2)
{
    /* prefix and suffix are no-ops, which do not need to be added to the editops */
    StringAffix affix = remove_common_affix(s1, s2);

    return recover_alignment(s1, s2, lcs_matrix(s1, s2), affix);
}

class LCSseq : public SimilarityBase<LCSseq> {
    friend SimilarityBase<LCSseq>;
    friend NormalizedMetricBase<LCSseq>;

    template <typename InputIt1, typename InputIt2>
    static int64_t maximum(Range<InputIt1> s1, Range<InputIt2> s2)
    {
        return std::max(s1.size(), s2.size());
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t _similarity(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_cutoff)
    {
        return lcs_seq_similarity(s1, s2, score_cutoff);
    }
};

} // namespace detail
} // namespace rapidfuzz

#include <algorithm>
#include <cmath>
#include <limits>

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::LCSseq::distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t lcs_seq_distance(const Sentence1& s1, const Sentence2& s2,
                         int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::LCSseq::distance(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
int64_t lcs_seq_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                           int64_t score_cutoff = 0)
{
    return detail::LCSseq::similarity(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t lcs_seq_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff = 0)
{
    return detail::LCSseq::similarity(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double lcs_seq_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff = 1.0)
{
    return detail::LCSseq::normalized_distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double lcs_seq_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    return detail::LCSseq::normalized_distance(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double lcs_seq_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                     double score_cutoff = 0.0)
{
    return detail::LCSseq::normalized_similarity(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double lcs_seq_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    return detail::LCSseq::normalized_similarity(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
Editops lcs_seq_editops(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
    return detail::lcs_seq_editops(detail::make_range(first1, last1), detail::make_range(first2, last2));
}

template <typename Sentence1, typename Sentence2>
Editops lcs_seq_editops(const Sentence1& s1, const Sentence2& s2)
{
    return detail::lcs_seq_editops(detail::make_range(s1), detail::make_range(s2));
}

template <typename CharT1>
struct CachedLCSseq : detail::CachedSimilarityBase<CachedLCSseq<CharT1>> {
    template <typename Sentence1>
    CachedLCSseq(const Sentence1& s1_) : CachedLCSseq(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt1>
    CachedLCSseq(InputIt1 first1, InputIt1 last1) : s1(first1, last1), PM(detail::make_range(first1, last1))
    {}

private:
    friend detail::CachedSimilarityBase<CachedLCSseq<CharT1>>;
    friend detail::CachedNormalizedMetricBase<CachedLCSseq<CharT1>>;

    template <typename InputIt2>
    int64_t maximum(detail::Range<InputIt2> s2) const
    {
        return std::max(static_cast<ptrdiff_t>(s1.size()), s2.size());
    }

    template <typename InputIt2>
    int64_t _similarity(detail::Range<InputIt2> s2, int64_t score_cutoff) const
    {
        return detail::lcs_seq_similarity(PM, detail::make_range(s1), s2, score_cutoff);
    }

    std::basic_string<CharT1> s1;
    detail::BlockPatternMatchVector PM;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedLCSseq(const Sentence1& s1_) -> CachedLCSseq<char_type<Sentence1>>;

template <typename InputIt1>
CachedLCSseq(InputIt1 first1, InputIt1 last1) -> CachedLCSseq<iter_value_t<InputIt1>>;
#endif

} // namespace rapidfuzz

namespace rapidfuzz {
namespace detail {

template <typename InputIt1, typename InputIt2>
int64_t indel_distance(const BlockPatternMatchVector& block, Range<InputIt1> s1, Range<InputIt2> s2,
                       int64_t score_cutoff)
{
    int64_t maximum = s1.size() + s2.size();
    int64_t lcs_cutoff = std::max<int64_t>(0, maximum / 2 - score_cutoff);
    int64_t lcs_sim = lcs_seq_similarity(block, s1, s2, lcs_cutoff);
    int64_t dist = maximum - 2 * lcs_sim;
    return (dist <= score_cutoff) ? dist : score_cutoff + 1;
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_distance(const BlockPatternMatchVector& block, Range<InputIt1> s1, Range<InputIt2> s2,
                                 double score_cutoff)
{
    int64_t maximum = s1.size() + s2.size();
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(static_cast<double>(maximum) * score_cutoff));
    int64_t dist = indel_distance(block, s1, s2, cutoff_distance);
    double norm_dist = (maximum) ? static_cast<double>(dist) / static_cast<double>(maximum) : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : 1.0;
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_similarity(const BlockPatternMatchVector& block, Range<InputIt1> s1,
                                   Range<InputIt2> s2, double score_cutoff)
{
    double cutoff_score = NormSim_to_NormDist(score_cutoff);
    double norm_dist = indel_normalized_distance(block, s1, s2, cutoff_score);
    double norm_sim = 1.0 - norm_dist;
    return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
}

class Indel : public DistanceBase<Indel> {
    friend DistanceBase<Indel>;
    friend NormalizedMetricBase<Indel>;

    template <typename InputIt1, typename InputIt2>
    static int64_t maximum(Range<InputIt1> s1, Range<InputIt2> s2)
    {
        return s1.size() + s2.size();
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t _distance(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_cutoff)
    {
        int64_t maximum = Indel::maximum(s1, s2);
        int64_t lcs_cutoff = std::max<int64_t>(0, maximum / 2 - score_cutoff);
        int64_t lcs_sim = LCSseq::similarity(s1, s2, lcs_cutoff);
        int64_t dist = maximum - 2 * lcs_sim;
        return (dist <= score_cutoff) ? dist : score_cutoff + 1;
    }
};

} // namespace detail
} // namespace rapidfuzz

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2>
int64_t indel_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                       int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::Indel::distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t indel_distance(const Sentence1& s1, const Sentence2& s2,
                       int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::Indel::distance(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
int64_t indel_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         int64_t score_cutoff = 0.0)
{
    return detail::Indel::similarity(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t indel_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff = 0.0)
{
    return detail::Indel::similarity(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                 double score_cutoff = 1.0)
{
    return detail::Indel::normalized_distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double indel_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 1.0)
{
    return detail::Indel::normalized_distance(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff = 0.0)
{
    return detail::Indel::normalized_similarity(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double indel_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0.0)
{
    return detail::Indel::normalized_similarity(s1, s2, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
Editops indel_editops(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
    return lcs_seq_editops(first1, last1, first2, last2);
}

template <typename Sentence1, typename Sentence2>
Editops indel_editops(const Sentence1& s1, const Sentence2& s2)
{
    return lcs_seq_editops(s1, s2);
}

template <typename CharT1>
struct CachedIndel : public detail::CachedDistanceBase<CachedIndel<CharT1>> {
    template <typename Sentence1>
    CachedIndel(const Sentence1& s1_) : CachedIndel(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt1>
    CachedIndel(InputIt1 first1, InputIt1 last1) : s1(first1, last1), PM(detail::make_range(first1, last1))
    {}

private:
    friend detail::CachedDistanceBase<CachedIndel<CharT1>>;
    friend detail::CachedNormalizedMetricBase<CachedIndel<CharT1>>;

    template <typename InputIt2>
    int64_t maximum(detail::Range<InputIt2> s2) const
    {
        return static_cast<int64_t>(s1.size()) + s2.size();
    }

    template <typename InputIt2>
    int64_t _distance(detail::Range<InputIt2> s2, int64_t score_cutoff) const
    {
        return detail::indel_distance(PM, detail::make_range(s1), s2, score_cutoff);
    }

    std::basic_string<CharT1> s1;
    detail::BlockPatternMatchVector PM;
};

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename Sentence1>
CachedIndel(const Sentence1& s1_) -> CachedIndel<char_type<Sentence1>>;

template <typename InputIt1>
CachedIndel(InputIt1 first1, InputIt1 last1) -> CachedIndel<iter_value_t<InputIt1>>;
#endif

} // namespace rapidfuzz

#include "rapidfuzz/details/Range.hpp"
#include <limits>

#include "rapidfuzz/details/type_traits.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace rapidfuzz {
namespace detail {

struct LevenshteinRow {
    uint64_t VP;
    uint64_t VN;

    LevenshteinRow() : VP(~UINT64_C(0)), VN(0)
    {}

    LevenshteinRow(uint64_t VP_, uint64_t VN_) : VP(VP_), VN(VN_)
    {}
};

template <bool RecordMatrix, bool RecordBitRow>
struct LevenshteinResult;

template <>
struct LevenshteinResult<true, false> {
    ShiftedBitMatrix<uint64_t> VP;
    ShiftedBitMatrix<uint64_t> VN;

    int64_t dist;
};

template <>
struct LevenshteinResult<false, true> {
    size_t first_block;
    size_t last_block;
    int64_t prev_score;
    std::vector<LevenshteinRow> vecs;

    int64_t dist;
};

template <>
struct LevenshteinResult<false, false> {
    int64_t dist;
};

template <typename InputIt1, typename InputIt2>
int64_t generalized_levenshtein_wagner_fischer(Range<InputIt1> s1, Range<InputIt2> s2,
                                               LevenshteinWeightTable weights, int64_t max)
{
    size_t cache_size = static_cast<size_t>(s1.size()) + 1;
    std::vector<int64_t> cache(cache_size);
    assume(cache_size != 0);

    cache[0] = 0;
    for (size_t i = 1; i < cache_size; ++i)
        cache[i] = cache[i - 1] + weights.delete_cost;

    for (const auto& ch2 : s2) {
        auto cache_iter = cache.begin();
        int64_t temp = *cache_iter;
        *cache_iter += weights.insert_cost;

        for (const auto& ch1 : s1) {
            if (ch1 != ch2)
                temp = std::min({*cache_iter + weights.delete_cost, *(cache_iter + 1) + weights.insert_cost,
                                 temp + weights.replace_cost});
            ++cache_iter;
            std::swap(*cache_iter, temp);
        }
    }

    int64_t dist = cache.back();
    return (dist <= max) ? dist : max + 1;
}

/**
 * @brief calculates the maximum possible Levenshtein distance based on
 * string lengths and weights
 */
template <typename InputIt1, typename InputIt2>
int64_t levenshtein_maximum(Range<InputIt1> s1, Range<InputIt2> s2, LevenshteinWeightTable weights)
{
    auto len1 = s1.size();
    auto len2 = s2.size();

    int64_t max_dist = len1 * weights.delete_cost + len2 * weights.insert_cost;

    if (len1 >= len2)
        max_dist = std::min(max_dist, len2 * weights.replace_cost + (len1 - len2) * weights.delete_cost);
    else
        max_dist = std::min(max_dist, len1 * weights.replace_cost + (len2 - len1) * weights.insert_cost);

    return max_dist;
}

/**
 * @brief calculates the minimal possible Levenshtein distance based on
 * string lengths and weights
 */
template <typename InputIt1, typename InputIt2>
int64_t levenshtein_min_distance(Range<InputIt1> s1, Range<InputIt2> s2, LevenshteinWeightTable weights)
{
    return std::max((s1.size() - s2.size()) * weights.delete_cost,
                    (s2.size() - s1.size()) * weights.insert_cost);
}

template <typename InputIt1, typename InputIt2>
int64_t generalized_levenshtein_distance(Range<InputIt1> s1, Range<InputIt2> s2,
                                         LevenshteinWeightTable weights, int64_t max)
{
    int64_t min_edits = levenshtein_min_distance(s1, s2, weights);
    if (min_edits > max) return max + 1;

    /* common affix does not effect Levenshtein distance */
    remove_common_affix(s1, s2);

    return generalized_levenshtein_wagner_fischer(s1, s2, weights, max);
}

/*
 * An encoded mbleven model table.
 *
 * Each 8-bit integer represents an edit sequence, with using two
 * bits for a single operation.
 *
 * Each Row of 8 integers represent all possible combinations
 * of edit sequences for a gived maximum edit distance and length
 * difference between the two strings, that is below the maximum
 * edit distance
 *
 *   01 = DELETE, 10 = INSERT, 11 = SUBSTITUTE
 *
 * For example, 3F -> 0b111111 means three substitutions
 */
static constexpr std::array<std::array<uint8_t, 8>, 9> levenshtein_mbleven2018_matrix = {{
    /* max edit distance 1 */
    {0x03}, /* len_diff 0 */
    {0x01}, /* len_diff 1 */
    /* max edit distance 2 */
    {0x0F, 0x09, 0x06}, /* len_diff 0 */
    {0x0D, 0x07},       /* len_diff 1 */
    {0x05},             /* len_diff 2 */
    /* max edit distance 3 */
    {0x3F, 0x27, 0x2D, 0x39, 0x36, 0x1E, 0x1B}, /* len_diff 0 */
    {0x3D, 0x37, 0x1F, 0x25, 0x19, 0x16},       /* len_diff 1 */
    {0x35, 0x1D, 0x17},                         /* len_diff 2 */
    {0x15},                                     /* len_diff 3 */
}};

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_mbleven2018(Range<InputIt1> s1, Range<InputIt2> s2, int64_t max)
{
    auto len1 = s1.size();
    auto len2 = s2.size();
    assert(len1 > 0);
    assert(len2 > 0);
    assert(*s1.begin() != *s2.begin());
    assert(*(s1.end() - 1) != *(s2.end() - 1));

    if (len1 < len2) return levenshtein_mbleven2018(s2, s1, max);

    auto len_diff = len1 - len2;

    if (max == 1) return max + static_cast<int64_t>(len_diff == 1 || len1 != 1);

    auto ops_index = (max + max * max) / 2 + len_diff - 1;
    auto& possible_ops = levenshtein_mbleven2018_matrix[static_cast<size_t>(ops_index)];
    int64_t dist = max + 1;

    for (uint8_t ops : possible_ops) {
        ptrdiff_t s1_pos = 0;
        ptrdiff_t s2_pos = 0;
        int64_t cur_dist = 0;
        while (s1_pos < len1 && s2_pos < len2) {
            if (s1[s1_pos] != s2[s2_pos]) {
                cur_dist++;
                if (!ops) break;
                if (ops & 1) s1_pos++;
                if (ops & 2) s2_pos++;
#if defined(__GNUC__) && !defined(__clang__) && !defined(__ICC) && __GNUC__ < 10
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wconversion"
#endif
                ops >>= 2;
#if defined(__GNUC__) && !defined(__clang__) && !defined(__ICC) && __GNUC__ < 10
#    pragma GCC diagnostic pop
#endif
            }
            else {
                s1_pos++;
                s2_pos++;
            }
        }
        cur_dist += (len1 - s1_pos) + (len2 - s2_pos);
        dist = std::min(dist, cur_dist);
    }

    return (dist <= max) ? dist : max + 1;
}

/**
 * @brief Bitparallel implementation of the Levenshtein distance.
 *
 * This implementation requires the first string to have a length <= 64.
 * The algorithm used is described @cite hyrro_2002 and has a time complexity
 * of O(N). Comments and variable names in the implementation follow the
 * paper. This implementation is used internally when the strings are short enough
 *
 * @tparam CharT1 This is the char type of the first sentence
 * @tparam CharT2 This is the char type of the second sentence
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 *
 * @return returns the levenshtein distance between s1 and s2
 */
template <bool RecordMatrix, bool RecordBitRow, typename PM_Vec, typename InputIt1, typename InputIt2>
auto levenshtein_hyrroe2003(const PM_Vec& PM, Range<InputIt1> s1, Range<InputIt2> s2,
                            int64_t max = std::numeric_limits<int64_t>::max())
    -> LevenshteinResult<RecordMatrix, RecordBitRow>
{
    /* VP is set to 1^m. Shifting by bitwidth would be undefined behavior */
    uint64_t VP = ~UINT64_C(0);
    uint64_t VN = 0;

    LevenshteinResult<RecordMatrix, RecordBitRow> res;
    res.dist = s1.size();
    static_if<RecordMatrix>([&](auto f) {
        f(res).VP = ShiftedBitMatrix<uint64_t>(static_cast<size_t>(s2.size()), 1, ~UINT64_C(0));
        f(res).VN = ShiftedBitMatrix<uint64_t>(static_cast<size_t>(s2.size()), 1, 0);
    });

    /* mask used when computing D[m,j] in the paper 10^(m-1) */
    uint64_t mask = UINT64_C(1) << (s1.size() - 1);

    /* Searching */
    for (ptrdiff_t i = 0; i < s2.size(); ++i) {
        /* Step 1: Computing D0 */
        uint64_t PM_j = PM.get(0, s2[i]);
        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        res.dist += bool(HP & mask);
        res.dist -= bool(HN & mask);

        /* Step 4: Computing Vp and VN */
        HP = (HP << 1) | 1;
        HN = (HN << 1);

        VP = HN | ~(D0 | HP);
        VN = HP & D0;

        static_if<RecordMatrix>([&](auto f) {
            f(res).VP[static_cast<size_t>(i)][0] = VP;
            f(res).VN[static_cast<size_t>(i)][0] = VN;
        });
    }

    if (res.dist > max) res.dist = max + 1;

    static_if<RecordBitRow>([&](auto f) {
        f(res).first_block = 0;
        f(res).last_block = 0;
        f(res).prev_score = s2.size();
        f(res).vecs.emplace_back(VP, VN);
    });

    return res;
}

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_hyrroe2003_small_band(const BlockPatternMatchVector& PM, Range<InputIt1> s1,
                                          Range<InputIt2> s2, int64_t max)
{
    /* VP is set to 1^m. Shifting by bitwidth would be undefined behavior */
    uint64_t VP = ~UINT64_C(0) << (64 - max - 1);
    uint64_t VN = 0;

    const auto words = PM.size();
    int64_t currDist = max;
    uint64_t diagonal_mask = UINT64_C(1) << 63;
    uint64_t horizontal_mask = UINT64_C(1) << 62;
    ptrdiff_t start_pos = max + 1 - 64;

    /* score can decrease along the horizontal, but not along the diagonal */
    int64_t break_score = max + s2.size() - (s1.size() - max);

    /* Searching */
    ptrdiff_t i = 0;
    for (; i < s1.size() - max; ++i, ++start_pos) {
        /* Step 1: Computing D0 */
        uint64_t PM_j = 0;
        if (start_pos < 0) {
            PM_j = PM.get(0, s2[i]) << (-start_pos);
        }
        else {
            size_t word = static_cast<size_t>(start_pos) / 64;
            size_t word_pos = static_cast<size_t>(start_pos) % 64;

            PM_j = PM.get(word, s2[i]) >> word_pos;

            if (word + 1 < words && word_pos != 0) PM_j |= PM.get(word + 1, s2[i]) << (64 - word_pos);
        }
        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        currDist += !bool(D0 & diagonal_mask);

        if (currDist > break_score) return max + 1;

        /* Step 4: Computing Vp and VN */
        VP = HN | ~((D0 >> 1) | HP);
        VN = (D0 >> 1) & HP;
    }

    for (; i < s2.size(); ++i, ++start_pos) {
        /* Step 1: Computing D0 */
        uint64_t PM_j = 0;
        if (start_pos < 0) {
            PM_j = PM.get(0, s2[i]) << (-start_pos);
        }
        else {
            size_t word = static_cast<size_t>(start_pos) / 64;
            size_t word_pos = static_cast<size_t>(start_pos) % 64;

            PM_j = PM.get(word, s2[i]) >> word_pos;

            if (word + 1 < words && word_pos != 0) PM_j |= PM.get(word + 1, s2[i]) << (64 - word_pos);
        }
        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        currDist += bool(HP & horizontal_mask);
        currDist -= bool(HN & horizontal_mask);
        horizontal_mask >>= 1;

        if (currDist > break_score) return max + 1;

        /* Step 4: Computing Vp and VN */
        VP = HN | ~((D0 >> 1) | HP);
        VN = (D0 >> 1) & HP;
    }

    return (currDist <= max) ? currDist : max + 1;
}

template <bool RecordMatrix, typename InputIt1, typename InputIt2>
auto levenshtein_hyrroe2003_small_band(Range<InputIt1> s1, Range<InputIt2> s2, int64_t max)
    -> LevenshteinResult<RecordMatrix, false>
{
    assert(max <= s1.size());
    assert(max <= s2.size());

    /* VP is set to 1^m. Shifting by bitwidth would be undefined behavior */
    uint64_t VP = ~UINT64_C(0) << (64 - max - 1);
    uint64_t VN = 0;

    LevenshteinResult<RecordMatrix, false> res;
    res.dist = max;
    static_if<RecordMatrix>([&](auto f) {
        f(res).VP = ShiftedBitMatrix<uint64_t>(static_cast<size_t>(s2.size()), 1, ~UINT64_C(0));
        f(res).VN = ShiftedBitMatrix<uint64_t>(static_cast<size_t>(s2.size()), 1, 0);

        ptrdiff_t start_offset = max + 2 - 64;
        for (ptrdiff_t i = 0; i < s2.size(); ++i) {
            f(res).VP.set_offset(static_cast<size_t>(i), start_offset + i);
            f(res).VN.set_offset(static_cast<size_t>(i), start_offset + i);
        }
    });

    uint64_t diagonal_mask = UINT64_C(1) << 63;
    uint64_t horizontal_mask = UINT64_C(1) << 62;

    /* score can decrease along the horizontal, but not along the diagonal */
    int64_t break_score = max + s2.size() - (s1.size() - max);
    HybridGrowingHashmap<typename Range<InputIt1>::value_type, std::pair<ptrdiff_t, uint64_t>> PM;

    for (ptrdiff_t j = -max; j < 0; ++j) {
        auto& x = PM[s1[j + max]];
        x.second = shr64(x.second, j - x.first) | (UINT64_C(1) << 63);
        x.first = j;
    }

    /* Searching */
    ptrdiff_t i = 0;
    for (; i < s1.size() - max; ++i) {
        /* Step 1: Computing D0 */
        /* update bitmasks online */
        uint64_t PM_j = 0;
        if (i + max < s1.size()) {
            auto& x = PM[s1[i + max]];
            x.second = shr64(x.second, i - x.first) | (UINT64_C(1) << 63);
            x.first = i;
        }
        {
            auto x = PM.get(s2[i]);
            PM_j = shr64(x.second, i - x.first);
        }

        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        res.dist += !bool(D0 & diagonal_mask);

        if (res.dist > break_score) {
            res.dist = max + 1;
            return res;
        }

        /* Step 4: Computing Vp and VN */
        VP = HN | ~((D0 >> 1) | HP);
        VN = (D0 >> 1) & HP;

        static_if<RecordMatrix>([&](auto f) {
            f(res).VP[static_cast<size_t>(i)][0] = VP;
            f(res).VN[static_cast<size_t>(i)][0] = VN;
        });
    }

    for (; i < s2.size(); ++i) {
        /* Step 1: Computing D0 */
        /* update bitmasks online */
        uint64_t PM_j = 0;
        if (i + max < s1.size()) {
            auto& x = PM[s1[i + max]];
            x.second = shr64(x.second, i - x.first) | (UINT64_C(1) << 63);
            x.first = i;
        }
        {
            auto x = PM.get(s2[i]);
            PM_j = shr64(x.second, i - x.first);
        }

        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        res.dist += bool(HP & horizontal_mask);
        res.dist -= bool(HN & horizontal_mask);
        horizontal_mask >>= 1;

        if (res.dist > break_score) {
            res.dist = max + 1;
            return res;
        }

        /* Step 4: Computing Vp and VN */
        VP = HN | ~((D0 >> 1) | HP);
        VN = (D0 >> 1) & HP;

        static_if<RecordMatrix>([&](auto f) {
            f(res).VP[static_cast<size_t>(i)][0] = VP;
            f(res).VN[static_cast<size_t>(i)][0] = VN;
        });
    }

    if (res.dist > max) res.dist = max + 1;

    return res;
}

/**
 * @param stop_row specifies the row to record when using RecordBitRow
 */
template <bool RecordMatrix, bool RecordBitRow, typename InputIt1, typename InputIt2>
auto levenshtein_hyrroe2003_block(const BlockPatternMatchVector& PM, Range<InputIt1> s1, Range<InputIt2> s2,
                                  int64_t max = std::numeric_limits<int64_t>::max(), ptrdiff_t stop_row = -1)
    -> LevenshteinResult<RecordMatrix, RecordBitRow>
{
    ptrdiff_t word_size = sizeof(uint64_t) * 8;
    auto words = PM.size();
    std::vector<LevenshteinRow> vecs(words);
    std::vector<int64_t> scores(words);
    uint64_t Last = UINT64_C(1) << ((s1.size() - 1) % word_size);

    for (size_t i = 0; i < words - 1; ++i)
        scores[i] = static_cast<int64_t>(i + 1) * word_size;

    scores[words - 1] = s1.size();

    LevenshteinResult<RecordMatrix, RecordBitRow> res;
    static_if<RecordMatrix>([&](auto f) {
        int64_t full_band = std::min<int64_t>(s1.size(), 2 * max + 1);
        size_t full_band_words = std::min(words, static_cast<size_t>(full_band / word_size) + 2);
        f(res).VP = ShiftedBitMatrix<uint64_t>(static_cast<size_t>(s2.size()), full_band_words, ~UINT64_C(0));
        f(res).VN = ShiftedBitMatrix<uint64_t>(static_cast<size_t>(s2.size()), full_band_words, 0);
    });

    static_if<RecordBitRow>([&](auto f) {
        f(res).first_block = 0;
        f(res).last_block = 0;
        f(res).prev_score = 0;
    });

    max = std::min(max, static_cast<int64_t>(std::max(s1.size(), s2.size())));

    /* first_block is the index of the first block in Ukkonen band. */
    size_t first_block = 0;
    /* last_block is the index of the last block in Ukkonen band. */
    size_t last_block =
        std::min(words, static_cast<size_t>(
                            ceil_div(std::min(max, (max + s1.size() - s2.size()) / 2) + 1, word_size))) -
        1;

    /* Searching */
    for (ptrdiff_t row = 0; row < s2.size(); ++row) {
        uint64_t HP_carry = 1;
        uint64_t HN_carry = 0;

        static_if<RecordMatrix>([&](auto f) {
            f(res).VP.set_offset(static_cast<size_t>(row), static_cast<int64_t>(first_block) * word_size);
            f(res).VN.set_offset(static_cast<size_t>(row), static_cast<int64_t>(first_block) * word_size);
        });

        auto advance_block = [&](size_t word) {
            /* Step 1: Computing D0 */
            uint64_t PM_j = PM.get(word, s2[row]);
            uint64_t VN = vecs[word].VN;
            uint64_t VP = vecs[word].VP;

            uint64_t X = PM_j | HN_carry;
            uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

            /* Step 2: Computing HP and HN */
            uint64_t HP = VN | ~(D0 | VP);
            uint64_t HN = D0 & VP;

            uint64_t HP_carry_temp = HP_carry;
            uint64_t HN_carry_temp = HN_carry;
            if (word < words - 1) {
                HP_carry = HP >> 63;
                HN_carry = HN >> 63;
            }
            else {
                HP_carry = bool(HP & Last);
                HN_carry = bool(HN & Last);
            }

            /* Step 4: Computing Vp and VN */
            HP = (HP << 1) | HP_carry_temp;
            HN = (HN << 1) | HN_carry_temp;

            vecs[word].VP = HN | ~(D0 | HP);
            vecs[word].VN = HP & D0;

            static_if<RecordMatrix>([&](auto f) {
                f(res).VP[static_cast<size_t>(row)][word - first_block] = vecs[word].VP;
                f(res).VN[static_cast<size_t>(row)][word - first_block] = vecs[word].VN;
            });

            return static_cast<int64_t>(HP_carry) - static_cast<int64_t>(HN_carry);
        };

        auto get_row_num = [&](size_t word) {
            if (word + 1 == words) return s1.size() - 1;
            return static_cast<ptrdiff_t>(word + 1) * word_size - 1;
        };

        for (size_t word = first_block; word <= last_block /* - 1*/; word++) {
            /* Step 3: Computing the value D[m,j] */
            scores[word] += advance_block(word);
        }

        max = std::min(
            max, scores[last_block] +
                     std::max(s2.size() - row - 1,
                              s1.size() - (static_cast<ptrdiff_t>(1 + last_block) * word_size - 1) - 1));

        /*---------- Adjust number of blocks according to Ukkonen ----------*/
        // todo on the last word instead of word_size often s1.size() % 64 should be used

        /* Band adjustment: last_block */
        /*  If block is not beneath band, calculate next block. Only next because others are certainly beneath
         * band. */
        if (last_block + 1 < words && !(get_row_num(last_block) > max - scores[last_block] + 2 * word_size -
                                                                      2 - s2.size() + row + s1.size()))
        {
            last_block++;
            vecs[last_block].VP = ~UINT64_C(0);
            vecs[last_block].VN = 0;

            int64_t chars_in_block = (last_block + 1 == words) ? ((s1.size() - 1) % word_size + 1) : 64;
            scores[last_block] = scores[last_block - 1] + chars_in_block -
                                 (static_cast<int64_t>(HP_carry) - static_cast<int64_t>(HN_carry));
            scores[last_block] += advance_block(last_block);
        }

        for (; last_block >= first_block; --last_block) {
            /* in band if score <= k where score >= score_last - word_size + 1 */
            bool in_band_cond1 = scores[last_block] < max + word_size;

            /* in band if row <= max - score - len2 + len1 + i
             * if the condition is met for the first cell in the block, it
             * is met for all other cells in the blocks as well
             *
             * this uses a more loose condition similar to edlib:
             * https://github.com/Martinsos/edlib
             */
            bool in_band_cond2 = get_row_num(last_block) <= max - scores[last_block] + 2 * word_size - 2 -
                                                                s2.size() + row + s1.size() + 1;

            if (in_band_cond1 && in_band_cond2) break;
        }

        /* Band adjustment: first_block */
        for (; first_block <= last_block; ++first_block) {
            /* in band if score <= k where score >= score_last - word_size + 1 */
            bool in_band_cond1 = scores[first_block] < max + word_size;

            /* in band if row >= score - max - len2 + len1 + i
             * if this condition is met for the last cell in the block, it
             * is met for all other cells in the blocks as well
             */
            bool in_band_cond2 =
                get_row_num(first_block) >= scores[first_block] - max - s2.size() + s1.size() + row;

            if (in_band_cond1 && in_band_cond2) break;
        }

        /* distance is larger than max, so band stops to exist */
        if (last_block < first_block) {
            res.dist = max + 1;
            return res;
        }

        bool exit_ = false;
        static_if<RecordBitRow>([&](auto f) {
            if (row == stop_row) {
                if (first_block == 0)
                    f(res).prev_score = stop_row + 1;
                else {
                    /* count backwards to find score at last position in previous block */
                    ptrdiff_t relevant_bits =
                        std::min(static_cast<ptrdiff_t>((first_block + 1) * 64), s1.size()) % 64;
                    uint64_t mask = ~UINT64_C(0);
                    if (relevant_bits) mask >>= 64 - relevant_bits;

                    f(res).prev_score = scores[first_block] + popcount(vecs[first_block].VN & mask) -
                                        popcount(vecs[first_block].VP & mask);
                }

                f(res).first_block = first_block;
                f(res).last_block = last_block;
                f(res).vecs = std::move(vecs);

                /* unknown so make sure it is <= max */
                f(res).dist = 0;
                exit_ = true;
            }
        });

        if (exit_) return res;
    }

    res.dist = scores[words - 1];

    if (res.dist > max) res.dist = max + 1;

    return res;
}

template <typename InputIt1, typename InputIt2>
int64_t uniform_levenshtein_distance(const BlockPatternMatchVector& block, Range<InputIt1> s1,
                                     Range<InputIt2> s2, int64_t max)
{
    /* upper bound */
    max = std::min(max, std::max<int64_t>(s1.size(), s2.size()));

    // when no differences are allowed a direct comparision is sufficient
    if (max == 0) return !std::equal(s1.begin(), s1.end(), s2.begin(), s2.end());

    if (max < std::abs(s1.size() - s2.size())) return max + 1;

    // important to catch, since this causes block to be empty -> raises exception on access
    if (s1.empty()) {
        return (s2.size() <= max) ? s2.size() : max + 1;
    }

    /* do this first, since we can not remove any affix in encoded form
     * todo actually we could at least remove the common prefix and just shift the band
     */
    if (max >= 4) {
        // todo could safe up to 25% even without max when ignoring irrelevant paths
        // in the upper and lower corner
        int64_t full_band = std::min<int64_t>(s1.size(), 2 * max + 1);

        if (s1.size() < 65)
            return levenshtein_hyrroe2003<false, false>(block, s1, s2, max).dist;
        else if (full_band <= 64)
            return levenshtein_hyrroe2003_small_band(block, s1, s2, max);
        else
            return levenshtein_hyrroe2003_block<false, false>(block, s1, s2, max).dist;
    }

    /* common affix does not effect Levenshtein distance */
    remove_common_affix(s1, s2);
    if (s1.empty() || s2.empty()) return s1.size() + s2.size();

    return levenshtein_mbleven2018(s1, s2, max);
}

template <typename InputIt1, typename InputIt2>
int64_t uniform_levenshtein_distance(Range<InputIt1> s1, Range<InputIt2> s2, int64_t max)
{
    /* Swapping the strings so the second string is shorter */
    if (s1.size() < s2.size()) return uniform_levenshtein_distance(s2, s1, max);

    /* upper bound */
    max = std::min(max, std::max<int64_t>(s1.size(), s2.size()));

    // when no differences are allowed a direct comparision is sufficient
    if (max == 0) return !std::equal(s1.begin(), s1.end(), s2.begin(), s2.end());

    // at least length difference insertions/deletions required
    if (max < (s1.size() - s2.size())) return max + 1;

    /* common affix does not effect Levenshtein distance */
    remove_common_affix(s1, s2);
    if (s1.empty() || s2.empty()) return s1.size() + s2.size();

    if (max < 4) return levenshtein_mbleven2018(s1, s2, max);

    // todo could safe up to 25% even without max when ignoring irrelevant paths
    // in the upper and lower corner
    int64_t full_band = std::min<int64_t>(s1.size(), 2 * max + 1);

    /* when the short strings has less then 65 elements Hyyrös' algorithm can be used */
    if (s2.size() < 65)
        return levenshtein_hyrroe2003<false, false>(PatternMatchVector(s2), s2, s1, max).dist;
    else if (full_band <= 64)
        return levenshtein_hyrroe2003_small_band<false>(s1, s2, max).dist;
    else
        return levenshtein_hyrroe2003_block<false, false>(BlockPatternMatchVector(s1), s1, s2, max).dist;
}

/**
 * @brief recover alignment from bitparallel Levenshtein matrix
 */
template <typename InputIt1, typename InputIt2>
void recover_alignment(Editops& editops, Range<InputIt1> s1, Range<InputIt2> s2,
                       const LevenshteinResult<true, false>& matrix, size_t src_pos, size_t dest_pos,
                       size_t editop_pos)
{
    size_t dist = static_cast<size_t>(matrix.dist);
    size_t col = static_cast<size_t>(s1.size());
    size_t row = static_cast<size_t>(s2.size());

    while (row && col) {
        /* Deletion */
        if (matrix.VP.test_bit(row - 1, col - 1)) {
            assert(dist > 0);
            dist--;
            col--;
            editops[editop_pos + dist].type = EditType::Delete;
            editops[editop_pos + dist].src_pos = col + src_pos;
            editops[editop_pos + dist].dest_pos = row + dest_pos;
        }
        else {
            row--;

            /* Insertion */
            if (row && matrix.VN.test_bit(row - 1, col - 1)) {
                assert(dist > 0);
                dist--;
                editops[editop_pos + dist].type = EditType::Insert;
                editops[editop_pos + dist].src_pos = col + src_pos;
                editops[editop_pos + dist].dest_pos = row + dest_pos;
            }
            /* Match/Mismatch */
            else {
                col--;

                /* Replace (Matches are not recorded) */
                if (s1[static_cast<ptrdiff_t>(col)] != s2[static_cast<ptrdiff_t>(row)]) {
                    assert(dist > 0);
                    dist--;
                    editops[editop_pos + dist].type = EditType::Replace;
                    editops[editop_pos + dist].src_pos = col + src_pos;
                    editops[editop_pos + dist].dest_pos = row + dest_pos;
                }
            }
        }
    }

    while (col) {
        dist--;
        col--;
        editops[editop_pos + dist].type = EditType::Delete;
        editops[editop_pos + dist].src_pos = col + src_pos;
        editops[editop_pos + dist].dest_pos = row + dest_pos;
    }

    while (row) {
        dist--;
        row--;
        editops[editop_pos + dist].type = EditType::Insert;
        editops[editop_pos + dist].src_pos = col + src_pos;
        editops[editop_pos + dist].dest_pos = row + dest_pos;
    }
}

template <typename InputIt1, typename InputIt2>
void levenshtein_align(Editops& editops, Range<InputIt1> s1, Range<InputIt2> s2,
                       int64_t max = std::numeric_limits<int64_t>::max(), size_t src_pos = 0,
                       size_t dest_pos = 0, size_t editop_pos = 0)
{
    /* upper bound */
    max = std::min(max, std::max<int64_t>(s1.size(), s2.size()));
    int64_t full_band = std::min<int64_t>(s1.size(), 2 * max + 1);

    LevenshteinResult<true, false> matrix;
    do {
        if (s1.empty() || s2.empty())
            matrix.dist = s1.size() + s2.size();
        else if (s1.size() <= 64)
            matrix = levenshtein_hyrroe2003<true, false>(PatternMatchVector(s1), s1, s2);
        else if (full_band <= 64)
            matrix = levenshtein_hyrroe2003_small_band<true>(s1, s2, max);
        else
            matrix = levenshtein_hyrroe2003_block<true, false>(BlockPatternMatchVector(s1), s1, s2, max);

        max *= 2;
    } while (matrix.dist > max);

    if (matrix.dist != 0) {
        if (editops.size() == 0) editops.resize(static_cast<size_t>(matrix.dist));

        recover_alignment(editops, s1, s2, matrix, src_pos, dest_pos, editop_pos);
    }
}

template <typename InputIt1, typename InputIt2>
LevenshteinResult<false, true> levenshtein_row(Range<InputIt1> s1, Range<InputIt2> s2, int64_t max,
                                               ptrdiff_t stop_row)
{
    return levenshtein_hyrroe2003_block<false, true>(BlockPatternMatchVector(s1), s1, s2, max, stop_row);
}

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_distance(Range<InputIt1> s1, Range<InputIt2> s2,
                             LevenshteinWeightTable weights = {1, 1, 1},
                             int64_t max = std::numeric_limits<int64_t>::max())
{
    if (weights.insert_cost == weights.delete_cost) {
        /* when insertions + deletions operations are free there can not be any edit distance */
        if (weights.insert_cost == 0) return 0;

        /* uniform Levenshtein multiplied with the common factor */
        if (weights.insert_cost == weights.replace_cost) {
            // max can make use of the common divisor of the three weights
            int64_t new_max = ceil_div(max, weights.insert_cost);
            int64_t distance = uniform_levenshtein_distance(s1, s2, new_max);
            distance *= weights.insert_cost;
            return (distance <= max) ? distance : max + 1;
        }
        /*
         * when replace_cost >= insert_cost + delete_cost no substitutions are performed
         * therefore this can be implemented as InDel distance multiplied with the common factor
         */
        else if (weights.replace_cost >= weights.insert_cost + weights.delete_cost) {
            // max can make use of the common divisor of the three weights
            int64_t new_max = ceil_div(max, weights.insert_cost);
            int64_t distance = rapidfuzz::indel_distance(s1, s2, new_max);
            distance *= weights.insert_cost;
            return (distance <= max) ? distance : max + 1;
        }
    }

    return generalized_levenshtein_wagner_fischer(s1, s2, weights, max);
}
struct HirschbergPos {
    int64_t left_score;
    int64_t right_score;
    ptrdiff_t s1_mid;
    ptrdiff_t s2_mid;
};

template <typename InputIt1, typename InputIt2>
HirschbergPos find_hirschberg_pos(Range<InputIt1> s1, Range<InputIt2> s2,
                                  int64_t max = std::numeric_limits<int64_t>::max())
{
    HirschbergPos hpos = {};
    ptrdiff_t left_size = s2.size() / 2;
    ptrdiff_t right_size = s2.size() - left_size;
    hpos.s2_mid = left_size;
    size_t s1_len = static_cast<size_t>(s1.size());
    int64_t best_score = std::numeric_limits<int64_t>::max();
    size_t right_first_pos = 0;
    size_t right_last_pos = 0;
    std::vector<int64_t> right_scores;

    {
        auto right_row = levenshtein_row(s1.reversed(), s2.reversed(), max, right_size - 1);
        if (right_row.dist > max) return find_hirschberg_pos(s1, s2, max * 2);

        right_first_pos = right_row.first_block * 64;
        right_last_pos = std::min(s1_len, right_row.last_block * 64 + 64);

        right_scores.resize(right_last_pos - right_first_pos + 1, 0);
        assume(right_scores.size() != 0);
        right_scores[0] = right_row.prev_score;

        for (size_t i = right_first_pos; i < right_last_pos; ++i) {
            size_t col_pos = i % 64;
            size_t col_word = i / 64;
            uint64_t col_mask = UINT64_C(1) << col_pos;

            right_scores[i - right_first_pos + 1] = right_scores[i - right_first_pos];
            right_scores[i - right_first_pos + 1] -= bool(right_row.vecs[col_word].VN & col_mask);
            right_scores[i - right_first_pos + 1] += bool(right_row.vecs[col_word].VP & col_mask);
        }
    }

    auto left_row = levenshtein_row(s1, s2, max, left_size - 1);
    if (left_row.dist > max) return find_hirschberg_pos(s1, s2, max * 2);

    auto left_first_pos = left_row.first_block * 64;
    auto left_last_pos = std::min(s1_len, left_row.last_block * 64 + 64);

    int64_t left_score = left_row.prev_score;
    for (size_t i = left_first_pos; i < left_last_pos; ++i) {
        size_t col_pos = i % 64;
        size_t col_word = i / 64;
        uint64_t col_mask = UINT64_C(1) << col_pos;

        left_score -= bool(left_row.vecs[col_word].VN & col_mask);
        left_score += bool(left_row.vecs[col_word].VP & col_mask);

        if (s1_len < i + 1 + right_first_pos) continue;

        size_t right_index = s1_len - i - 1 - right_first_pos;
        if (right_index >= right_scores.size()) continue;

        if (right_scores[right_index] + left_score < best_score) {
            best_score = right_scores[right_index] + left_score;
            hpos.left_score = left_score;
            hpos.right_score = right_scores[right_index];
            hpos.s1_mid = static_cast<ptrdiff_t>(i + 1);
        }
    }

    assert(hpos.left_score >= 0);
    assert(hpos.right_score >= 0);

    if (hpos.left_score + hpos.right_score > max)
        return find_hirschberg_pos(s1, s2, max * 2);
    else {
        assert(levenshtein_distance(s1, s2) == hpos.left_score + hpos.right_score);
        return hpos;
    }
}

template <typename InputIt1, typename InputIt2>
void levenshtein_align_hirschberg(Editops& editops, Range<InputIt1> s1, Range<InputIt2> s2,
                                  size_t src_pos = 0, size_t dest_pos = 0, size_t editop_pos = 0,
                                  int64_t max = std::numeric_limits<int64_t>::max())
{
    /* prefix and suffix are no-ops, which do not need to be added to the editops */
    StringAffix affix = remove_common_affix(s1, s2);
    src_pos += affix.prefix_len;
    dest_pos += affix.prefix_len;

    max = std::min(max, std::max<int64_t>(s1.size(), s2.size()));
    int64_t full_band = std::min<int64_t>(s1.size(), 2 * max + 1);

    ptrdiff_t matrix_size = 2 * full_band * s2.size() / 8;
    if (matrix_size < 1024 * 1024 || s1.size() < 65 || s2.size() < 10) {
        levenshtein_align(editops, s1, s2, max, src_pos, dest_pos, editop_pos);
    }
    /* Hirschbergs algorithm */
    else {
        auto hpos = find_hirschberg_pos(s1, s2, max);

        if (editops.size() == 0) editops.resize(static_cast<size_t>(hpos.left_score + hpos.right_score));

        levenshtein_align_hirschberg(editops, s1.subseq(0, hpos.s1_mid), s2.subseq(0, hpos.s2_mid), src_pos,
                                     dest_pos, editop_pos, hpos.left_score);
        levenshtein_align_hirschberg(editops, s1.subseq(hpos.s1_mid), s2.subseq(hpos.s2_mid),
                                     src_pos + static_cast<size_t>(hpos.s1_mid),
                                     dest_pos + static_cast<size_t>(hpos.s2_mid),
                                     editop_pos + static_cast<size_t>(hpos.left_score), hpos.right_score);
    }
}

template <typename InputIt1, typename InputIt2>
Editops levenshtein_editops(Range<InputIt1> s1, Range<InputIt2> s2, int64_t score_hint)
{
    Editops editops;
    if (score_hint < 31) score_hint = 31;

    levenshtein_align_hirschberg(editops, s1, s2, 0, 0, 0, score_hint);
    editops.set_src_len(static_cast<size_t>(s1.size()));
    editops.set_dest_len(static_cast<size_t>(s2.size()));
    return editops;
}

class Levenshtein : public DistanceBase<Levenshtein, LevenshteinWeightTable> {
    friend DistanceBase<Levenshtein, LevenshteinWeightTable>;
    friend NormalizedMetricBase<Levenshtein, LevenshteinWeightTable>;

    template <typename InputIt1, typename InputIt2>
    static int64_t maximum(Range<InputIt1> s1, Range<InputIt2> s2, LevenshteinWeightTable weights)
    {
        return levenshtein_maximum(s1, s2, weights);
    }

    template <typename InputIt1, typename InputIt2>
    static int64_t _distance(Range<InputIt1> s1, Range<InputIt2> s2, LevenshteinWeightTable weights,
                             int64_t score_cutoff)
    {
        return levenshtein_distance(s1, s2, weights, score_cutoff);
    }
};

} // namespace detail
} // namespace rapidfuzz

namespace rapidfuzz {

/**
 * @brief Calculates the minimum number of insertions, deletions, and substitutions
 * required to change one sequence into the other according to Levenshtein with custom
 * costs for insertion, deletion and substitution
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 * @param weights
 *   The weights for the three operations in the form
 *   (insertion, deletion, substitution). Default is {1, 1, 1},
 *   which gives all three operations a weight of 1.
 * @param max
 *   Maximum Levenshtein distance between s1 and s2, that is
 *   considered as a result. If the distance is bigger than max,
 *   max + 1 is returned instead. Default is std::numeric_limits<int64_t>::max(),
 *   which deactivates this behaviour.
 *
 * @return returns the levenshtein distance between s1 and s2
 *
 * @remarks
 * @parblock
 * Depending on the input parameters different optimized implementation are used
 * to improve the performance. Worst-case performance is ``O(m * n)``.
 *
 * <b>Insertion = Deletion = Substitution:</b>
 *
 *    This is known as uniform Levenshtein distance and is the distance most commonly
 *    referred to as Levenshtein distance. The following implementation is used
 *    with a worst-case performance of ``O([N/64]M)``.
 *
 *    - if max is 0 the similarity can be calculated using a direct comparision,
 *      since no difference between the strings is allowed.  The time complexity of
 *      this algorithm is ``O(N)``.
 *
 *    - A common prefix/suffix of the two compared strings does not affect
 *      the Levenshtein distance, so the affix is removed before calculating the
 *      similarity.
 *
 *    - If max is <= 3 the mbleven algorithm is used. This algorithm
 *      checks all possible edit operations that are possible under
 *      the threshold `max`. The time complexity of this algorithm is ``O(N)``.
 *
 *    - If the length of the shorter string is <= 64 after removing the common affix
 *      Hyyrös' algorithm is used, which calculates the Levenshtein distance in
 *      parallel. The algorithm is described by @cite hyrro_2002. The time complexity of this
 *      algorithm is ``O(N)``.
 *
 *    - If the length of the shorter string is >= 64 after removing the common affix
 *      a blockwise implementation of Myers' algorithm is used, which calculates
 *      the Levenshtein distance in parallel (64 characters at a time).
 *      The algorithm is described by @cite myers_1999. The time complexity of this
 *      algorithm is ``O([N/64]M)``.
 *
 *
 * <b>Insertion = Deletion, Substitution >= Insertion + Deletion:</b>
 *
 *    Since every Substitution can be performed as Insertion + Deletion, this variant
 *    of the Levenshtein distance only uses Insertions and Deletions. Therefore this
 *    variant is often referred to as InDel-Distance.  The following implementation
 *    is used with a worst-case performance of ``O([N/64]M)``.
 *
 *    - if max is 0 the similarity can be calculated using a direct comparision,
 *      since no difference between the strings is allowed.  The time complexity of
 *      this algorithm is ``O(N)``.
 *
 *    - if max is 1 and the two strings have a similar length, the similarity can be
 *      calculated using a direct comparision aswell, since a substitution would cause
 *      a edit distance higher than max. The time complexity of this algorithm
 *      is ``O(N)``.
 *
 *    - A common prefix/suffix of the two compared strings does not affect
 *      the Levenshtein distance, so the affix is removed before calculating the
 *      similarity.
 *
 *    - If max is <= 4 the mbleven algorithm is used. This algorithm
 *      checks all possible edit operations that are possible under
 *      the threshold `max`. As a difference to the normal Levenshtein distance this
 *      algorithm can even be used up to a threshold of 4 here, since the higher weight
 *      of substitutions decreases the amount of possible edit operations.
 *      The time complexity of this algorithm is ``O(N)``.
 *
 *    - If the length of the shorter string is <= 64 after removing the common affix
 *      Hyyrös' lcs algorithm is used, which calculates the InDel distance in
 *      parallel. The algorithm is described by @cite hyrro_lcs_2004 and is extended with support
 *      for UTF32 in this implementation. The time complexity of this
 *      algorithm is ``O(N)``.
 *
 *    - If the length of the shorter string is >= 64 after removing the common affix
 *      a blockwise implementation of Hyyrös' lcs algorithm is used, which calculates
 *      the Levenshtein distance in parallel (64 characters at a time).
 *      The algorithm is described by @cite hyrro_lcs_2004. The time complexity of this
 *      algorithm is ``O([N/64]M)``.
 *
 * <b>Other weights:</b>
 *
 *   The implementation for other weights is based on Wagner-Fischer.
 *   It has a performance of ``O(N * M)`` and has a memory usage of ``O(N)``.
 *   Further details can be found in @cite wagner_fischer_1974.
 * @endparblock
 *
 * @par Examples
 * @parblock
 * Find the Levenshtein distance between two strings:
 * @code{.cpp}
 * // dist is 2
 * int64_t dist = levenshtein_distance("lewenstein", "levenshtein");
 * @endcode
 *
 * Setting a maximum distance allows the implementation to select
 * a more efficient implementation:
 * @code{.cpp}
 * // dist is 2
 * int64_t dist = levenshtein_distance("lewenstein", "levenshtein", {1, 1, 1}, 1);
 * @endcode
 *
 * It is possible to select different weights by passing a `weight` struct.
 * @code{.cpp}
 * // dist is 3
 * int64_t dist = levenshtein_distance("lewenstein", "levenshtein", {1, 1, 2});
 * @endcode
 * @endparblock
 */
template <typename InputIt1, typename InputIt2>
int64_t levenshtein_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                             LevenshteinWeightTable weights = {1, 1, 1},
                             int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::Levenshtein::distance(first1, last1, first2, last2, weights, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t levenshtein_distance(const Sentence1& s1, const Sentence2& s2,
                             LevenshteinWeightTable weights = {1, 1, 1},
                             int64_t score_cutoff = std::numeric_limits<int64_t>::max())
{
    return detail::Levenshtein::distance(s1, s2, weights, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                               LevenshteinWeightTable weights = {1, 1, 1}, int64_t score_cutoff = 0.0)
{
    return detail::Levenshtein::similarity(first1, last1, first2, last2, weights, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t levenshtein_similarity(const Sentence1& s1, const Sentence2& s2,
                               LevenshteinWeightTable weights = {1, 1, 1}, int64_t score_cutoff = 0.0)
{
    return detail::Levenshtein::similarity(s1, s2, weights, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double levenshtein_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                       LevenshteinWeightTable weights = {1, 1, 1}, double score_cutoff = 1.0)
{
    return detail::Levenshtein::normalized_distance(first1, last1, first2, last2, weights, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double levenshtein_normalized_distance(const Sentence1& s1, const Sentence2& s2,
                                       LevenshteinWeightTable weights = {1, 1, 1}, double score_cutoff = 1.0)
{
    return detail::Levenshtein::normalized_distance(s1, s2, weights, score_cutoff);
}

/**
 * @brief Calculates a normalized levenshtein distance using custom
 * costs for insertion, deletion and substitution.
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 * @param weights
 *   The weights for the three operations in the form
 *   (insertion, deletion, substitution). Default is {1, 1, 1},
 *   which gives all three operations a weight of 1.
 * @param score_cutoff
 *   Optional argument for a score threshold as a float between 0 and 1.0.
 *   For ratio < score_cutoff 0 is returned instead. Default is 0,
 *   which deactivates this behaviour.
 *
 * @return Normalized weighted levenshtein distance between s1 and s2
 *   as a double between 0 and 1.0
 *
 * @see levenshtein()
 *
 * @remarks
 * @parblock
 * The normalization of the Levenshtein distance is performed in the following way:
 *
 * \f{align*}{
 *   ratio &= \frac{distance(s1, s2)}{max_dist}
 * \f}
 * @endparblock
 *
 *
 * @par Examples
 * @parblock
 * Find the normalized Levenshtein distance between two strings:
 * @code{.cpp}
 * // ratio is 81.81818181818181
 * double ratio = normalized_levenshtein("lewenstein", "levenshtein");
 * @endcode
 *
 * Setting a score_cutoff allows the implementation to select
 * a more efficient implementation:
 * @code{.cpp}
 * // ratio is 0.0
 * double ratio = normalized_levenshtein("lewenstein", "levenshtein", {1, 1, 1}, 85.0);
 * @endcode
 *
 * It is possible to select different weights by passing a `weight` struct
 * @code{.cpp}
 * // ratio is 85.71428571428571
 * double ratio = normalized_levenshtein("lewenstein", "levenshtein", {1, 1, 2});
 * @endcode
 * @endparblock
 */
template <typename InputIt1, typename InputIt2>
double levenshtein_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                         LevenshteinWeightTable weights = {1, 1, 1},
                                         double score_cutoff = 0.0)
{
    return detail::Levenshtein::normalized_similarity(first1, last1, first2, last2, weights, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double levenshtein_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                         LevenshteinWeightTable weights = {1, 1, 1},
                                         double score_cutoff = 0.0)
{
    return detail::Levenshtein::normalized_similarity(s1, s2, weights, score_cutoff);
}

/**
 * @brief Return list of EditOp describing how to turn s1 into s2.
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1
 *   string to compare with s2 (for type info check Template parameters above)
 * @param s2
 *   string to compare with s1 (for type info check Template parameters above)
 *
 * @return Edit operations required to turn s1 into s2
 */
template <typename InputIt1, typename InputIt2>
Editops levenshtein_editops(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                            int64_t score_hint = std::numeric_limits<int64_t>::max())
{
    return detail::levenshtein_editops(detail::make_range(first1, last1), detail::make_range(first2, last2),
                                       score_hint);
}

template <typename Sentence1, typename Sentence2>
Editops levenshtein_editops(const Sentence1& s1, const Sentence2& s2,
                            int64_t score_hint = std::numeric_limits<int64_t>::max())
{
    return detail::levenshtein_editops(detail::make_range(s1), detail::make_range(s2), score_hint);
}

template <typename CharT1>
struct CachedLevenshtein : public detail::CachedDistanceBase<CachedLevenshtein<CharT1>> {
    template <typename Sentence1>
    CachedLevenshtein(const Sentence1& s1_, LevenshteinWeightTable aWeights = {1, 1, 1})
        : CachedLevenshtein(detail::to_begin(s1_), detail::to_end(s1_), aWeights)
    {}

    template <typename InputIt1>
    CachedLevenshtein(InputIt1 first1, InputIt1 last1, LevenshteinWeightTable aWeights = {1, 1, 1})
        : s1(first1, last1), PM(detail::make_range(first1, last1)), weights(aWeights)
    {}

private:
    friend detail::CachedDistanceBase<CachedLevenshtein<CharT1>>;
    friend detail::CachedNormalizedMetricBase<CachedLevenshtein<CharT1>>;

    template <typename InputIt2>
    int64_t maximum(detail::Range<InputIt2> s2) const
    {
        return detail::levenshtein_maximum(detail::make_range(s1), s2, weights);
    }

    template <typename InputIt2>
    int64_t _distance(detail::Range<InputIt2> s2, int64_t score_cutoff) const
    {
        if (weights.insert_cost == weights.delete_cost) {
            /* when insertions + deletions operations are free there can not be any edit distance */
            if (weights.insert_cost == 0) return 0;

            /* uniform Levenshtein multiplied with the common factor */
            if (weights.insert_cost == weights.replace_cost) {
                // max can make use of the common divisor of the three weights
                int64_t new_max = detail::ceil_div(score_cutoff, weights.insert_cost);
                int64_t dist = detail::uniform_levenshtein_distance(PM, detail::make_range(s1), s2, new_max);
                dist *= weights.insert_cost;

                return (dist <= score_cutoff) ? dist : score_cutoff + 1;
            }
            /*
             * when replace_cost >= insert_cost + delete_cost no substitutions are performed
             * therefore this can be implemented as InDel distance multiplied with the common factor
             */
            else if (weights.replace_cost >= weights.insert_cost + weights.delete_cost) {
                // max can make use of the common divisor of the three weights
                int64_t new_max = detail::ceil_div(score_cutoff, weights.insert_cost);
                int64_t dist = detail::indel_distance(PM, detail::make_range(s1), s2, new_max);
                dist *= weights.insert_cost;
                return (dist <= score_cutoff) ? dist : score_cutoff + 1;
            }
        }

        return detail::generalized_levenshtein_distance(detail::make_range(s1), s2, weights, score_cutoff);
    }

    std::basic_string<CharT1> s1;
    detail::BlockPatternMatchVector PM;
    LevenshteinWeightTable weights;
};

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename Sentence1>
CachedLevenshtein(const Sentence1& s1_, LevenshteinWeightTable aWeights)
    -> CachedLevenshtein<char_type<Sentence1>>;

template <typename InputIt1>
CachedLevenshtein(InputIt1 first1, InputIt1 last1, LevenshteinWeightTable aWeights)
    -> CachedLevenshtein<iter_value_t<InputIt1>>;
#endif

} // namespace rapidfuzz

namespace rapidfuzz {

template <typename CharT, typename InputIt1, typename InputIt2>
std::basic_string<CharT> editops_apply(const Editops& ops, InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                       InputIt2 last2)
{
    auto len1 = static_cast<size_t>(std::distance(first1, last1));
    auto len2 = static_cast<size_t>(std::distance(first2, last2));

    std::basic_string<CharT> res_str;
    res_str.resize(len1 + len2);
    size_t src_pos = 0;
    size_t dest_pos = 0;

    for (const auto& op : ops) {
        /* matches between last and current editop */
        while (src_pos < op.src_pos) {
            res_str[dest_pos] = static_cast<CharT>(first1[static_cast<ptrdiff_t>(src_pos)]);
            src_pos++;
            dest_pos++;
        }

        switch (op.type) {
        case EditType::None:
        case EditType::Replace:
            res_str[dest_pos] = static_cast<CharT>(first2[static_cast<ptrdiff_t>(op.dest_pos)]);
            src_pos++;
            dest_pos++;
            break;
        case EditType::Insert:
            res_str[dest_pos] = static_cast<CharT>(first2[static_cast<ptrdiff_t>(op.dest_pos)]);
            dest_pos++;
            break;
        case EditType::Delete: src_pos++; break;
        }
    }

    /* matches after the last editop */
    while (src_pos < len1) {
        res_str[dest_pos] = static_cast<CharT>(first1[static_cast<ptrdiff_t>(src_pos)]);
        src_pos++;
        dest_pos++;
    }

    res_str.resize(dest_pos);
    return res_str;
}

template <typename CharT, typename Sentence1, typename Sentence2>
std::basic_string<CharT> editops_apply(const Editops& ops, const Sentence1& s1, const Sentence2& s2)
{
    return editops_apply<CharT>(ops, detail::to_begin(s1), detail::to_end(s1), detail::to_begin(s2),
                                detail::to_end(s2));
}

template <typename CharT, typename InputIt1, typename InputIt2>
std::basic_string<CharT> opcodes_apply(const Opcodes& ops, InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                       InputIt2 last2)
{
    auto len1 = static_cast<size_t>(std::distance(first1, last1));
    auto len2 = static_cast<size_t>(std::distance(first2, last2));

    std::basic_string<CharT> res_str;
    res_str.resize(len1 + len2);
    size_t dest_pos = 0;

    for (const auto& op : ops) {
        switch (op.type) {
        case EditType::None:
            for (auto i = op.src_begin; i < op.src_end; ++i) {
                res_str[dest_pos++] = static_cast<CharT>(first1[static_cast<ptrdiff_t>(i)]);
            }
            break;
        case EditType::Replace:
        case EditType::Insert:
            for (auto i = op.dest_begin; i < op.dest_end; ++i) {
                res_str[dest_pos++] = static_cast<CharT>(first2[static_cast<ptrdiff_t>(i)]);
            }
            break;
        case EditType::Delete: break;
        }
    }

    res_str.resize(dest_pos);
    return res_str;
}

template <typename CharT, typename Sentence1, typename Sentence2>
std::basic_string<CharT> opcodes_apply(const Opcodes& ops, const Sentence1& s1, const Sentence2& s2)
{
    return opcodes_apply<CharT>(ops, detail::to_begin(s1), detail::to_end(s1), detail::to_begin(s2),
                                detail::to_end(s2));
}

} // namespace rapidfuzz

#include <array>
#include <limits>
#include <stdio.h>
#include <type_traits>
#include <unordered_set>

namespace rapidfuzz {
namespace detail {

/*
 * taken from https://stackoverflow.com/a/17251989/11335032
 */
template <typename T, typename U>
bool CanTypeFitValue(const U value)
{
    const intmax_t botT = intmax_t(std::numeric_limits<T>::min());
    const intmax_t botU = intmax_t(std::numeric_limits<U>::min());
    const uintmax_t topT = uintmax_t(std::numeric_limits<T>::max());
    const uintmax_t topU = uintmax_t(std::numeric_limits<U>::max());
    return !((botT > botU && value < static_cast<U>(botT)) || (topT < topU && value > static_cast<U>(topT)));
}

template <typename CharT1, size_t size = sizeof(CharT1)>
struct CharSet;

template <typename CharT1>
struct CharSet<CharT1, 1> {
    using UCharT1 = typename std::make_unsigned<CharT1>::type;

    std::array<bool, std::numeric_limits<UCharT1>::max() + 1> m_val;

    CharSet() : m_val{}
    {}

    void insert(CharT1 ch)
    {
        m_val[UCharT1(ch)] = true;
    }

    template <typename CharT2>
    bool find(CharT2 ch) const
    {
        if (!CanTypeFitValue<CharT1>(ch)) return false;

        return m_val[UCharT1(ch)];
    }
};

template <typename CharT1, size_t size>
struct CharSet {
    std::unordered_set<CharT1> m_val;

    CharSet() : m_val{}
    {}

    void insert(CharT1 ch)
    {
        m_val.insert(ch);
    }

    template <typename CharT2>
    bool find(CharT2 ch) const
    {
        if (!CanTypeFitValue<CharT1>(ch)) return false;

        return m_val.find(CharT1(ch)) != m_val.end();
    }
};

} // namespace detail
} // namespace rapidfuzz

#include <type_traits>

namespace rapidfuzz {
namespace fuzz {

/**
 * @defgroup Fuzz Fuzz
 * A collection of string matching algorithms from FuzzyWuzzy
 * @{
 */

/**
 * @brief calculates a simple ratio between two strings
 *
 * @details
 * @code{.cpp}
 * // score is 96.55
 * double score = ratio("this is a test", "this is a test!")
 * @endcode
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename InputIt1, typename InputIt2>
double ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// TODO documentation
template <typename CharT1>
struct CachedRatio {
    template <typename InputIt1>
    CachedRatio(InputIt1 first1, InputIt1 last1) : cached_indel(first1, last1)
    {}

    template <typename Sentence1>
    CachedRatio(const Sentence1& s1) : cached_indel(s1)
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0.0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

    // private:
    CachedIndel<CharT1> cached_indel;
};

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename Sentence1>
CachedRatio(const Sentence1& s1) -> CachedRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedRatio(InputIt1 first1, InputIt1 last1) -> CachedRatio<iter_value_t<InputIt1>>;
#endif

template <typename InputIt1, typename InputIt2>
ScoreAlignment<double> partial_ratio_alignment(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                               InputIt2 last2, double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
ScoreAlignment<double> partial_ratio_alignment(const Sentence1& s1, const Sentence2& s2,
                                               double score_cutoff = 0);

/**
 * @brief calculates the fuzz::ratio of the optimal string alignment
 *
 * @details
 * test @cite hyrro_2004 @cite wagner_fischer_1974
 * @code{.cpp}
 * // score is 100
 * double score = partial_ratio("this is a test", "this is a test!")
 * @endcode
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename InputIt1, typename InputIt2>
double partial_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                     double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double partial_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// todo add real implementation
template <typename CharT1>
struct CachedPartialRatio {
    template <typename>
    friend struct CachedWRatio;

    template <typename InputIt1>
    CachedPartialRatio(InputIt1 first1, InputIt1 last1);

    template <typename Sentence1>
    CachedPartialRatio(const Sentence1& s1_) : CachedPartialRatio(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0.0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0.0) const;

private:
    std::basic_string<CharT1> s1;
    rapidfuzz::detail::CharSet<CharT1> s1_char_set;
    CachedRatio<CharT1> cached_ratio;
};

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename Sentence1>
CachedPartialRatio(const Sentence1& s1) -> CachedPartialRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedPartialRatio(InputIt1 first1, InputIt1 last1) -> CachedPartialRatio<iter_value_t<InputIt1>>;
#endif

/**
 * @brief Sorts the words in the strings and calculates the fuzz::ratio between
 * them
 *
 * @details
 * @code{.cpp}
 * // score is 100
 * double score = token_sort_ratio("fuzzy wuzzy was a bear", "wuzzy fuzzy was a
 * bear")
 * @endcode
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename InputIt1, typename InputIt2>
double token_sort_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                        double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double token_sort_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// todo CachedRatio speed for equal strings vs original implementation
// TODO documentation
template <typename CharT1>
struct CachedTokenSortRatio {
    template <typename InputIt1>
    CachedTokenSortRatio(InputIt1 first1, InputIt1 last1)
        : s1_sorted(detail::sorted_split(first1, last1).join()), cached_ratio(s1_sorted)
    {}

    template <typename Sentence1>
    CachedTokenSortRatio(const Sentence1& s1) : CachedTokenSortRatio(detail::to_begin(s1), detail::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1_sorted;
    CachedRatio<CharT1> cached_ratio;
};

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename Sentence1>
CachedTokenSortRatio(const Sentence1& s1) -> CachedTokenSortRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedTokenSortRatio(InputIt1 first1, InputIt1 last1) -> CachedTokenSortRatio<iter_value_t<InputIt1>>;
#endif

/**
 * @brief Sorts the words in the strings and calculates the fuzz::partial_ratio
 * between them
 *
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename InputIt1, typename InputIt2>
double partial_token_sort_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double partial_token_sort_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// TODO documentation
template <typename CharT1>
struct CachedPartialTokenSortRatio {
    template <typename InputIt1>
    CachedPartialTokenSortRatio(InputIt1 first1, InputIt1 last1)
        : s1_sorted(detail::sorted_split(first1, last1).join()), cached_partial_ratio(s1_sorted)
    {}

    template <typename Sentence1>
    CachedPartialTokenSortRatio(const Sentence1& s1)
        : CachedPartialTokenSortRatio(detail::to_begin(s1), detail::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1_sorted;
    CachedPartialRatio<CharT1> cached_partial_ratio;
};

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename Sentence1>
CachedPartialTokenSortRatio(const Sentence1& s1) -> CachedPartialTokenSortRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedPartialTokenSortRatio(InputIt1 first1, InputIt1 last1)
    -> CachedPartialTokenSortRatio<iter_value_t<InputIt1>>;
#endif

/**
 * @brief Compares the words in the strings based on unique and common words
 * between them using fuzz::ratio
 *
 * @details
 * @code{.cpp}
 * // score1 is 83.87
 * double score1 = token_sort_ratio("fuzzy was a bear", "fuzzy fuzzy was a
 * bear")
 * // score2 is 100
 * double score2 = token_set_ratio("fuzzy was a bear", "fuzzy fuzzy was a bear")
 * @endcode
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename InputIt1, typename InputIt2>
double token_set_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                       double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double token_set_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// TODO documentation
template <typename CharT1>
struct CachedTokenSetRatio {
    template <typename InputIt1>
    CachedTokenSetRatio(InputIt1 first1, InputIt1 last1)
        : s1(first1, last1), tokens_s1(detail::sorted_split(std::begin(s1), std::end(s1)))
    {}

    template <typename Sentence1>
    CachedTokenSetRatio(const Sentence1& s1_)
        : CachedTokenSetRatio(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    SplittedSentenceView<typename std::basic_string<CharT1>::iterator> tokens_s1;
};

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename Sentence1>
CachedTokenSetRatio(const Sentence1& s1) -> CachedTokenSetRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedTokenSetRatio(InputIt1 first1, InputIt1 last1) -> CachedTokenSetRatio<iter_value_t<InputIt1>>;
#endif

/**
 * @brief Compares the words in the strings based on unique and common words
 * between them using fuzz::partial_ratio
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename InputIt1, typename InputIt2>
double partial_token_set_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                               double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double partial_token_set_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// TODO documentation
template <typename CharT1>
struct CachedPartialTokenSetRatio {
    template <typename InputIt1>
    CachedPartialTokenSetRatio(InputIt1 first1, InputIt1 last1)
        : s1(first1, last1), tokens_s1(detail::sorted_split(std::begin(s1), std::end(s1)))
    {}

    template <typename Sentence1>
    CachedPartialTokenSetRatio(const Sentence1& s1_)
        : CachedPartialTokenSetRatio(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    SplittedSentenceView<typename std::basic_string<CharT1>::iterator> tokens_s1;
};

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename Sentence1>
CachedPartialTokenSetRatio(const Sentence1& s1) -> CachedPartialTokenSetRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedPartialTokenSetRatio(InputIt1 first1, InputIt1 last1)
    -> CachedPartialTokenSetRatio<iter_value_t<InputIt1>>;
#endif

/**
 * @brief Helper method that returns the maximum of fuzz::token_set_ratio and
 * fuzz::token_sort_ratio (faster than manually executing the two functions)
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename InputIt1, typename InputIt2>
double token_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double token_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// todo add real implementation
template <typename CharT1>
struct CachedTokenRatio {
    template <typename InputIt1>
    CachedTokenRatio(InputIt1 first1, InputIt1 last1)
        : s1(first1, last1),
          s1_tokens(detail::sorted_split(std::begin(s1), std::end(s1))),
          s1_sorted(s1_tokens.join()),
          cached_ratio_s1_sorted(s1_sorted)
    {}

    template <typename Sentence1>
    CachedTokenRatio(const Sentence1& s1_) : CachedTokenRatio(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    SplittedSentenceView<typename std::basic_string<CharT1>::iterator> s1_tokens;
    std::basic_string<CharT1> s1_sorted;
    CachedRatio<CharT1> cached_ratio_s1_sorted;
};

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename Sentence1>
CachedTokenRatio(const Sentence1& s1) -> CachedTokenRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedTokenRatio(InputIt1 first1, InputIt1 last1) -> CachedTokenRatio<iter_value_t<InputIt1>>;
#endif

/**
 * @brief Helper method that returns the maximum of
 * fuzz::partial_token_set_ratio and fuzz::partial_token_sort_ratio (faster than
 * manually executing the two functions)
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename InputIt1, typename InputIt2>
double partial_token_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                           double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double partial_token_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// todo add real implementation
template <typename CharT1>
struct CachedPartialTokenRatio {
    template <typename InputIt1>
    CachedPartialTokenRatio(InputIt1 first1, InputIt1 last1)
        : s1(first1, last1),
          tokens_s1(detail::sorted_split(std::begin(s1), std::end(s1))),
          s1_sorted(tokens_s1.join())
    {}

    template <typename Sentence1>
    CachedPartialTokenRatio(const Sentence1& s1_)
        : CachedPartialTokenRatio(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    SplittedSentenceView<typename std::basic_string<CharT1>::iterator> tokens_s1;
    std::basic_string<CharT1> s1_sorted;
};

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename Sentence1>
CachedPartialTokenRatio(const Sentence1& s1) -> CachedPartialTokenRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedPartialTokenRatio(InputIt1 first1, InputIt1 last1) -> CachedPartialTokenRatio<iter_value_t<InputIt1>>;
#endif

/**
 * @brief Calculates a weighted ratio based on the other ratio algorithms
 *
 * @details
 * @todo add a detailed description
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename InputIt1, typename InputIt2>
double WRatio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double WRatio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// todo add real implementation
template <typename CharT1>
struct CachedWRatio {
    template <typename InputIt1>
    CachedWRatio(InputIt1 first1, InputIt1 last1);

    template <typename Sentence1>
    CachedWRatio(const Sentence1& s1_) : CachedWRatio(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    // todo somehow implement this using other ratios with creating PatternMatchVector
    // multiple times
    std::basic_string<CharT1> s1;
    CachedPartialRatio<CharT1> cached_partial_ratio;
    SplittedSentenceView<typename std::basic_string<CharT1>::iterator> tokens_s1;
    std::basic_string<CharT1> s1_sorted;
    rapidfuzz::detail::BlockPatternMatchVector blockmap_s1_sorted;
};

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename Sentence1>
CachedWRatio(const Sentence1& s1) -> CachedWRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedWRatio(InputIt1 first1, InputIt1 last1) -> CachedWRatio<iter_value_t<InputIt1>>;
#endif

/**
 * @brief Calculates a quick ratio between two strings using fuzz.ratio
 *
 * @details
 * @todo add a detailed description
 *
 * @tparam Sentence1 This is a string that can be converted to
 * basic_string_view<char_type>
 * @tparam Sentence2 This is a string that can be converted to
 * basic_string_view<char_type>
 *
 * @param s1 string to compare with s2 (for type info check Template parameters
 * above)
 * @param s2 string to compare with s1 (for type info check Template parameters
 * above)
 * @param score_cutoff Optional argument for a score threshold between 0% and
 * 100%. Matches with a lower score than this number will not be returned.
 * Defaults to 0.
 *
 * @return returns the ratio between s1 and s2 or 0 when ratio < score_cutoff
 */
template <typename InputIt1, typename InputIt2>
double QRatio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double QRatio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

template <typename CharT1>
struct CachedQRatio {
    template <typename InputIt1>
    CachedQRatio(InputIt1 first1, InputIt1 last1) : s1(first1, last1), cached_ratio(first1, last1)
    {}

    template <typename Sentence1>
    CachedQRatio(const Sentence1& s1_) : CachedQRatio(detail::to_begin(s1_), detail::to_end(s1_))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    CachedRatio<CharT1> cached_ratio;
};

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
template <typename Sentence1>
CachedQRatio(const Sentence1& s1) -> CachedQRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedQRatio(InputIt1 first1, InputIt1 last1) -> CachedQRatio<iter_value_t<InputIt1>>;
#endif

/**@}*/

} // namespace fuzz
} // namespace rapidfuzz

#include <limits>

#include <algorithm>
#include <cmath>
#include <iterator>
#include <unordered_map>
#include <vector>

namespace rapidfuzz {
namespace fuzz {

/**********************************************
 *                  ratio
 *********************************************/

template <typename InputIt1, typename InputIt2>
double ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, double score_cutoff)
{
    return ratio(detail::make_range(first1, last1), detail::make_range(first2, last2), score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double ratio(const Sentence1& s1, const Sentence2& s2, const double score_cutoff)
{
    return indel_normalized_similarity(s1, s2, score_cutoff / 100) * 100;
}

template <typename CharT1>
template <typename InputIt2>
double CachedRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2, double score_cutoff) const
{
    return similarity(detail::make_range(first2, last2), score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return cached_indel.normalized_similarity(s2, score_cutoff / 100) * 100;
}

/**********************************************
 *              partial_ratio
 *********************************************/

namespace fuzz_detail {

template <typename InputIt1, typename InputIt2, typename CachedCharT1>
ScoreAlignment<double>
partial_ratio_short_needle(rapidfuzz::detail::Range<InputIt1> s1, rapidfuzz::detail::Range<InputIt2> s2,
                           const CachedRatio<CachedCharT1>& cached_ratio,
                           const detail::CharSet<iter_value_t<InputIt1>>& s1_char_set, double score_cutoff)
{
    ScoreAlignment<double> res;
    auto len1 = static_cast<size_t>(s1.size());
    auto len2 = static_cast<size_t>(s2.size());
    assert(len2 >= len1);
    res.src_start = 0;
    res.src_end = len1;
    res.dest_start = 0;
    res.dest_end = len1;

    if (len2 > len1) {
        int64_t maximum = static_cast<int64_t>(len1) * 2;
        double norm_cutoff_sim = rapidfuzz::detail::NormSim_to_NormDist(score_cutoff / 100);
        int64_t cutoff_dist = static_cast<int64_t>(std::ceil(static_cast<double>(maximum) * norm_cutoff_sim));
        int64_t best_dist = std::numeric_limits<int64_t>::max();
        std::vector<int64_t> scores(len2 - len1, -1);
        std::vector<std::pair<size_t, size_t>> windows = {{0, len2 - len1 - 1}};
        std::vector<std::pair<size_t, size_t>> new_windows;

        while (!windows.empty()) {
            for (const auto& window : windows) {
                size_t cell_diff = window.second - window.first;
                if (cell_diff == 1) continue;

                auto subseq1 = s2.subseq(static_cast<ptrdiff_t>(window.first), static_cast<ptrdiff_t>(len1));
                auto subseq2 = s2.subseq(static_cast<ptrdiff_t>(window.second), static_cast<ptrdiff_t>(len1));
                if (scores[window.first] == -1) {
                    scores[window.first] = cached_ratio.cached_indel.distance(subseq1);
                    if (scores[window.first] < cutoff_dist) {
                        cutoff_dist = best_dist = scores[window.first];
                        res.dest_start = window.first;
                        res.dest_end = window.first + len1;
                        if (best_dist == 0) {
                            new_windows.clear();
                            break;
                        }
                    }
                }
                if (scores[window.second] == -1) {
                    scores[window.second] = cached_ratio.cached_indel.distance(subseq2);
                    if (scores[window.second] < cutoff_dist) {
                        cutoff_dist = best_dist = scores[window.second];
                        res.dest_start = window.second;
                        res.dest_end = window.second + len1;
                        if (best_dist == 0) {
                            new_windows.clear();
                            break;
                        }
                    }
                }

                /* find the minimum score possible in the range first <-> last */
                int64_t known_edits = std::abs(scores[window.first] - scores[window.second]);
                /* half of the cells that are not needed for known_edits can lead to a better score */
                int64_t min_score = std::min(scores[window.first], scores[window.second]) -
                                    (static_cast<int64_t>(cell_diff) + known_edits / 2);
                if (min_score < cutoff_dist) {
                    size_t center = cell_diff / 2;
                    new_windows.emplace_back(window.first, window.first + center);
                    new_windows.emplace_back(window.first + center, window.second);
                }
            }

            std::swap(windows, new_windows);
            new_windows.clear();
        }

        double score = 1.0 - (static_cast<double>(best_dist) / static_cast<double>(maximum));
        score *= 100;
        if (score >= score_cutoff) score_cutoff = res.score = score;
    }

    for (size_t i = 1; i < len1; ++i) {
        auto subseq = s2.subseq(0, static_cast<ptrdiff_t>(i));
        if (!s1_char_set.find(subseq.back())) continue;

        double ls_ratio = cached_ratio.similarity(subseq, score_cutoff);
        if (ls_ratio > res.score) {
            score_cutoff = res.score = ls_ratio;
            res.dest_start = 0;
            res.dest_end = i;
            if (res.score == 100.0) return res;
        }
    }

    for (size_t i = len2 - len1; i < len2; ++i) {
        auto subseq = s2.subseq(static_cast<ptrdiff_t>(i), static_cast<ptrdiff_t>(len1));
        if (!s1_char_set.find(subseq.front())) continue;

        double ls_ratio = cached_ratio.similarity(subseq, score_cutoff);
        if (ls_ratio > res.score) {
            score_cutoff = res.score = ls_ratio;
            res.dest_start = i;
            res.dest_end = len2;
            if (res.score == 100.0) return res;
        }
    }

    return res;
}

template <typename InputIt1, typename InputIt2, typename CharT1 = iter_value_t<InputIt1>>
ScoreAlignment<double> partial_ratio_short_needle(rapidfuzz::detail::Range<InputIt1> s1,
                                                  rapidfuzz::detail::Range<InputIt2> s2, double score_cutoff)
{
    CachedRatio<CharT1> cached_ratio(s1);

    detail::CharSet<CharT1> s1_char_set;
    for (auto ch : s1)
        s1_char_set.insert(ch);

    return partial_ratio_short_needle(s1, s2, cached_ratio, s1_char_set, score_cutoff);
}

} // namespace fuzz_detail

template <typename InputIt1, typename InputIt2>
ScoreAlignment<double> partial_ratio_alignment(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                               InputIt2 last2, double score_cutoff)
{
    auto len1 = static_cast<size_t>(std::distance(first1, last1));
    auto len2 = static_cast<size_t>(std::distance(first2, last2));

    if (len1 > len2) {
        ScoreAlignment<double> result = partial_ratio_alignment(first2, last2, first1, last1, score_cutoff);
        std::swap(result.src_start, result.dest_start);
        std::swap(result.src_end, result.dest_end);
        return result;
    }

    if (score_cutoff > 100) return ScoreAlignment<double>(0, 0, len1, 0, len1);

    if (!len1 || !len2)
        return ScoreAlignment<double>(static_cast<double>(len1 == len2) * 100.0, 0, len1, 0, len1);

    return fuzz_detail::partial_ratio_short_needle(detail::make_range(first1, last1),
                                                   detail::make_range(first2, last2), score_cutoff);
}

template <typename Sentence1, typename Sentence2>
ScoreAlignment<double> partial_ratio_alignment(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return partial_ratio_alignment(detail::to_begin(s1), detail::to_end(s1), detail::to_begin(s2),
                                   detail::to_end(s2), score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double partial_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, double score_cutoff)
{
    return partial_ratio_alignment(first1, last1, first2, last2, score_cutoff).score;
}

template <typename Sentence1, typename Sentence2>
double partial_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return partial_ratio_alignment(s1, s2, score_cutoff).score;
}

template <typename CharT1>
template <typename InputIt1>
CachedPartialRatio<CharT1>::CachedPartialRatio(InputIt1 first1, InputIt1 last1)
    : s1(first1, last1), cached_ratio(first1, last1)
{
    for (const auto& ch : s1)
        s1_char_set.insert(ch);
}

template <typename CharT1>
template <typename InputIt2>
double CachedPartialRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2, double score_cutoff) const
{
    size_t len1 = s1.size();
    size_t len2 = static_cast<size_t>(std::distance(first2, last2));

    if (len1 > len2)
        return partial_ratio(detail::to_begin(s1), detail::to_end(s1), first2, last2, score_cutoff);

    if (score_cutoff > 100) return 0;

    if (!len1 || !len2) return static_cast<double>(len1 == len2) * 100.0;

    return fuzz_detail::partial_ratio_short_needle(detail::make_range(s1), detail::make_range(first2, last2),
                                                   cached_ratio, s1_char_set, score_cutoff)
        .score;
}

template <typename CharT1>
template <typename Sentence2>
double CachedPartialRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(detail::to_begin(s2), detail::to_end(s2), score_cutoff);
}

/**********************************************
 *             token_sort_ratio
 *********************************************/
template <typename InputIt1, typename InputIt2>
double token_sort_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    return ratio(detail::sorted_split(first1, last1).join(), detail::sorted_split(first2, last2).join(),
                 score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double token_sort_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return token_sort_ratio(detail::to_begin(s1), detail::to_end(s1), detail::to_begin(s2),
                            detail::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedTokenSortRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2, double score_cutoff) const
{
    if (score_cutoff > 100) return 0;

    return cached_ratio.similarity(detail::sorted_split(first2, last2).join(), score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedTokenSortRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(detail::to_begin(s2), detail::to_end(s2), score_cutoff);
}

/**********************************************
 *          partial_token_sort_ratio
 *********************************************/

template <typename InputIt1, typename InputIt2>
double partial_token_sort_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    return partial_ratio(detail::sorted_split(first1, last1).join(),
                         detail::sorted_split(first2, last2).join(), score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double partial_token_sort_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return partial_token_sort_ratio(detail::to_begin(s1), detail::to_end(s1), detail::to_begin(s2),
                                    detail::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedPartialTokenSortRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2,
                                                       double score_cutoff) const
{
    if (score_cutoff > 100) return 0;

    return cached_partial_ratio.similarity(detail::sorted_split(first2, last2).join(), score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedPartialTokenSortRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(detail::to_begin(s2), detail::to_end(s2), score_cutoff);
}

/**********************************************
 *               token_set_ratio
 *********************************************/

namespace fuzz_detail {
template <typename InputIt1, typename InputIt2>
double token_set_ratio(const SplittedSentenceView<InputIt1>& tokens_a,
                       const SplittedSentenceView<InputIt2>& tokens_b, const double score_cutoff)
{
    /* in FuzzyWuzzy this returns 0. For sake of compatibility return 0 here as well
     * see https://github.com/maxbachmann/RapidFuzz/issues/110 */
    if (tokens_a.empty() || tokens_b.empty()) return 0;

    auto decomposition = detail::set_decomposition(tokens_a, tokens_b);
    auto intersect = decomposition.intersection;
    auto diff_ab = decomposition.difference_ab;
    auto diff_ba = decomposition.difference_ba;

    // one sentence is part of the other one
    if (!intersect.empty() && (diff_ab.empty() || diff_ba.empty())) return 100;

    auto diff_ab_joined = diff_ab.join();
    auto diff_ba_joined = diff_ba.join();

    size_t ab_len = diff_ab_joined.length();
    size_t ba_len = diff_ba_joined.length();
    size_t sect_len = intersect.length();

    // string length sect+ab <-> sect and sect+ba <-> sect
    int64_t sect_ab_len = static_cast<int64_t>(sect_len + bool(sect_len) + ab_len);
    int64_t sect_ba_len = static_cast<int64_t>(sect_len + bool(sect_len) + ba_len);

    double result = 0;
    auto cutoff_distance = detail::score_cutoff_to_distance<100>(score_cutoff, sect_ab_len + sect_ba_len);
    int64_t dist = indel_distance(diff_ab_joined, diff_ba_joined, cutoff_distance);

    if (dist <= cutoff_distance)
        result = detail::norm_distance<100>(dist, sect_ab_len + sect_ba_len, score_cutoff);

    // exit early since the other ratios are 0
    if (!sect_len) return result;

    // levenshtein distance sect+ab <-> sect and sect+ba <-> sect
    // since only sect is similar in them the distance can be calculated based on
    // the length difference
    int64_t sect_ab_dist = static_cast<int64_t>(bool(sect_len) + ab_len);
    double sect_ab_ratio =
        detail::norm_distance<100>(sect_ab_dist, static_cast<int64_t>(sect_len) + sect_ab_len, score_cutoff);

    int64_t sect_ba_dist = static_cast<int64_t>(bool(sect_len) + ba_len);
    double sect_ba_ratio =
        detail::norm_distance<100>(sect_ba_dist, static_cast<int64_t>(sect_len) + sect_ba_len, score_cutoff);

    return std::max({result, sect_ab_ratio, sect_ba_ratio});
}
} // namespace fuzz_detail

template <typename InputIt1, typename InputIt2>
double token_set_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    return fuzz_detail::token_set_ratio(detail::sorted_split(first1, last1),
                                        detail::sorted_split(first2, last2), score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double token_set_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return token_set_ratio(detail::to_begin(s1), detail::to_end(s1), detail::to_begin(s2), detail::to_end(s2),
                           score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedTokenSetRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2, double score_cutoff) const
{
    if (score_cutoff > 100) return 0;

    return fuzz_detail::token_set_ratio(tokens_s1, detail::sorted_split(first2, last2), score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedTokenSetRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(detail::to_begin(s2), detail::to_end(s2), score_cutoff);
}

/**********************************************
 *          partial_token_set_ratio
 *********************************************/

namespace fuzz_detail {
template <typename InputIt1, typename InputIt2>
double partial_token_set_ratio(const SplittedSentenceView<InputIt1>& tokens_a,
                               const SplittedSentenceView<InputIt2>& tokens_b, const double score_cutoff)
{
    /* in FuzzyWuzzy this returns 0. For sake of compatibility return 0 here as well
     * see https://github.com/maxbachmann/RapidFuzz/issues/110 */
    if (tokens_a.empty() || tokens_b.empty()) return 0;

    auto decomposition = detail::set_decomposition(tokens_a, tokens_b);

    // exit early when there is a common word in both sequences
    if (!decomposition.intersection.empty()) return 100;

    return partial_ratio(decomposition.difference_ab.join(), decomposition.difference_ba.join(),
                         score_cutoff);
}
} // namespace fuzz_detail

template <typename InputIt1, typename InputIt2>
double partial_token_set_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                               double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    return fuzz_detail::partial_token_set_ratio(detail::sorted_split(first1, last1),
                                                detail::sorted_split(first2, last2), score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double partial_token_set_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return partial_token_set_ratio(detail::to_begin(s1), detail::to_end(s1), detail::to_begin(s2),
                                   detail::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedPartialTokenSetRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2,
                                                      double score_cutoff) const
{
    if (score_cutoff > 100) return 0;

    return fuzz_detail::partial_token_set_ratio(tokens_s1, detail::sorted_split(first2, last2), score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedPartialTokenSetRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(detail::to_begin(s2), detail::to_end(s2), score_cutoff);
}

/**********************************************
 *                token_ratio
 *********************************************/

template <typename InputIt1, typename InputIt2>
double token_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    auto tokens_a = detail::sorted_split(first1, last1);
    auto tokens_b = detail::sorted_split(first2, last2);

    auto decomposition = detail::set_decomposition(tokens_a, tokens_b);
    auto intersect = decomposition.intersection;
    auto diff_ab = decomposition.difference_ab;
    auto diff_ba = decomposition.difference_ba;

    if (!intersect.empty() && (diff_ab.empty() || diff_ba.empty())) return 100;

    auto diff_ab_joined = diff_ab.join();
    auto diff_ba_joined = diff_ba.join();

    size_t ab_len = diff_ab_joined.length();
    size_t ba_len = diff_ba_joined.length();
    size_t sect_len = intersect.length();

    double result = ratio(tokens_a.join(), tokens_b.join(), score_cutoff);

    // string length sect+ab <-> sect and sect+ba <-> sect
    int64_t sect_ab_len = static_cast<int64_t>(sect_len + bool(sect_len) + ab_len);
    int64_t sect_ba_len = static_cast<int64_t>(sect_len + bool(sect_len) + ba_len);

    auto cutoff_distance = detail::score_cutoff_to_distance<100>(score_cutoff, sect_ab_len + sect_ba_len);
    int64_t dist = indel_distance(diff_ab_joined, diff_ba_joined, cutoff_distance);
    if (dist <= cutoff_distance)
        result = std::max(result, detail::norm_distance<100>(dist, sect_ab_len + sect_ba_len, score_cutoff));

    // exit early since the other ratios are 0
    if (!sect_len) return result;

    // levenshtein distance sect+ab <-> sect and sect+ba <-> sect
    // since only sect is similar in them the distance can be calculated based on
    // the length difference
    int64_t sect_ab_dist = static_cast<int64_t>(bool(sect_len) + ab_len);
    double sect_ab_ratio =
        detail::norm_distance<100>(sect_ab_dist, static_cast<int64_t>(sect_len) + sect_ab_len, score_cutoff);

    int64_t sect_ba_dist = static_cast<int64_t>(bool(sect_len) + ba_len);
    double sect_ba_ratio =
        detail::norm_distance<100>(sect_ba_dist, static_cast<int64_t>(sect_len) + sect_ba_len, score_cutoff);

    return std::max({result, sect_ab_ratio, sect_ba_ratio});
}

template <typename Sentence1, typename Sentence2>
double token_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return token_ratio(detail::to_begin(s1), detail::to_end(s1), detail::to_begin(s2), detail::to_end(s2),
                       score_cutoff);
}

namespace fuzz_detail {
template <typename CharT1, typename CachedCharT1, typename InputIt2>
double token_ratio(const SplittedSentenceView<CharT1>& s1_tokens,
                   const CachedRatio<CachedCharT1>& cached_ratio_s1_sorted, InputIt2 first2, InputIt2 last2,
                   double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    auto s2_tokens = detail::sorted_split(first2, last2);

    auto decomposition = detail::set_decomposition(s1_tokens, s2_tokens);
    auto intersect = decomposition.intersection;
    auto diff_ab = decomposition.difference_ab;
    auto diff_ba = decomposition.difference_ba;

    if (!intersect.empty() && (diff_ab.empty() || diff_ba.empty())) return 100;

    auto diff_ab_joined = diff_ab.join();
    auto diff_ba_joined = diff_ba.join();

    int64_t ab_len = diff_ab_joined.length();
    int64_t ba_len = diff_ba_joined.length();
    int64_t sect_len = intersect.length();

    double result = cached_ratio_s1_sorted.similarity(s2_tokens.join(), score_cutoff);

    // string length sect+ab <-> sect and sect+ba <-> sect
    int64_t sect_ab_len = sect_len + bool(sect_len) + ab_len;
    int64_t sect_ba_len = sect_len + bool(sect_len) + ba_len;

    auto cutoff_distance = detail::score_cutoff_to_distance<100>(score_cutoff, sect_ab_len + sect_ba_len);
    int64_t dist = indel_distance(diff_ab_joined, diff_ba_joined, cutoff_distance);
    if (dist <= cutoff_distance)
        result = std::max(result, detail::norm_distance<100>(dist, sect_ab_len + sect_ba_len, score_cutoff));

    // exit early since the other ratios are 0
    if (!sect_len) return result;

    // levenshtein distance sect+ab <-> sect and sect+ba <-> sect
    // since only sect is similar in them the distance can be calculated based on
    // the length difference
    int64_t sect_ab_dist = bool(sect_len) + ab_len;
    double sect_ab_ratio = detail::norm_distance<100>(sect_ab_dist, sect_len + sect_ab_len, score_cutoff);

    int64_t sect_ba_dist = bool(sect_len) + ba_len;
    double sect_ba_ratio = detail::norm_distance<100>(sect_ba_dist, sect_len + sect_ba_len, score_cutoff);

    return std::max({result, sect_ab_ratio, sect_ba_ratio});
}

// todo this is a temporary solution until WRatio is properly implemented using other scorers
template <typename CharT1, typename InputIt1, typename InputIt2>
double token_ratio(const std::basic_string<CharT1>& s1_sorted,
                   const SplittedSentenceView<InputIt1>& tokens_s1,
                   const detail::BlockPatternMatchVector& blockmap_s1_sorted, InputIt2 first2, InputIt2 last2,
                   double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    auto tokens_b = detail::sorted_split(first2, last2);

    auto decomposition = detail::set_decomposition(tokens_s1, tokens_b);
    auto intersect = decomposition.intersection;
    auto diff_ab = decomposition.difference_ab;
    auto diff_ba = decomposition.difference_ba;

    if (!intersect.empty() && (diff_ab.empty() || diff_ba.empty())) return 100;

    auto diff_ab_joined = diff_ab.join();
    auto diff_ba_joined = diff_ba.join();

    int64_t ab_len = diff_ab_joined.length();
    int64_t ba_len = diff_ba_joined.length();
    int64_t sect_len = intersect.length();

    double result = 0;
    auto s2_sorted = tokens_b.join();
    if (s1_sorted.size() < 65) {
        double norm_sim =
            detail::indel_normalized_similarity(blockmap_s1_sorted, detail::make_range(s1_sorted),
                                                detail::make_range(s2_sorted), score_cutoff / 100);
        result = norm_sim * 100;
    }
    else {
        result = fuzz::ratio(s1_sorted, s2_sorted, score_cutoff);
    }

    // string length sect+ab <-> sect and sect+ba <-> sect
    int64_t sect_ab_len = sect_len + bool(sect_len) + ab_len;
    int64_t sect_ba_len = sect_len + bool(sect_len) + ba_len;

    auto cutoff_distance = detail::score_cutoff_to_distance<100>(score_cutoff, sect_ab_len + sect_ba_len);
    int64_t dist = indel_distance(diff_ab_joined, diff_ba_joined, cutoff_distance);
    if (dist <= cutoff_distance)
        result = std::max(result, detail::norm_distance<100>(dist, sect_ab_len + sect_ba_len, score_cutoff));

    // exit early since the other ratios are 0
    if (!sect_len) return result;

    // levenshtein distance sect+ab <-> sect and sect+ba <-> sect
    // since only sect is similar in them the distance can be calculated based on
    // the length difference
    int64_t sect_ab_dist = bool(sect_len) + ab_len;
    double sect_ab_ratio = detail::norm_distance<100>(sect_ab_dist, sect_len + sect_ab_len, score_cutoff);

    int64_t sect_ba_dist = bool(sect_len) + ba_len;
    double sect_ba_ratio = detail::norm_distance<100>(sect_ba_dist, sect_len + sect_ba_len, score_cutoff);

    return std::max({result, sect_ab_ratio, sect_ba_ratio});
}
} // namespace fuzz_detail

template <typename CharT1>
template <typename InputIt2>
double CachedTokenRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2, double score_cutoff) const
{
    return fuzz_detail::token_ratio(s1_tokens, cached_ratio_s1_sorted, first2, last2, score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedTokenRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(detail::to_begin(s2), detail::to_end(s2), score_cutoff);
}

/**********************************************
 *            partial_token_ratio
 *********************************************/

template <typename InputIt1, typename InputIt2>
double partial_token_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                           double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    auto tokens_a = detail::sorted_split(first1, last1);
    auto tokens_b = detail::sorted_split(first2, last2);

    auto decomposition = detail::set_decomposition(tokens_a, tokens_b);

    // exit early when there is a common word in both sequences
    if (!decomposition.intersection.empty()) return 100;

    auto diff_ab = decomposition.difference_ab;
    auto diff_ba = decomposition.difference_ba;

    double result = partial_ratio(tokens_a.join(), tokens_b.join(), score_cutoff);

    // do not calculate the same partial_ratio twice
    if (tokens_a.word_count() == diff_ab.word_count() && tokens_b.word_count() == diff_ba.word_count()) {
        return result;
    }

    score_cutoff = std::max(score_cutoff, result);
    return std::max(result, partial_ratio(diff_ab.join(), diff_ba.join(), score_cutoff));
}

template <typename Sentence1, typename Sentence2>
double partial_token_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return partial_token_ratio(detail::to_begin(s1), detail::to_end(s1), detail::to_begin(s2),
                               detail::to_end(s2), score_cutoff);
}

namespace fuzz_detail {
template <typename CharT1, typename InputIt1, typename InputIt2>
double partial_token_ratio(const std::basic_string<CharT1>& s1_sorted,
                           const SplittedSentenceView<InputIt1>& tokens_s1, InputIt2 first2, InputIt2 last2,
                           double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    auto tokens_b = detail::sorted_split(first2, last2);

    auto decomposition = detail::set_decomposition(tokens_s1, tokens_b);

    // exit early when there is a common word in both sequences
    if (!decomposition.intersection.empty()) return 100;

    auto diff_ab = decomposition.difference_ab;
    auto diff_ba = decomposition.difference_ba;

    double result = partial_ratio(s1_sorted, tokens_b.join(), score_cutoff);

    // do not calculate the same partial_ratio twice
    if (tokens_s1.word_count() == diff_ab.word_count() && tokens_b.word_count() == diff_ba.word_count()) {
        return result;
    }

    score_cutoff = std::max(score_cutoff, result);
    return std::max(result, partial_ratio(diff_ab.join(), diff_ba.join(), score_cutoff));
}

} // namespace fuzz_detail

template <typename CharT1>
template <typename InputIt2>
double CachedPartialTokenRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2, double score_cutoff) const
{
    return fuzz_detail::partial_token_ratio(s1_sorted, tokens_s1, first2, last2, score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedPartialTokenRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(detail::to_begin(s2), detail::to_end(s2), score_cutoff);
}

/**********************************************
 *                  WRatio
 *********************************************/

template <typename InputIt1, typename InputIt2>
double WRatio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    constexpr double UNBASE_SCALE = 0.95;

    auto len1 = std::distance(first1, last1);
    auto len2 = std::distance(first2, last2);

    /* in FuzzyWuzzy this returns 0. For sake of compatibility return 0 here as well
     * see https://github.com/maxbachmann/RapidFuzz/issues/110 */
    if (!len1 || !len2) return 0;

    double len_ratio = (len1 > len2) ? static_cast<double>(len1) / static_cast<double>(len2)
                                     : static_cast<double>(len2) / static_cast<double>(len1);

    double end_ratio = ratio(first1, last1, first2, last2, score_cutoff);

    if (len_ratio < 1.5) {
        score_cutoff = std::max(score_cutoff, end_ratio) / UNBASE_SCALE;
        return std::max(end_ratio, token_ratio(first1, last1, first2, last2, score_cutoff) * UNBASE_SCALE);
    }

    const double PARTIAL_SCALE = (len_ratio < 8.0) ? 0.9 : 0.6;

    score_cutoff = std::max(score_cutoff, end_ratio) / PARTIAL_SCALE;
    end_ratio =
        std::max(end_ratio, partial_ratio(first1, last1, first2, last2, score_cutoff) * PARTIAL_SCALE);

    score_cutoff = std::max(score_cutoff, end_ratio) / UNBASE_SCALE;
    return std::max(end_ratio, partial_token_ratio(first1, last1, first2, last2, score_cutoff) *
                                   UNBASE_SCALE * PARTIAL_SCALE);
}

template <typename Sentence1, typename Sentence2>
double WRatio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return WRatio(detail::to_begin(s1), detail::to_end(s1), detail::to_begin(s2), detail::to_end(s2),
                  score_cutoff);
}

template <typename Sentence1>
template <typename InputIt1>
CachedWRatio<Sentence1>::CachedWRatio(InputIt1 first1, InputIt1 last1)
    : s1(first1, last1),
      cached_partial_ratio(first1, last1),
      tokens_s1(detail::sorted_split(std::begin(s1), std::end(s1))),
      s1_sorted(tokens_s1.join()),
      blockmap_s1_sorted(detail::make_range(s1_sorted))
{}

template <typename CharT1>
template <typename InputIt2>
double CachedWRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2, double score_cutoff) const
{
    if (score_cutoff > 100) return 0;

    constexpr double UNBASE_SCALE = 0.95;

    ptrdiff_t len1 = s1.size();
    ptrdiff_t len2 = std::distance(first2, last2);

    /* in FuzzyWuzzy this returns 0. For sake of compatibility return 0 here as well
     * see https://github.com/maxbachmann/RapidFuzz/issues/110 */
    if (!len1 || !len2) return 0;

    double len_ratio = (len1 > len2) ? static_cast<double>(len1) / static_cast<double>(len2)
                                     : static_cast<double>(len2) / static_cast<double>(len1);

    double end_ratio = cached_partial_ratio.cached_ratio.similarity(first2, last2, score_cutoff);

    if (len_ratio < 1.5) {
        score_cutoff = std::max(score_cutoff, end_ratio) / UNBASE_SCALE;
        // use pre calculated values
        auto r =
            fuzz_detail::token_ratio(s1_sorted, tokens_s1, blockmap_s1_sorted, first2, last2, score_cutoff);
        return std::max(end_ratio, r * UNBASE_SCALE);
    }

    const double PARTIAL_SCALE = (len_ratio < 8.0) ? 0.9 : 0.6;

    score_cutoff = std::max(score_cutoff, end_ratio) / PARTIAL_SCALE;
    end_ratio =
        std::max(end_ratio, cached_partial_ratio.similarity(first2, last2, score_cutoff) * PARTIAL_SCALE);

    score_cutoff = std::max(score_cutoff, end_ratio) / UNBASE_SCALE;
    auto r = fuzz_detail::partial_token_ratio(s1_sorted, tokens_s1, first2, last2, score_cutoff);
    return std::max(end_ratio, r * UNBASE_SCALE * PARTIAL_SCALE);
}

template <typename CharT1>
template <typename Sentence2>
double CachedWRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(detail::to_begin(s2), detail::to_end(s2), score_cutoff);
}

/**********************************************
 *                QRatio
 *********************************************/

template <typename InputIt1, typename InputIt2>
double QRatio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, double score_cutoff)
{
    auto len1 = std::distance(first1, last1);
    auto len2 = std::distance(first2, last2);

    /* in FuzzyWuzzy this returns 0. For sake of compatibility return 0 here as well
     * see https://github.com/maxbachmann/RapidFuzz/issues/110 */
    if (!len1 || !len2) return 0;

    return ratio(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double QRatio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return QRatio(detail::to_begin(s1), detail::to_end(s1), detail::to_begin(s2), detail::to_end(s2),
                  score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedQRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2, double score_cutoff) const
{
    auto len2 = std::distance(first2, last2);

    /* in FuzzyWuzzy this returns 0. For sake of compatibility return 0 here as well
     * see https://github.com/maxbachmann/RapidFuzz/issues/110 */
    if (s1.empty() || !len2) return 0;

    return cached_ratio.similarity(first2, last2, score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedQRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(detail::to_begin(s2), detail::to_end(s2), score_cutoff);
}

} // namespace fuzz
} // namespace rapidfuzz

#endif // RAPIDFUZZ_AMALGAMATED_HPP_INCLUDED
