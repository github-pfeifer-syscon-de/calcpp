/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 rpf
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

#pragma once

#include <gtkmm.h>

class CalcppWin;

class ColorDialog
: public Gtk::Dialog
{
public:
    ColorDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent);
    explicit ColorDialog(const ColorDialog& orig) = delete;
    virtual ~ColorDialog() = default;

    void updateFromSpinner();
    void updateFromHex();
    void updateFromColor();
protected:
    // the internal passing uses Gdk::RGBA _u notation, values 0...65535
    void setSpinner(uint32_t red, uint32_t green, uint32_t blue);
    void setColorButton(uint32_t red, uint32_t green, uint32_t blue);
    void setHexEntry(uint32_t red, uint32_t green, uint32_t blue);

private:
    CalcppWin* m_parent;
    Gtk::SpinButton* m_red;
    Gtk::SpinButton* m_green;
    Gtk::SpinButton* m_blue;
    Gtk::ColorButton* m_color;
    Gtk::Entry* m_hex;
    bool m_signalBlocked{false};    // block signal for internal changes
};

