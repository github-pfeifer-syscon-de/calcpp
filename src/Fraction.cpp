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

#include <iostream>
#include <cmath>

#include "Fraction.hpp"


Fraction::Fraction()
{
}

Fraction::Fraction(uint64_t numerator)
: m_numerator{numerator}
{
}


Fraction::Fraction(uint64_t numerator, uint64_t denominator)
: m_numerator{numerator}
, m_denominator{denominator}
{
}

void
Fraction::fromDecimal(double decimal, double precision)
{
    m_negative = decimal < 0.0;
    decimal = std::abs(decimal);
    precision = std::max(precision, 1e-10);     // since we don't want to search forever
    double full;
    double fract = std::modf(decimal, &full);
    Fraction low{0, 1};
    Fraction high{1, 1};
    // Farey algo
    double diff = 1.0;
    while (std::abs(diff) > precision) {
        m_numerator = low.getNumerator() + high.getNumerator();
        m_denominator = low.getDenominator() + high.getDenominator();
        diff = getDecimal() - fract;
        //std::cout << "Val " << val << " diff " << diff << std::endl;
        if (diff > 0.0) {
            high = *this;
        }
        else {
            low = *this;
        }
    }
    m_numerator += static_cast<int64_t>(full) * m_denominator;
}

Fraction
Fraction::operator +(const Fraction& fract) const
{
    Fraction ret;
    uint64_t base = m_denominator * fract.m_denominator;
    int64_t ext_num = m_numerator * fract.m_denominator;
    if (m_negative) {
        ext_num = -ext_num;
    }
    int64_t ext_fract_num = fract.m_numerator * m_denominator;
    if (fract.m_negative) {
        ext_fract_num = -ext_fract_num;
    }
    auto sum = ext_num + ext_fract_num;
    ret.m_negative = sum < 0l;
    ret.m_numerator = std::abs(sum);
    ret.m_denominator = base;
    ret.shorten();
    return ret;
}

Fraction
Fraction::operator -(const Fraction& fract) const
{
    Fraction ret;
    uint64_t base = m_denominator * fract.m_denominator;
    int64_t ext_num = m_numerator * fract.m_denominator;
    if (m_negative) {
        ext_num = -ext_num;
    }
    int64_t ext_fract_num = fract.m_numerator * m_denominator;
    if (fract.m_negative) {
        ext_fract_num = -ext_fract_num;
    }
    auto sum = ext_num - ext_fract_num;
    ret.m_negative = sum < 0l;
    ret.m_numerator = std::abs(sum);
    ret.m_denominator = base;
    ret.shorten();
    return ret;
}

Fraction
Fraction::operator *(const Fraction& fract) const
{
    Fraction ret;
    ret.m_negative = (m_negative != fract.m_negative);
    ret.m_numerator = m_numerator * fract.m_numerator;
    ret.m_denominator = m_denominator * fract.m_denominator;
    ret.shorten();
    return ret;
}

Fraction
Fraction::operator /(const Fraction& fract) const
{
    Fraction ret;
    ret.m_negative = (m_negative != fract.m_negative);
    ret.m_numerator = m_numerator * fract.m_denominator;
    ret.m_denominator = m_denominator * fract.m_numerator;
    ret.shorten();
    return ret;
}

static uint64_t
untilOdd(uint64_t a)
{
    while (a % 2 == 0){
        a /= 2;
    }
    return a;
}

void
Fraction::shorten()
{
    // Binary gcd
    uint64_t a{m_numerator};
    uint64_t b{m_denominator};
    uint32_t d{};
    while (a % 2 == 0 &&
        b % 2 == 0) {
        a /= 2;
        b /= 2;
        ++d;
    }
    a = untilOdd(a);
    b = untilOdd(b);
    while (a != b) {
        if (a > b) {
            a -= b;
            a = untilOdd(a);
        }
        else if (a < b) {
            b -= a;
            b = untilOdd(b);
        }
    }
    uint64_t gcd = (1ul << d) * a;  // shift = 2^d
    //std::cout << "Found gcd a " << a << " b " << b << " d " << d << " gcd " << gcd << std::endl;
    if (gcd > 1) {
        m_numerator /= gcd;
        m_denominator /= gcd;
    }
}