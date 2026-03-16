/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2026 RPf
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
#include <cstdint>

class Fraction
{
public:
    Fraction();
    Fraction(uint64_t numerator);
    Fraction(uint64_t numerator, uint64_t denominator);
    Fraction(const Fraction& other) = default;
    Fraction& operator=(const Fraction& other) = default;
    virtual  ~Fraction() = default;

    inline uint64_t getNumerator() const
    {
        return m_numerator;
    }
    inline uint64_t getDenominator() const
    {
        return m_denominator;
    }
    inline bool isNegative() const
    {
        return m_negative;
    }
    inline double getDecimal() const
    {
        return static_cast<double>(m_numerator) / static_cast<double>(m_denominator);
    }
    void fromDecimal(double decimal, double precision = 1e-8);
    Fraction operator +(const Fraction& fract) const;
    Fraction operator -(const Fraction& fract) const;
    Fraction operator *(const Fraction& fract) const;
    Fraction operator /(const Fraction& fract) const;
    // reduce to shortest representation e.g. 2/6 -> 1/3
    void shorten();

private:
    bool m_negative{false};
    uint64_t m_numerator{};
    uint64_t m_denominator{1ul};
};

