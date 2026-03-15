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
        double  dec = parse(m_entryDecimal);
        double full;
        double fract = std::modf(dec, &full);
        int64_t lowNum = 0;
        int64_t lowDen = 1;
        int64_t highNum = 1;
        int64_t highDen = 1;
        // Farey algo
        int64_t num{};
        int64_t den{};
        double diff = 1.0;
        while (std::abs(diff) > 1e-8) {
            num = lowNum + highNum;
            den =  lowDen + highDen;
            double val = static_cast<double>(num) / static_cast<double>(den);
            diff = val - fract;
            //std::cout << "Val " << val << " diff " << diff << std::endl;
            if (diff > 0.0) {
                highNum = num;
                highDen = den;
            }
            else {
                lowNum = num;
                lowDen = den;
            }
        }
        num += static_cast<int64_t>(full) * den;
        text1 = format(num);
        text2 = format(den);
    }
    catch (const std::exception& ex) {
        std::cout << "Error fract  " << ex.what() << std::endl;
    }
    m_entryNum->set_text(text1);
    m_entryDenom->set_text(text2);
}
