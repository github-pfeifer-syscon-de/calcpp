/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf <gpl3@pfeifer-syscon.de>
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

class BaseEval;

// provide the usual suspects for functions
class Function
{
public:
    Function() = default;
    virtual ~Function() = default;

    virtual double eval(double argument, BaseEval *evalContext) = 0;
private:

};

class FunctionSqrt : public Function
{
public:
    double eval(double argument, BaseEval *evalContext) override;
};

class FunctionCbrt : public Function
{
public:
    double eval(double argument, BaseEval *evalContext) override;
};

class FunctionLog : public Function
{
public:
    double eval(double argument, BaseEval *evalContext) override;
};

class FunctionExp : public Function
{
public:
    double eval(double argument, BaseEval *evalContext) override;
};

class FunctionSin : public Function
{
public:
    double eval(double argument, BaseEval *evalContext) override;
};

class FunctionCos : public Function
{
public:
    double eval(double argument, BaseEval *evalContext) override;
};

class FunctionTan : public Function
{
public:
    double eval(double argument, BaseEval *evalContext) override;
};

class FunctionAsin : public Function
{
public:
    double eval(double argument, BaseEval *evalContext) override;
};

class FunctionAcos : public Function
{
public:
    double eval(double argument, BaseEval *evalContext) override;
};

class FunctionAtan : public Function
{
public:
    double eval(double argument, BaseEval *evalContext) override;
};

class FunctionLog2 : public Function
{
public:
    double eval(double argument, BaseEval *evalContext) override;
};

class FunctionLog10 : public Function
{
public:
    double eval(double argument, BaseEval *evalContext) override;
};

class FunctionAbs : public Function
{
public:
    double eval(double argument, BaseEval *evalContext) override;
};

class FunctionFactorial : public Function
{
public:
    double eval(double argument, BaseEval *evalContext) override;
};