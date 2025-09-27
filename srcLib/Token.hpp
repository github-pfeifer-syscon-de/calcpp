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
#include <memory>

class ParseError
: public std::exception
{
public:

    ParseError(std::string error)
    : std::exception()
    , swhat(error)
    {
    }

    virtual const char * what() const noexcept
    {
        return swhat.c_str();
    }
private:
    std::string swhat;
};

class LexingError
: public ParseError
{
public:

    LexingError(std::string error)
    : ParseError(error)
    {
    }
};

class EvalError
: public ParseError
{
public:

    EvalError(std::string error)
    : ParseError(error)
    {
    }
};

class Token
{
public:
    Token();
    virtual ~Token();

    virtual Glib::ustring show() = 0;
private:

};

class NumberFormat;

class NumToken : public Token
{
public:
    NumToken(double val);

    static std::shared_ptr<NumToken> create(const Glib::ustring& val,
                                        Glib::ustring::iterator& i,
                                        const NumberFormat* m_numberFormat);
    Glib::ustring show() override;
    double getValue();
private:
    double m_val;
};

class DelimToken : public Token
{
public:
    DelimToken();

    static std::shared_ptr<DelimToken> create(const Glib::ustring& value, Glib::ustring::iterator& i);
    Glib::ustring show() override;
};

class OpToken : public Token
{
public:
    OpToken(gunichar op);

    static std::shared_ptr<OpToken> create(const Glib::ustring& value, Glib::ustring::iterator& i);
    Glib::ustring show() override;

    virtual bool is_left_paren();
    virtual bool is_right_paren();

    virtual int precedence() = 0;
    virtual bool is_left_assoc();
    gunichar get_op();
    virtual double eval(double valL, double valR) = 0;
    virtual bool is_binary();
protected:
    gunichar m_op;
};

class OpAddToken : public OpToken
{
public:
    OpAddToken(gunichar op);

    int precedence() override;
    double eval(double valL, double valR) override;
    static bool is_minus(gunichar c);
};

class OpMulToken : public OpToken
{
public:
    OpMulToken(gunichar op);

    int precedence() override;
    double eval(double valL, double valR) override;
    static bool is_mult(gunichar c);
    static bool is_div(gunichar c);
};

class OpPowToken : public OpToken
{
public:
    OpPowToken(gunichar op);

    int precedence() override;
    bool is_left_assoc() override;
    double eval(double valL, double valR) override;
};

class OpParenToken : public OpToken
{
public:
    OpParenToken(gunichar op);

    int precedence() override;
    bool is_left_paren() override;
    bool is_right_paren() override;
    double eval(double valL, double valR) override;
};

class OpShiftToken : public OpToken
{
public:
    OpShiftToken(gunichar op);

    int precedence() override;
    double eval(double valL, double valR) override;
};

class OpBitsToken : public OpToken
{
public:
    OpBitsToken(gunichar op);

    int precedence() override;
    double eval(double valL, double valR) override;
};

class NegateToken : public OpToken
{
public:
    NegateToken(gunichar op);

    int precedence() override;
    double eval(double valL, double valR) override;
    bool is_left_assoc() override;
    bool is_binary() override;
    Glib::ustring show() override;
};

class AssignToken : public Token
{
public:
    AssignToken();
    virtual ~AssignToken();

    static std::shared_ptr<AssignToken> create(const Glib::ustring& value, Glib::ustring::iterator& i);
    Glib::ustring show() override;
};

class IdToken : public Token
{
public:
    IdToken(const Glib::ustring& id);
    virtual ~IdToken();

    static std::shared_ptr<IdToken> create(Glib::ustring& value, Glib::ustring::iterator& i);
    Glib::ustring show() override;
    Glib::ustring getId();
    void set_function(bool function);
    bool is_function();
private:
    Glib::ustring m_id;
    bool m_function;
};
