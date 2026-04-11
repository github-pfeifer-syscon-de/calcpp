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
    const auto e = static_cast<T>(std::sqrt(size)) + 1u;
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
    prim.reserve(size / 7u);
    prim.push_back(2u);
    for (T n = 3u; n < size; n += 2u) {
        if (!sieve[n / 2u]) {
           prim.push_back(n);
        }
    }
    return prim;
}

// computation without much assumption
template <typename T>
std::vector<T>
Primes::compute_simple(T size, std::chrono::duration<double>* timeDur)
{
    std::vector<T> prim;
    //auto u_sieve = std::make_unique<std::bitset<N>>();
    std::vector<bool> sieve(size, false);
    const auto start{std::chrono::steady_clock::now()};
    const auto e = size / 2u;
    for (T n = 2u; n < e; ++n) {
        if (!sieve[n]) {
            auto j = 2u * n;
            while (j < sieve.size()) {
                sieve[j] = true;
                j += n;
            }
        }
    }
    const auto end{std::chrono::steady_clock::now()};
    if (timeDur) {
        *timeDur = (end - start);
    }
    prim.reserve(size / 7u);    // overallocate to avoid reallocation
    for (T p : std::views::iota(2u, sieve.size())
                | std::views::filter(
                      [&sieve](const T i) -> bool
                        { return !sieve[i]; })) {
        prim.push_back(p);
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


// dijkstra 28.1671s count 78498
//  worst, worst, worst version, but to get the gist of it
std::vector<uint64_t>
Primes::dijkstra_simple(uint64_t max, std::chrono::duration<double>* timeDur)
{
    uint64_t size = max/10ul;
    auto primes = std::vector<uint64_t>();
    primes.reserve(size);
    auto pool = std::vector<uint64_t>();
    pool.reserve(size);
    const auto start{std::chrono::steady_clock::now()};
    for (uint32_t i = 2; i < max; ++i) {
        uint64_t smallest = std::numeric_limits<uint64_t>::max();
        auto smallestPos = std::vector<uint32_t>();
        for (uint32_t j = 0; j < pool.size(); ++j) {
            if (smallest >= pool[j]) {
                if (smallest > pool[j]) {
                    smallestPos.clear();
                }
                smallest = pool[j];
                smallestPos.push_back(j);
            }
        }
        if (i < smallest) {
            primes.push_back(i);
            pool.push_back(static_cast<uint64_t>(i)*static_cast<uint64_t>(i));
        }
        else {
            for (auto pos : smallestPos) {
                pool[pos] = pool[pos] + primes[pos];
            }
        }
    }
    const auto end{std::chrono::steady_clock::now()};
    if (timeDur != nullptr) {
        *timeDur = (end - start);
    }
    return primes;
}


// using map to handle pool
// the original pool structure was like
//   prim 2 3  5  7  11
//   pool 4 9 25 49 121 ...
// optimized for getting lowest:
//   use map with pool value and the list of primes e.g.
//   4 -> 2
//   8 -> 3 ...
// 32bit dijkstra 0.325925s count 78498
// 64bit dijkstra 0.343218s count 78498 (unlike java this has no performance penalty!)
std::vector<uint64_t>
Primes::dijkstra(uint64_t max, std::chrono::duration<double>* timeDur)
{
    std::vector<uint64_t> primes;
    auto limit = static_cast<double>(max) * static_cast<double>(max) + static_cast<double>(max)*2.0;    // assume that the highest values will hardly be added as only the lowest values will be wrapped, at least where the get into a critical range
    //std::cout <<  "limit " << limit << " prec " << static_cast<double>(std::numeric_limits<uint64_t>::max()) << std::endl;
    if (limit > static_cast<double>(std::numeric_limits<uint64_t>::max())) {     // avoid numeric overflow
        std::cout <<  "the requested limit " << max << " may exceed the estimated precision "  << limit << std::endl;
        return primes;
    }
    auto pool = std::map<uint64_t, std::shared_ptr<std::vector<uint64_t>>>();
    const auto start{std::chrono::steady_clock::now()};
    for (uint64_t i = 2; i < max; ++i) {
        uint64_t smallest = std::numeric_limits<uint64_t>::max();
        auto begin = pool.begin();
        if (begin != pool.end()) {
            smallest = (*begin).first;
        }
        if (i < smallest) {
            auto poolVal = i * i;
            auto entry = pool.find(poolVal);
            if (entry == pool.end()) {
                auto vect = std::make_shared<std::vector<uint64_t>>();
                vect->push_back(i);
                pool.emplace(std::pair(poolVal, vect));
            }
            else {
                (*entry).second->push_back(i);
            }
        }
        else {
            auto poolVal = (*begin).first;
            auto pvect = (*begin).second;
            pool.erase(begin);
            auto vect = *pvect;
            for (auto prim : vect) {
                auto nextPoolVal = poolVal + prim;
                auto entry = pool.find(nextPoolVal);
                if (entry == pool.end()) {
                    auto vect = std::make_shared<std::vector<uint64_t>>();
                    vect->push_back(prim);
                    pool.emplace(std::pair(nextPoolVal, vect));
                }
                else {
                    (*entry).second->push_back(prim);
                }
            }
        }
    }
    const auto end{std::chrono::steady_clock::now()};
    if (timeDur) {
        *timeDur = (end - start);
    }
    primes.reserve(max / 7u);
    for (auto entry : pool) {
        auto pvect = entry.second;
        auto vect = *pvect;
        for (auto prim : vect) {
            primes.push_back(prim);
        }
    }
    std::sort(primes.begin(), primes.end());    // as we may not vist the primes in order
    return primes;
}


// probably useful with these
template std::vector<size_t>
Primes::compute<size_t>(size_t size, std::chrono::duration<double>* timeDur);
template std::vector<size_t>
Primes::compute_simple<size_t>(size_t size, std::chrono::duration<double>* timeDur);
template std::vector<size_t>
Primes::factorize<size_t>(size_t size);


} // psc::math

