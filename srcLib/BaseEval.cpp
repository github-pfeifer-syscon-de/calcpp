/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
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

#include <iostream>
#include <psc_format.hpp>
#include <psc_i18n.hpp>
#include <StringUtils.hpp>


#include "BaseEval.hpp"
#include "config.h"

BaseEval::BaseEval()
{
}


// simple check if expression will have one result, and no underrun
int
BaseEval::validate(std::list<std::shared_ptr<Token>>& stack)
{
	int cnt = 0;
	for (auto token : stack) {
		std::shared_ptr<NumToken> numToken = std::dynamic_pointer_cast<NumToken>(token);
		if (numToken) {
			++cnt;
		}
		std::shared_ptr<IdToken> idToken = std::dynamic_pointer_cast<IdToken>(token);
		if (idToken) {
            auto function = getFunction(idToken->getId());
			if (!function) {
				++cnt;      // expect var
			}
			else {	// function will consume 1 and and add 1, at least for in the momentary state
				if (cnt < 1) {
					return 0;
				}
			}
		}
		std::shared_ptr<OpToken> op = std::dynamic_pointer_cast<OpToken>(token);
		if (op) {
			int opCnt = op->is_binary() ? 2 : 1;
			if (cnt < opCnt) {
				return 0;
			}
			cnt -= opCnt - 1;	// -1 as we add a result
		}
		std::shared_ptr<AssignToken> assignToken = std::dynamic_pointer_cast<AssignToken>(token);
		if (assignToken) {
			throw EvalError(_("Assignment operator only allowed once"));
		}
	}
	return cnt;
}

// if this is an assigment return the id it shoud be assigned to.
//   the stack on return in this case will only contain the expression
std::shared_ptr<IdToken>
BaseEval::assign_token(std::list<std::shared_ptr<Token>>& stack)
{
	std::shared_ptr<IdToken> idAssignToken;
	auto iter = stack.begin();
	if (iter != stack.end()) {
		idAssignToken = std::dynamic_pointer_cast<IdToken>(*iter);
		if (idAssignToken) {
			++iter;
			if (iter != stack.end()) {
				std::shared_ptr<AssignToken> assignToken = std::dynamic_pointer_cast<AssignToken>(*iter);
				if (assignToken) {
					stack.pop_front();	// keep just expression
					stack.pop_front();
				}
				else {
					idAssignToken.reset();	// allow simple check later
				}
			}
		}
	}
	return idAssignToken;
}

double
BaseEval::eval(std::list<std::shared_ptr<Token>> stack)
{
	if (DEBUG) {
		std::cout << "Stack -------------" << std::endl;
		for (auto token : stack) {
			std::cout << token->show() << std::endl;
		}
		std::cout << "-------------------" << std::endl;
	}
	std::shared_ptr<IdToken> idAssignToken = assign_token(stack);
	int cnt = validate(stack);
	if (cnt != 1) {
		throw EvalError(psc::fmt::vformat(
                _("The calculation is not balanced {} (expect 1)")
                , psc::fmt::make_format_args(cnt)));
	}
	//#pragma GCC diagnostic push
	//#pragma GCC diagnostic ignored "-Wno-psabi"		// only supported as compile option
	std::list<double> values;
	for (auto token : stack) {
		std::shared_ptr<NumToken> numToken = std::dynamic_pointer_cast<NumToken>(token);
		if (numToken) {
			values.push_back(numToken->getValue());
		}
		std::shared_ptr<IdToken> idToken = std::dynamic_pointer_cast<IdToken>(token);
		if (idToken) {
            auto function = getFunction(idToken->getId());
			if (function) {
				//if (values.empty()) {	consistency was checked before
				double valueR = values.back();
				values.pop_back();
                if (function) {
					double result = function->eval(valueR, this);
					values.push_back(result);
                }
                else {
                    auto idTokenName = idToken->show();
					throw EvalError(psc::fmt::vformat(
                            _("No function named {}")
                            , psc::fmt::make_format_args(idTokenName)));
				}
			}
			else {
				double val = 0.0;
				if (!get_variable(idToken->getId(), &val)) {
                    auto idTokenName = idToken->show();
					throw EvalError(psc::fmt::vformat(
                            _("No variable named {}")
                            , psc::fmt::make_format_args(idTokenName)));
				}
				values.push_back(val);
			}
		}
		std::shared_ptr<OpToken> op = std::dynamic_pointer_cast<OpToken>(token);
		if (op) {
			//if (values.empty()) {	consistency was checked before
			double valueR = values.back();
			values.pop_back();
			double valueL = valueR;
			if (op->is_binary()) {
				//if (values.empty()) {	consistency was checked before
				valueL = values.back();
				values.pop_back();
			}
			double result = op->eval(valueL, valueR);
			values.push_back(result);
		}
	}
	//if (values.empty()) {	consistency was checked before
	double total = values.back();
	if (idAssignToken) {	// if this was a assignment assign value
		if (DEBUG)
			std::cout << "Set " << idAssignToken->getId() << " = " << total << std::endl;
		set_variable(idAssignToken->getId(), total);
	}
	//#pragma GCC diagnostic pop
	return total;
}
