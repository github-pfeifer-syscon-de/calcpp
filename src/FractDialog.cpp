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
    builder->get_widget("dec", m_dec);
    builder->get_widget("add", m_add);
    builder->get_widget("sub", m_sub);
    builder->get_widget("mul", m_mul);
    builder->get_widget("div", m_div);
    builder->get_widget("aNum",  m_aNum);
    builder->get_widget("aDenom", m_aDenom);
    builder->get_widget("bNum", m_bNum);
    builder->get_widget("bDenom", m_bDenom);
    m_dec->signal_toggled().connect(sigc::mem_fun(*this, &FractDialog::calc_changed));
    m_add->signal_toggled().connect(sigc::mem_fun(*this, &FractDialog::calc_changed));
    m_sub->signal_toggled().connect(sigc::mem_fun(*this, &FractDialog::calc_changed));
    m_mul->signal_toggled().connect(sigc::mem_fun(*this, &FractDialog::calc_changed));
    m_div->signal_toggled().connect(sigc::mem_fun(*this, &FractDialog::calc_changed));
    m_dec->set_active(true);
}

void
FractDialog::calc_changed()
{
    m_entryDecimal->set_sensitive(m_dec->get_active());
    m_aNum->set_sensitive(!m_dec->get_active());
    m_aDenom->set_sensitive(!m_dec->get_active());
    m_bNum->set_sensitive(!m_dec->get_active());
    m_bDenom->set_sensitive(!m_dec->get_active());
}

void
FractDialog::evaluate()
{
    Glib::ustring text1,text2;
    try {
        Fraction result;
        if (m_dec->get_active()) {
            double decimal = parse(m_entryDecimal);
            Fraction fraction;
            result.fromDecimal(decimal);
        }
        else {
            uint64_t aNum = parse(m_aNum);
            uint64_t aDemom = parse(m_aDenom);
            Fraction a{aNum, aDemom};
            uint64_t bNum = parse(m_bNum);
            uint64_t bDemom = parse(m_bDenom);
            Fraction b{bNum, bDemom};
            if (m_add->get_active()) {
                result = a + b;
            }
            else if (m_sub->get_active()) {
                result = a - b;
            }
            else if (m_mul->get_active()) {
                result = a * b;
            }
            else if (m_div->get_active()) {
                result = a / b;
            }
        }
        int64_t num = result.getNumerator();
        if (result.isNegative()) {
            num = -num;
        }
        text1 = format(num);
        text2 = format(result.getDenominator());
    }
    catch (const std::exception& ex) {
        auto what = ex.what();
        m_parent->show_error(psc::fmt::vformat(_("Unable to calculate \"{}\""),
                                               psc::fmt::make_format_args(what)));
    }
    m_entryNum->set_text(text1);
    m_entryDenom->set_text(text2);
}
