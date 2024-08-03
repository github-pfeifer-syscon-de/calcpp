/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2024 RPf <gpl3@pfeifer-syscon.de>
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

#include <string>   // stod
#include <StringUtils.hpp>

#include "NumDialog.hpp"
#include "CalcppWin.hpp"

NumDialog::NumDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent)
: Gtk::Dialog(cobject)
, m_parent{parent}
{
    Gtk::Button* btnApply;
    builder->get_widget("apply", btnApply);
    btnApply->signal_clicked().connect(
            sigc::mem_fun(*this, &NumDialog::evaluate));
    set_transient_for(*parent);
}


double
NumDialog::parse(Gtk::Entry* entry)
{
    auto val = entry->get_text();
    StringUtils::trim(val);
    //char* end;
    //double dbl = std::strtod(val.c_str(), &end);
    std::string::size_type offs;
    double dbl = std::stod(val, &offs);
    if (offs != val.size()) {   // expect number to be completely parsable
        entry->grab_focus();
        throw std::invalid_argument("Invalid number");
    }
    return dbl;
}