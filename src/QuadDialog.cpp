/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2024 RPf
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
#include <string>
#include <cmath>
#include <locale>
#include <psc_i18n.hpp>
#include <psc_format.hpp>
#include <StringUtils.hpp>

#include "QuadDialog.hpp"
#include "QuadraticEquation.hpp"


QuadDialog::QuadDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent)
: NumDialog(cobject, builder, parent)
{
    builder->get_widget("a", m_entryA);
    builder->get_widget("b", m_entryB);
    builder->get_widget("c", m_entryC);
    builder->get_widget("x1", m_entryX1);
    builder->get_widget("x2", m_entryX2);
}


void
QuadDialog::evaluate()
{
    Glib::ustring text1,text2;
    try {
        double a = parse(m_entryA);
        double b = parse(m_entryB);
        double c = parse(m_entryC);
        //std::cout << "a " << a
        //          << " b " << b
        //          << " c " << c << std::endl;
        psc::math::QuadraticEquation<double> quad;
        quad.setA(a);
        quad.setB(b);
        quad.setC(c);
        if (quad.isRootPositive()) {
            //std::cout << "x1 " << quad.getX1()
            //          << " x2 " << quad.getX2() << std::endl;
            text1 = format(quad.getX1());
            text2 = format(quad.getX2());
        }
        else {
            auto fmtd = format(quad.getInnerRoot());
            m_parent->show_error(
                psc::fmt::vformat(
                  _("No result (square-root of negative number {} is undefined)")
                , psc::fmt::make_format_args(fmtd)));
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Error evaluating quad " << ex.what() << std::endl;
    }
    m_entryX1->set_text(text1);
    m_entryX2->set_text(text2);
}
