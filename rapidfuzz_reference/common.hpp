/* SPDX-License-Identifier: MIT */
/* Copyright © 2021 Max Bachmann */

#pragma once
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace rapidfuzz_reference {

template <typename T>
class Matrix {
public:
    Matrix(size_t _rows, size_t _cols) : rows(_rows), cols(_cols)
    {
        matrix = new T[rows * cols];
        std::fill(matrix, matrix + rows * cols, T());
    }

    ~Matrix()
    {
        delete[] matrix;
    }

    T& operator()(ptrdiff_t row, ptrdiff_t col)
    {
        return matrix[static_cast<size_t>(row) + static_cast<size_t>(col) * rows];
    }

    T& back()
    {
        return matrix[rows * cols - 1];
    }

    size_t rows;
    size_t cols;
    T* matrix;
};

} // namespace rapidfuzz_reference
