/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4 -*-  */
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

#include <iostream>
#include <charconv>
#include <optional>
#include <system_error>

#include "BaseEval.hpp"
#include "NumberFormat.hpp"

class TestEval
: public BaseEval
{
public:
    TestEval() = default;
    ~TestEval() = default;

    // provide a minimal set of these functions
    std::shared_ptr<Function> getFunction(const Glib::ustring& name) override
    {
        return std::shared_ptr<Function>();
    }
    bool get_variable(const Glib::ustring& name, double* val) override
    {
        *val = 0.0;
        return false;
    }
    void set_variable(const Glib::ustring& name, double val) override
    {
    }
    double toRadian(double val) override
    {
        return val;
    }
    double fromRadian(double val) override
    {
        return val;
    }

};

// parses double locale independent
class TestFormat
: public NumberFormat
{
public:
    TestFormat() = default;
    ~TestFormat() = default;

    bool parse(const Glib::ustring& remain, double& value, std::string::size_type* offs) const override
    {
        double result{};
        auto cstr = remain.c_str();
        auto cend = cstr + remain.bytes();
        auto [ptr, ec] = std::from_chars(cstr, cend, result);
        if (ec == std::errc()) {
            *offs = ptr - cstr;
            //std::cout << "Result: " << result << " offs " << *offs << '\n';
            value = result;
            return true;
        }
        return false;
    }

};
