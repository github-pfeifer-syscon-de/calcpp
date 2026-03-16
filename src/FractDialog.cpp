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

#include "Fraction.hpp"
#include "FractDialog.hpp"


FractDialog::FractDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent)
: NumDialog(cobject, builder, parent)
{
    builder->get_widget("decimal", m_entryDecimal);
    builder->get_widget("num", m_entryNum);
    builder->get_widget("denom", m_entryDenom);
}


void
FractDialog::evaluate()
{
    Glib::ustring text1,text2;
    try {
        double decimal = parse(m_entryDecimal);
        Fraction fraction;
        fraction.fromDecimal(decimal);
        text1 = format(fraction.getNumerator());
        text2 = format(fraction.getDenominator());
    }
    catch (const std::exception& ex) {
        std::cout << "Error fract  " << ex.what() << std::endl;
    }
    m_entryNum->set_text(text1);
    m_entryDenom->set_text(text2);
}
