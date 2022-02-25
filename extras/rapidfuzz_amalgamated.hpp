//  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
//  SPDX-License-Identifier: MIT
//  RapidFuzz v1.0.0
//  Generated: 2022-02-25 18:49:26.056051
//  ----------------------------------------------------------
//  This file is an amalgamation of multiple different files.
//  You probably shouldn't edit it directly.
//  ----------------------------------------------------------
#ifndef RAPIDFUZZ_AMALGAMATED_HPP_INCLUDED
#define RAPIDFUZZ_AMALGAMATED_HPP_INCLUDED



#include <cmath>
#include <numeric>

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
    using std::vector<EditOp>::resize;

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
    using std::vector<Opcode>::resize;

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

template <typename T>
struct ScoreAlignment {
    T score;            /**< resulting score of the algorithm */
    int64_t src_start;  /**< index into the source string */
    int64_t src_end;    /**< index into the source string */
    int64_t dest_start; /**< index into the destination string */
    int64_t dest_end;   /**< index into the destination string */

    ScoreAlignment() : score(T()), src_start(0), src_end(0), dest_start(0), dest_end(0)
    {}

    ScoreAlignment(T score_, int64_t src_start_, int64_t src_end_, int64_t dest_start_,
                   int64_t dest_end_)
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

template <typename T>
using plain = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename T>
using iterator_type = plain<decltype(*std::declval<T>())>;

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
    static constexpr auto test(int /*unused*/)
        -> decltype(f(static_cast<T>(std::declval<F>())), true)
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

#define GENERATE_HAS_MEMBER(member)                                                                \
                                                                                                   \
    template <typename T>                                                                          \
    struct has_member_##member {                                                                   \
    private:                                                                                       \
        using yes = std::true_type;                                                                \
        using no = std::false_type;                                                                \
                                                                                                   \
        struct Fallback {                                                                          \
            int member;                                                                            \
        };                                                                                         \
        struct Derived : T, Fallback {};                                                           \
                                                                                                   \
        template <class U>                                                                         \
        static no test(decltype(U::member)*);                                                      \
        template <typename U>                                                                      \
        static yes test(U*);                                                                       \
                                                                                                   \
        template <typename U, typename = std::enable_if_t<std::is_class<U>::value>>                \
        static constexpr bool class_test(U*)                                                       \
        {                                                                                          \
            return std::is_same<decltype(test<Derived>(nullptr)), yes>::value;                     \
        }                                                                                          \
                                                                                                   \
        template <typename U, typename = std::enable_if_t<!std::is_class<U>::value>>               \
        static constexpr bool class_test(const U&)                                                 \
        {                                                                                          \
            return false;                                                                          \
        }                                                                                          \
                                                                                                   \
    public:                                                                                        \
        static constexpr bool value = class_test(static_cast<T*>(nullptr));                        \
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

} // namespace rapidfuzz
#include <string>

namespace rapidfuzz {

template <typename InputIt>
class SplittedSentenceView {
public:
    using CharT = iterator_type<InputIt>;

    SplittedSentenceView(IteratorViewVec<InputIt> sentence) : m_sentence(std::move(sentence))
    {}

    int64_t dedupe();
    int64_t size() const;

    int64_t length() const
    {
        return size();
    }

    bool empty() const
    {
        return m_sentence.empty();
    }

    int64_t word_count() const
    {
        return m_sentence.size();
    }

    std::basic_string<CharT> join() const;

    const IteratorViewVec<InputIt>& words() const
    {
        return m_sentence;
    }

private:
    IteratorViewVec<InputIt> m_sentence;
};

template <typename InputIt>
int64_t SplittedSentenceView<InputIt>::dedupe()
{
    int64_t old_word_count = word_count();
    m_sentence.erase(std::unique(m_sentence.begin(), m_sentence.end()), m_sentence.end());
    return old_word_count - word_count();
}

template <typename InputIt>
int64_t SplittedSentenceView<InputIt>::size() const
{
    if (m_sentence.empty()) return 0;

    // there is a whitespace between each word
    int64_t result = m_sentence.size() - 1;
    for (const auto& word : m_sentence) {
        result += std::distance(word.first, word.last);
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
    std::basic_string<CharT> joined(sentence_iter->first, sentence_iter->last);
    const std::basic_string<CharT> whitespace{0x20};
    ++sentence_iter;
    for (; sentence_iter != m_sentence.end(); ++sentence_iter) {
        joined.append(whitespace)
            .append(std::basic_string<CharT>(sentence_iter->first, sentence_iter->last));
    }
    return joined;
}

} // namespace rapidfuzz#include <tuple>
#include <unordered_set>
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

namespace common {

/**
 * @defgroup Common Common
 * Common utilities shared among multiple functions
 * @{
 */

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
    return result_cutoff((lensum > 0) ? (max - max * dist / lensum) : max, score_cutoff);
}

template <int Max = 1>
static inline int64_t score_cutoff_to_distance(double score_cutoff, int64_t lensum)
{
    return static_cast<int64_t>(
        std::ceil(static_cast<double>(lensum) * (1.0 - score_cutoff / Max)));
}

template <typename T>
constexpr bool is_zero(T a, T tolerance = std::numeric_limits<T>::epsilon())
{
    return std::fabs(a) <= tolerance;
}

template <typename Sentence, typename CharT = char_type<Sentence>,
          typename = std::enable_if_t<
              is_explicitly_convertible<Sentence, std::basic_string<CharT>>::value>>
std::basic_string<CharT> to_string(Sentence&& str);

template <typename Sentence, typename CharT = char_type<Sentence>,
          typename = std::enable_if_t<
              !is_explicitly_convertible<Sentence, std::basic_string<CharT>>::value &&
              has_data_and_size<Sentence>::value>>
std::basic_string<CharT> to_string(const Sentence& str);

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

/**
 * @brief Finds the first mismatching pair of elements from two ranges:
 * one defined by [first1, last1) and another defined by [first2,last2).
 * Similar implementation to std::mismatch from C++14
 *
 * @param first1, last1 - the first range of the elements
 * @param first2, last2 - the second range of the elements
 *
 * @return std::pair with iterators to the first two non-equal elements.
 */
template <typename InputIterator1, typename InputIterator2>
std::pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1,
                                                   InputIterator2 first2, InputIterator2 last2);

template <typename InputIt1, typename InputIt2>
StringAffix remove_common_affix(InputIt1& first1, InputIt1& last1, InputIt2& first2,
                                InputIt2& last2);

template <typename InputIt1, typename InputIt2>
int64_t remove_common_prefix(InputIt1& first1, InputIt1 last1, InputIt2& first2, InputIt2 last2);

template <typename InputIt1, typename InputIt2>
int64_t remove_common_suffix(InputIt1 first1, InputIt1& last1, InputIt2 first2, InputIt2& last2);

template <typename InputIt, typename CharT = iterator_type<InputIt>>
SplittedSentenceView<InputIt> sorted_split(InputIt first, InputIt last);

template <typename T>
constexpr auto to_unsigned(T value) -> typename std::make_unsigned<T>::type
{
    return typename std::make_unsigned<T>::type(value);
}

template <typename T>
constexpr auto to_signed(T value) -> typename std::make_unsigned<T>::type
{
    return typename std::make_signed<T>::type(value);
}

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
    return !((botT > botU && value < static_cast<U>(botT)) ||
             (topT < topU && value > static_cast<U>(topT)));
}

struct PatternMatchVector {
    struct MapElem {
        uint64_t key = 0;
        uint64_t value = 0;
    };
    std::array<MapElem, 128> m_map;
    std::array<uint64_t, 256> m_extendedAscii;

    PatternMatchVector() : m_map(), m_extendedAscii()
    {}

    template <typename InputIt>
    PatternMatchVector(InputIt first, InputIt last) : m_map(), m_extendedAscii()
    {
        insert(first, last);
    }

    template <typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        uint64_t mask = 1;
        for (; first != last; ++first) {
            insert_mask(*first, mask);
            mask <<= 1;
        }
    }

    template <typename CharT>
    void insert(CharT key, int64_t pos)
    {
        insert_mask(key, 1ull << pos);
    }

    template <typename CharT>
    uint64_t get(CharT key) const
    {
        if (key >= 0 && key <= 255) {
            return m_extendedAscii[(uint8_t)key];
        }
        else {
            return m_map[lookup((uint64_t)key)].value;
        }
    }

    template <typename CharT>
    uint64_t get(int64_t block, CharT key) const
    {
        assert(block == 0);
        (void)block;
        return get(key);
    }

private:
    template <typename CharT>
    void insert_mask(CharT key, uint64_t mask)
    {
        if (key >= 0 && key <= 255) {
            m_extendedAscii[(uint8_t)key] |= mask;
        }
        else {
            uint64_t i = lookup((uint64_t)key);
            m_map[i].key = key;
            m_map[i].value |= mask;
        }
    }

    /**
     * lookup key inside the hasmap using a similar collision resolution
     * strategy to CPython and Ruby
     */
    uint64_t lookup(uint64_t key) const
    {
        uint64_t i = key % 128;

        if (!m_map[i].value || m_map[i].key == key) {
            return i;
        }

        uint64_t perturb = key;
        while (true) {
            i = ((i * 5) + perturb + 1) % 128;
            if (!m_map[i].value || m_map[i].key == key) {
                return i;
            }

            perturb >>= 5;
        }
    }
};

struct BlockPatternMatchVector {
    std::vector<PatternMatchVector> m_val;

    BlockPatternMatchVector()
    {}

    template <typename InputIt>
    BlockPatternMatchVector(InputIt first, InputIt last)
    {
        insert(first, last);
    }

    template <typename CharT>
    void insert(int64_t block, CharT ch, int pos)
    {
        auto* be = &m_val[block];
        be->insert(ch, pos);
    }

    template <typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        int64_t len = std::distance(first, last);
        int64_t block_count = (len / 64) + bool(len % 64);
        m_val.resize(block_count);

        for (int64_t block = 0; block < block_count; ++block) {
            if (std::distance(first + block * 64, last) > 64) {
                m_val[block].insert(first + block * 64, first + (block + 1) * 64);
            }
            else {
                m_val[block].insert(first + block * 64, last);
            }
        }
    }

    template <typename CharT>
    uint64_t get(int64_t block, CharT ch) const
    {
        auto* be = &m_val[block];
        return be->get(ch);
    }
};

template <typename CharT1, int64_t size = sizeof(CharT1)>
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
        if (!CanTypeFitValue<CharT1>(ch)) {
            return false;
        }

        return m_val[UCharT1(ch)];
    }
};

template <typename CharT1, int64_t size>
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
        if (!CanTypeFitValue<CharT1>(ch)) {
            return false;
        }

        return m_val.find(CharT1(ch)) != m_val.end();
    }
};

template <typename T>
struct MatrixVectorView {
    explicit MatrixVectorView(T* vector, int64_t cols) : m_vector(vector), m_cols(cols)
    {}

    T& operator[](uint64_t col)
    {
        assert(col < m_cols);
        return m_vector[col];
    }

    int64_t size() const
    {
        return m_cols;
    }

private:
    T* m_vector;
    int64_t m_cols;
};

template <typename T>
struct ConstMatrixVectorView {
    explicit ConstMatrixVectorView(const T* vector, int64_t cols) : m_vector(vector), m_cols(cols)
    {}

    ConstMatrixVectorView(const MatrixVectorView<T>& other) : m_vector(other.m_vector)
    {}

    const T& operator[](uint64_t col)
    {
        assert(col < m_cols);
        return m_vector[col];
    }

    int64_t size() const
    {
        return m_cols;
    }

private:
    const T* m_vector;
    int64_t m_cols;
};

template <typename T>
struct Matrix {
    Matrix(uint64_t rows, uint64_t cols, uint64_t val)
    {
        m_rows = rows;
        m_cols = cols;
        if (rows * cols > 0) {
            m_matrix = new T[rows * cols];
            std::fill_n(m_matrix, rows * cols, val);
        }
        else {
            m_matrix = nullptr;
        }
    }

    ~Matrix()
    {
        delete[] m_matrix;
    }

    MatrixVectorView<uint64_t> operator[](uint64_t row)
    {
        assert(row < m_rows);
        return MatrixVectorView<uint64_t>(&m_matrix[row * m_cols], m_cols);
    }

    ConstMatrixVectorView<uint64_t> operator[](uint64_t row) const
    {
        assert(row < m_rows);
        return ConstMatrixVectorView<uint64_t>(&m_matrix[row * m_cols], m_cols);
    }

    uint64_t rows() const
    {
        return m_rows;
    }

    uint64_t cols() const
    {
        return m_cols;
    }

private:
    uint64_t m_rows;
    uint64_t m_cols;
    T* m_matrix;
};

/**@}*/

} // namespace common
} // namespace rapidfuzz


#include <algorithm>
#include <array>
#include <cctype>
#include <cwctype>

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2>
DecomposedSet<InputIt1, InputIt2, InputIt1>
common::set_decomposition(SplittedSentenceView<InputIt1> a, SplittedSentenceView<InputIt2> b)
{
    a.dedupe();
    b.dedupe();

    IteratorViewVec<InputIt1> intersection;
    IteratorViewVec<InputIt1> difference_ab;
    IteratorViewVec<InputIt2> difference_ba = b.words();

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
std::basic_string<CharT> common::to_string(Sentence&& str)
{
    return std::basic_string<CharT>(std::forward<Sentence>(str));
}

template <typename Sentence, typename CharT, typename>
std::basic_string<CharT> common::to_string(const Sentence& str)
{
    return std::basic_string<CharT>(str.data(), str.size());
}

template <typename InputIterator1, typename InputIterator2>
std::pair<InputIterator1, InputIterator2>
common::mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                 InputIterator2 last2)
{
    while (first1 != last1 && first2 != last2 && *first1 == *first2) {
        ++first1;
        ++first2;
    }
    return std::pair<InputIterator1, InputIterator2>(first1, first2);
}

/**
 * Removes common prefix of two string views
 */
template <typename InputIt1, typename InputIt2>
int64_t common::remove_common_prefix(InputIt1& first1, InputIt1 last1, InputIt2& first2,
                                     InputIt2 last2)
{
    int64_t prefix = std::distance(first1, common::mismatch(first1, last1, first2, last2).first);
    first1 += prefix;
    first2 += prefix;
    return prefix;
}

/**
 * Removes common suffix of two string views
 */
template <typename InputIt1, typename InputIt2>
int64_t common::remove_common_suffix(InputIt1 first1, InputIt1& last1, InputIt2 first2,
                                     InputIt2& last2)
{
    auto rfirst1 = std::make_reverse_iterator(last1);
    auto rlast1 = std::make_reverse_iterator(first1);
    auto rfirst2 = std::make_reverse_iterator(last2);
    auto rlast2 = std::make_reverse_iterator(first2);

    int64_t suffix =
        std::distance(rfirst1, common::mismatch(rfirst1, rlast1, rfirst2, rlast2).first);
    last1 -= suffix;
    last2 -= suffix;
    return suffix;
}

