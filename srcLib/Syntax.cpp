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
#include <cmath>
#include <stack>

#include "Syntax.hpp"
#include "BaseEval.hpp"
#include "config.h"

Syntax::Syntax(const NumberFormat* numberFormat, const std::shared_ptr<BaseEval>& conversionContext)
: m_numberFormat{numberFormat}
, m_conversionContext{conversionContext}
{
}


std::list<std::shared_ptr<Token>>
Syntax::parse(Glib::ustring& input)
{
	auto tokens = lexing(input);
	insertNegate(tokens);
	auto queue = shuntingYard(tokens);
	return queue;
}

// replace Op'-' by negate'-' where its necessary
//	 allows -- that is no decrement!
void
Syntax::insertNegate(std::list<std::shared_ptr<Token>>& tokens)
{
	bool negPossibleOp = true;	// expression may start with sign
	auto iter = tokens.begin();
	while (iter != tokens.end()) {
		auto token = *iter;
		std::shared_ptr<OpToken> op = std::dynamic_pointer_cast<OpToken>(token);
		if (negPossibleOp
		 && op
		 && (OpAddToken::is_minus(op->get_op()))) {
			(*iter) = std::make_shared<NegateToken>('-');
		}
		std::shared_ptr<AssignToken> assign = std::dynamic_pointer_cast<AssignToken>(token);
		negPossibleOp = (op && !op->is_right_paren()) && (op.operator bool() || assign.operator bool());
		++iter;
	}
#   ifdef DEBUG
        std::cout << "after insertNegate ---------" << std::endl;
        for (auto token : tokens) {
            std::cout << type_name<decltype(token)>() << " " << token->show() << std::endl;
        }
#   endif
}


// see https://en.wikipedia.org/wiki/Shunting_yard_algorithm
std::list<std::shared_ptr<Token>>
Syntax::shuntingYard(std::list<std::shared_ptr<Token>> tokens)
{
	std::list<std::shared_ptr<Token>> queue;	// queue woud resemble the functions more closely, but later we require a serial access
	std::stack<std::shared_ptr<Token>> ops;		// stack will resemble the need function more closely
	while (!tokens.empty()) {
		// read a token
		auto token = tokens.front();
		tokens.pop_front();
		std::shared_ptr<NumToken> numToken = std::dynamic_pointer_cast<NumToken>(token);
		if (numToken) {
			// - a number: put it into the output queue
			queue.push_back(numToken);
		}
		std::shared_ptr<AssignToken> assingToken = std::dynamic_pointer_cast<AssignToken>(token);
		if (assingToken) {
			// - a assignment: put it into the output queue
			queue.push_back(assingToken);
		}
		std::shared_ptr<IdToken> idToken = std::dynamic_pointer_cast<IdToken>(token);
		if (idToken) {
			auto id = idToken->getId();
            auto funct = m_conversionContext->getFunction(id);
			if (funct) {
				//  - a function: push it onto the operator stack
				idToken->set_function(true);
				ops.push(idToken);
			}
			else {
				//  - a identifier: handle like number
				queue.push_back(idToken);
			}
		}
		std::shared_ptr<OpToken> op1 = std::dynamic_pointer_cast<OpToken>(token);
		if (op1) {
			// - an operator o1:
			while (!ops.empty()) {
				std::shared_ptr<Token> op2token = ops.top();
				std::shared_ptr<OpToken> op2 = std::dynamic_pointer_cast<OpToken>(op2token);
				if (op2) {
					// there is an operator o2 other than the left parenthesis at the top of the operator stack,
					// and (o2 has greater precedence than o1
					// or they have the same precedence and o1 is left-associative)
					bool is_greater_prece = op2->precedence() > op1->precedence();
					bool is_same_prece = op2->precedence() == op1->precedence();
					//std::cout << "op1 " << op1->show()
					//		  << " op2 " << op2->show()
					//		  << " greater " << (is_greater_prece ? "yes" : "no")
					//		  << " same " << (is_same_prece ? "yes" : "no")
					//	      << std::endl;
					if (op2->is_left_paren()
						|| (!is_greater_prece
							&& (!is_same_prece
								|| !op1->is_left_assoc()))) {
                        break;
					}
                    // pop o2 from the operator stack into the output queue
                    ops.pop();
                    queue.push_back(op2token);
				}
				else {
					break;
				}
			}
            if (!op1->is_right_paren()) {   // don't want paren on stack
                // push o1 onto the operator stack
                ops.push(op1);
            }
		}
        // multi parameter fuctions
		std::shared_ptr<DelimToken> delimToken = std::dynamic_pointer_cast<DelimToken>(token);
		if (delimToken) {
            while (!ops.empty()) {
                //  while the operator at the top of the operator stack is not a left parenthesis:
				std::shared_ptr<Token> op2token = ops.top();
				std::shared_ptr<OpToken> op2 = std::dynamic_pointer_cast<OpToken>(op2token);
				if (op2->is_left_paren()) {
                    break;
                }
                // pop the operator from the operator stack into the output queue
                ops.pop();
                queue.push_back(op2);
            }
		}
        op1 = std::dynamic_pointer_cast<OpToken>(token);
        if (op1 && op1->is_right_paren()) {
            // a right parenthesis (i.e. ")"):
            while (true) {
                // while the operator at the top of the operator stack is not a left parenthesis:
                //    {assert the operator stack is not empty}
                //    /* If the stack runs out without finding a left parenthesis, then there are mismatched parentheses. */
                if (ops.empty()) {
                    throw EvalError("Missmatched parenthesis");
                }
                std::shared_ptr<Token> op2token = ops.top();
                std::shared_ptr<OpToken> op2 = std::dynamic_pointer_cast<OpToken>(op2token);
                if (op2 && op2->is_left_paren()) {
                    break;
                }
                //    pop the operator from the operator stack into the output queue
                ops.pop();
                queue.push_back(op2token);
            }
            //{assert there is a left parenthesis at the top of the operator stack}
            std::shared_ptr<Token> op2token = ops.top();
            std::shared_ptr<OpToken> op2 = std::dynamic_pointer_cast<OpToken>(op2token);
            if (!op2 || !op2->is_left_paren()) {
                throw EvalError("Expected left parenthesis");
            }
            //pop the left parenthesis from the operator stack and discard it
            ops.pop();
            if (!ops.empty()) {
                //if there is a function token at the top of the operator stack, then:
                auto frontToken = ops.top();
                std::shared_ptr<IdToken> idToken = std::dynamic_pointer_cast<IdToken>(frontToken);
                if (idToken) {
                    //    pop the function from the operator stack into the output queue
                    queue.push_back(idToken);
                    ops.pop();
                }
            }
        }
    }
	///* After the while loop, pop the remaining items from the operator stack into the output queue. */
	//while there are tokens on the operator stack:
	while (!ops.empty()) {
		auto frontToken = ops.top();
		ops.pop();
		//    /* If the operator token on the top of the stack is a parenthesis, then there are mismatched parentheses. */
		//    {assert the operator on top of the stack is not a (left) parenthesis}
		auto opToken = std::dynamic_pointer_cast<OpToken>(frontToken);
		if (opToken && opToken->is_left_paren()) {
			throw EvalError(Glib::ustring::sprintf("Missmatched parenthesis %s", opToken->show()));
		}
		//    pop the operator from the operator stack onto the output queue
		queue.push_back(frontToken);
	}
	return queue;
}

