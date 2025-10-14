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
        double insqrt = b * b - 4.0 * a * c;
        if (insqrt >= 0) {
            double sqrt = std::sqrt(insqrt);
            double x1 = (- b + sqrt) / (2.0 * a);
            double x2 = (- b - sqrt) / (2.0 * a);
            //std::cout << "x1 " << x1
            //          << " x2 " << x2 << std::endl;
            text1 = format(x1);
            text2 = format(x2);
        }
        else {
            auto fmtd = format(insqrt);
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
