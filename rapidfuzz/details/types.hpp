/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */

#pragma once

#include <algorithm>
#include <cassert>
#include <stddef.h>
#include <stdexcept>
#include <stdint.h>
#include <type_traits>
#include <vector>

namespace rapidfuzz {

template <typename InputIt>
class IteratorView {
public:
    IteratorView(InputIt first_, InputIt last_) : first(first_), last(last_)
    {}

    InputIt first;
    InputIt last;
};

template <typename InputIt1, typename InputIt2>
inline bool operator==(const IteratorView<InputIt1>& a, const IteratorView<InputIt2>& b)
{
    return std::equal(a.first, a.last, b.first, b.last);
}

template <typename InputIt1, typename InputIt2>
inline bool operator!=(const IteratorView<InputIt1>& a, const IteratorView<InputIt2>& b)
{
    return !(a == b);
}

template <typename InputIt1, typename InputIt2>
inline bool operator<(const IteratorView<InputIt1>& a, const IteratorView<InputIt2>& b)
{
    return (std::lexicographical_compare(a.first, a.last, b.first, b.last));
}

template <typename InputIt1, typename InputIt2>
inline bool operator>(const IteratorView<InputIt1>& a, const IteratorView<InputIt2>& b)
{
    return b < a;
}

template <typename InputIt1, typename InputIt2>
inline bool operator<=(const IteratorView<InputIt1>& a, const IteratorView<InputIt2>& b)
{
    return !(b < a);
}

template <typename InputIt1, typename InputIt2>
inline bool operator>=(const IteratorView<InputIt1>& a, const IteratorView<InputIt2>& b)
{
    return !(a < b);
}

template <typename InputIt>
using IteratorViewVec = std::vector<IteratorView<InputIt>>;

struct StringAffix {
    int64_t prefix_len;
    int64_t suffix_len;
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
    EditType type;    /**< type of the edit operation */
    int64_t src_pos;  /**< index into the source string */
    int64_t dest_pos; /**< index into the destination string */

    EditOp() : type(EditType::None), src_pos(0), dest_pos(0)
    {}

    EditOp(EditType type_, int64_t src_pos_, int64_t dest_pos_)
        : type(type_), src_pos(src_pos_), dest_pos(dest_pos_)
    {}
};

inline bool operator==(EditOp a, EditOp b)
{
    return (a.type == b.type) && (a.src_pos == b.src_pos) && (a.dest_pos == b.dest_pos);
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
    EditType type;      /**< type of the edit operation */
    int64_t src_begin;  /**< index into the source string */
    int64_t src_end;    /**< index into the source string */
    int64_t dest_begin; /**< index into the destination string */
    int64_t dest_end;   /**< index into the destination string */

    Opcode() : type(EditType::None), src_begin(0), src_end(0), dest_begin(0), dest_end(0)
    {}

    Opcode(EditType type_, int64_t src_begin_, int64_t src_end_, int64_t dest_begin_,
           int64_t dest_end_)
        : type(type_),
          src_begin(src_begin_),
          src_end(src_end_),
          dest_begin(dest_begin_),
          dest_end(dest_end_)
    {}
};

inline bool operator==(Opcode a, Opcode b)
{
    return (a.type == b.type) && (a.src_begin == b.src_begin) && (a.src_end == b.src_end) &&
           (a.dest_begin == b.dest_begin) && (a.dest_end == b.dest_end);
}

namespace detail {
template <typename T>
void vector_slice(std::vector<T>& new_vec, const std::vector<T>& vec, int start, int stop, int step)
{
    if (step > 0) {
        if (start < 0) {
            start = std::max((int)(start + (int)vec.size()), 0);
        }
        else if (start > (int)vec.size()) {
            start = (int)vec.size();
        }

        if (stop < 0) {
            stop = std::max((int)(stop + (int)vec.size()), 0);
        }
        else if (stop > (int)vec.size()) {
            stop = (int)vec.size();
        }

        if (start >= stop) {
            return;
        }

        int count = (stop - 1 - start) / step + 1;
        new_vec.reserve(count);

        for (int i = start; i < stop; i += step) {
            new_vec.push_back(vec[i]);
        }
    }
    else if (step < 0) {
        if (start < 0) {
            start = std::max((int)(start + (int)vec.size()), -1);
        }
        else if (start >= (int)vec.size()) {
            start = (int)vec.size() - 1;
        }

        if (stop < 0) {
            stop = std::max((int)(stop + (int)vec.size()), -1);
        }
        else if (stop >= (int)vec.size()) {
            stop = (int)vec.size() - 1;
        }

        if (start <= stop) {
            return;
        }

        int count = (stop + 1 - start) / step + 1;
        new_vec.reserve(count);

        for (int i = start; i > stop; i += step) {
            new_vec.push_back(vec[i]);
        }
    }
    else {
        throw std::invalid_argument("slice step cannot be zero");
    }
}
} // namespace detail

class Opcodes;

class Editops : private std::vector<EditOp> {
public:
    using std::vector<EditOp>::size_type;

    Editops() : src_len(0), dest_len(0)
    {}

    Editops(size_type count, const EditOp& value)
        : std::vector<EditOp>(count, value), src_len(0), dest_len(0)
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

    Editops& operator=(Editops other)
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

    void swap(Editops& rhs) noexcept
    {
        std::swap(src_len, rhs.src_len);
        std::swap(dest_len, rhs.dest_len);
        std::vector<EditOp>::swap(rhs);
    }

