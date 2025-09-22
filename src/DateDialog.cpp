/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2020 rpf
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

#include "DateDialog.hpp"
#include "CalcppWin.hpp"


DateDialog::DateDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent, Glib::RefPtr<Gio::Settings> settings)
: Gtk::Dialog(cobject)
{
    builder->get_widget("calendar", m_calendar);

    set_transient_for(*parent);
    Glib::DateTime date = Glib::DateTime::create_now_local();
    m_calendar->property_year() = date.get_year ();
    m_calendar->property_month() = date.get_month () - 1;
    m_calendar->property_day() = date.get_day_of_month ();

    show_all();
    signal_response().connect(
        [this] (int response)
        {
            hide();
        }
    );
}


DateDialog::~DateDialog()
{
}
