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

#include <iostream>

#include "PrefDialog.hpp"
#include "CalcppWin.hpp"

PrefDialog::PrefDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent, Glib::RefPtr<Gio::Settings> settings)
: Gtk::Dialog(cobject)
{

    builder->get_widget("fontButton", m_fontButton);
    builder->get_widget("defaultFont", m_defaultFont);
    //Glib::ustring font = settings->get_string(CONFIG_FONT);
    /*bool defaultFont =*/ settings->get_boolean(CONFIG_DEFAULT_FONT);
    //m_fontButton->property_sensitive() = !defaultFont;
    settings->bind(CONFIG_FONT, m_fontButton->property_font_name());
    settings->bind(CONFIG_DEFAULT_FONT, m_defaultFont->property_active());
    // does not work
    //settings->bind_writable(CONFIG_DEFAULT_FONT, m_fontButton->property_sensitive(), true);
    //m_defaultFont->property_active() = defaultFont;
    set_transient_for(*parent);
    //show_all();
    m_defaultFont->signal_toggled().connect(
        [this,parent] {        // as we use primarily this reference capture seems best
			//std::cout << "Signal toggled" << std::endl;
			bool defaultFont = m_defaultFont->property_active();    // need to pass param as settings are updated later
			m_fontButton->property_sensitive() = !defaultFont;
			//std::cout << "Signal toggled apply" << std::endl;
			parent->apply_font(defaultFont);		// only needed as we may swiched enable / disable
		});
    signal_response().connect(
        [this] (int response) {
			hide();
		});
}


PrefDialog::~PrefDialog()
{
}

