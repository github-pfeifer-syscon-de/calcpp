/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
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

#include <iostream>
#include <cstdlib>
#include <charconv>
#include <optional>
#include <system_error>

#include "calc_test.hpp"
#include "Syntax.hpp"

// eval basic expression
static
bool testEval()
{
    auto testEval = std::make_shared<TestEval>();
    TestFormat testFormat;
    Syntax syntax(&testFormat, testEval);
    Glib::ustring expr{"3.1+4*5"};
    auto list = syntax.parse(expr);
    double res = testEval->eval(list);
    std::cout << "testEval " << res << std::endl;
    return std::abs(res - 23.1) < 0.000001;
}

static
bool testEvalBraced()
{
    auto testEval = std::make_shared<TestEval>();
    TestFormat testFormat;
    Syntax syntax(&testFormat, testEval);
    Glib::ustring expr{"(3+4.1)*5"};
    auto list = syntax.parse(expr);
    double res = testEval->eval(list);
    std::cout << "testEvalBraced " << res << std::endl;
    return std::abs(res - 35.5) < 0.000001;
}

/*
 *
 */
int main(int argc, char** argv)
{
    if (!testEval()) {
        return 1;
    }
    if (!testEvalBraced()) {
        return 2;
    }

    return 0;
}

