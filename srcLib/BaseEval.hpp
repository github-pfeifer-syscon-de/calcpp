/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
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

#include <list>
#include <memory>


#include "Token.hpp"
#include "Function.hpp"

class BaseEval
{
public:
    BaseEval();
    explicit BaseEval(const BaseEval& orig) = delete;
    virtual ~BaseEval() = default;

    double eval(std::list<std::shared_ptr<Token>> stack);
    virtual std::shared_ptr<Function> getFunction(const Glib::ustring& name) = 0;
    virtual bool get_variable(const Glib::ustring& name, double* val) = 0;
    virtual void set_variable(const Glib::ustring& name, double val) = 0;
    virtual double toRadian(double val) = 0;
    virtual double fromRadian(double val) = 0;

protected:

    std::shared_ptr<IdToken> assign_token(std::list<std::shared_ptr<Token>>& stack);
    int validate(std::list<std::shared_ptr<Token>>& stack);

private:

};

