/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
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

#pragma once

class Quad
{
public:
    Quad() = default;
    explicit Quad(const Quad& orig) = delete;
    virtual ~Quad() = default;

    void setA(double a);
    void setB(double b);
    void setC(double c);
    double getInnerRoot();
    bool isRootPositive();
    double getX1();
    double getX2();
protected:
    double a{};
    double b{};
    double c{};
};

