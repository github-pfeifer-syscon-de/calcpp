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
#include <vector>
#include <ranges>
#include <cmath>
#include <map>

#include "Primes.hpp"

namespace psc::math {


// use assumption that event numbers are not prime
//   -> so don't store them
template <typename T>
std::vector<T>
Primes::compute(T size, std::chrono::duration<double>* timeDur)
{
    std::vector<T> prim;
    auto alloc = size / 2u + 1u;
    std::vector<bool> sieve(alloc, false);    // storage is bitwise
    const auto start{std::chrono::steady_clock::now()};
    const auto e = static_cast<T>(std::ceil(std::sqrt(size)));
    for (T n = 3u; n < e; n += 2u) {
        if (!sieve[n / 2u]) {
            auto j = 2u * n;
            while (j < size) {
                if (j % 2u != 0) {  // don't access if even as we don't store these
                    sieve[j / 2u] = true;
                }
                j += n;
            }
        }
    }
    const auto end{std::chrono::steady_clock::now()};
    if (timeDur) {
        *timeDur = (end - start);
    }
    prim.reserve(size / PRIME_COUNT_FACTOR);
    prim.push_back(2u);
    for (T n = 3u, i = 1u; n < size; n += 2u, ++i) {
        if (!sieve[i]) {
           prim.push_back(n);
        }
    }
    return prim;
}

template <typename T>
std::vector<T>
Primes::factorize(T n)
{
    // https://cp-algorithms.com/algebra/factorization.html#fermats-factorization-method
    //    there are more advanced options but as we have the primes around
    std::vector<T> factorization;
    auto primes = Primes::compute(static_cast<T>(std::sqrt(n)) + 1u);
    for (T d : primes) {
        if (d * d > n) {
            break;
        }
        while (n % d == 0) {
            factorization.push_back(d);
            n /= d;
        }
    }
    if (n > 1) {
        factorization.push_back(n);
    }
    return factorization;
}



// probably useful with these
template std::vector<size_t>
Primes::compute<size_t>(size_t size, std::chrono::duration<double>* timeDur);
template std::vector<size_t>
Primes::factorize<size_t>(size_t size);


} // psc::math

