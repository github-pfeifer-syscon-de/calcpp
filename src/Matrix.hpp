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


namespace psc {
namespace mat {

template<typename T>
class Matrix
{
public:
    virtual ~Matrix() = default;

    virtual T* operator[](size_t row) = 0;
    virtual T get(size_t row, size_t col) = 0;
    virtual void set(size_t row, size_t col, T val) = 0;
    virtual size_t getColumns() = 0;
    virtual size_t getRows() = 0;
    virtual void swapRow(size_t a, size_t b) = 0;
protected:
private:
};


// allows compact storage
template<typename T>
class MatrixV
: public Matrix<T>
{
public:
    MatrixV(size_t rows);
    MatrixV(size_t rows, size_t cols);
    explicit MatrixV(const MatrixV& orig) = delete;
    virtual ~MatrixV() = default;

    T* operator[](size_t row);
    T get(size_t row, size_t col);
    void set(size_t row, size_t col, T val);
    size_t getColumns();
    size_t getRows();
    void swapRow(size_t a, size_t b);
protected:
private:
    size_t m_rows;
    size_t m_cols;
    std::vector<T> m_elem;
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

    T* operator[](size_t row);
    T get(size_t row, size_t col);
    void set(size_t row, size_t col, T val);
    size_t getColumns();
    size_t getRows();
    void swapRow(size_t a, size_t b);

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

} /* namespace mat */
} /* namespace psc */
