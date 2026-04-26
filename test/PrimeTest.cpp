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
#include <random>
#include <algorithm>
#include <map>
#include <ranges>
#include <iterator>

#include "PrimeTest.hpp"
#include "Primes.hpp"

namespace psc::math {
// computation without much assumption, used as reference
template <typename T>
std::vector<T>
PrimeTest::compute(T size, std::chrono::duration<double>* timeDur)
{
    std::vector<T> prim;
    //auto u_sieve = std::make_unique<std::bitset<N>>();
    std::vector<bool> sieve(size, false);
    const auto start{std::chrono::steady_clock::now()};
    const auto e = static_cast<T>(std::sqrt(size)) + 1u;
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
    prim.reserve(size / Primes::PRIME_COUNT_FACTOR);
    for (T p : std::views::iota(2u, sieve.size())
            | std::views::filter(
                [&sieve](const T i) -> bool { return !sieve[i]; })) {
        prim.push_back(p);
    }
    return prim;
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
PrimeTest::dijkstra(uint64_t max, std::chrono::duration<double>* timeDur)
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
    primes.reserve(max / Primes::PRIME_COUNT_FACTOR);
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

template std::vector<size_t>
PrimeTest::compute<size_t>(size_t size, std::chrono::duration<double>* timeDur);


} /* end namespace psc::math */

namespace {

bool
compare(const std::vector<size_t>& prim, const std::vector<size_t>& optimized)
{
    auto diff = std::mismatch(prim.begin(), prim.end(), optimized.begin());
    if (diff.first != prim.end()
     || diff.second != optimized.end()) {
        for (size_t i = 0; i < std::max(prim.size(), optimized.size()); ++i) {
            auto p = i < prim.size() ? prim[i] : 0;
            auto o = i < optimized.size() ? optimized[i] : 0;
            if (p != o) {
                std::cout << i << " missmatch primes "<< p << " optimized " << o << std::endl;
            }
        }
        return false;
     }
    return true;
}

bool
check_prime(const size_t cnt)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    bool first{true};
    for (size_t i = 0; i < cnt; ++i) {
        size_t size = 5000u + (rng() % 5000u);
        std::chrono::duration<double> secSimple;
        auto prim = psc::math::PrimeTest::compute(size, &secSimple);
        std::chrono::duration<double> secOpt;
        auto optimized = psc::math::Primes::compute(size, &secOpt);
        if (!compare(prim, optimized)) {
            std::cout << "Computing primes to " << size << " failed" << std::endl;
            return false;
        }
        auto pv = prim[rng() % prim.size()];
        auto ov = optimized[rng() % optimized.size()];
        auto n = pv * ov;       // use product of primes to check factorize
        auto factors = psc::math::Primes::factorize(n);
        if (factors.size() != 2u) {
            std::cout << "factorisation returned " << factors.size() << "<> 2 elements" << std::endl;
            return false;
        }
        if (!(factors[0] == pv && factors[1] == ov) &&
            !(factors[0] == ov && factors[1] == pv)) {
            std::cout << "factorisation found " << factors[0] << ", " << factors[1]
                      << " expected " << pv << ", " << ov << std::endl;
            return false;
        }
        if (first) {    // just run these once as these are not optimal (at least with modern architectures where memory and caches dominate)
            first = false;
            std::cout << "Computing eratosthenes     took " << secSimple.count()  << " to " << size << " primes " << prim.size() << std::endl;
            std::cout << "Computing eratosth. optim. took " << secOpt.count()  << " to " << size << " primes " << optimized.size() << std::endl;
            std::chrono::duration<double> dijOpt;
            auto primDij = psc::math::PrimeTest::dijkstra(size, &dijOpt);
            if (!compare(prim, primDij)) {
                std::cout << "Computing dijkstra primes to " << size << " failed" << std::endl;
                return false;
            }
            std::cout << "Computing dijkstra         took " << dijOpt.count() << " to " << size << " primes " << primDij.size()  << std::endl;
        }
    }
    return true;
}

} /* namespace anonymous */

/*
 *
 */
int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");      // make locale dependent, and make glib accept u8 const !!!
    //Glib::init();

    if (!check_prime(1000u)) {
        return 7;
    }

    return 0;
}

