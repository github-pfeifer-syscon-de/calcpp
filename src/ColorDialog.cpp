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

#include <iostream>

#include "CalcppWin.hpp"
#include "ColorDialog.hpp"
#include <limits>

ColorDialog::ColorDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent)
: Gtk::Dialog(cobject)
, m_parent{parent}
{
    builder->get_widget("red", m_red);
    builder->get_widget("green", m_green);
    builder->get_widget("blue", m_blue);
    builder->get_widget("color", m_color);
    builder->get_widget("hex", m_hex);

    m_red->signal_value_changed().connect(sigc::mem_fun(*this, &ColorDialog::updateFromSpinner));
    m_green->signal_value_changed().connect(sigc::mem_fun(*this, &ColorDialog::updateFromSpinner));
    m_blue->signal_value_changed().connect(sigc::mem_fun(*this, &ColorDialog::updateFromSpinner));
    m_hex->signal_changed().connect(sigc::mem_fun(*this, &ColorDialog::updateFromHex));
    m_color->signal_color_set().connect(sigc::mem_fun(*this, &ColorDialog::updateFromColor));
    updateFromSpinner();
}

void
ColorDialog::updateFromSpinner()
{
    if (!m_signalBlocked) {
        m_signalBlocked = true;
        uint32_t red = m_red->get_value_as_int();
        uint32_t green = m_green->get_value_as_int();
        uint32_t blue = m_blue->get_value_as_int();
        red <<= 8u;
        green <<= 8u;
        blue <<= 8u;
        setColorButton(red, green, blue);
        setHexEntry(red, green, blue);
        m_signalBlocked = false;
    }
}

void
ColorDialog::updateFromHex()
{
    if (!m_signalBlocked) {
        m_signalBlocked = true;
        auto hex = m_hex->get_text();
        if (hex.length() > 0 && hex.at(0) == '#') {
            hex = hex.substr(1, hex.length() - 1);
        }
        uint32_t red{},green{},blue{};
        if (hex.length() >= 12) {
            red = static_cast<uint32_t>(g_ascii_strtoull(hex.substr(0, 4).c_str(), nullptr, 16));
            green = static_cast<uint32_t>(g_ascii_strtoull(hex.substr(4, 4).c_str(), nullptr, 16));
            blue = static_cast<uint32_t>(g_ascii_strtoull(hex.substr(8, 4).c_str(), nullptr, 16));
        }
        else if (hex.length() >= 6) {
            red = static_cast<uint32_t>(g_ascii_strtoull(hex.substr(0, 2).c_str(), nullptr, 16) * 0x100);
            green = static_cast<uint32_t>(g_ascii_strtoull(hex.substr(2, 2).c_str(), nullptr, 16) * 0x100);
            blue = static_cast<uint32_t>(g_ascii_strtoull(hex.substr(4, 2).c_str(), nullptr, 16) * 0x100);
        }
        else if (hex.length() >= 3) {
            red = static_cast<uint32_t>(g_ascii_strtoull(hex.substr(0, 1).c_str(), nullptr, 16) * 0x1100);
            green = static_cast<uint32_t>(g_ascii_strtoull(hex.substr(1, 1).c_str(), nullptr, 16) * 0x1100);
            blue = static_cast<uint32_t>(g_ascii_strtoull(hex.substr(2, 1).c_str(), nullptr, 16) * 0x1100);
        }
        setSpinner(red, green, blue);
        setColorButton(red, green, blue);
        m_signalBlocked = false;
    }
}

void
ColorDialog::updateFromColor()
{
    if (!m_signalBlocked) {
        m_signalBlocked = true;
        Gdk::RGBA color = m_color->get_rgba();
        uint32_t red = color.get_red_u();
        uint32_t green = color.get_green_u();
        uint32_t blue = color.get_blue_u();
        setSpinner(red, green, blue);
        setHexEntry(red, green, blue);
        m_signalBlocked = false;
    }
}

void
ColorDialog::setHexEntry(uint32_t red, uint32_t green, uint32_t blue)
{
    auto hex = Glib::ustring::sprintf("#%02x%02x%02x"
                                            , red >> 8u, green >> 8u, blue>> 8u);
    m_hex->set_text(hex);
}

void
ColorDialog::setSpinner(uint32_t red, uint32_t green, uint32_t blue)
{
    m_red->set_value(red >> 8u);
    m_green->set_value(green >> 8u);
    m_blue->set_value(blue >> 8u);
}

void
ColorDialog::setColorButton(uint32_t red, uint32_t green, uint32_t blue)
{
    Gdk::RGBA color;
    color.set_red_u(static_cast<gushort>(red));
    color.set_green_u(static_cast<gushort>(green));
    color.set_blue_u(static_cast<gushort>(blue));
    color.set_alpha_u(std::numeric_limits<uint16_t>::max());
    m_color->set_rgba(color);
}
