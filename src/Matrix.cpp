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

#include <psc_i18n.hpp>
#include <psc_format.hpp>

#include "Matrix.hpp"

namespace psc::mat
{


template<typename T>
MatrixV<T>::MatrixV(size_t rows)
: MatrixV(rows, rows+1)  // create default layout for system of linear equations
{
}

template<typename T>
MatrixV<T>::MatrixV(size_t rows, size_t cols)
: m_rows{rows}
, m_cols{cols}
, m_elem(rows * cols, static_cast<T>(0))
{
}

template<typename T> T*
MatrixV<T>::operator[](size_t row)
{
    if (row >= m_rows) {
        throw std::invalid_argument(psc::fmt::vformat(
                _("Matrix row index {} exceeds limit {}")
                , psc::fmt::make_format_args(row, m_rows)));
    }
    return &m_elem[row * m_cols];
}

template<typename T> void
MatrixV<T>::swapRow(size_t a, size_t b)
{
    auto ra = operator[](a);
    auto rb = operator[](b);
    for (size_t l = 0; l < m_cols; ++l) {
        std::swap(ra[l], rb[l]);
    }
}

template<typename T> T
MatrixV<T>::get(size_t row, size_t col)
{
    return m_elem[row * m_cols + col];
}

template<typename T> void
MatrixV<T>::set(size_t row, size_t col, T val)
{
    m_elem[row * m_cols + col] = val;
}

template<typename T> size_t
MatrixV<T>::getColumns()
{
    return m_cols;
}

template<typename T> size_t
MatrixV<T>::getRows()
{
    return m_rows;
}

// should be useful with these
template class MatrixV<double>;
template class MatrixV<float>;

template<typename T>
MatrixU<T>::MatrixU(size_t rows)
: MatrixU(rows, rows+1)
{
}

template<typename T>
MatrixU<T>::MatrixU(size_t rows, size_t cols)
: m_rows{rows}
, m_cols{cols}
, m_elem{std::make_unique<T[]>(rows*cols)}
{
    //make_unique cares for init :)
}

template<typename T> T*
MatrixU<T>::operator[](size_t row)
{
    if (row >= m_rows) {
        throw std::invalid_argument(psc::fmt::vformat(
                _("Matrix row index {} exceeds limit {}")
                , psc::fmt::make_format_args(row, m_rows)));
    }
    return &m_elem[row * m_cols];
}

template<typename T> T
MatrixU<T>::get(size_t row, size_t col)
{
    return m_elem[row * m_cols + col];
}

template<typename T> void
MatrixU<T>::set(size_t row, size_t col, T val)
{
    m_elem[row * m_cols + col] = val;
}

template<typename T> size_t
MatrixU<T>::getColumns()
{
    return m_cols;
}

template<typename T> size_t
MatrixU<T>::getRows()
{
    return m_rows;
}

template<typename T> void
MatrixU<T>::swapRow(size_t a, size_t b) {
    auto ra = operator[](a);
    auto rb = operator[](b);
    for (size_t l = 0; l < m_cols; ++l) {
        std::swap(ra[l], rb[l]);
    }
}


// should be useful with these
template class MatrixU<double>;
template class MatrixU<float>;



// simple evaluation
void
Gauss::eliminate(Matrix<double>& m)
{
    if (m.getColumns() != m.getRows() + 1) {
        auto cols = m.getColumns();
        auto rows = m.getRows();
        throw std::invalid_argument(psc::fmt::vformat(
                _("Matrix cols {} must be rows {}+1")
                , psc::fmt::make_format_args(cols, rows)));
    }
    // following wikipedia to avoid numeric instability (but may still have issues, but it is as far as it is feasible at the moment)
    // https://en.wikipedia.org/wiki/Gaussian_elimination
	size_t h = 0; /* Initialization of the pivot row */
	size_t k = 0; /* Initialization of the pivot column */
	while (h < m.getRows() && k < m.getColumns()) {
	    /* Find the k-th pivot: */
	    size_t i_max = 0;
	    double max = 0.0;
        for (size_t i = h; i < m.getRows(); ++i) {
            double abs = std::abs(m[i][k]);
            if (abs > max) {
                max = abs;
                i_max = i;
            }
	    }
	    if (m[i_max][k] == 0.0) {
            // as we decided to provied the exact number of cols,rows so they should be <> 0
            throw std::invalid_argument(psc::fmt::vformat(
                    _("Value for col {} row {} is 0, matrix not solveable.")
                    , psc::fmt::make_format_args(i_max, k)));
            //++k;    /* No pivot in this column, pass to next column, or throw exception? */
	    }
	    else {
            m.swapRow(h, i_max);
            /* Do for all rows below pivot: */
            for (size_t i = h + 1; i < m.getRows(); ++i) {
                double f = m[i][k] / m[h][k];   // mat[i][k] might be 0, so f will also, continue and get Nan on output?
                /* Fill with zeros the lower part of pivot column: */
                m[i][k] = 0.0;
                /* Do for all remaining elements in current row: */
                for (size_t j = k + 1; j < m.getColumns(); ++j) {
                    m[i][j] = m[i][j] - m[h][j] * f;
                }
            }
            ++h;    /* Increase pivot row and column */
            ++k;
	    }
	}
	// back substitution
	for (size_t row = m.getRows()-1; row < m.getRows(); --row) {  // the condition would be >= 0 but as this is unsigned expect overflow
	    double sum = 0.0;
	    for (size_t col = row+1; col < m.getRows(); ++col) {
            sum += m[row][col] * m[col][m.getColumns()-1];
            m[row][col] = 0.0;	// as we canceling these out
	    }
	    m[row][m.getColumns()-1] =  (m[row][m.getColumns()-1] - sum) / m[row][row];
	    m[row][row] = 1.0;
	}
}


} /* namespace psc::mat */
