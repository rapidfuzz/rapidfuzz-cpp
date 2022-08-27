/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2022 Max Bachmann */

#pragma once
#include <algorithm>
#include <cassert>
#include <stdio.h>

namespace rapidfuzz {
namespace detail {

template <typename T, bool IsConst>
struct MatrixVectorView {

    using value_type = T;
    using size_type = size_t;
    using pointer = std::conditional_t<IsConst, const value_type*, value_type*>;
    using reference = std::conditional_t<IsConst, const value_type&, value_type&>;

    MatrixVectorView(pointer vector, size_type cols) noexcept : m_vector(vector), m_cols(cols)
    {}

    bool test_bit(size_type bit) const noexcept
    {
        size_t word_size = sizeof(value_type) * 8;
        size_t word = bit / word_size;
        bit = bit % word_size;
        value_type mask = value_type(1) << bit;

        assert(word < m_cols);
        return bool(m_vector[word] & mask);
    }

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
struct Matrix {

    using value_type = T;

    Matrix() : m_rows(0), m_cols(0), m_matrix(nullptr)
    {}

    Matrix(size_t rows, size_t cols, T val) : m_rows(rows), m_cols(cols), m_matrix(nullptr)
    {
        if (m_rows && m_cols) m_matrix = new T[m_rows * m_cols];
        std::fill_n(m_matrix, m_rows * m_cols, val);
    }

    Matrix(const Matrix& other) : m_rows(other.m_rows), m_cols(other.m_cols), m_matrix(nullptr)
    {
        if (m_rows && m_cols) m_matrix = new T[m_rows * m_cols];
        std::copy(other.m_matrix, other.m_matrix + m_rows * m_cols, m_matrix);
    }

    Matrix(Matrix&& other) noexcept : m_rows(0), m_cols(0), m_matrix(nullptr)
    {
        other.swap(*this);
    }

    Matrix& operator=(Matrix&& other) noexcept
    {
        Matrix temp = other;
        temp.swap(*this);
        return *this;
    }

    Matrix& operator=(const Matrix& other)
    {
        other.swap(*this);
        return *this;
    }

    void swap(Matrix& rhs) noexcept
    {
        using std::swap;
        swap(m_rows, rhs.m_rows);
        swap(m_cols, rhs.m_cols);
        swap(m_matrix, rhs.m_matrix);
    }

    ~Matrix()
    {
        delete[] m_matrix;
    }

    MatrixVectorView<value_type, false> operator[](size_t row) noexcept
    {
        assert(row < m_rows);
        return {&m_matrix[row * m_cols], m_cols};
    }

    MatrixVectorView<value_type, true> operator[](size_t row) const noexcept
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

} // namespace detail
} // namespace rapidfuzz