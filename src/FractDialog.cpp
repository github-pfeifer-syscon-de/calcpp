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

#include "CalcppApp.hpp"
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
    builder->get_widget("sign", m_sign);
    std::array entries {m_entryNum, m_entryDenom, m_aNum, m_aDenom, m_bNum, m_bDenom};
    updateCssProvider(entries);
    std::array radios {m_dec, m_add, m_sub, m_mul, m_div};
    connectRadios(radios);
    m_dec->set_active(true);
}

template <size_t N> void
FractDialog::connectRadios(const std::array<Gtk::RadioButton*,N>& radios)
{
    for (auto radio : radios) {
        radio->signal_toggled().connect(sigc::mem_fun(*this, &FractDialog::calc_changed));
    }
}

template <size_t N> void
FractDialog::updateCssProvider(const std::array<Gtk::Entry*,N>& entries)
{
    auto cssProvider =  Gtk::CssProvider::create();
    cssProvider->load_from_resource(m_parent->getApplication()->get_resource_base_path() + "/fract-dlg.css");
    for (auto entry : entries) {
        entry->get_style_context()->add_provider(cssProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
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

int64_t
FractDialog::parseInt(Gtk::Entry* aNum)
{
    auto num = parse(aNum);
    double numFull{};
    double numFract = std::modf(num, &numFull);
    if (numFract != 0.0) {
        aNum->grab_focus();
        throw std::invalid_argument(_("Expected integer"));
    }
    return static_cast<int64_t>(numFull);
}

Fraction
FractDialog::parseFraction(Gtk::Entry* aNum, Gtk::Entry* aDenom)
{
    Fraction ret;
    auto iNum = parseInt(aNum);
    auto iDenom = parseInt(aDenom);
    ret.setNumerator(std::abs(iNum));
    ret.setDenominator(std::abs(iDenom));
    ret.setNegative((iNum < 0l) != (iDenom < 0l));
    return ret;
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
            Fraction a = parseFraction(m_aNum, m_aDenom);
            Fraction b = parseFraction(m_bNum, m_bDenom);
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
        m_sign->set_label(result.isNegative() ? "-" : "+");
        text1 = format(static_cast<double>(result.getNumerator()));
        text2 = format(static_cast<double>(result.getDenominator()));
    }
    catch (const std::exception& ex) {
        auto what = ex.what();
        m_parent->show_error(psc::fmt::vformat(_("Unable to calculate \"{}\""),
                                               psc::fmt::make_format_args(what)));
    }
    m_entryNum->set_text(text1);
    m_entryDenom->set_text(text2);
}
