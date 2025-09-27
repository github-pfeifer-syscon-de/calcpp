/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf
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

#include <psc_format.hpp>
#include <string>

#include "CalcppWin.hpp"
#include "NumBaseDialog.hpp"

NumBaseDialog::NumBaseDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent)
: Gtk::Dialog(cobject)
, m_parent{parent}
{
    builder->get_widget("input", m_entryInput);
    builder->get_widget("dec", m_entryDec);
    builder->get_widget("hex", m_entryHex);
    builder->get_widget("oct", m_entryOct);
    builder->get_widget("bin", m_entryBin);
    builder->get_widget("uni", m_entryUni);

    m_entryInput->signal_changed().connect(sigc::mem_fun(*this, &NumBaseDialog::convert));
}

void
NumBaseDialog::convert()
{
    Glib::ustring val = m_entryInput->get_text();
    int base{10};
    if ("0x" == val.substr(0, 2)) {
        base = 16;
        val = val.substr(2);
    }
    else if ("0b" == val.substr(0, 2)) {
        base = 2;
        val = val.substr(2);
    }
    else if ("0" == val.substr(0, 1)) {
        base = 8;
        val = val.substr(1);
    }
    try {
        auto num = std::stoll(val, nullptr, base);
        m_entryDec->set_text(psc::fmt::format("{:d}", num));
        m_entryHex->set_text(psc::fmt::format("{:#x}", num));
        m_entryOct->set_text(psc::fmt::format("{:#o}", num));
        m_entryBin->set_text(psc::fmt::format("{:#b}", num));
        m_entryUni->set_text(Glib::ustring::sprintf("%lc", num));
    }
    catch (const std::exception& exc)  {
        m_entryDec->set_text("");
        m_entryHex->set_text("");
        m_entryOct->set_text("");
        m_entryBin->set_text("");
        m_entryUni->set_text("");
    }
}
