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

#pragma once

class EvalContext;

class Function
{
public:
    Function();
    virtual ~Function();

    int get_argument_count();
    virtual double eval(double argument, EvalContext *evalContext) = 0;
private:

};

class FunctionSqrt : public Function
{
public:
    double eval(double argument, EvalContext *evalContext) override;
};

class FunctionCbrt : public Function
{
public:
    double eval(double argument, EvalContext *evalContext) override;
};

class FunctionLog : public Function
{
public:
    double eval(double argument, EvalContext *evalContext) override;
};

class FunctionExp : public Function
{
public:
    double eval(double argument, EvalContext *evalContext) override;
};

class FunctionSin : public Function
{
public:
    double eval(double argument, EvalContext *evalContext) override;
};

class FunctionCos : public Function
{
public:
    double eval(double argument, EvalContext *evalContext) override;
};

class FunctionTan : public Function
{
public:
    double eval(double argument, EvalContext *evalContext) override;
};

class FunctionAsin : public Function
{
public:
    double eval(double argument, EvalContext *evalContext) override;
};

class FunctionAcos : public Function
{
public:
    double eval(double argument, EvalContext *evalContext) override;
};

class FunctionAtan : public Function
{
public:
    double eval(double argument, EvalContext *evalContext) override;
};

class FunctionLog2 : public Function
{
public:
    double eval(double argument, EvalContext *evalContext) override;
};

class FunctionLog10 : public Function
{
public:
    double eval(double argument, EvalContext *evalContext) override;
};

class FunctionAbs : public Function
{
public:
    double eval(double argument, EvalContext *evalContext) override;
};

class FunctionFactorial : public Function
{
public:
    double eval(double argument, EvalContext *evalContext) override;
};