/**
 * Removes common affix of two string views
 */
template <typename InputIt1, typename InputIt2>
StringAffix common::remove_common_affix(InputIt1& first1, InputIt1& last1, InputIt2& first2,
                                        InputIt2& last2)
{
    return StringAffix{(int64_t)remove_common_prefix(first1, last1, first2, last2),
                       (int64_t)remove_common_suffix(first1, last1, first2, last2)};
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
    case 0x3000:
        return true;
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
    case 0x0020:
        return true;
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
SplittedSentenceView<InputIt> common::sorted_split(InputIt first, InputIt last)
{
    IteratorViewVec<InputIt> splitted;
    auto second = first;

    for (; second != last && first != last; first = second + 1) {
        second = std::find_if(first, last, is_space<CharT>);

        if (first != second) {
            splitted.emplace_back(first, second);
        }
    }

    std::sort(splitted.begin(), splitted.end());

    return SplittedSentenceView<InputIt>(splitted);
}

} // namespace rapidfuzz
#include <stdexcept>

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
                         int64_t score_cutoff = std::numeric_limits<int64_t>::max());

template <typename Sentence1, typename Sentence2>
int64_t hamming_distance(const Sentence1& s1, const Sentence2& s2,
                         int64_t score_cutoff = std::numeric_limits<int64_t>::max());

template <typename InputIt1, typename InputIt2>
int64_t hamming_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                           int64_t score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
int64_t hamming_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff = 0);

template <typename InputIt1, typename InputIt2>
double hamming_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff = 1.0);

template <typename Sentence1, typename Sentence2>
double hamming_normalized_distance(const Sentence1& s1, const Sentence2& s2,
                                   double score_cutoff = 1.0);

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
double hamming_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                     InputIt2 last2, double score_cutoff = 0.0);

template <typename Sentence1, typename Sentence2>
double hamming_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                     double score_cutoff = 0.0);

template <typename CharT1>
struct CachedHamming {
    template <typename Sentence1>
    CachedHamming(const Sentence1& s1_) : s1(common::to_string(s1_))
    {}

    template <typename InputIt1>
    CachedHamming(InputIt1 first1, InputIt1 last1) : s1(first1, last1)
    {}

    template <typename InputIt2>
    int64_t distance(InputIt2 first2, InputIt2 last2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const;

    template <typename Sentence2>
    int64_t distance(const Sentence2& s2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const;

    template <typename InputIt2>
    int64_t similarity(InputIt2 first2, InputIt2 last2, int64_t score_cutoff = 0) const;

    template <typename Sentence2>
    int64_t similarity(const Sentence2& s2, int64_t score_cutoff = 0) const;

    template <typename InputIt2>
    double normalized_distance(InputIt2 first2, InputIt2 last2, double score_cutoff = 1.0) const;

    template <typename Sentence2>
    double normalized_distance(const Sentence2& s2, double score_cutoff = 1.0) const;

    template <typename InputIt2>
    double normalized_similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0.0) const;

    template <typename Sentence2>
    double normalized_similarity(const Sentence2& s2, double score_cutoff = 0.0) const;

private:
    std::basic_string<CharT1> s1;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedHamming(const Sentence1& s1_) -> CachedHamming<char_type<Sentence1>>;

template <typename InputIt1>
CachedHamming(InputIt1 first1, InputIt1 last1) -> CachedHamming<iterator_type<InputIt1>>;
#endif

/**@}*/

} // namespace rapidfuzz


#include <cmath>

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2>
int64_t hamming_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         int64_t score_cutoff)
{
    if (std::distance(first1, last1) != std::distance(first2, last2)) {
        throw std::invalid_argument("Sequences are not the same length.");
    }

    int64_t dist = 0;
    for (; first1 != last1; first1++, first2++) {
        dist += bool(*first1 != *first2);
    }

    return (dist <= score_cutoff) ? dist : score_cutoff + 1;
}

template <typename Sentence1, typename Sentence2>
int64_t hamming_distance(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff)
{
    return hamming_distance(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                            common::to_end(s2), score_cutoff);
}

template <typename InputIt1, typename InputIt2>
int64_t hamming_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                           int64_t score_cutoff)
{
    int64_t maximum = std::distance(first1, last1);
    int64_t cutoff_distance = maximum - score_cutoff;
    int64_t dist = hamming_distance(first1, last1, first2, last2, cutoff_distance);
    int64_t sim = maximum - dist;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename Sentence1, typename Sentence2>
int64_t hamming_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff)
{
    return hamming_similarity(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                              common::to_end(s2), score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double hamming_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff)
{
    int64_t maximum = std::distance(first1, last1);
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(maximum * score_cutoff));
    int64_t dist = hamming_distance(first1, last1, first2, last2, cutoff_distance);
    double norm_dist = (maximum) ? (double)dist / (double)maximum : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : 1.0;
}

