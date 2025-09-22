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

#include <iostream>
#include <iterator>
#include <locale.h>
#include <cmath>

#include "NumberFormat.hpp"
#include "Token.hpp"

Token::Token()
{
}

Token::~Token()
{
}


NumToken::NumToken(double val)
: m_val{val}
{
}

double
NumToken::getValue()
{
	return m_val;
}

std::shared_ptr<NumToken>
NumToken::create(const Glib::ustring& val, Glib::ustring::iterator& i, const NumberFormat* m_numberFormat)
{
	std::shared_ptr<NumToken> numToken;
    gunichar c = *i;
    struct lconv* lconv = localeconv();
    if (g_unichar_isdigit(c) || c == static_cast<gunichar>(lconv->decimal_point[0])) {
    	std::string::size_type conv;
		double num;
        if (!m_numberFormat->parse(val, num, &conv)) {
            Glib::ustring err = Glib::ustring::sprintf("Cound not read number at %d", *i);
            throw LexingError(err);
        }
		//std::cout << "Parsed " << val << " to " << num << " places " << conv << std::endl;
		numToken = std::make_shared<NumToken>(num);
        std::advance(i, conv);
    }
	return numToken;
}

Glib::ustring
NumToken::show()
{
	return Glib::ustring::sprintf("%f", m_val);
}

DelimToken::DelimToken()
{
}

std::shared_ptr<DelimToken>
DelimToken::create(const Glib::ustring& value, Glib::ustring::iterator& i)
{
	std::shared_ptr<DelimToken> delimToken;
    gunichar c = *i;
	if (c == ';') {	// need to use ; as , may be decimal separator for some locales
		delimToken = std::make_shared<DelimToken>();
		++i;
	}
	return delimToken;
}

Glib::ustring
DelimToken::show()
{
	return ";";
}

OpToken::OpToken(gunichar op)
: m_op{op}
{
}

std::shared_ptr<OpToken>
OpToken::create(const Glib::ustring& value, Glib::ustring::iterator& i)
{
	std::shared_ptr<OpToken> opToken;
    Glib::ustring::iterator next = i;
	++next;
	gunichar nc = ' ';
	bool isNext = next != value.end();
	if (isNext) {
		nc = *next;
	}
	gunichar c = *i;
	if (c == '+'
	 || OpAddToken::is_minus(c)) {
		opToken = std::make_shared<OpAddToken>(c);
		++i;
	}
	else if (c == '^'
		  || (isNext && c == '*' && nc == '*')) {
		opToken = std::make_shared<OpPowToken>('^');
		if (isNext && c == '*' && nc == '*') {
            i = next;
		}
		++i;
	}
	else if (c == '%'
		  || OpMulToken::is_mult(c)
		  || OpMulToken::is_div(c)) {
		opToken = std::make_shared<OpMulToken>(c);
		++i;
	}
	else if (c == '<'
		  || c == '>') {
		opToken = std::make_shared<OpShiftToken>(c);
		if (isNext && nc == c) {
            i = next;
		}
		++i;
	}
	else if (c == '&'
		  || c == '|') {
		opToken = std::make_shared<OpBitsToken>(c);
		++i;
	}
	else if (c == '('
			|| c == ')') {
		opToken = std::make_shared<OpParenToken>(c);
		++i;
	}

	return opToken;
}

bool
OpToken::is_left_paren()
{
	return false;
}

bool
OpToken::is_right_paren()
{
	return false;
}

bool
OpToken::is_left_assoc()
{
	return true;	// assoc left
}

gunichar
OpToken::get_op()
{
	return m_op;
}

Glib::ustring
OpToken::show()
{
	return Glib::ustring::sprintf("%c", m_op);
}

bool OpToken::is_binary()
{
	return true;	// default to binary
}

OpAddToken::OpAddToken(gunichar opAdd)
: OpToken(opAdd)
{
}

int
OpAddToken::precedence()
{
	return 11;
}

double
OpAddToken::eval(double valL, double valR)
{
	if (is_minus(m_op)) {
		return valL - valR;
	}
	if (m_op == '+') {
		return valL + valR;
	}
	throw EvalError(Glib::ustring::format("Unexpected add operator %c", m_op));
}

bool OpAddToken::is_minus(gunichar c)
{
	return c == '-'
		|| c == L'\u2212';		// − using gutf8.c functions here seem most convenient
}

OpMulToken::OpMulToken(gunichar opMul)
: OpToken(opMul)
{
}

int
OpMulToken::precedence()
{
	return 12;
}

bool
OpMulToken::is_mult(gunichar c)
{
	return c == '*'		// \u2217 ∗ alternative ?
		|| c == L'\u00d7';		// ×
}