    Editops slice(int start, int stop, int step = 1) const
    {
        Editops ed_slice;
        detail::vector_slice(ed_slice, *this, start, stop, step);
        ed_slice.src_len = src_len;
        ed_slice.dest_len = dest_len;
        return ed_slice;
    }

    Editops reverse() const
    {
        Editops reversed = *this;
        std::reverse(reversed.begin(), reversed.end());
        return reversed;
    }

    int64_t get_src_len() const
    {
        return src_len;
    }
    void set_src_len(int64_t len)
    {
        src_len = len;
    }
    int64_t get_dest_len() const
    {
        return dest_len;
    }
    void set_dest_len(int64_t len)
    {
        dest_len = len;
    }

    Editops inverse() const
    {
        Editops inv_ops = *this;
        std::swap(inv_ops.src_len, inv_ops.dest_len);
        for (auto& op : inv_ops) {
            std::swap(op.src_pos, op.dest_pos);
            if (op.type == EditType::Delete) {
                op.type = EditType::Insert;
            }
            else if (op.type == EditType::Insert) {
                op.type = EditType::Delete;
            }
        }
        return inv_ops;
    }

private:
    int64_t src_len;
    int64_t dest_len;
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

inline void swap(Editops& lhs, Editops& rhs)
{
    lhs.swap(rhs);
}

class Opcodes : private std::vector<Opcode> {
public:
    using std::vector<Opcode>::size_type;

    Opcodes() : src_len(0), dest_len(0)
    {}

    Opcodes(size_type count, const Opcode& value)
        : std::vector<Opcode>(count, value), src_len(0), dest_len(0)
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

    Opcodes& operator=(Opcodes other)
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

    void swap(Opcodes& rhs) noexcept
    {
        std::swap(src_len, rhs.src_len);
        std::swap(dest_len, rhs.dest_len);
        std::vector<Opcode>::swap(rhs);
    }

    Opcodes slice(int start, int stop, int step = 1) const
    {
        Opcodes ed_slice;
        detail::vector_slice(ed_slice, *this, start, stop, step);
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

    int64_t get_src_len() const
    {
        return src_len;
    }
    void set_src_len(int64_t len)
    {
        src_len = len;
    }
    int64_t get_dest_len() const
    {
        return dest_len;
    }
    void set_dest_len(int64_t len)
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
            if (op.type == EditType::Delete) {
                op.type = EditType::Insert;
            }
            else if (op.type == EditType::Insert) {
                op.type = EditType::Delete;
            }
        }
        return inv_ops;
    }

private:
    int64_t src_len;
    int64_t dest_len;
};

inline bool operator==(const Opcodes& lhs, const Opcodes& rhs)
{
    if (lhs.get_src_len() != rhs.get_src_len() || lhs.get_dest_len() != rhs.get_dest_len()) {
        return false;
    }

    if (lhs.size() != rhs.size()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

inline bool operator!=(const Opcodes& lhs, const Opcodes& rhs)
{
    return !(lhs == rhs);
}

inline void swap(Opcodes& lhs, Opcodes& rhs)
{
    lhs.swap(rhs);
}

inline Editops::Editops(const Opcodes& other)
{
    src_len = other.get_src_len();
    dest_len = other.get_dest_len();
    for (const auto& op : other) {
        switch (op.type) {
        case EditType::None:
            break;

        case EditType::Replace:
            for (int64_t j = 0; j < op.src_end - op.src_begin; j++) {
                push_back({EditType::Replace, op.src_begin + j, op.dest_begin + j});
            }
            break;

        case EditType::Insert:
            for (int64_t j = 0; j < op.dest_end - op.dest_begin; j++) {
                push_back({EditType::Insert, op.src_begin, op.dest_begin + j});
            }
            break;

        case EditType::Delete:
            for (int64_t j = 0; j < op.src_end - op.src_begin; j++) {
                push_back({EditType::Delete, op.src_begin + j, op.dest_begin});
            }
            break;
        }
    }
}

inline Opcodes::Opcodes(const Editops& other)
{
    src_len = other.get_src_len();
    dest_len = other.get_dest_len();
    int64_t src_pos = 0;
    int64_t dest_pos = 0;
    for (size_t i = 0; i < other.size();) {
        if (src_pos < other[i].src_pos || dest_pos < other[i].dest_pos) {
            push_back({EditType::None, src_pos, other[i].src_pos, dest_pos, other[i].dest_pos});
            src_pos = other[i].src_pos;
            dest_pos = other[i].dest_pos;
        }

        int64_t src_begin = src_pos;
        int64_t dest_begin = dest_pos;
        EditType type = other[i].type;
        do {
            switch (type) {
            case EditType::None:
                break;

            case EditType::Replace:
                src_pos++;
                dest_pos++;
                break;

            case EditType::Insert:
                dest_pos++;
                break;

            case EditType::Delete:
                src_pos++;
                break;
            }
            i++;
        } while (i < other.size() && other[i].type == type && src_pos && other[i].src_pos &&
                 dest_pos == other[i].dest_pos);

        push_back({type, src_begin, src_pos, dest_begin, dest_pos});
    }

    if (src_pos < other.get_src_len() || dest_pos < other.get_dest_len()) {
        push_back({EditType::None, src_pos, other.get_src_len(), dest_pos, other.get_dest_len()});
    }
}

} // namespace rapidfuzz
