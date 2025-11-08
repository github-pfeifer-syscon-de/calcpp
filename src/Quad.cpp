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

#include "Quad.hpp"


void
Quad::setA(double a)
{
    this->a = a;
}

void
Quad::setB(double b)
{
    this->b = b;
}

void
Quad::setC(double c)
{
    this->c = c;
}

double
Quad::getInnerRoot()
{
    return b * b - 4.0 * a * c;
}

bool
Quad::isRootPositive()
{
    return getInnerRoot() >= 0.0;
}

double
Quad::getX1()
{
    double sqrt = std::sqrt(getInnerRoot());
    return (- b + sqrt) / (2.0 * a);
}

double
Quad::getX2()
{
    double sqrt = std::sqrt(getInnerRoot());
    return (- b - sqrt) / (2.0 * a);
}