std::list<std::shared_ptr<Token>>
Syntax::lexing(Glib::ustring& input)
{
    Glib::ustring::iterator index = input.begin();
    std::list<std::shared_ptr<Token>> tokens;
    while (true) {
        std::shared_ptr<Token> token = get_next_token(input, index);
        if (!token)
            break;
        //g_print("Token: %s at %i\n", token2str(token), token->position);
        tokens.push_back(token);
    }

    return tokens;
}

std::shared_ptr<Token>
Syntax::get_next_token(Glib::ustring& input, Glib::ustring::iterator& i)
{
    std::shared_ptr<Token> token;
    while (true) {
        if (i == input.end())
            return token;
        if (!g_unichar_isspace(*i)) // also skips \t \n
            break;
        ++i;
    }

    Glib::ustring remain = input.substr(std::distance(input.begin(), i));
	std::shared_ptr<NumToken> numToken = NumToken::create(remain, i, m_numberFormat);
	if (numToken) {
		return numToken;
	}
	std::shared_ptr<DelimToken> delimToken = DelimToken::create(remain, i);
	if (delimToken) {
		return delimToken;
	}
    std::shared_ptr<OpToken> opToken = parseOp(remain, i);
	if (opToken) {
		return opToken;
	}
	std::shared_ptr<AssignToken> assignToken = AssignToken::create(remain, i);
	if (assignToken) {
		return assignToken;
	}
	std::shared_ptr<IdToken> idToken = IdToken::create(remain, i);
	if (idToken) {
		return idToken;
	}
    //    auto otherToken = std::make_shared<OtherToken>();
    //    otherToken->other = c;
    //    token = otherToken;
	throw LexingError(Glib::ustring::sprintf("Cannot parse char %c", *i));
    //++i;
    //token->position = std::distance(input.begin(), i);

    //index = i;
    //return token;
}

std::shared_ptr<OpToken>
Syntax::parseOp(const Glib::ustring& value, Glib::ustring::iterator& i)
{
	return OpToken::create(value, i);
}
