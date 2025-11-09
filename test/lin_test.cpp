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



#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <random>

#include "Matrix.hpp"
#include "QuadraticEquation.hpp"

static constexpr auto VALUE_LIMIT{0.000001};

static bool
check_gauss()
{
    try {
        const size_t rows{3u};
        psc::mat::MatrixU<double> m(rows);
        //row,col
        m[0][0] = 2.0;   m[0][1] = 1.0;  m[0][2] = -1.0;  m[0][3] = 8.0;
        m[1][0] = -3.0;  m[1][1] = -1.0; m[1][2] = 2.0;   m[1][3] = -11.0;
        m[2][0] = -2.0;  m[2][1] = 1.0;  m[2][2] = 2.0;   m[2][3] = -3.0;

        psc::mat::Gauss::eliminate(m);
        if (std::abs(m[0][rows] - 2.0) > VALUE_LIMIT
         || std::abs(m[1][rows] - 3.0) > VALUE_LIMIT
         || std::abs(m[2][rows] - (-1.0)) > VALUE_LIMIT) {
            std::cout << "expecting 2,3,-1 got:" << std::endl;
            for (int row = 0; row < static_cast<int32_t>(m.getRows()); ++row) {
                std::cout << "val[" << row <<  "] = " << m[row][rows] << std::endl;
            }
            return false;
        }
        return true;
    }
    catch (const std::exception& err) {
        std::cout << "Error " << err.what() << "calculating!" << std::endl;
    }
    return false;
}

static bool
check_matrix()
{
    psc::mat::MatrixU<double> m(3);
    m(1, 1) = 1.234;
    if (m[1][1] != 1.234) {
        std::cout << "check_matrix exp " << 1.234 << " got " << m[1][1] << std::endl;
        return false;
    }
    try {
        size_t col{4};
        auto a = m[1][col];
        if (col >= m.getColumns()) {
            std::cout << "The access for the element 4 beyond limit did succeed!" << std::endl;
            return false;
        }
    }
    catch (const std::exception& exc) {   // expected exception
        std::cout << "Exception " << exc.what() << std::endl;
    }
    return true;
}


static bool
check_quad()
{
    psc::math::QuadraticEquation<double> quad;
    quad.setA(2.0);
    quad.setB(4.0);
    quad.setC(-4.0);
    if (!quad.isRootPositive()) {
        std::cout << "Root should be positive is  " << quad.getInnerRoot() << std::endl;
        return false;
    }
    double sqrt3 = std::sqrt(3.0);
    double expX1 = -1.0 + sqrt3;
    double expX2 = -1.0 - sqrt3;
    if (std::abs(quad.getX1() - expX1) > VALUE_LIMIT
     || std::abs(quad.getX2() - expX2) > VALUE_LIMIT) {
        std::cout << "Expecting x1 " << expX1 << " x2 " << expX2 << std::endl;
        std::cout << "Got       x1 " << quad.getX1() << " x2 " << quad.getX2() << std::endl;
        return false;
    }
    return true;
}

static double
random(std::mt19937& rng, bool allowNegative = true)
{
    auto n = static_cast<int32_t>(rng() % 10000u);
    if (allowNegative && (rng() % 3) == 0) {
        n = -n;
    }
    auto d = static_cast<int32_t>(rng() % 1000u + 1u);
    //std::cout << "n " << n << " d " << d << " n/d "<< (static_cast<double>(n) / static_cast<double>(d)) << std::endl;
    return static_cast<double>(n) / static_cast<double>(d);
}

static bool
check_gauss_rng(size_t cnt)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    const size_t rows{3u};
    psc::mat::MatrixU<double> m(rows);
    for (size_t i = 0; i < cnt; ++i) {
        auto x1 = random(rng);
        auto x2 = random(rng);
        auto x3 = random(rng);
        m[0][0] = random(rng);   m[0][1] = random(rng);  m[0][2] = random(rng);
        m[0][3] = (x1 * m[0][0] + x2 * m[0][1] + x3 * m[0][2]);
        m[1][0] = random(rng);   m[1][1] = random(rng);  m[1][2] = random(rng);
        m[1][3] = (x1 * m[1][0] + x2 * m[1][1] + x3 * m[1][2]);
        m[2][0] = random(rng);   m[2][1] = random(rng);  m[2][2] = random(rng);
        m[2][3] = (x1 * m[2][0] + x2 * m[2][1] + x3 * m[2][2]);
        psc::mat::Gauss::eliminate(m);
        if (std::abs(m[0][rows] - x1) > VALUE_LIMIT
         || std::abs(m[1][rows] - x2) > VALUE_LIMIT
         || std::abs(m[2][rows] - x3) > VALUE_LIMIT) {
            std::cout << "x1 exp " << x1 << " got " << m[0][rows]
                      << "  x2 exp " << x2 << " got " << m[1][rows]
                      << "  x3 exp " << x3 << " got " << m[2][rows]
                      << std::endl;
            return false;
        }
    }
    return true;
}

static bool
check_quad_rng(size_t cnt)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    psc::math::QuadraticEquation<double> quad;
    for (size_t i = 0; i < cnt; ++i) {
        auto a = random(rng);
        auto b = random(rng);
        auto x = random(rng);
        auto c = -(a * x * x + b * x);
        quad.setA(a);
        quad.setB(b);
        quad.setC(c);
        bool isX1 = std::abs(quad.getX1() - x) <= VALUE_LIMIT;
        bool isX2 = std::abs(quad.getX2() - x) <= VALUE_LIMIT;
        if (!isX1 && !isX2) {
            std::cout << "not expected x for a " << a
                      << " b " << b
                      << " c " << c
                      << " x " << x
                      << " x1 " << quad.getX1()
                      << " x2 " << quad.getX2()  << std::endl;
            return false;
        }
        auto otherX = isX1 ? quad.getX2() : quad.getX1();
        auto tc = -(a * otherX * otherX + b * otherX);
        if (std::abs(tc - c) > VALUE_LIMIT) {
            std::cout << "not expected c for a " << a
                      << " b " << b
                      << " c " << c
                      << " x " << x
                      << " useX " << otherX
                      << " x1 " << quad.getX1()
                      << " x2 " << quad.getX2()
                      << " tc " << tc << std::endl;
            return false;
        }
    }
    return true;
}
/*
 *
 */
int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");      // make locale dependent, and make glib accept u8 const !!!
    Glib::init();

    if (!check_gauss()) {
        return 1;
    }
    if (!check_quad()) {
        return 2;
    }
    if (!check_quad_rng(1000u)) {
        return 3;
    }
    if (!check_gauss_rng(1000u)) {
        return 4;
    }
    if (!check_matrix()) {
        return 5;
    }

    return 0;
}