bool
OpMulToken::is_div(gunichar c)
{
	return c == '/'
		|| c == L'\u00f7';	// ÷ wiki says used as minus in Scandinavia
}

double
OpMulToken::eval(double valL, double valR)
{
	if (is_div(m_op)) {
		return valL / valR;
	}
	if (m_op == '%') {
		return std::fmod(valL, valR);
	}
	if (is_mult(m_op)) {
		return valL * valR;
	}
	throw EvalError(Glib::ustring::format("Unexpected mult operator %c", m_op));
}

OpPowToken::OpPowToken(gunichar opPow)
: OpToken(opPow)
{
}

int
OpPowToken::precedence()
{
	return 13;
}

double
OpPowToken::eval(double valL, double valR)
{
	switch (m_op) {
	case '^':
		return std::pow(valL, valR);;
	default:
		throw EvalError(Glib::ustring::format("Unexpected pow operator %c", m_op));
	}
}

bool
OpPowToken::is_left_assoc()
{
	return false;	// assoc right
}

OpParenToken::OpParenToken(gunichar opParen)
: OpToken(opParen)
{
}

bool
OpParenToken::is_left_paren()
{
	return m_op == '(';
}

bool
OpParenToken::is_right_paren()
{
	return m_op == ')';
}

int OpParenToken::precedence()
{
	return 15;
}

double
OpParenToken::eval(double valL, double valR)
{
	throw EvalError("Parenthese shoud not get evaluated!");
}

OpShiftToken::OpShiftToken(gunichar opShift)
: OpToken(opShift)
{

}

int
OpShiftToken::precedence()
{
	return 10;
}

double
OpShiftToken::eval(double valL, double valR)
{
	switch (m_op) {
	case '<':
		return (uint64_t)valL << (uint64_t)valR;
	case '>':
		return (uint64_t)valL >> (uint64_t)valR;
	default:
		throw EvalError(Glib::ustring::format("Unexpected shift operator %c", m_op));
	}
}

OpBitsToken::OpBitsToken(gunichar opBits)
: OpToken(opBits)
{
}

int
OpBitsToken::precedence()
{
	switch (m_op) {
	case '&':
		return 7;
	case '|':
		return 5;
	default:
		throw EvalError(Glib::ustring::format("Unexpected bit operator %c", m_op));
	}
}

double
OpBitsToken::eval(double valL, double valR)
{
	switch (m_op) {
	case '&':
		return (uint64_t)valL & (uint64_t)valR;
	case '|':
		return (uint64_t)valL | (uint64_t)valR;
	default:
		throw EvalError(Glib::ustring::format("Unexpected bit operator %c", m_op));
	}
}

NegateToken::NegateToken(gunichar op)
: OpToken(op)
{
}

int
NegateToken::precedence()
{
	return 13;
}

double NegateToken::eval(double valL, double valR)
{
	return -valR;
}

bool
NegateToken::is_left_assoc()
{
	return false;	// assoc right
}

bool
NegateToken::is_binary()
{
	return false;	// here we are unary
}

Glib::ustring
NegateToken::show()
{
    return "u-";
}

AssignToken::AssignToken()
{
}

AssignToken::~AssignToken()
{
}

std::shared_ptr<AssignToken>
AssignToken::create(const Glib::ustring& value, Glib::ustring::iterator& i)
{
	std::shared_ptr<AssignToken> assignToken;
	gunichar c = *i;
	if (c == '=') {
		assignToken = std::make_shared<AssignToken>();
		++i;
	}
	return assignToken;
}

Glib::ustring
AssignToken::show()
{
	return "=";
}

IdToken::IdToken(const Glib::ustring& id)
: m_id{id}
, m_function{false}
{
}

IdToken::~IdToken()
{
}

std::shared_ptr<IdToken>
IdToken::create(Glib::ustring& value, Glib::ustring::iterator& i)
{
	std::shared_ptr<IdToken> idToken;
	gunichar c = *i;
    if (g_unichar_isalpha(c)) {
        Glib::ustring::iterator end = i;
        while ((end != value.end())
			&& g_unichar_isalnum(*end)) {	// use isalnum for parsing e.g. ln2
            ++end;
        }
		// auto start = std::distance(value.begin(), i);
		auto remain = std::distance(i, end);
        Glib::ustring id = value.substr(0, remain);
        idToken = std::make_shared<IdToken>(id);
        //std::cout << "start " << std::distance(input.begin(), i) << " len " << std::distance(i, e) << " id \"" << id << "\"" << std::endl;
        i = end;
    }
	return idToken;
}

Glib::ustring
IdToken::getId()
{
	return m_id;
}

void
IdToken::set_function(bool function)
{
	m_function = function;
}

bool
IdToken::is_function()
{
	return m_function;
}

Glib::ustring
IdToken::show()
{
	return m_id;
}