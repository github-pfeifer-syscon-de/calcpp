/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2024 RPf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <vector>
#include <array>
#include <memory>
#include <stdexcept>
#include <glibmm.h>

#include <span>

namespace psc::mat
{


// this is basically a std::span
//   but using exceptions in case of a out of range access
template<typename T>
class Row
{
public:
    Row(T* ptr, size_t cols);
    ~Row() = default;

    T& operator[](size_t col);  // rhs
    const T operator[](size_t col) const;  // lhs

protected:
    T* m_ptr;
    size_t m_cols;
};


template<typename T>
class Matrix
{
public:
    virtual ~Matrix() = default;

    // with c++23 we may use operator[](size_t row, size_t col)
    virtual Row<T> operator[](size_t row) = 0;
    // compound (fortran) style access
    virtual T& operator()(size_t row, size_t col) = 0;
    virtual T get(size_t row, size_t col) = 0;
    virtual void set(size_t row, size_t col, T val) = 0;
    virtual size_t getColumns() = 0;
    virtual size_t getRows() = 0;
    virtual void swapRow(size_t a, size_t b) = 0;
protected:
private:
};


// try to reduce overhead to MatrixV as we don't need option to change size
template<typename T>
class MatrixU
: public Matrix<T>
{
public:
    MatrixU(size_t rows);
    MatrixU(size_t rows, size_t cols);
    explicit MatrixU(const MatrixU& orig) = delete;
    virtual ~MatrixU() = default;

    Row<T> operator[](size_t row) override;
    T& operator()(size_t row, size_t col) override;
    T get(size_t row, size_t col) override;
    void set(size_t row, size_t col, T val) override;
    size_t getColumns() override;
    size_t getRows() override;
    void swapRow(size_t a, size_t b) override;

protected:
private:
    size_t m_rows;
    size_t m_cols;
    std::unique_ptr<T[]> m_elem;
};



// leave this as reference, usable if values are known beforehand
template<typename T, size_t t_rows, size_t t_cols>
class MatrixA
: public Matrix<T>
{
public:
    MatrixA()
    {
    }
    explicit MatrixA(const MatrixA& orig) = delete;
    virtual ~MatrixA() = default;

    T* operator[](size_t row) {
        if (row >= t_rows) {
            throw std::invalid_argument(Glib::ustring::sprintf("Matrix row index %ld exceeds limit %ld", row, t_rows));
        }
        return &m_elem[row * t_cols];
    }

    T get(size_t row, size_t col) {
        return m_elem[row * t_cols + col];
    }
    void set(size_t row, size_t col, T val) {
        m_elem[row * t_cols + col] = val;
    }
    size_t getColumns() {
        return t_cols;
    }
    size_t getRows() {
        return t_rows;
    }
    void swapRow(size_t a, size_t b) {
        auto ra = operator[](a);
        auto rb = operator[](b);
        for (size_t l = 0; l < t_cols; ++l) {
            T temp = ra[l];
            ra[l] = rb[l];
            rb[l] = temp;
        }
    }

protected:
private:
    std::array<T, t_rows * t_cols> m_elem;
};

class Gauss {
public:
    static void eliminate(Matrix<double>& m);

};


} /* namespace psc::mat */

