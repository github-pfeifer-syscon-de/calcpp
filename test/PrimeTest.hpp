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

#include <vector>
#include <chrono>

namespace psc::math {

class PrimeTest {
public:
    explicit  PrimeTest() = delete;
    explicit  PrimeTest(const  PrimeTest& other) = delete;
    virtual ~ PrimeTest() = default;

    template <typename T>
    static std::vector<T> compute(T max, std::chrono::duration<double>* timeDur = nullptr);
    static std::vector<uint64_t> dijkstra(uint64_t max, std::chrono::duration<double>* timeDur = nullptr);

};

} // psc::math
