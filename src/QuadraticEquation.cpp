/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf
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

#include <cmath>

#include "QuadraticEquation.hpp"

namespace psc::math
{

template<typename T> void
QuadraticEquation<T>::setA(T a)
{
    this->a = a;
}

template<typename T> void
QuadraticEquation<T>::setB(T b)
{
    this->b = b;
}

template<typename T> void
QuadraticEquation<T>::setC(T c)
{
    this->c = c;
}

template<typename T> T
QuadraticEquation<T>::getInnerRoot()
{
    return b * b - static_cast<T>(4.0) * a * c;
}

template<typename T> bool
QuadraticEquation<T>::isRootPositive()
{
    return getInnerRoot() >= static_cast<T>(0.0);
}

template<typename T> T
QuadraticEquation<T>::getX1()
{
    auto sqrt = std::sqrt(getInnerRoot());
    return (- b + sqrt) / (static_cast<T>(2.0) * a);
}

template<typename T> T
QuadraticEquation<T>::getX2()
{
    auto sqrt = std::sqrt(getInnerRoot());
    return (- b - sqrt) / (static_cast<T>(2.0) * a);
}


// probably useful with these
template class QuadraticEquation<double>;
template class QuadraticEquation<float>;

} /* end namspace psc::math */