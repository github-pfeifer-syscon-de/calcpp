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
#include "EvalContext.hpp"

Function::Function()
{
}

Function::~Function()
{
}

int
Function::get_argument_count()
{
	return 1;
}

double
FunctionSqrt::eval(double val, EvalContext *evalContext)
{
	return std::sqrt(val);
}

double
FunctionCbrt::eval(double val, EvalContext *evalContext)
{
	return std::cbrt(val);
}

double
FunctionLog::eval(double val, EvalContext *evalContext)
{
	return std::log(val);
}

double
FunctionExp::eval(double val, EvalContext *evalContext)
{
	return std::exp(val);
}

double
FunctionSin::eval(double val, EvalContext *evalContext)
{
	return std::sin(evalContext->toRadian(val));
}

double
FunctionCos::eval(double val, EvalContext *evalContext)
{
	return std::cos(evalContext->toRadian(val));
}

double
FunctionTan::eval(double val, EvalContext *evalContext)
{
	return std::tan(evalContext->toRadian(val));
}

double
FunctionAsin::eval(double val, EvalContext *evalContext)
{
	return evalContext->fromRadian(std::asin(val));
}

double
FunctionAcos::eval(double val, EvalContext *evalContext)
{
	return evalContext->fromRadian(std::acos(val));
}

double
FunctionAtan::eval(double val, EvalContext *evalContext)
{
	return evalContext->fromRadian(std::atan(val));
}

double
FunctionLog2::eval(double val, EvalContext *evalContext)
{
	return std::log2(val);
}

double
FunctionLog10::eval(double val, EvalContext *evalContext)
{
	return std::log10(val);
}

double
FunctionAbs::eval(double val, EvalContext *evalContext)
{
	return std::fabs(val);
}