template <typename Sentence1, typename Sentence2>
double hamming_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return hamming_normalized_distance(common::to_begin(s1), common::to_end(s1),
                                       common::to_begin(s2), common::to_end(s2), score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double hamming_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                     InputIt2 last2, double score_cutoff)
{
    int64_t maximum = std::distance(first1, last1);
    int64_t cutoff_distance = maximum - score_cutoff;
    int64_t dist = hamming_distance(first1, last1, first2, last2, cutoff_distance);
    int64_t sim = maximum - dist;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename Sentence1, typename Sentence2>
double hamming_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return hamming_normalized_similarity(common::to_begin(s1), common::to_end(s1),
                                         common::to_begin(s2), common::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
int64_t CachedHamming<CharT1>::distance(InputIt2 first2, InputIt2 last2, int64_t score_cutoff) const
{
    return hamming_distance(common::to_begin(s1), common::to_end(s1), first2, last2, score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
int64_t CachedHamming<CharT1>::distance(const Sentence2& s2, int64_t score_cutoff) const
{
    return hamming_distance(s1, s2, score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
int64_t CachedHamming<CharT1>::similarity(InputIt2 first2, InputIt2 last2,
                                          int64_t score_cutoff) const
{
    return hamming_similarity(common::to_begin(s1), common::to_end(s1), first2, last2,
                              score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
int64_t CachedHamming<CharT1>::similarity(const Sentence2& s2, int64_t score_cutoff) const
{
    return hamming_similarity(s1, s2, score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedHamming<CharT1>::normalized_distance(InputIt2 first2, InputIt2 last2,
                                                  double score_cutoff) const
{
    return hamming_normalized_distance(common::to_begin(s1), common::to_end(s1), first2, last2,
                                       score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedHamming<CharT1>::normalized_distance(const Sentence2& s2, double score_cutoff) const
{
    return hamming_normalized_distance(s1, s2, score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedHamming<CharT1>::normalized_similarity(InputIt2 first2, InputIt2 last2,
                                                    double score_cutoff) const
{
    return hamming_normalized_similarity(common::to_begin(s1), common::to_end(s1), first2, last2,
                                         score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedHamming<CharT1>::normalized_similarity(const Sentence2& s2, double score_cutoff) const
{
    return hamming_normalized_similarity(s1, s2, score_cutoff);
}

/**@}*/

} // namespace rapidfuzz


#include <cmath>
#include <limits>

namespace rapidfuzz {

template <typename InputIt1, typename InputIt2>
int64_t indel_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                       int64_t max = std::numeric_limits<int64_t>::max());

template <typename Sentence1, typename Sentence2>
int64_t indel_distance(const Sentence1& s1, const Sentence2& s2,
                       int64_t max = std::numeric_limits<int64_t>::max());

template <typename InputIt1, typename InputIt2>
int64_t indel_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         int64_t score_cutoff = 0.0);

template <typename Sentence1, typename Sentence2>
int64_t indel_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff = 0.0);

template <typename InputIt1, typename InputIt2>
double indel_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                 double score_cutoff = 1.0);

template <typename Sentence1, typename Sentence2>
double indel_normalized_distance(const Sentence1& s1, const Sentence2& s2,
                                 double score_cutoff = 1.0);

template <typename InputIt1, typename InputIt2>
double indel_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff = 0.0);

template <typename Sentence1, typename Sentence2>
double indel_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                   double score_cutoff = 0.0);

template <typename InputIt1, typename InputIt2>
Editops indel_editops(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2);

template <typename Sentence1, typename Sentence2>
Editops indel_editops(const Sentence1& s1, const Sentence2& s2);

template <typename CharT1>
struct CachedIndel {
    template <typename Sentence1>
    CachedIndel(const Sentence1& s1_)
        : s1(common::to_string(s1_)), PM(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt1>
    CachedIndel(InputIt1 first1, InputIt1 last1) : s1(first1, last1), PM(first1, last1)
    {}

    template <typename InputIt2>
    int64_t distance(InputIt2 first2, InputIt2 last2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const;

    template <typename Sentence2>
    int64_t distance(const Sentence2& s2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const;

    template <typename InputIt2>
    int64_t similarity(InputIt2 first2, InputIt2 last2, int64_t score_cutoff = 0) const;

    template <typename Sentence2>
    int64_t similarity(const Sentence2& s2, int64_t score_cutoff = 0) const;

    template <typename InputIt2>
    double normalized_distance(InputIt2 first2, InputIt2 last2, double score_cutoff = 1.0) const;

    template <typename Sentence2>
    double normalized_distance(const Sentence2& s2, double score_cutoff = 1.0) const;

    template <typename InputIt2>
    double normalized_similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0.0) const;

    template <typename Sentence2>
    double normalized_similarity(const Sentence2& s2, double score_cutoff = 0.0) const;

private:
    std::basic_string<CharT1> s1;
    common::BlockPatternMatchVector PM;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedIndel(const Sentence1& s1_) -> CachedIndel<char_type<Sentence1>>;

template <typename InputIt1>
CachedIndel(InputIt1 first1, InputIt1 last1) -> CachedIndel<iterator_type<InputIt1>>;
#endif

} // namespace rapidfuzz




#include <cstddef>
#include <cstdint>

#if defined(_MSC_VER) && !defined(__clang__)
#    include <intrin.h>
#endif

namespace rapidfuzz {
namespace detail {

static inline uint64_t addc64(uint64_t a, uint64_t b, uint64_t carryin, uint64_t* carryout)
{
    /* todo should use _addcarry_u64 when available */
    a += carryin;
    *carryout = a < carryin;
    a += b;
    *carryout |= a < b;
    return a;
}

template <typename T, typename U>
T ceil_div(T a, U divisor)
{
    return a / divisor + static_cast<T>(a % divisor != 0);
}

static inline int64_t popcount64(uint64_t x)
{
    const uint64_t m1 = 0x5555555555555555;
    const uint64_t m2 = 0x3333333333333333;
    const uint64_t m4 = 0x0f0f0f0f0f0f0f0f;
    const uint64_t h01 = 0x0101010101010101;

    x -= (x >> 1) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;
    return static_cast<int64_t>((x * h01) >> 56);
}

/**
 * Extract the lowest set bit from a. If no bits are set in a returns 0.
 */
template <typename T>
T blsi(T a)
{
    return a & -a;
}

/**
 * Clear the lowest set bit in a.
 */
template <typename T>
T blsr(T x)
{
    return x & (x - 1);
}

/**
 * Sets all the lower bits of the result to 1 up to and including lowest set bit (=1) in a.
 * If a is zero, blsmsk sets all bits to 1.
 */
template <typename T>
T blsmsk(T a)
{
    return a ^ (a - 1);
}

#if defined(_MSC_VER) && !defined(__clang__)
static inline int tzcnt(uint32_t x)
{
    unsigned long trailing_zero = 0;
    _BitScanForward(&trailing_zero, x);
    return trailing_zero;
}

#    if defined(_M_ARM) || defined(_M_X64)
static inline int tzcnt(uint64_t x)
{
    unsigned long trailing_zero = 0;
    _BitScanForward64(&trailing_zero, x);
    return trailing_zero;
}
#    else
static inline int tzcnt(uint64_t x)
{
    uint32_t msh = (uint32_t)(x >> 32);
    uint32_t lsh = (uint32_t)(x & 0xFFFFFFFF);
    if (lsh != 0) {
        return tzcnt(lsh);
    }
    return 32 + tzcnt(msh);
}
#    endif

#else /*  gcc / clang */
static inline int tzcnt(uint32_t x)
{
    return __builtin_ctz(x);
}

static inline int tzcnt(uint64_t x)
{
    return __builtin_ctzll(x);
}
#endif

} // namespace detail
} // namespace rapidfuzz

#include <algorithm>
#include <array>
#include <limits>
#include <string>

namespace rapidfuzz {
namespace detail {

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
static constexpr uint8_t indel_mbleven2018_matrix[14][7] = {
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
};

template <typename InputIt1, typename InputIt2>
int64_t indel_mbleven2018(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                          int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    if (len1 < len2) {
        return indel_mbleven2018(first2, last2, first1, last1, max);
    }

    int64_t len_diff = len1 - len2;
    auto possible_ops = indel_mbleven2018_matrix[(max + max * max) / 2 + len_diff - 1];
    int64_t dist = max + 1;

    for (int pos = 0; possible_ops[pos] != 0; ++pos) {
        uint8_t ops = possible_ops[pos];
        int64_t s1_pos = 0;
        int64_t s2_pos = 0;
        int64_t cur_dist = 0;

        while (s1_pos < len1 && s2_pos < len2) {
            if (first1[s1_pos] != first2[s2_pos]) {
                cur_dist++;

                if (!ops) break;
                if (ops & 1) s1_pos++;
                if (ops & 2) s2_pos++;
                ops >>= 2;
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

template <int64_t N, typename PMV, typename InputIt1, typename InputIt2>
static inline int64_t longest_common_subsequence_unroll(const PMV& block, InputIt1 first1,
                                                        InputIt1 last1, InputIt2 first2,
                                                        InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    uint64_t S[N];
    for (int64_t i = 0; i < N; ++i) {
        S[i] = ~0x0ull;
    }

    for (; first2 != last2; ++first2) {
        uint64_t carry = 0;
        uint64_t Matches[N];
        uint64_t u[N];
        uint64_t x[N];
        for (int64_t i = 0; i < N; ++i) {
            Matches[i] = block.get(i, *first2);
            u[i] = S[i] & Matches[i];
            x[i] = addc64(S[i], u[i], carry, &carry);
            S[i] = x[i] | (S[i] - u[i]);
        }
    }

    int64_t res = 0;
    for (int64_t i = 0; i < N; ++i) {
        res += popcount64(~S[i]);
    }

    int64_t dist = len1 + len2 - 2 * res;
    return (dist <= max) ? dist : max + 1;
}

template <typename InputIt1, typename InputIt2>
static inline int64_t
longest_common_subsequence_blockwise(const common::BlockPatternMatchVector& block, InputIt1 first1,
                                     InputIt1 last1, InputIt2 first2, InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    int64_t words = block.m_val.size();
    std::vector<uint64_t> S(words, ~0x0ull);

    for (; first2 != last2; ++first2) {
        uint64_t carry = 0;
        for (int64_t word = 0; word < words; ++word) {
            const uint64_t Matches = block.get(word, *first2);
            uint64_t Stemp = S[word];

            uint64_t u = Stemp & Matches;

            uint64_t x = addc64(Stemp, u, carry, &carry);
            S[word] = x | (Stemp - u);
        }
    }

    int64_t res = 0;
    for (uint64_t Stemp : S) {
        res += popcount64(~Stemp);
    }

    int64_t dist = len1 + len2 - 2 * res;
    return (dist <= max) ? dist : max + 1;
}

template <typename InputIt1, typename InputIt2>
int64_t longest_common_subsequence(const common::BlockPatternMatchVector& block, InputIt1 first1,
                                   InputIt1 last1, InputIt2 first2, InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t nr = ceil_div(len1, 64);
    switch (nr) {
    case 0:
        return (len2 <= max) ? len2 : max + 1;
    case 1:
        return longest_common_subsequence_unroll<1>(block, first1, last1, first2, last2, max);
    case 2:
        return longest_common_subsequence_unroll<2>(block, first1, last1, first2, last2, max);
    case 3:
        return longest_common_subsequence_unroll<3>(block, first1, last1, first2, last2, max);
    case 4:
        return longest_common_subsequence_unroll<4>(block, first1, last1, first2, last2, max);
    case 5:
        return longest_common_subsequence_unroll<5>(block, first1, last1, first2, last2, max);
    case 6:
        return longest_common_subsequence_unroll<6>(block, first1, last1, first2, last2, max);
    case 7:
        return longest_common_subsequence_unroll<7>(block, first1, last1, first2, last2, max);
    case 8:
        return longest_common_subsequence_unroll<8>(block, first1, last1, first2, last2, max);
    default:
        return longest_common_subsequence_blockwise(block, first1, last1, first2, last2, max);
    }
}

template <typename InputIt1, typename InputIt2>
int64_t longest_common_subsequence(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t nr = ceil_div(len1, 64);
    switch (nr) {
    case 1:
    {
        auto block = common::PatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<1>(block, first1, last1, first2, last2, max);
    }
    case 2:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<2>(block, first1, last1, first2, last2, max);
    }
    case 3:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<3>(block, first1, last1, first2, last2, max);
    }
    case 4:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<4>(block, first1, last1, first2, last2, max);
    }
    case 5:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<5>(block, first1, last1, first2, last2, max);
    }
    case 6:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<6>(block, first1, last1, first2, last2, max);
    }
    case 7:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<7>(block, first1, last1, first2, last2, max);
    }
    case 8:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_unroll<8>(block, first1, last1, first2, last2, max);
    }
    default:
    {
        auto block = common::BlockPatternMatchVector(first1, last1);
        return longest_common_subsequence_blockwise(block, first1, last1, first2, last2, max);
    }
    }
}

template <typename InputIt1, typename InputIt2>
int64_t indel_distance(const common::BlockPatternMatchVector& block, InputIt1 first1,
                       InputIt1 last1, InputIt2 first2, InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    /* no edits are allowed */
    if (max == 0 || (max == 1 && len1 == len2)) {
        return std::equal(first1, last1, first2, last2) ? 0 : max + 1;
    }

    if (max < std::abs(len1 - len2)) {
        return max + 1;
    }

    // do this first, since we can not remove any affix in encoded form
    if (max >= 5) {
        return longest_common_subsequence(block, first1, last1, first2, last2, max);
    }

    /* common affix does not effect Levenshtein distance */
    common::remove_common_affix(first1, last1, first2, last2);
    len1 = std::distance(first1, last1);
    len2 = std::distance(first2, last2);
    if (!len1 || !len2) {
        return len1 + len2;
    }

    return indel_mbleven2018(first1, last1, first2, last2, max);
}

template <typename InputIt1, typename InputIt2>
int64_t indel_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                       int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    // Swapping the strings so the second string is shorter
    if (len1 < len2) {
        return indel_distance(first2, last2, first1, last1, max);
    }

    /* no edits are allowed */
    if (max == 0 || (max == 1 && len1 == len2)) {
        return std::equal(first1, last1, first2, last2) ? 0 : max + 1;
    }

    if (max < std::abs(len1 - len2)) {
        return max + 1;
    }

    /* common affix does not effect Levenshtein distance */
    common::remove_common_affix(first1, last1, first2, last2);
    len1 = std::distance(first1, last1);
    len2 = std::distance(first2, last2);
    if (!len1 || !len2) {
        return len1 + len2;
    }

    if (max < 5) {
        return indel_mbleven2018(first1, last1, first2, last2, max);
    }

    return longest_common_subsequence(first1, last1, first2, last2, max);
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_distance(const common::BlockPatternMatchVector& block, InputIt1 first1,
                                 InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                 double score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t maximum = len1 + len2;
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(maximum * score_cutoff));
    int64_t dist = indel_distance(block, first1, last1, first2, last2, cutoff_distance);
    double norm_dist = (maximum) ? (double)dist / (double)maximum : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : 1.0;
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                 double score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t maximum = len1 + len2;
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(maximum * score_cutoff));
    int64_t dist = indel_distance(first1, last1, first2, last2, cutoff_distance);
    double norm_dist = (maximum) ? (double)dist / (double)maximum : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : 1.0;
}

template <typename InputIt1, typename InputIt2>
int64_t indel_similarity(const common::BlockPatternMatchVector& block, InputIt1 first1,
                         InputIt1 last1, InputIt2 first2, InputIt2 last2, int64_t score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t maximum = len1 + len2;
    int64_t cutoff_distance = maximum - score_cutoff;
    int64_t dist = indel_distance(block, first1, last1, first2, last2, cutoff_distance);
    int64_t sim = maximum - dist;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename InputIt1, typename InputIt2>
int64_t indel_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         int64_t score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t maximum = len1 + len2;
    int64_t cutoff_distance = maximum - score_cutoff;
    int64_t dist = indel_distance(first1, last1, first2, last2, cutoff_distance);
    int64_t sim = maximum - dist;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_similarity(const common::BlockPatternMatchVector& block, InputIt1 first1,
                                   InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff)
{
    double norm_dist =
        indel_normalized_distance(block, first1, last1, first2, last2, 1.0 - score_cutoff);
    double norm_sim = 1.0 - norm_dist;
    return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff)
{
    double norm_dist = indel_normalized_distance(first1, last1, first2, last2, 1.0 - score_cutoff);
    double norm_sim = 1.0 - norm_dist;
    return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
}

struct LLCSBitMatrix {
    LLCSBitMatrix(uint64_t rows, uint64_t cols) : S(rows, cols, (uint64_t)-1), dist(0)
    {}

    common::Matrix<uint64_t> S;

    int64_t dist;
};

/**
 * @brief recover alignment from bitparallel Levenshtein matrix
 */
template <typename InputIt1, typename InputIt2>
Editops recover_alignment(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                          const LLCSBitMatrix& matrix, StringAffix affix)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t dist = matrix.dist;
    Editops editops(dist);
    editops.set_src_len(len1 + affix.prefix_len + affix.suffix_len);
    editops.set_dest_len(len2 + affix.prefix_len + affix.suffix_len);

    if (dist == 0) {
        return editops;
    }

    int64_t col = len1;
    int64_t row = len2;

    while (row && col) {
        uint64_t col_pos = col - 1;
        uint64_t col_word = col_pos / 64;
        col_pos = col_pos % 64;
        uint64_t mask = 1ull << col_pos;

        /* Deletion */
        if (matrix.S[row - 1][col_word] & mask) {
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
            if (row && (~matrix.S[row - 1][col_word]) & mask) {
                assert(dist > 0);
                dist--;
                editops[dist].type = EditType::Insert;
                editops[dist].src_pos = col + affix.prefix_len;
                editops[dist].dest_pos = row + affix.prefix_len;
            }
            /* Match */
            else {
                col--;
                assert(first1[col] == first2[row]);
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

template <int64_t N, typename PMV, typename InputIt1, typename InputIt2>
LLCSBitMatrix llcs_matrix_unroll(const PMV& block, InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                 InputIt2 last2)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    uint64_t S[N];
    for (int64_t i = 0; i < N; ++i) {
        S[i] = ~0x0ull;
    }

    LLCSBitMatrix matrix(len2, N);

    for (int64_t i = 0; i < len2; ++i) {
        uint64_t carry = 0;
        uint64_t Matches[N];
        uint64_t u[N];
        uint64_t x[N];
        for (int64_t word = 0; word < N; ++word) {
            Matches[word] = block.get(word, first2[i]);
            u[word] = S[word] & Matches[word];
            x[word] = addc64(S[word], u[word], carry, &carry);
            S[word] = matrix.S[i][word] = x[word] | (S[word] - u[word]);
        }
    }

    int64_t res = 0;
    for (int64_t i = 0; i < N; ++i) {
        res += popcount64(~S[i]);
    }

    matrix.dist = len1 + len2 - 2 * res;

    return matrix;
}

template <typename InputIt1, typename InputIt2>
LLCSBitMatrix llcs_matrix_blockwise(const common::BlockPatternMatchVector& block, InputIt1 first1,
                                    InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t words = block.m_val.size();
    /* todo could be replaced with access to matrix which would slightly
     * reduce memory usage */
    std::vector<uint64_t> S(words, ~0x0ull);
    LLCSBitMatrix matrix(len2, words);

    for (int64_t i = 0; i < len2; ++i) {
        uint64_t carry = 0;
        for (int64_t word = 0; word < words; ++word) {
            const uint64_t Matches = block.get(word, first2[i]);
            uint64_t Stemp = S[word];

            uint64_t u = Stemp & Matches;

            uint64_t x = addc64(Stemp, u, carry, &carry);
            S[word] = matrix.S[i][word] = x | (Stemp - u);
        }
    }

    int64_t res = 0;
    for (uint64_t Stemp : S) {
        res += popcount64(~Stemp);
    }

    matrix.dist = len1 + len2 - 2 * res;

    return matrix;
}

template <typename InputIt1, typename InputIt2>
LLCSBitMatrix llcs_matrix(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    if (!len1 || !len2) {
        LLCSBitMatrix matrix(0, 0);
        matrix.dist = len1 + len2;
        return matrix;
    }
    else if (len1 <= 64) {
        common::PatternMatchVector block(first1, last1);
        return llcs_matrix_unroll<1>(block, first1, last1, first2, last2);
    }
    else {
        common::BlockPatternMatchVector block(first1, last1);
        switch (block.m_val.size()) {
        case 1:
            return llcs_matrix_unroll<1>(block, first1, last1, first2, last2);
        case 2:
            return llcs_matrix_unroll<2>(block, first1, last1, first2, last2);
        case 3:
            return llcs_matrix_unroll<3>(block, first1, last1, first2, last2);
        case 4:
            return llcs_matrix_unroll<4>(block, first1, last1, first2, last2);
        case 5:
            return llcs_matrix_unroll<5>(block, first1, last1, first2, last2);
        case 6:
            return llcs_matrix_unroll<6>(block, first1, last1, first2, last2);
        case 7:
            return llcs_matrix_unroll<7>(block, first1, last1, first2, last2);
        case 8:
            return llcs_matrix_unroll<8>(block, first1, last1, first2, last2);
        default:
            return llcs_matrix_blockwise(block, first1, last1, first2, last2);
        }
    }
}

} // namespace detail

template <typename InputIt1, typename InputIt2>
int64_t indel_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                       int64_t score_cutoff)
{
    return detail::indel_distance(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
int64_t indel_distance(const Sentence1& s1, const Sentence2& s2, int64_t max)
{
    return indel_distance(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                          common::to_end(s2), max);
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                 double score_cutoff)
{
    int64_t maximum = std::distance(first1, last1) + std::distance(first2, last2);
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(maximum * score_cutoff));
    int64_t dist = indel_distance(first1, last1, first2, last2, cutoff_distance);
    double norm_dist = (maximum) ? (double)dist / (double)maximum : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : 1.0;
}

template <typename Sentence1, typename Sentence2>
double indel_normalized_distance(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return indel_normalized_distance(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                                     common::to_end(s2), score_cutoff);
}

template <typename InputIt1, typename InputIt2>
int64_t indel_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                         int64_t score_cutoff)
{
    int64_t maximum = std::distance(first1, last1) + std::distance(first2, last2);
    int64_t cutoff_distance = maximum - score_cutoff;
    int64_t dist = indel_distance(first1, last1, first2, last2, cutoff_distance);
    int64_t sim = maximum - dist;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename Sentence1, typename Sentence2>
int64_t indel_similarity(const Sentence1& s1, const Sentence2& s2, int64_t score_cutoff)
{
    return indel_similarity(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                            common::to_end(s2), score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double indel_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                   double score_cutoff)
{
    double norm_dist = indel_normalized_distance(first1, last1, first2, last2, 1.0 - score_cutoff);
    double norm_sim = 1.0 - norm_dist;
    return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
}

template <typename Sentence1, typename Sentence2>
double indel_normalized_similarity(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return indel_normalized_similarity(common::to_begin(s1), common::to_end(s1),
                                       common::to_begin(s2), common::to_end(s2), score_cutoff);
}

template <typename InputIt1, typename InputIt2>
Editops indel_editops(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
    /* prefix and suffix are no-ops, which do not need to be added to the editops */
    StringAffix affix = common::remove_common_affix(first1, last1, first2, last2);

    return detail::recover_alignment(first1, last1, first2, last2,
                                     detail::llcs_matrix(first1, last1, first2, last2), affix);
}

template <typename Sentence1, typename Sentence2>
Editops indel_editops(const Sentence1& s1, const Sentence2& s2)
{
    return indel_editops(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                         common::to_end(s2));
}

template <typename CharT1>
template <typename InputIt2>
int64_t CachedIndel<CharT1>::distance(InputIt2 first2, InputIt2 last2, int64_t score_cutoff) const
{
    return detail::indel_distance(PM, common::to_begin(s1), common::to_end(s1), first2, last2,
                                  score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
int64_t CachedIndel<CharT1>::distance(const Sentence2& s2, int64_t score_cutoff) const
{
    return distance(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedIndel<CharT1>::normalized_distance(InputIt2 first2, InputIt2 last2,
                                                double score_cutoff) const
{
    int64_t maximum = s1.size() + std::distance(first2, last2);
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(maximum * score_cutoff));
    int64_t dist = distance(first2, last2, cutoff_distance);
    double norm_dist = (maximum) ? (double)dist / (double)maximum : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : 1.0;
}

template <typename CharT1>
template <typename Sentence2>
double CachedIndel<CharT1>::normalized_distance(const Sentence2& s2, double score_cutoff) const
{
    return normalized_distance(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
int64_t CachedIndel<CharT1>::similarity(InputIt2 first2, InputIt2 last2, int64_t score_cutoff) const
{
    int64_t maximum = s1.size() + std::distance(first2, last2);
    int64_t cutoff_distance = maximum - score_cutoff;
    int64_t dist = distance(first2, last2, cutoff_distance);
    int64_t sim = maximum - dist;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename CharT1>
template <typename Sentence2>
int64_t CachedIndel<CharT1>::similarity(const Sentence2& s2, int64_t score_cutoff) const
{
    return similarity(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedIndel<CharT1>::normalized_similarity(InputIt2 first2, InputIt2 last2,
                                                  double score_cutoff) const
{
    double norm_dist = normalized_distance(first2, last2, 1.0 - score_cutoff);
    double norm_sim = 1.0 - norm_dist;
    return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
}

template <typename CharT1>
template <typename Sentence2>
double CachedIndel<CharT1>::normalized_similarity(const Sentence2& s2, double score_cutoff) const
{
    return normalized_similarity(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

} // namespace rapidfuzz

#include <limits>

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
 *    - If max is ≤ 3 the mbleven algorithm is used. This algorithm
 *      checks all possible edit operations that are possible under
 *      the threshold `max`. The time complexity of this algorithm is ``O(N)``.
 *
 *    - If the length of the shorter string is ≤ 64 after removing the common affix
 *      Hyyrös' algorithm is used, which calculates the Levenshtein distance in
 *      parallel. The algorithm is described by @cite hyrro_2002. The time complexity of this
 *      algorithm is ``O(N)``.
 *
 *    - If the length of the shorter string is ≥ 64 after removing the common affix
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
 *    - If max is ≤ 4 the mbleven algorithm is used. This algorithm
 *      checks all possible edit operations that are possible under
 *      the threshold `max`. As a difference to the normal Levenshtein distance this
 *      algorithm can even be used up to a threshold of 4 here, since the higher weight
 *      of substitutions decreases the amount of possible edit operations.
 *      The time complexity of this algorithm is ``O(N)``.
 *
 *    - If the length of the shorter string is ≤ 64 after removing the common affix
 *      Hyyrös' lcs algorithm is used, which calculates the InDel distance in
 *      parallel. The algorithm is described by @cite hyrro_lcs_2004 and is extended with support
 *      for UTF32 in this implementation. The time complexity of this
 *      algorithm is ``O(N)``.
 *
 *    - If the length of the shorter string is ≥ 64 after removing the common affix
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
                             int64_t max = std::numeric_limits<int64_t>::max());

template <typename Sentence1, typename Sentence2>
int64_t levenshtein_distance(const Sentence1& s1, const Sentence2& s2,
                             LevenshteinWeightTable weights = {1, 1, 1},
                             int64_t max = std::numeric_limits<int64_t>::max());

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                               LevenshteinWeightTable weights = {1, 1, 1},
                               int64_t score_cutoff = 0.0);

template <typename Sentence1, typename Sentence2>
int64_t levenshtein_similarity(const Sentence1& s1, const Sentence2& s2,
                               LevenshteinWeightTable weights = {1, 1, 1},
                               int64_t score_cutoff = 0.0);

template <typename InputIt1, typename InputIt2>
double levenshtein_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                       InputIt2 last2, LevenshteinWeightTable weights = {1, 1, 1},
                                       double score_cutoff = 1.0);

template <typename Sentence1, typename Sentence2>
double levenshtein_normalized_distance(const Sentence1& s1, const Sentence2& s2,
                                       LevenshteinWeightTable weights = {1, 1, 1},
                                       double score_cutoff = 1.0);

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
double levenshtein_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                         InputIt2 last2, LevenshteinWeightTable weights = {1, 1, 1},
                                         double score_cutoff = 0.0);

template <typename Sentence1, typename Sentence2>
double levenshtein_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                         LevenshteinWeightTable weights = {1, 1, 1},
                                         double score_cutoff = 0.0);

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
Editops levenshtein_editops(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2);

template <typename Sentence1, typename Sentence2>
Editops levenshtein_editops(const Sentence1& s1, const Sentence2& s2);

template <typename CharT1>
struct CachedLevenshtein {
    template <typename Sentence1>
    CachedLevenshtein(const Sentence1& s1_, LevenshteinWeightTable aWeights = {1, 1, 1})
        : s1(common::to_string(s1_)),
          PM(common::to_begin(s1), common::to_end(s1)),
          weights(aWeights)
    {}

    template <typename InputIt1>
    CachedLevenshtein(InputIt1 first1, InputIt1 last1, LevenshteinWeightTable aWeights = {1, 1, 1})
        : s1(first1, last1), PM(first1, last1), weights(aWeights)
    {}

    template <typename InputIt2>
    int64_t distance(InputIt2 first2, InputIt2 last2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const;

    template <typename Sentence2>
    int64_t distance(const Sentence2& s2,
                     int64_t score_cutoff = std::numeric_limits<int64_t>::max()) const;

    template <typename InputIt2>
    int64_t similarity(InputIt2 first2, InputIt2 last2, int64_t score_cutoff = 0) const;

    template <typename Sentence2>
    int64_t similarity(const Sentence2& s2, int64_t score_cutoff = 0) const;

    template <typename InputIt2>
    double normalized_distance(InputIt2 first2, InputIt2 last2, double score_cutoff = 1.0) const;

    template <typename Sentence2>
    double normalized_distance(const Sentence2& s2, double score_cutoff = 1.0) const;

    template <typename InputIt2>
    double normalized_similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0.0) const;

    template <typename Sentence2>
    double normalized_similarity(const Sentence2& s2, double score_cutoff = 0.0) const;

private:
    std::basic_string<CharT1> s1;
    common::BlockPatternMatchVector PM;
    LevenshteinWeightTable weights;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedLevenshtein(const Sentence1& s1_, LevenshteinWeightTable aWeights)
    -> CachedLevenshtein<char_type<Sentence1>>;

template <typename InputIt1>
CachedLevenshtein(InputIt1 first1, InputIt1 last1, LevenshteinWeightTable aWeights)
    -> CachedLevenshtein<iterator_type<InputIt1>>;
#endif

} // namespace rapidfuzz


#include <cmath>

namespace rapidfuzz {
namespace detail {
template <typename InputIt1, typename InputIt2>
int64_t generalized_levenshtein_wagner_fischer(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                               InputIt2 last2, LevenshteinWeightTable weights,
                                               int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t cache_size = len1 + 1;
    std::vector<int64_t> cache(cache_size);

    cache[0] = 0;
    for (int64_t i = 1; i < cache_size; ++i) {
        cache[i] = cache[i - 1] + (int64_t)weights.delete_cost;
    }

    for (; first2 != last2; ++first2) {
        auto cache_iter = cache.begin();
        int64_t temp = *cache_iter;
        *cache_iter += (int64_t)weights.insert_cost;

        auto _first1 = first1;
        for (; _first1 != last1; ++_first1) {
            if (*_first1 != *first2) {
                temp = std::min({*cache_iter + (int64_t)weights.delete_cost,
                                 *(cache_iter + 1) + (int64_t)weights.insert_cost,
                                 temp + (int64_t)weights.replace_cost});
            }
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
int64_t levenshtein_maximum(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                            LevenshteinWeightTable weights)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    int64_t max_dist = len1 * (int64_t)weights.delete_cost + len2 * (int64_t)weights.insert_cost;

    if (len1 >= len2) {
        max_dist = std::min(max_dist, len2 * (int64_t)weights.replace_cost +
                                          (len1 - len2) * (int64_t)weights.delete_cost);
    }
    else {
        max_dist = std::min(max_dist, len1 * (int64_t)weights.replace_cost +
                                          (len2 - len1) * (int64_t)weights.insert_cost);
    }

    return max_dist;
}

/**
 * @brief calculates the minimal possible Levenshtein distance based on
 * string lengths and weights
 */
template <typename InputIt1, typename InputIt2>
int64_t levenshtein_min_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                 LevenshteinWeightTable weights)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    return std::max((len1 - len2) * (int64_t)weights.delete_cost,
                    (len2 - len1) * (int64_t)weights.insert_cost);
}

template <typename InputIt1, typename InputIt2>
int64_t generalized_levenshtein_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                         InputIt2 last2, LevenshteinWeightTable weights,
                                         int64_t max)
{
    int64_t min_edits = levenshtein_min_distance(first1, last1, first2, last2, weights);
    if (min_edits > max) {
        return max + 1;
    }

    /* common affix does not effect Levenshtein distance */
    common::remove_common_affix(first1, last1, first2, last2);

    return generalized_levenshtein_wagner_fischer(first1, last1, first2, last2, weights, max);
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
static constexpr uint8_t levenshtein_mbleven2018_matrix[9][8] = {
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
};

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_mbleven2018(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    if (len1 < len2) {
        return levenshtein_mbleven2018(first2, last2, first1, last1, max);
    }

    int64_t len_diff = len1 - len2;
    auto possible_ops = levenshtein_mbleven2018_matrix[(max + max * max) / 2 + len_diff - 1];
    int64_t dist = max + 1;

    for (int pos = 0; possible_ops[pos] != 0; ++pos) {
        uint8_t ops = possible_ops[pos];
        int64_t s1_pos = 0;
        int64_t s2_pos = 0;
        int64_t cur_dist = 0;
        while (s1_pos < len1 && s2_pos < len2) {
            if (first1[s1_pos] != first2[s2_pos]) {
                cur_dist++;
                if (!ops) break;
                if (ops & 1) s1_pos++;
                if (ops & 2) s2_pos++;
                ops >>= 2;
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
template <typename InputIt1, typename InputIt2>
int64_t levenshtein_hyrroe2003(const common::PatternMatchVector& PM, InputIt1 first1,
                               InputIt1 last1, InputIt2 first2, InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);

    /* VP is set to 1^m. Shifting by bitwidth would be undefined behavior */
    uint64_t VP = (uint64_t)-1;
    uint64_t VN = 0;
    int64_t currDist = len1;

    /* mask used when computing D[m,j] in the paper 10^(m-1) */
    uint64_t mask = (uint64_t)1 << (len1 - 1);

    /* Searching */
    for (; first2 != last2; ++first2) {
        /* Step 1: Computing D0 */
        uint64_t PM_j = PM.get(*first2);
        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        currDist += bool(HP & mask);
        currDist -= bool(HN & mask);

        /* Step 4: Computing Vp and VN */
        HP = (HP << 1) | 1;
        HN = (HN << 1);

        VP = HN | ~(D0 | HP);
        VN = HP & D0;
    }

    return (currDist <= max) ? currDist : max + 1;
}

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_hyrroe2003_small_band(const common::BlockPatternMatchVector& PM,
                                          InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                          InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    /* VP is set to 1^m. Shifting by bitwidth would be undefined behavior */
    uint64_t VP = (uint64_t)-1;
    uint64_t VN = 0;

    int64_t currDist = len1;

    /* mask used when computing D[m,j] in the paper 10^(m-1) */
    uint64_t mask = 1ull << 63;

    const int64_t words = PM.m_val.size();

    /* Searching */
    for (int64_t i = 0; i < len2; ++i) {
        /* Step 1: Computing D0 */
        int64_t word = i / 64;
        int64_t word_pos = i % 64;

        uint64_t PM_j = PM.get(word, first2[i]) >> word_pos;

        if (word + 1 < words && word_pos != 0) {
            PM_j |= PM.get(word + 1, first2[i]) << (64 - word_pos);
        }

        /* Step 1: Computing D0 */
        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        currDist += bool(HP & mask);
        currDist -= bool(HN & mask);

        /* Step 4: Computing Vp and VN */
        VP = HN | ~((D0 >> 1) | HP);
        VN = (D0 >> 1) & HP;
    }

    return (currDist <= max) ? currDist : max + 1;
}

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_myers1999_block(const common::BlockPatternMatchVector& PM, InputIt1 first1,
                                    InputIt1 last1, InputIt2 first2, InputIt2 last2, int64_t max)
{
    struct Vectors {
        uint64_t VP;
        uint64_t VN;

        Vectors() : VP(~0x0ull), VN(0)
        {}
    };

    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t words = PM.m_val.size();
    int64_t currDist = len1;

    /* upper bound */
    max = std::min(max, std::max(len1, len2));

    // todo could safe up to 25% even without max when ignoring irrelevant paths
    int64_t full_band = std::min(len1, 2 * max + 1);

    if (full_band <= 64) {
        return levenshtein_hyrroe2003_small_band(PM, first1, last1, first2, last2, max);
    }

    std::vector<Vectors> vecs(words);
    uint64_t Last = (uint64_t)1 << ((len1 - 1) % 64);

    /* Searching */
    for (int64_t i = 0; i < len2; i++) {
        uint64_t HP_carry = 1;
        uint64_t HN_carry = 0;

        for (int64_t word = 0; word < words - 1; word++) {
            /* Step 1: Computing D0 */
            uint64_t PM_j = PM.get(word, first2[i]);
            uint64_t VN = vecs[word].VN;
            uint64_t VP = vecs[word].VP;

            uint64_t X = PM_j | HN_carry;
            uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

            /* Step 2: Computing HP and HN */
            uint64_t HP = VN | ~(D0 | VP);
            uint64_t HN = D0 & VP;

            /* Step 3: Computing the value D[m,j] */
            // only required for last vector

            /* Step 4: Computing Vp and VN */
            uint64_t HP_carry_temp = HP_carry;
            HP_carry = HP >> 63;
            HP = (HP << 1) | HP_carry_temp;

            uint64_t HN_carry_temp = HN_carry;
            HN_carry = HN >> 63;
            HN = (HN << 1) | HN_carry_temp;

            vecs[word].VP = HN | ~(D0 | HP);
            vecs[word].VN = HP & D0;
        }

        {
            /* Step 1: Computing D0 */
            uint64_t PM_j = PM.get(words - 1, first2[i]);
            uint64_t VN = vecs[words - 1].VN;
            uint64_t VP = vecs[words - 1].VP;

            uint64_t X = PM_j | HN_carry;
            uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

            /* Step 2: Computing HP and HN */
            uint64_t HP = VN | ~(D0 | VP);
            uint64_t HN = D0 & VP;

            /* Step 3: Computing the value D[m,j] */
            currDist += bool(HP & Last);
            currDist -= bool(HN & Last);

            /* Step 4: Computing Vp and VN */
            HP = (HP << 1) | HP_carry;
            HN = (HN << 1) | HN_carry;

            vecs[words - 1].VP = HN | ~(D0 | HP);
            vecs[words - 1].VN = HP & D0;
        }
    }

    return (currDist <= max) ? currDist : max + 1;
}

template <typename InputIt1, typename InputIt2>
int64_t uniform_levenshtein_distance(const common::BlockPatternMatchVector& block, InputIt1 first1,
                                     InputIt1 last1, InputIt2 first2, InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    // when no differences are allowed a direct comparision is sufficient
    if (max == 0) {
        return !std::equal(first1, last1, first2, last2);
    }

    if (max < std::abs(len1 - len2)) {
        return max + 1;
    }

    // important to catch, since this causes block.m_val to be empty -> raises exception on access
    if (!len1) {
        return (len2 <= max) ? len2 : max + 1;
    }

    /* do this first, since we can not remove any affix in encoded form
     * todo actually we could at least remove the common prefix and just shift the band
     */
    if (max >= 4) {
        if (len1 < 65) {
            return levenshtein_hyrroe2003(block.m_val[0], first1, last1, first2, last2, max);
        }
        else {
            return levenshtein_myers1999_block(block, first1, last1, first2, last2, max);
        }
    }

    /* common affix does not effect Levenshtein distance */
    common::remove_common_affix(first1, last1, first2, last2);
    len1 = std::distance(first1, last1);
    len2 = std::distance(first2, last2);
    if (!len1 || !len2) {
        return len1 + len2;
    }

    return levenshtein_mbleven2018(first1, last1, first2, last2, max);
}

template <typename InputIt1, typename InputIt2>
int64_t uniform_levenshtein_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                     InputIt2 last2, int64_t max)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    /* Swapping the strings so the second string is shorter */
    if (len1 < len2) {
        return uniform_levenshtein_distance(first2, last2, first1, last1, max);
    }

    // when no differences are allowed a direct comparision is sufficient
    if (max == 0) {
        return !std::equal(first1, last1, first2, last2);
    }

    // at least length difference insertions/deletions required
    if (max < (len1 - len2)) {
        return max + 1;
    }

    /* common affix does not effect Levenshtein distance */
    common::remove_common_affix(first1, last1, first2, last2);
    len1 = std::distance(first1, last1);
    len2 = std::distance(first2, last2);
    if (!len1 || !len2) {
        return len1 + len2;
    }

    if (max < 4) {
        return levenshtein_mbleven2018(first1, last1, first2, last2, max);
    }

    /* when the short strings has less then 65 elements Hyyrös' algorithm can be used */
    if (len1 < 65) {
        return levenshtein_hyrroe2003(common::PatternMatchVector(first1, last1), first1, last1,
                                      first2, last2, max);
    }
    else {
        return levenshtein_myers1999_block(common::BlockPatternMatchVector(first1, last1), first1,
                                           last1, first2, last2, max);
    }
}

template <typename InputIt1, typename InputIt2>
double uniform_levenshtein_normalized_distance(const common::BlockPatternMatchVector& block,
                                               InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                               InputIt2 last2, double score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t maximum = std::max(len1, len2);
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(maximum * score_cutoff));
    int64_t dist =
        uniform_levenshtein_distance(block, first1, last1, first2, last2, cutoff_distance);
    double norm_dist = (maximum) ? (double)dist / (double)maximum : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : 1.0;
}

template <typename InputIt1, typename InputIt2>
int64_t uniform_levenshtein_similarity(const common::BlockPatternMatchVector& block,
                                       InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                       InputIt2 last2, int64_t score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t maximum = std::max(len1, len2);
    int64_t cutoff_distance = maximum - score_cutoff;
    int64_t dist =
        uniform_levenshtein_distance(block, first1, last1, first2, last2, cutoff_distance);
    int64_t sim = maximum - dist;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename InputIt1, typename InputIt2>
double uniform_levenshtein_normalized_similarity(const common::BlockPatternMatchVector& block,
                                                 InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                                 InputIt2 last2, double score_cutoff)
{
    double norm_dist = uniform_levenshtein_normalized_distance(block, first1, last1, first2, last2,
                                                               1.0 - score_cutoff);
    double norm_sim = 1.0 - norm_dist;
    return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
}

struct LevenshteinBitMatrix {
    LevenshteinBitMatrix(uint64_t rows, uint64_t cols)
        : VP(rows, cols, (uint64_t)-1), VN(rows, cols, 0), dist(0)
    {}

    common::Matrix<uint64_t> VP;
    common::Matrix<uint64_t> VN;

    int64_t dist;
};

/**
 * @brief recover alignment from bitparallel Levenshtein matrix
 */
template <typename InputIt1, typename InputIt2>
Editops recover_alignment(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                          const LevenshteinBitMatrix& matrix, StringAffix affix)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    int64_t dist = matrix.dist;
    Editops editops(dist);
    editops.set_src_len(len1 + affix.prefix_len + affix.suffix_len);
    editops.set_dest_len(len2 + affix.prefix_len + affix.suffix_len);

    if (dist == 0) {
        return editops;
    }

    int64_t col = len1;
    int64_t row = len2;

    while (row && col) {
        uint64_t col_pos = col - 1;
        uint64_t col_word = col_pos / 64;
        col_pos = col_pos % 64;
        uint64_t mask = 1ull << col_pos;

        /* Deletion */
        if (matrix.VP[row - 1][col_word] & mask) {
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
            if (row && matrix.VN[row - 1][col_word] & mask) {
                assert(dist > 0);
                dist--;
                editops[dist].type = EditType::Insert;
                editops[dist].src_pos = col + affix.prefix_len;
                editops[dist].dest_pos = row + affix.prefix_len;
            }
            /* Match/Mismatch */
            else {
                col--;

                /* Replace (Matches are not recorded) */
                if (first1[col] != first2[row]) {
                    assert(dist > 0);
                    dist--;
                    editops[dist].type = EditType::Replace;
                    editops[dist].src_pos = col + affix.prefix_len;
                    editops[dist].dest_pos = row + affix.prefix_len;
                }
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
LevenshteinBitMatrix levenshtein_matrix_hyrroe2003(const common::PatternMatchVector& PM,
                                                   InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                                   InputIt2 last2)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    uint64_t VP = ~0x0ull;
    uint64_t VN = 0;

    LevenshteinBitMatrix matrix(len2, 1);
    matrix.dist = len1;

    /* mask used when computing D[m,j] in the paper 10^(m-1) */
    uint64_t mask = (uint64_t)1 << (len1 - 1);

    /* Searching */
    for (int64_t i = 0; i < len2; ++i) {
        /* Step 1: Computing D0 */
        uint64_t PM_j = PM.get(first2[i]);
        uint64_t X = PM_j;
        uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

        /* Step 2: Computing HP and HN */
        uint64_t HP = VN | ~(D0 | VP);
        uint64_t HN = D0 & VP;

        /* Step 3: Computing the value D[m,j] */
        matrix.dist += bool(HP & mask);
        matrix.dist -= bool(HN & mask);

        /* Step 4: Computing Vp and VN */
        HP = (HP << 1) | 1;
        HN = (HN << 1);

        VP = matrix.VP[i][0] = HN | ~(D0 | HP);
        VN = matrix.VN[i][0] = HP & D0;
    }

    return matrix;
}

template <typename InputIt1, typename InputIt2>
LevenshteinBitMatrix levenshtein_matrix_hyrroe2003_block(const common::BlockPatternMatchVector& PM,
                                                         InputIt1 first1, InputIt1 last1,
                                                         InputIt2 first2, InputIt2 last2)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    /* todo could be replaced with access to matrix which would slightly
     * reduce memory usage */
    struct Vectors {
        uint64_t VP;
        uint64_t VN;

        Vectors() : VP(~0x0ull), VN(0)
        {}
    };

    int64_t words = PM.m_val.size();
    LevenshteinBitMatrix matrix(len2, words);
    matrix.dist = len1;

    std::vector<Vectors> vecs(words);
    uint64_t Last = (uint64_t)1 << ((len1 - 1) % 64);

    /* Searching */
    for (int64_t i = 0; i < len2; i++) {
        uint64_t HP_carry = 1;
        uint64_t HN_carry = 0;

        for (int64_t word = 0; word < words - 1; word++) {
            /* Step 1: Computing D0 */
            uint64_t PM_j = PM.get(word, first2[i]);
            uint64_t VN = vecs[word].VN;
            uint64_t VP = vecs[word].VP;

            uint64_t X = PM_j | HN_carry;
            uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

            /* Step 2: Computing HP and HN */
            uint64_t HP = VN | ~(D0 | VP);
            uint64_t HN = D0 & VP;

            /* Step 3: Computing the value D[m,j] */
            // only required for last vector

            /* Step 4: Computing Vp and VN */
            uint64_t HP_carry_temp = HP_carry;
            HP_carry = HP >> 63;
            HP = (HP << 1) | HP_carry_temp;

            uint64_t HN_carry_temp = HN_carry;
            HN_carry = HN >> 63;
            HN = (HN << 1) | HN_carry_temp;

            vecs[word].VP = matrix.VP[i][word] = HN | ~(D0 | HP);
            vecs[word].VN = matrix.VN[i][word] = HP & D0;
        }

        {
            /* Step 1: Computing D0 */
            uint64_t PM_j = PM.get(words - 1, first2[i]);
            uint64_t VN = vecs[words - 1].VN;
            uint64_t VP = vecs[words - 1].VP;

            uint64_t X = PM_j | HN_carry;
            uint64_t D0 = (((X & VP) + VP) ^ VP) | X | VN;

            /* Step 2: Computing HP and HN */
            uint64_t HP = VN | ~(D0 | VP);
            uint64_t HN = D0 & VP;

            /* Step 3: Computing the value D[m,j] */
            matrix.dist += bool(HP & Last);
            matrix.dist -= bool(HN & Last);

            /* Step 4: Computing Vp and VN */
            HP = (HP << 1) | HP_carry;
            HN = (HN << 1) | HN_carry;

            vecs[words - 1].VP = matrix.VP[i][words - 1] = HN | ~(D0 | HP);
            vecs[words - 1].VN = matrix.VN[i][words - 1] = HP & D0;
        }
    }

    return matrix;
}

template <typename InputIt1, typename InputIt2>
LevenshteinBitMatrix levenshtein_matrix(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                        InputIt2 last2)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    if (!len1 || !len2) {
        LevenshteinBitMatrix matrix(0, 0);
        matrix.dist = len1 + len2;
        return matrix;
    }
    else if (len1 <= 64) {
        return levenshtein_matrix_hyrroe2003(common::PatternMatchVector(first1, last1), first1,
                                             last1, first2, last2);
    }
    else {
        return levenshtein_matrix_hyrroe2003_block(common::BlockPatternMatchVector(first1, last1),
                                                   first1, last1, first2, last2);
    }
}

} // namespace detail

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                             LevenshteinWeightTable weights, int64_t max)
{
    if (weights.insert_cost == weights.delete_cost) {
        /* when insertions + deletions operations are free there can not be any edit distance */
        if (weights.insert_cost == 0) {
            return 0;
        }

        /* uniform Levenshtein multiplied with the common factor */
        if (weights.insert_cost == weights.replace_cost) {
            // max can make use of the common divisor of the three weights
            int64_t new_max = detail::ceil_div(max, weights.insert_cost);
            int64_t distance =
                detail::uniform_levenshtein_distance(first1, last1, first2, last2, new_max);
            distance *= weights.insert_cost;
            return (distance <= max) ? distance : max + 1;
        }
        /*
         * when replace_cost >= insert_cost + delete_cost no substitutions are performed
         * therefore this can be implemented as InDel distance multiplied with the common factor
         */
        else if (weights.replace_cost >= weights.insert_cost + weights.delete_cost) {
            // max can make use of the common divisor of the three weights
            int64_t new_max = detail::ceil_div(max, weights.insert_cost);
            int64_t distance = indel_distance(first1, last1, first2, last2, new_max);
            distance *= weights.insert_cost;
            return (distance <= max) ? distance : max + 1;
        }
    }

    return detail::generalized_levenshtein_wagner_fischer(first1, last1, first2, last2, weights,
                                                          max);
}

template <typename Sentence1, typename Sentence2>
int64_t levenshtein_distance(const Sentence1& s1, const Sentence2& s2,
                             LevenshteinWeightTable weights, int64_t max)
{
    return levenshtein_distance(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                                common::to_end(s2), weights, max);
}

template <typename InputIt1, typename InputIt2>
double levenshtein_normalized_distance(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                       InputIt2 last2, LevenshteinWeightTable weights,
                                       double score_cutoff)
{
    int64_t maximum = detail::levenshtein_maximum(first1, last1, first2, last2, weights);
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(maximum * score_cutoff));
    int64_t dist = levenshtein_distance(first1, last1, first2, last2, weights, cutoff_distance);
    double norm_dist = (maximum) ? (double)dist / (double)maximum : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : 1.0;
}

template <typename Sentence1, typename Sentence2>
double levenshtein_normalized_distance(const Sentence1& s1, const Sentence2& s2,
                                       LevenshteinWeightTable weights, double score_cutoff)
{
    return levenshtein_normalized_distance(common::to_begin(s1), common::to_end(s1),
                                           common::to_begin(s2), common::to_end(s2), weights,
                                           score_cutoff);
}

template <typename InputIt1, typename InputIt2>
int64_t levenshtein_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                               LevenshteinWeightTable weights, int64_t score_cutoff)
{
    int64_t maximum = detail::levenshtein_maximum(first1, last1, first2, last2, weights);
    int64_t cutoff_distance = maximum - score_cutoff;
    int64_t dist = levenshtein_distance(first1, last1, first2, last2, weights, cutoff_distance);
    int64_t sim = maximum - dist;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename Sentence1, typename Sentence2>
int64_t levenshtein_similarity(const Sentence1& s1, const Sentence2& s2,
                               LevenshteinWeightTable weights, int64_t score_cutoff)
{
    return levenshtein_similarity(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                                  common::to_end(s2), weights, score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double levenshtein_normalized_similarity(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                         InputIt2 last2, LevenshteinWeightTable weights,
                                         double score_cutoff)
{
    double norm_dist =
        levenshtein_normalized_distance(first1, last1, first2, last2, weights, 1.0 - score_cutoff);
    double norm_sim = 1.0 - norm_dist;
    return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
}

template <typename Sentence1, typename Sentence2>
double levenshtein_normalized_similarity(const Sentence1& s1, const Sentence2& s2,
                                         LevenshteinWeightTable weights, double score_cutoff)
{
    return levenshtein_normalized_similarity(common::to_begin(s1), common::to_end(s1),
                                             common::to_begin(s2), common::to_end(s2), weights,
                                             score_cutoff);
}

template <typename InputIt1, typename InputIt2>
Editops levenshtein_editops(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
    /* prefix and suffix are no-ops, which do not need to be added to the editops */
    StringAffix affix = common::remove_common_affix(first1, last1, first2, last2);

    return detail::recover_alignment(first1, last1, first2, last2,
                                     detail::levenshtein_matrix(first1, last1, first2, last2),
                                     affix);
}

template <typename Sentence1, typename Sentence2>
Editops levenshtein_editops(const Sentence1& s1, const Sentence2& s2)
{
    return levenshtein_editops(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                               common::to_end(s2));
}

template <typename CharT1>
template <typename InputIt2>
int64_t CachedLevenshtein<CharT1>::distance(InputIt2 first2, InputIt2 last2,
                                            int64_t score_cutoff) const
{
    auto first1 = common::to_begin(s1);
    auto last1 = common::to_end(s1);

    if (weights.insert_cost == weights.delete_cost) {
        /* when insertions + deletions operations are free there can not be any edit distance */
        if (weights.insert_cost == 0) {
            return 0;
        }

        /* uniform Levenshtein multiplied with the common factor */
        if (weights.insert_cost == weights.replace_cost) {
            // max can make use of the common divisor of the three weights
            int64_t new_max = detail::ceil_div(score_cutoff, weights.insert_cost);
            int64_t dist =
                detail::uniform_levenshtein_distance(PM, first1, last1, first2, last2, new_max);
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
            int64_t dist = detail::indel_distance(PM, first1, last1, first2, last2, new_max);
            dist *= weights.insert_cost;
            return (dist <= score_cutoff) ? dist : score_cutoff + 1;
        }
    }

    return detail::generalized_levenshtein_distance(first1, last1, first2, last2, weights,
                                                    score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
int64_t CachedLevenshtein<CharT1>::distance(const Sentence2& s2, int64_t score_cutoff) const
{
    return distance(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedLevenshtein<CharT1>::normalized_distance(InputIt2 first2, InputIt2 last2,
                                                      double score_cutoff) const
{
    auto first1 = common::to_begin(s1);
    auto last1 = common::to_end(s1);
    int64_t maximum = detail::levenshtein_maximum(first1, last1, first2, last2, weights);
    int64_t cutoff_distance = static_cast<int64_t>(std::ceil(maximum * score_cutoff));
    int64_t dist = distance(first2, last2, cutoff_distance);
    double norm_dist = (maximum) ? (double)dist / (double)maximum : 0.0;
    return (norm_dist <= score_cutoff) ? norm_dist : 1.0;
}

template <typename CharT1>
template <typename Sentence2>
double CachedLevenshtein<CharT1>::normalized_distance(const Sentence2& s2,
                                                      double score_cutoff) const
{
    return normalized_distance(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
int64_t CachedLevenshtein<CharT1>::similarity(InputIt2 first2, InputIt2 last2,
                                              int64_t score_cutoff) const
{
    auto first1 = common::to_begin(s1);
    auto last1 = common::to_end(s1);
    int64_t maximum = detail::levenshtein_maximum(first1, last1, first2, last2, weights);
    int64_t cutoff_distance = maximum - score_cutoff;
    int64_t dist = distance(first2, last2, cutoff_distance);
    int64_t sim = maximum - dist;
    return (sim >= score_cutoff) ? sim : 0;
}

template <typename CharT1>
template <typename Sentence2>
int64_t CachedLevenshtein<CharT1>::similarity(const Sentence2& s2, int64_t score_cutoff) const
{
    return similarity(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedLevenshtein<CharT1>::normalized_similarity(InputIt2 first2, InputIt2 last2,
                                                        double score_cutoff) const
{
    double norm_dist = normalized_distance(first2, last2, 1.0 - score_cutoff);
    double norm_sim = 1.0 - norm_dist;
    return (norm_sim >= score_cutoff) ? norm_sim : 0.0;
}

template <typename CharT1>
template <typename Sentence2>
double CachedLevenshtein<CharT1>::normalized_similarity(const Sentence2& s2,
                                                        double score_cutoff) const
{
    return normalized_similarity(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

} // namespace rapidfuzz
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
double ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
             double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// TODO documentation
template <typename CharT1>
struct CachedRatio {
    template <typename InputIt1>
    CachedRatio(InputIt1 first1, InputIt1 last1) : s1(first1, last1), PM(first1, last1)
    {}

    template <typename Sentence1>
    CachedRatio(const Sentence1& s1) : CachedRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0.0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    common::BlockPatternMatchVector PM;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedRatio(const Sentence1& s1) -> CachedRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedRatio(InputIt1 first1, InputIt1 last1) -> CachedRatio<iterator_type<InputIt1>>;
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
    CachedPartialRatio(const Sentence1& s1)
        : CachedPartialRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0.0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0.0) const;

private:
    std::basic_string<CharT1> s1;
    common::CharSet<CharT1> s1_char_set;
    CachedRatio<CharT1> cached_ratio;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedPartialRatio(const Sentence1& s1) -> CachedPartialRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedPartialRatio(InputIt1 first1, InputIt1 last1) -> CachedPartialRatio<iterator_type<InputIt1>>;
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

template <typename Sentence1, typename Sentence2, typename CharT1 = char_type<Sentence1>,
          typename CharT2 = char_type<Sentence2>>
double token_sort_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// todo CachedRatio speed for equal strings vs original implementation
// TODO documentation
template <typename CharT1>
struct CachedTokenSortRatio {
    template <typename InputIt1>
    CachedTokenSortRatio(InputIt1 first1, InputIt1 last1)
        : s1_sorted(common::sorted_split(first1, last1).join()), cached_ratio(s1_sorted)
    {}

    template <typename Sentence1>
    CachedTokenSortRatio(const Sentence1& s1)
        : CachedTokenSortRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1_sorted;
    CachedRatio<CharT1> cached_ratio;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedTokenSortRatio(const Sentence1& s1) -> CachedTokenSortRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedTokenSortRatio(InputIt1 first1, InputIt1 last1)
    -> CachedTokenSortRatio<iterator_type<InputIt1>>;
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
        : s1_sorted(common::sorted_split(first1, last1).join()), cached_partial_ratio(s1_sorted)
    {}

    template <typename Sentence1>
    CachedPartialTokenSortRatio(const Sentence1& s1)
        : CachedPartialTokenSortRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1_sorted;
    CachedPartialRatio<CharT1> cached_partial_ratio;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedPartialTokenSortRatio(const Sentence1& s1)
    -> CachedPartialTokenSortRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedPartialTokenSortRatio(InputIt1 first1, InputIt1 last1)
    -> CachedPartialTokenSortRatio<iterator_type<InputIt1>>;
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
        : s1(first1, last1), tokens_s1(common::sorted_split(std::begin(s1), std::end(s1)))
    {}

    template <typename Sentence1>
    CachedTokenSetRatio(const Sentence1& s1)
        : CachedTokenSetRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    SplittedSentenceView<typename std::basic_string<CharT1>::iterator> tokens_s1;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedTokenSetRatio(const Sentence1& s1) -> CachedTokenSetRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedTokenSetRatio(InputIt1 first1, InputIt1 last1)
    -> CachedTokenSetRatio<iterator_type<InputIt1>>;
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
        : s1(first1, last1), tokens_s1(common::sorted_split(std::begin(s1), std::end(s1)))
    {}

    template <typename Sentence1>
    CachedPartialTokenSetRatio(const Sentence1& s1)
        : CachedPartialTokenSetRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    SplittedSentenceView<typename std::basic_string<CharT1>::iterator> tokens_s1;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedPartialTokenSetRatio(const Sentence1& s1) -> CachedPartialTokenSetRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedPartialTokenSetRatio(InputIt1 first1, InputIt1 last1)
    -> CachedPartialTokenSetRatio<iterator_type<InputIt1>>;
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
double token_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                   double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double token_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// todo add real implementation
template <typename CharT1>
struct CachedTokenRatio {
    template <typename InputIt1>
    CachedTokenRatio(InputIt1 first1, InputIt1 last1)
        : s1(first1, last1),
          s1_tokens(common::sorted_split(std::begin(s1), std::end(s1))),
          s1_sorted(s1_tokens.join()),
          cached_ratio_s1_sorted(s1_sorted)
    {}

    template <typename Sentence1>
    CachedTokenRatio(const Sentence1& s1)
        : CachedTokenRatio(common::to_begin(s1), common::to_end(s1))
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

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedTokenRatio(const Sentence1& s1) -> CachedTokenRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedTokenRatio(InputIt1 first1, InputIt1 last1) -> CachedTokenRatio<iterator_type<InputIt1>>;
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
          tokens_s1(common::sorted_split(std::begin(s1), std::end(s1))),
          s1_sorted(tokens_s1.join())
    {}

    template <typename Sentence1>
    CachedPartialTokenRatio(const Sentence1& s1)
        : CachedPartialTokenRatio(common::to_begin(s1), common::to_end(s1))
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

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedPartialTokenRatio(const Sentence1& s1) -> CachedPartialTokenRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedPartialTokenRatio(InputIt1 first1, InputIt1 last1)
    -> CachedPartialTokenRatio<iterator_type<InputIt1>>;
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
double WRatio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
              double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double WRatio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

// todo add real implementation
template <typename CharT1>
struct CachedWRatio {
    template <typename InputIt1>
    CachedWRatio(InputIt1 first1, InputIt1 last1);

    template <typename Sentence1>
    CachedWRatio(const Sentence1& s1) : CachedWRatio(common::to_begin(s1), common::to_end(s1))
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
    common::BlockPatternMatchVector blockmap_s1_sorted;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedWRatio(const Sentence1& s1) -> CachedWRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedWRatio(InputIt1 first1, InputIt1 last1) -> CachedWRatio<iterator_type<InputIt1>>;
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
double QRatio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
              double score_cutoff = 0);

template <typename Sentence1, typename Sentence2>
double QRatio(const Sentence1& s1, const Sentence2& s2, double score_cutoff = 0);

template <typename CharT1>
struct CachedQRatio {
    template <typename InputIt1>
    CachedQRatio(InputIt1 first1, InputIt1 last1) : s1(first1, last1), cached_ratio(first1, last1)
    {}

    template <typename Sentence1>
    CachedQRatio(const Sentence1& s1) : CachedQRatio(common::to_begin(s1), common::to_end(s1))
    {}

    template <typename InputIt2>
    double similarity(InputIt2 first2, InputIt2 last2, double score_cutoff = 0) const;

    template <typename Sentence2>
    double similarity(const Sentence2& s2, double score_cutoff = 0) const;

private:
    std::basic_string<CharT1> s1;
    CachedRatio<CharT1> cached_ratio;
};

#if __cplusplus >= 201703L
template <typename Sentence1>
CachedQRatio(const Sentence1& s1) -> CachedQRatio<char_type<Sentence1>>;

template <typename InputIt1>
CachedQRatio(InputIt1 first1, InputIt1 last1) -> CachedQRatio<iterator_type<InputIt1>>;
#endif

/**@}*/

} // namespace fuzz
} // namespace rapidfuzz


// The MIT License (MIT)
//
// Copyright (c) 2020 Max Bachmann
// Copyright (c) 2014 Jean-Bernard Jansen
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.



#include <algorithm>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace rapidfuzz {
namespace detail {
struct MatchingBlock {
    int64_t spos;
    int64_t dpos;
    int64_t length;
    MatchingBlock(int64_t aSPos, int64_t aDPos, int64_t aLength)
        : spos(aSPos), dpos(aDPos), length(aLength)
    {}
};

namespace difflib {

template <typename InputIt1, typename InputIt2>
class SequenceMatcher {
public:
    using match_t = std::tuple<int64_t, int64_t, int64_t>;

    SequenceMatcher(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
        : a_first(first1), a_last(last1), b_first(first2), b_last(last2)
    {
        int64_t b_len = std::distance(b_first, b_last);
        j2len_.resize(b_len + 1);
        for (int64_t i = 0; i < b_len; ++i) {
            b2j_[b_first[i]].push_back(i);
        }
    }

    match_t find_longest_match(int64_t a_low, int64_t a_high, int64_t b_low, int64_t b_high)
    {
        int64_t best_i = a_low;
        int64_t best_j = b_low;
        int64_t best_size = 0;

        // Find longest junk free match
        {
            for (int64_t i = a_low; i < a_high; ++i) {
                bool found = false;
                auto iter = b2j_.find(a_first[i]);
                if (iter != std::end(b2j_)) {
                    const auto& indexes = iter->second;

                    size_t pos = 0;
                    int64_t next_val = 0;
                    for (; pos < indexes.size(); pos++) {
                        int64_t j = indexes[pos];
                        if (j < b_low) continue;

                        next_val = j2len_[j];
                        break;
                    }

                    for (; pos < indexes.size(); pos++) {
                        int64_t j = indexes[pos];
                        if (j >= b_high) break;

                        found = true;
                        int64_t k = next_val + 1;

                        /* the next value might be overwritten below
                         * so cache it */
                        if (pos + 1 < indexes.size()) {
                            next_val = j2len_[indexes[pos + 1]];
                        }

                        j2len_[j + 1] = k;
                        if (k > best_size) {
                            best_i = i - k + 1;
                            best_j = j - k + 1;
                            best_size = k;
                        }
                    }
                }

                if (!found) {
                    std::fill(j2len_.begin() + b_low, j2len_.begin() + b_high, 0);
                }
            }

            std::fill(j2len_.begin() + b_low, j2len_.begin() + b_high, 0);
        }

        while (best_i > a_low && best_j > b_low && a_first[best_i - 1] == b_first[best_j - 1]) {
            --best_i;
            --best_j;
            ++best_size;
        }

        while ((best_i + best_size) < a_high && (best_j + best_size) < b_high &&
               a_first[best_i + best_size] == b_first[best_j + best_size])
        {
            ++best_size;
        }

        return std::make_tuple(best_i, best_j, best_size);
    }

    std::vector<MatchingBlock> get_matching_blocks()
    {
        int64_t a_len = std::distance(a_first, a_last);
        int64_t b_len = std::distance(b_first, b_last);
        // The following are tuple extracting aliases
        std::vector<std::tuple<int64_t, int64_t, int64_t, int64_t>> queue;
        std::vector<match_t> matching_blocks_pass1;

        size_t queue_head = 0;
        queue.reserve(std::min(a_len, b_len));
        queue.emplace_back(0, a_len, 0, b_len);

        while (queue_head < queue.size()) {
            int64_t a_low, a_high, b_low, b_high;
            std::tie(a_low, a_high, b_low, b_high) = queue[queue_head++];
            int64_t spos, dpos, length;
            std::tie(spos, dpos, length) = find_longest_match(a_low, a_high, b_low, b_high);
            if (length) {
                if (a_low < spos && b_low < dpos) {
                    queue.emplace_back(a_low, spos, b_low, dpos);
                }
                if ((spos + length) < a_high && (dpos + length) < b_high) {
                    queue.emplace_back(spos + length, a_high, dpos + length, b_high);
                }
                matching_blocks_pass1.emplace_back(spos, dpos, length);
            }
        }
        std::sort(common::to_begin(matching_blocks_pass1), common::to_end(matching_blocks_pass1));

        std::vector<MatchingBlock> matching_blocks;

        matching_blocks.reserve(matching_blocks_pass1.size());

        int64_t i1, j1, k1;
        i1 = j1 = k1 = 0;

        for (match_t const& m : matching_blocks_pass1) {
            if (i1 + k1 == std::get<0>(m) && j1 + k1 == std::get<1>(m)) {
                k1 += std::get<2>(m);
            }
            else {
                if (k1) {
                    matching_blocks.emplace_back(i1, j1, k1);
                }
                std::tie(i1, j1, k1) = m;
            }
        }
        if (k1) {
            matching_blocks.emplace_back(i1, j1, k1);
        }
        matching_blocks.emplace_back(a_len, b_len, 0);

        return matching_blocks;
    }

protected:
    InputIt1 a_first;
    InputIt1 a_last;
    InputIt2 b_first;
    InputIt2 b_last;

private:
    // Cache to avoid reallocations
    std::vector<int64_t> j2len_;
    std::unordered_map<iterator_type<InputIt2>, std::vector<int64_t>> b2j_;
};
} // namespace difflib

template <typename InputIt1, typename InputIt2>
std::vector<MatchingBlock> get_matching_blocks(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                               InputIt2 last2)
{
    return difflib::SequenceMatcher<InputIt1, InputIt2>(first1, last1, first2, last2)
        .get_matching_blocks();
}

} /* namespace detail */
} /* namespace rapidfuzz */

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
    return indel_normalized_similarity(first1, last1, first2, last2, score_cutoff / 100) * 100;
}

template <typename Sentence1, typename Sentence2>
double ratio(const Sentence1& s1, const Sentence2& s2, const double score_cutoff)
{
    return ratio(common::to_begin(s1), common::to_end(s1), common::to_begin(s2), common::to_end(s2),
                 score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2, double score_cutoff) const
{
    double norm_sim = rapidfuzz::detail::indel_normalized_similarity(
        PM, common::to_begin(s1), common::to_end(s1), first2, last2, score_cutoff / 100);
    return norm_sim * 100;
}

template <typename CharT1>
template <typename Sentence2>
double CachedRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

/**********************************************
 *              partial_ratio
 *********************************************/

namespace detail {

template <typename InputIt1, typename InputIt2, typename CachedCharT1>
ScoreAlignment<double>
partial_ratio_short_needle(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                           const CachedRatio<CachedCharT1>& cached_ratio,
                           const common::CharSet<iterator_type<InputIt1>>& s1_char_set,
                           double score_cutoff)
{
    ScoreAlignment<double> res;
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    assert(len2 >= len1);
    res.src_start = 0;
    res.src_end = len1;
    res.dest_start = 0;
    res.dest_end = len1;

    for (int64_t i = 1; i < len1; ++i) {
        auto substr_last = first2 + i;

        if (!s1_char_set.find(*(substr_last - 1))) {
            continue;
        }

        double ls_ratio = cached_ratio.similarity(first2, substr_last, score_cutoff);
        if (ls_ratio > res.score) {
            score_cutoff = res.score = ls_ratio;
            res.dest_start = 0;
            res.dest_end = i;
            if (res.score == 100.0) {
                return res;
            }
        }
    }

    for (int64_t i = 0; i < len2 - len1; ++i) {
        auto substr_first = first2 + i;
        auto substr_last = substr_first + len1;

        if (!s1_char_set.find(*(substr_last - 1))) {
            continue;
        }

        double ls_ratio = cached_ratio.similarity(substr_first, substr_last, score_cutoff);
        if (ls_ratio > res.score) {
            score_cutoff = res.score = ls_ratio;
            res.dest_start = i;
            res.dest_end = i + len1;
            if (res.score == 100.0) {
                return res;
            }
        }
    }

    for (int64_t i = len2 - len1; i < len2; ++i) {
        auto substr_first = first2 + i;

        if (!s1_char_set.find(*substr_first)) {
            continue;
        }

        double ls_ratio = cached_ratio.similarity(substr_first, last2, score_cutoff);
        if (ls_ratio > res.score) {
            score_cutoff = res.score = ls_ratio;
            res.dest_start = i;
            res.dest_end = len2;
            if (res.score == 100.0) {
                return res;
            }
        }
    }

    return res;
}

template <typename InputIt1, typename InputIt2, typename CharT1 = iterator_type<InputIt1>>
ScoreAlignment<double> partial_ratio_short_needle(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                                  InputIt2 last2, double score_cutoff)
{
    CachedRatio<CharT1> cached_ratio(first1, last1);

    common::CharSet<CharT1> s1_char_set;
    int64_t len1 = std::distance(first1, last1);
    for (int64_t i = 0; i < len1; ++i) {
        s1_char_set.insert(first1[i]);
    }

    return partial_ratio_short_needle(first1, last1, first2, last2, cached_ratio, s1_char_set,
                                      score_cutoff);
}

template <typename InputIt1, typename InputIt2, typename CachedCharT1>
ScoreAlignment<double>
partial_ratio_long_needle(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                          const CachedRatio<CachedCharT1>& cached_ratio, double score_cutoff)
{
    ScoreAlignment<double> res;
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);
    assert(len2 >= len1);
    res.src_start = 0;
    res.src_end = len1;
    res.dest_start = 0;
    res.dest_end = len1;

    auto blocks = rapidfuzz::detail::get_matching_blocks(first1, last1, first2, last2);

    // when there is a full match exit early
    for (const auto& block : blocks) {
        if (block.length == len1) {
            res.score = 100;
            res.dest_start = std::max(int64_t(0), block.dpos - block.spos);
            res.dest_end = std::min(len2, res.dest_start + len1);
            return res;
        }
    }

    for (const auto& block : blocks) {
        int64_t long_start = std::max(int64_t(0), block.dpos - block.spos);
        int64_t long_end = std::min(len2, long_start + len1);
        auto substr_first = first2 + long_start;
        auto substr_last = first2 + long_end;

        double ls_ratio = cached_ratio.similarity(substr_first, substr_last, score_cutoff);
        if (ls_ratio > res.score) {
            score_cutoff = res.score = ls_ratio;
            res.dest_start = long_start;
            res.dest_end = long_end;
        }
    }

    return res;
}

template <typename InputIt1, typename InputIt2, typename CharT1 = iterator_type<InputIt1>>
ScoreAlignment<double> partial_ratio_long_needle(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                                 InputIt2 last2, double score_cutoff)
{
    CachedRatio<CharT1> cached_ratio(first1, last1);

    return partial_ratio_long_needle(first1, last1, first2, last2, cached_ratio, score_cutoff);
}

} // namespace detail

template <typename InputIt1, typename InputIt2>
ScoreAlignment<double> partial_ratio_alignment(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                               InputIt2 last2, double score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    if (len1 > len2) {
        ScoreAlignment<double> result =
            partial_ratio_alignment(first2, last2, first1, last1, score_cutoff);
        std::swap(result.src_start, result.dest_start);
        std::swap(result.src_end, result.dest_end);
        return result;
    }

    if (score_cutoff > 100) {
        return ScoreAlignment<double>(0, 0, len1, 0, len1);
    }

    if (!len1 || !len2) {
        return ScoreAlignment<double>(static_cast<double>(len1 == len2) * 100.0, 0, len1, 0, len1);
    }

    if (len1 <= 64) {
        return detail::partial_ratio_short_needle(first1, last1, first2, last2, score_cutoff);
    }
    else {
        return detail::partial_ratio_long_needle(first1, last1, first2, last2, score_cutoff);
    }
}

template <typename Sentence1, typename Sentence2>
ScoreAlignment<double> partial_ratio_alignment(const Sentence1& s1, const Sentence2& s2,
                                               double score_cutoff)
{
    return partial_ratio_alignment(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                                   common::to_end(s2), score_cutoff);
}

template <typename InputIt1, typename InputIt2>
double partial_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                     double score_cutoff)
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
    for (const auto& ch : s1) {
        s1_char_set.insert(ch);
    }
}

template <typename CharT1>
template <typename InputIt2>
double CachedPartialRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2,
                                              double score_cutoff) const
{
    int64_t len1 = (int64_t)s1.size();
    int64_t len2 = std::distance(first2, last2);

    if (len1 > len2) {
        return partial_ratio(common::to_begin(s1), common::to_end(s1), first2, last2, score_cutoff);
    }

    if (score_cutoff > 100) {
        return 0;
    }

    if (!len1 || !len2) {
        return static_cast<double>(len1 == len2) * 100.0;
    }

    if (len1 <= 64) {
        return detail::partial_ratio_short_needle(common::to_begin(s1), common::to_end(s1), first2,
                                                  last2, cached_ratio, s1_char_set, score_cutoff)
            .score;
    }
    else {
        return detail::partial_ratio_long_needle(common::to_begin(s1), common::to_end(s1), first2,
                                                 last2, cached_ratio, score_cutoff)
            .score;
    }
}

template <typename CharT1>
template <typename Sentence2>
double CachedPartialRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

/**********************************************
 *             token_sort_ratio
 *********************************************/
template <typename InputIt1, typename InputIt2>
double token_sort_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                        double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    return ratio(common::sorted_split(first1, last1).join(),
                 common::sorted_split(first2, last2).join(), score_cutoff);
}

template <typename Sentence1, typename Sentence2, typename CharT1, typename CharT2>
double token_sort_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return token_sort_ratio(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                            common::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedTokenSortRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2,
                                                double score_cutoff) const
{
    if (score_cutoff > 100) return 0;

    return cached_ratio.similarity(common::sorted_split(first2, last2).join(), score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedTokenSortRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

/**********************************************
 *          partial_token_sort_ratio
 *********************************************/

template <typename InputIt1, typename InputIt2>
double partial_token_sort_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                                double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    return partial_ratio(common::sorted_split(first1, last1).join(),
                         common::sorted_split(first2, last2).join(), score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double partial_token_sort_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return partial_token_sort_ratio(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                                    common::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedPartialTokenSortRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2,
                                                       double score_cutoff) const
{
    if (score_cutoff > 100) return 0;

    return cached_partial_ratio.similarity(common::sorted_split(first2, last2).join(),
                                           score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedPartialTokenSortRatio<CharT1>::similarity(const Sentence2& s2,
                                                       double score_cutoff) const
{
    return similarity(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

/**********************************************
 *               token_set_ratio
 *********************************************/

namespace detail {
template <typename InputIt1, typename InputIt2>
double token_set_ratio(const SplittedSentenceView<InputIt1>& tokens_a,
                       const SplittedSentenceView<InputIt2>& tokens_b, const double score_cutoff)
{
    /* in FuzzyWuzzy this returns 0. For sake of compatibility return 0 here as well
     * see https://github.com/maxbachmann/RapidFuzz/issues/110 */
    if (tokens_a.empty() || tokens_b.empty()) {
        return 0;
    }

    auto decomposition = common::set_decomposition(tokens_a, tokens_b);
    auto intersect = decomposition.intersection;
    auto diff_ab = decomposition.difference_ab;
    auto diff_ba = decomposition.difference_ba;

    // one sentence is part of the other one
    if (!intersect.empty() && (diff_ab.empty() || diff_ba.empty())) {
        return 100;
    }

    auto diff_ab_joined = diff_ab.join();
    auto diff_ba_joined = diff_ba.join();

    int64_t ab_len = diff_ab_joined.length();
    int64_t ba_len = diff_ba_joined.length();
    int64_t sect_len = intersect.length();

    // string length sect+ab <-> sect and sect+ba <-> sect
    int64_t sect_ab_len = sect_len + bool(sect_len) + ab_len;
    int64_t sect_ba_len = sect_len + bool(sect_len) + ba_len;

    double result = 0;
    auto cutoff_distance = common::score_cutoff_to_distance<100>(score_cutoff, ab_len + ba_len);
    int64_t dist = indel_distance(diff_ab_joined, diff_ba_joined, cutoff_distance);

    if (dist <= cutoff_distance) {
        result = common::norm_distance<100>(dist, sect_ab_len + sect_ba_len, score_cutoff);
    }

    // exit early since the other ratios are 0
    if (!sect_len) {
        return result;
    }

    // levenshtein distance sect+ab <-> sect and sect+ba <-> sect
    // since only sect is similar in them the distance can be calculated based on
    // the length difference
    int64_t sect_ab_dist = bool(sect_len) + ab_len;
    double sect_ab_ratio =
        common::norm_distance<100>(sect_ab_dist, sect_len + sect_ab_len, score_cutoff);

    int64_t sect_ba_dist = bool(sect_len) + ba_len;
    double sect_ba_ratio =
        common::norm_distance<100>(sect_ba_dist, sect_len + sect_ba_len, score_cutoff);

    return std::max({result, sect_ab_ratio, sect_ba_ratio});
}
} // namespace detail

template <typename InputIt1, typename InputIt2>
double token_set_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                       double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    return detail::token_set_ratio(common::sorted_split(first1, last1),
                                   common::sorted_split(first2, last2), score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double token_set_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return token_set_ratio(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                           common::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedTokenSetRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2,
                                               double score_cutoff) const
{
    if (score_cutoff > 100) return 0;

    return detail::token_set_ratio(tokens_s1, common::sorted_split(first2, last2), score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedTokenSetRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

/**********************************************
 *          partial_token_set_ratio
 *********************************************/

namespace detail {
template <typename InputIt1, typename InputIt2>
double partial_token_set_ratio(const SplittedSentenceView<InputIt1>& tokens_a,
                               const SplittedSentenceView<InputIt2>& tokens_b,
                               const double score_cutoff)
{
    /* in FuzzyWuzzy this returns 0. For sake of compatibility return 0 here as well
     * see https://github.com/maxbachmann/RapidFuzz/issues/110 */
    if (tokens_a.empty() || tokens_b.empty()) {
        return 0;
    }

    auto decomposition = common::set_decomposition(tokens_a, tokens_b);

    // exit early when there is a common word in both sequences
    if (!decomposition.intersection.empty()) return 100;

    return partial_ratio(decomposition.difference_ab.join(), decomposition.difference_ba.join(),
                         score_cutoff);
}
} // namespace detail

template <typename InputIt1, typename InputIt2>
double partial_token_set_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                               double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    return detail::partial_token_set_ratio(common::sorted_split(first1, last1),
                                           common::sorted_split(first2, last2), score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double partial_token_set_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return partial_token_set_ratio(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                                   common::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedPartialTokenSetRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2,
                                                      double score_cutoff) const
{
    if (score_cutoff > 100) return 0;

    return detail::partial_token_set_ratio(tokens_s1, common::sorted_split(first2, last2),
                                           score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedPartialTokenSetRatio<CharT1>::similarity(const Sentence2& s2,
                                                      double score_cutoff) const
{
    return similarity(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

/**********************************************
 *                token_ratio
 *********************************************/

template <typename InputIt1, typename InputIt2>
double token_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                   double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    auto tokens_a = common::sorted_split(first1, last1);
    auto tokens_b = common::sorted_split(first2, last2);

    auto decomposition = common::set_decomposition(tokens_a, tokens_b);
    auto intersect = decomposition.intersection;
    auto diff_ab = decomposition.difference_ab;
    auto diff_ba = decomposition.difference_ba;

    if (!intersect.empty() && (diff_ab.empty() || diff_ba.empty())) {
        return 100;
    }

    auto diff_ab_joined = diff_ab.join();
    auto diff_ba_joined = diff_ba.join();

    int64_t ab_len = diff_ab_joined.length();
    int64_t ba_len = diff_ba_joined.length();
    int64_t sect_len = intersect.length();

    double result = ratio(tokens_a.join(), tokens_b.join(), score_cutoff);

    // string length sect+ab <-> sect and sect+ba <-> sect
    int64_t sect_ab_len = sect_len + bool(sect_len) + ab_len;
    int64_t sect_ba_len = sect_len + bool(sect_len) + ba_len;

    auto cutoff_distance = common::score_cutoff_to_distance<100>(score_cutoff, ab_len + ba_len);
    int64_t dist = indel_distance(diff_ab_joined, diff_ba_joined, cutoff_distance);
    if (dist <= cutoff_distance) {
        result = std::max(
            result, common::norm_distance<100>(dist, sect_ab_len + sect_ba_len, score_cutoff));
    }

    // exit early since the other ratios are 0
    if (!sect_len) {
        return result;
    }

    // levenshtein distance sect+ab <-> sect and sect+ba <-> sect
    // since only sect is similar in them the distance can be calculated based on
    // the length difference
    int64_t sect_ab_dist = bool(sect_len) + ab_len;
    double sect_ab_ratio =
        common::norm_distance<100>(sect_ab_dist, sect_len + sect_ab_len, score_cutoff);

    int64_t sect_ba_dist = bool(sect_len) + ba_len;
    double sect_ba_ratio =
        common::norm_distance<100>(sect_ba_dist, sect_len + sect_ba_len, score_cutoff);

    return std::max({result, sect_ab_ratio, sect_ba_ratio});
}

template <typename Sentence1, typename Sentence2>
double token_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return token_ratio(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                       common::to_end(s2), score_cutoff);
}

namespace detail {
template <typename CharT1, typename CachedCharT1, typename InputIt2>
double token_ratio(const SplittedSentenceView<CharT1>& s1_tokens,
                   const CachedRatio<CachedCharT1>& cached_ratio_s1_sorted, InputIt2 first2,
                   InputIt2 last2, double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    auto s2_tokens = common::sorted_split(first2, last2);

    auto decomposition = common::set_decomposition(s1_tokens, s2_tokens);
    auto intersect = decomposition.intersection;
    auto diff_ab = decomposition.difference_ab;
    auto diff_ba = decomposition.difference_ba;

    if (!intersect.empty() && (diff_ab.empty() || diff_ba.empty())) {
        return 100;
    }

    auto diff_ab_joined = diff_ab.join();
    auto diff_ba_joined = diff_ba.join();

    int64_t ab_len = diff_ab_joined.length();
    int64_t ba_len = diff_ba_joined.length();
    int64_t sect_len = intersect.length();

    double result = cached_ratio_s1_sorted.similarity(s2_tokens.join(), score_cutoff);

    // string length sect+ab <-> sect and sect+ba <-> sect
    int64_t sect_ab_len = sect_len + bool(sect_len) + ab_len;
    int64_t sect_ba_len = sect_len + bool(sect_len) + ba_len;

    auto cutoff_distance = common::score_cutoff_to_distance<100>(score_cutoff, ab_len + ba_len);
    int64_t dist = indel_distance(diff_ab_joined, diff_ba_joined, cutoff_distance);
    if (dist <= cutoff_distance) {
        result = std::max(
            result, common::norm_distance<100>(dist, sect_ab_len + sect_ba_len, score_cutoff));
    }

    // exit early since the other ratios are 0
    if (!sect_len) {
        return result;
    }

    // levenshtein distance sect+ab <-> sect and sect+ba <-> sect
    // since only sect is similar in them the distance can be calculated based on
    // the length difference
    int64_t sect_ab_dist = bool(sect_len) + ab_len;
    double sect_ab_ratio =
        common::norm_distance<100>(sect_ab_dist, sect_len + sect_ab_len, score_cutoff);

    int64_t sect_ba_dist = bool(sect_len) + ba_len;
    double sect_ba_ratio =
        common::norm_distance<100>(sect_ba_dist, sect_len + sect_ba_len, score_cutoff);

    return std::max({result, sect_ab_ratio, sect_ba_ratio});
}

// todo this is a temporary solution until WRatio is properly implemented using other scorers
template <typename CharT1, typename InputIt1, typename InputIt2>
double token_ratio(const std::basic_string<CharT1>& s1_sorted,
                   const SplittedSentenceView<InputIt1>& tokens_s1,
                   const common::BlockPatternMatchVector& blockmap_s1_sorted, InputIt2 first2,
                   InputIt2 last2, double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    auto tokens_b = common::sorted_split(first2, last2);

    auto decomposition = common::set_decomposition(tokens_s1, tokens_b);
    auto intersect = decomposition.intersection;
    auto diff_ab = decomposition.difference_ab;
    auto diff_ba = decomposition.difference_ba;

    if (!intersect.empty() && (diff_ab.empty() || diff_ba.empty())) {
        return 100;
    }

    auto diff_ab_joined = diff_ab.join();
    auto diff_ba_joined = diff_ba.join();

    int64_t ab_len = diff_ab_joined.length();
    int64_t ba_len = diff_ba_joined.length();
    int64_t sect_len = intersect.length();

    double result = 0;
    auto s2_sorted = tokens_b.join();
    if (s1_sorted.size() < 65) {
        double norm_sim = rapidfuzz::detail::indel_normalized_similarity(
            blockmap_s1_sorted, common::to_begin(s1_sorted), common::to_end(s1_sorted),
            common::to_begin(s2_sorted), common::to_end(s2_sorted), score_cutoff / 100);
        result = norm_sim * 100;
    }
    else {
        result = fuzz::ratio(s1_sorted, s2_sorted, score_cutoff);
    }

    // string length sect+ab <-> sect and sect+ba <-> sect
    int64_t sect_ab_len = sect_len + bool(sect_len) + ab_len;
    int64_t sect_ba_len = sect_len + bool(sect_len) + ba_len;

    auto cutoff_distance = common::score_cutoff_to_distance<100>(score_cutoff, ab_len + ba_len);
    int64_t dist = indel_distance(diff_ab_joined, diff_ba_joined, cutoff_distance);
    if (dist <= cutoff_distance) {
        result = std::max(
            result, common::norm_distance<100>(dist, sect_ab_len + sect_ba_len, score_cutoff));
    }

    // exit early since the other ratios are 0
    if (!sect_len) {
        return result;
    }

    // levenshtein distance sect+ab <-> sect and sect+ba <-> sect
    // since only sect is similar in them the distance can be calculated based on
    // the length difference
    int64_t sect_ab_dist = bool(sect_len) + ab_len;
    double sect_ab_ratio =
        common::norm_distance<100>(sect_ab_dist, sect_len + sect_ab_len, score_cutoff);

    int64_t sect_ba_dist = bool(sect_len) + ba_len;
    double sect_ba_ratio =
        common::norm_distance<100>(sect_ba_dist, sect_len + sect_ba_len, score_cutoff);

    return std::max({result, sect_ab_ratio, sect_ba_ratio});
}
} // namespace detail

template <typename CharT1>
template <typename InputIt2>
double CachedTokenRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2,
                                            double score_cutoff) const
{
    return detail::token_ratio(s1_tokens, cached_ratio_s1_sorted, first2, last2, score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedTokenRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

/**********************************************
 *            partial_token_ratio
 *********************************************/

template <typename InputIt1, typename InputIt2>
double partial_token_ratio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,
                           double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    auto tokens_a = common::sorted_split(first1, last1);
    auto tokens_b = common::sorted_split(first2, last2);

    auto decomposition = common::set_decomposition(tokens_a, tokens_b);

    // exit early when there is a common word in both sequences
    if (!decomposition.intersection.empty()) return 100;

    auto diff_ab = decomposition.difference_ab;
    auto diff_ba = decomposition.difference_ba;

    double result = partial_ratio(tokens_a.join(), tokens_b.join(), score_cutoff);

    // do not calculate the same partial_ratio twice
    if (tokens_a.word_count() == diff_ab.word_count() &&
        tokens_b.word_count() == diff_ba.word_count()) {
        return result;
    }

    score_cutoff = std::max(score_cutoff, result);
    return std::max(result, partial_ratio(diff_ab.join(), diff_ba.join(), score_cutoff));
}

template <typename Sentence1, typename Sentence2>
double partial_token_ratio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return partial_token_ratio(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                               common::to_end(s2), score_cutoff);
}

namespace detail {
template <typename CharT1, typename InputIt1, typename InputIt2>
double partial_token_ratio(const std::basic_string<CharT1>& s1_sorted,
                           const SplittedSentenceView<InputIt1>& tokens_s1, InputIt2 first2,
                           InputIt2 last2, double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    auto tokens_b = common::sorted_split(first2, last2);

    auto decomposition = common::set_decomposition(tokens_s1, tokens_b);

    // exit early when there is a common word in both sequences
    if (!decomposition.intersection.empty()) return 100;

    auto diff_ab = decomposition.difference_ab;
    auto diff_ba = decomposition.difference_ba;

    double result = partial_ratio(s1_sorted, tokens_b.join(), score_cutoff);

    // do not calculate the same partial_ratio twice
    if (tokens_s1.word_count() == diff_ab.word_count() &&
        tokens_b.word_count() == diff_ba.word_count()) {
        return result;
    }

    score_cutoff = std::max(score_cutoff, result);
    return std::max(result, partial_ratio(diff_ab.join(), diff_ba.join(), score_cutoff));
}

} // namespace detail

template <typename CharT1>
template <typename InputIt2>
double CachedPartialTokenRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2,
                                                   double score_cutoff) const
{
    return detail::partial_token_ratio(s1_sorted, tokens_s1, first2, last2, score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedPartialTokenRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

/**********************************************
 *                  WRatio
 *********************************************/

template <typename InputIt1, typename InputIt2>
double WRatio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, double score_cutoff)
{
    if (score_cutoff > 100) return 0;

    constexpr double UNBASE_SCALE = 0.95;

    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    /* in FuzzyWuzzy this returns 0. For sake of compatibility return 0 here as well
     * see https://github.com/maxbachmann/RapidFuzz/issues/110 */
    if (!len1 || !len2) {
        return 0;
    }

    double len_ratio = (len1 > len2) ? static_cast<double>(len1) / static_cast<double>(len2)
                                     : static_cast<double>(len2) / static_cast<double>(len1);

    double end_ratio = ratio(first1, last1, first2, last2, score_cutoff);

    if (len_ratio < 1.5) {
        score_cutoff = std::max(score_cutoff, end_ratio) / UNBASE_SCALE;
        return std::max(end_ratio,
                        token_ratio(first1, last1, first2, last2, score_cutoff) * UNBASE_SCALE);
    }

    const double PARTIAL_SCALE = (len_ratio < 8.0) ? 0.9 : 0.6;

    score_cutoff = std::max(score_cutoff, end_ratio) / PARTIAL_SCALE;
    end_ratio = std::max(end_ratio,
                         partial_ratio(first1, last1, first2, last2, score_cutoff) * PARTIAL_SCALE);

    score_cutoff = std::max(score_cutoff, end_ratio) / UNBASE_SCALE;
    return std::max(end_ratio, partial_token_ratio(first1, last1, first2, last2, score_cutoff) *
                                   UNBASE_SCALE * PARTIAL_SCALE);
}

template <typename Sentence1, typename Sentence2>
double WRatio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return WRatio(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                  common::to_end(s2), score_cutoff);
}

template <typename Sentence1>
template <typename InputIt1>
CachedWRatio<Sentence1>::CachedWRatio(InputIt1 first1, InputIt1 last1)
    : s1(first1, last1),
      cached_partial_ratio(first1, last1),
      tokens_s1(common::sorted_split(std::begin(s1), std::end(s1))),
      s1_sorted(tokens_s1.join())
{
    blockmap_s1_sorted.insert(std::begin(s1_sorted), std::end(s1_sorted));
}

template <typename CharT1>
template <typename InputIt2>
double CachedWRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2, double score_cutoff) const
{
    if (score_cutoff > 100) return 0;

    constexpr double UNBASE_SCALE = 0.95;

    int64_t len1 = s1.size();
    int64_t len2 = std::distance(first2, last2);

    /* in FuzzyWuzzy this returns 0. For sake of compatibility return 0 here as well
     * see https://github.com/maxbachmann/RapidFuzz/issues/110 */
    if (!len1 || !len2) {
        return 0;
    }

    double len_ratio = (len1 > len2) ? static_cast<double>(len1) / static_cast<double>(len2)
                                     : static_cast<double>(len2) / static_cast<double>(len1);

    double end_ratio = cached_partial_ratio.cached_ratio.similarity(first2, last2, score_cutoff);

    if (len_ratio < 1.5) {
        score_cutoff = std::max(score_cutoff, end_ratio) / UNBASE_SCALE;
        // use pre calculated values
        auto r = detail::token_ratio(s1_sorted, tokens_s1, blockmap_s1_sorted, first2, last2,
                                     score_cutoff);
        return std::max(end_ratio, r * UNBASE_SCALE);
    }

    const double PARTIAL_SCALE = (len_ratio < 8.0) ? 0.9 : 0.6;

    score_cutoff = std::max(score_cutoff, end_ratio) / PARTIAL_SCALE;
    end_ratio = std::max(end_ratio, cached_partial_ratio.similarity(first2, last2, score_cutoff) *
                                        PARTIAL_SCALE);

    score_cutoff = std::max(score_cutoff, end_ratio) / UNBASE_SCALE;
    auto r = detail::partial_token_ratio(s1_sorted, tokens_s1, first2, last2, score_cutoff);
    return std::max(end_ratio, r * UNBASE_SCALE * PARTIAL_SCALE);
}

template <typename CharT1>
template <typename Sentence2>
double CachedWRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

/**********************************************
 *                QRatio
 *********************************************/

template <typename InputIt1, typename InputIt2>
double QRatio(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, double score_cutoff)
{
    int64_t len1 = std::distance(first1, last1);
    int64_t len2 = std::distance(first2, last2);

    /* in FuzzyWuzzy this returns 0. For sake of compatibility return 0 here as well
     * see https://github.com/maxbachmann/RapidFuzz/issues/110 */
    if (!len1 || !len2) {
        return 0;
    }

    return ratio(first1, last1, first2, last2, score_cutoff);
}

template <typename Sentence1, typename Sentence2>
double QRatio(const Sentence1& s1, const Sentence2& s2, double score_cutoff)
{
    return QRatio(common::to_begin(s1), common::to_end(s1), common::to_begin(s2),
                  common::to_end(s2), score_cutoff);
}

template <typename CharT1>
template <typename InputIt2>
double CachedQRatio<CharT1>::similarity(InputIt2 first2, InputIt2 last2, double score_cutoff) const
{
    int64_t len2 = std::distance(first2, last2);

    /* in FuzzyWuzzy this returns 0. For sake of compatibility return 0 here as well
     * see https://github.com/maxbachmann/RapidFuzz/issues/110 */
    if (s1.empty() || !len2) {
        return 0;
    }

    return cached_ratio.similarity(first2, last2, score_cutoff);
}

template <typename CharT1>
template <typename Sentence2>
double CachedQRatio<CharT1>::similarity(const Sentence2& s2, double score_cutoff) const
{
    return similarity(common::to_begin(s2), common::to_end(s2), score_cutoff);
}

} // namespace fuzz
} // namespace rapidfuzz
#endif // RAPIDFUZZ_AMALGAMATED_HPP_INCLUDED
