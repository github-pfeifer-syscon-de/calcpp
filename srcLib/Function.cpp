/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2022 rpf
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


#include <cmath>

#include "Function.hpp"
#include "BaseEval.hpp"


double
FunctionSqrt::eval(double val, BaseEval *evalContext)
{
	return std::sqrt(val);
}

double
FunctionCbrt::eval(double val, BaseEval *evalContext)
{
	return std::cbrt(val);
}

double
FunctionLog::eval(double val, BaseEval *evalContext)
{
	return std::log(val);
}

double
FunctionExp::eval(double val, BaseEval *evalContext)
{
	return std::exp(val);
}

double
FunctionSin::eval(double val, BaseEval *evalContext)
{
	return std::sin(evalContext->toRadian(val));
}

double
FunctionCos::eval(double val, BaseEval *evalContext)
{
	return std::cos(evalContext->toRadian(val));
}

double
FunctionTan::eval(double val, BaseEval *evalContext)
{
	return std::tan(evalContext->toRadian(val));
}

double
FunctionAsin::eval(double val, BaseEval *evalContext)
{
	return evalContext->fromRadian(std::asin(val));
}

double
FunctionAcos::eval(double val, BaseEval *evalContext)
{
	return evalContext->fromRadian(std::acos(val));
}

double
FunctionAtan::eval(double val, BaseEval *evalContext)
{
	return evalContext->fromRadian(std::atan(val));
}

double
FunctionLog2::eval(double val, BaseEval *evalContext)
{
	return std::log2(val);
}

double
FunctionLog10::eval(double val, BaseEval *evalContext)
{
	return std::log10(val);
}

double
FunctionAbs::eval(double val, BaseEval *evalContext)
{
	return std::fabs(val);
}

double
FunctionFactorial::eval(double val, BaseEval *evalContext)
{
    double fac = 1.0;
    while (val > 1.0) {
        fac *= val;
        val = val - 1.0;
    }
	return fac;
}

//std::vector<double>
//FunctionPrimfact::eval(double argument, BaseEval *evalContext)
//{
//    std::vector<double> ret;
//    ret.reserve(16);
//    // Print the number of 2s that divide n
//    int64_t n = static_cast<int64_t>(argument);
//    while (n % 2 == 0) {
//        ret.push_back(2);
//        n = n/2;
//    }
//    // n must be odd at this point. So we can skip
//    // one element (Note i = i +2)
//    for (int64_t i = 3; i <= std::sqrt(n); i += 2) {
//        // While i divides n, print i and divide n
//        while (n % i == 0) {
//            ret.push_back(i);
//            n = n / i;
//        }
//    }
//    // This condition is to handle the case when n
//    // is a prime number greater than 2
//    if (n > 2) {
//        ret.push_back(n);
//    }
//    return ret;
//}
