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

#pragma once

#include <glibmm.h>
#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#   include <cxxabi.h>
#endif
#include <list>
#include <queue>
#include <memory>

#include "NumberFormat.hpp"
#include "Token.hpp"

class ConversionContext;

class Syntax {
public:
    Syntax(const NumberFormat* numberFormat, const std::shared_ptr<ConversionContext>& conversionContext);

    std::list<std::shared_ptr<Token>> parse(Glib::ustring& input);

protected:

    std::list<std::shared_ptr<Token>> lexing(Glib::ustring& input);
    std::shared_ptr<Token> get_next_token(Glib::ustring& input, Glib::ustring::iterator& index);
    virtual std::shared_ptr<OpToken> parseOp(const Glib::ustring& value, Glib::ustring::iterator& i);

    std::list<std::shared_ptr<Token>> shuntingYard(std::list<std::shared_ptr<Token>> tokens);
    void insertNegate(std::list<std::shared_ptr<Token>>& tokens);
private:
    const NumberFormat* m_numberFormat;
    std::shared_ptr<ConversionContext> m_conversionContext;
};

template <class T>
std::string
type_name()
{
    typedef typename std::remove_reference<T>::type TR;
    std::unique_ptr<char, void(*)(void*)> own
           (
#ifndef _MSC_VER
                abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                           nullptr, nullptr),
#else
                nullptr,
#endif
                std::free
           );
    std::string r = own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}
