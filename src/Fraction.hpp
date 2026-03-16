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

class FractionException
: public std::runtime_error
{
public:
    FractionException(const char* msg)
    : std::runtime_error(msg)
    {
    }
};

class Fraction {
public:
    Fraction();
    explicit Fraction(uint64_t numerator);
    explicit Fraction(uint64_t numerator, uint64_t denominator);
    explicit Fraction(uint64_t numerator, uint64_t denominator, bool negative);
    Fraction(const Fraction& other) = default;
    Fraction& operator=(const Fraction& other) = default;
    virtual  ~Fraction() = default;

    [[nodiscard]] inline uint64_t getNumerator() const
    {
        return m_numerator;
    }
    void setNumerator(uint64_t num)
    {
        m_numerator = num;
    }
    [[nodiscard]] inline uint64_t getDenominator() const
    {
        return m_denominator;
    }
    void setDenominator(uint64_t denominator)
    {
        checkDenominator(denominator);
        m_denominator = denominator;
    }
    [[nodiscard]] inline bool isNegative() const
    {
        return m_negative;
    }
    void setNegative(bool negative)
    {
        m_negative = negative;
    }
    [[nodiscard]] inline operator double() const
    {
        return static_cast<double>(m_numerator) / static_cast<double>(m_denominator);
    }
    void fromDecimal(double decimal, double precision = 1e-8);
    [[nodiscard]] Fraction negate() const;
    [[nodiscard]] Fraction operator +(const Fraction& fract) const;
    [[nodiscard]] Fraction operator -(const Fraction& fract) const;
    [[nodiscard]] Fraction operator *(const Fraction& fract) const;
    [[nodiscard]] Fraction operator /(const Fraction& fract) const;
    // reduce to shortest representation e.g. 2/6 -> 1/3
    void shorten();
    static uint64_t lcm(uint64_t a, uint64_t b);
    static uint64_t binGcd(uint64_t a, uint64_t b);
protected:
    void checkDenominator(uint64_t denominator);

private:
    uint64_t m_numerator;
    uint64_t m_denominator;
    bool m_negative;
};

