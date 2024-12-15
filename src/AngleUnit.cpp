/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#include <glibmm.h>
#include <psc_i18n.hpp>

#include "AngleUnit.hpp"

// static collection of all angle conversions  (as pointer as they live as long as the application)
std::vector<AngleConversion *> AngleConversion::conversions;

AngleConversion::AngleConversion(const char* id, const char* name)
: m_id{id}
, m_name{name}
{
}

AngleConversion::~AngleConversion()
{
}

std::vector<AngleConversion *>
AngleConversion::get_conversions()
{
    if (conversions.empty()) {
        conversions.push_back(new RadianConversion());
        conversions.push_back(new DegreeConversion());
        conversions.push_back(new GonConversion());
    }
    return conversions;
}

Glib::ustring
AngleConversion::get_id()
{
    return m_id;
}

Glib::ustring
AngleConversion::get_name()
{
    return m_name;
}

AngleConversion *
AngleConversion::get_conversion(Glib::ustring id)
{
    std::vector<AngleConversion *> convs = get_conversions();
    for (AngleConversion* conv : convs) {
        if (conv->get_id() == id)
            return conv;
    }
    return (*convs.begin());
}

RadianConversion::RadianConversion()
: AngleConversion("rad", _("Radians"))
{
}

double
RadianConversion::convert_to_radian(double in)
{
    return in;
}

double
RadianConversion::convert_from_radian(double in)
{
    return in;
}

DegreeConversion::DegreeConversion()
: AngleConversion("deg", _("Degree (°)"))
{
}

double
DegreeConversion::convert_to_radian(double in)
{
    return in * G_PI / 180.0;
}

double
DegreeConversion::convert_from_radian(double in)
{
    return in * 180.0 / G_PI;
}

GonConversion::GonConversion()
: AngleConversion("gon", _("Gon"))
{
}

double
GonConversion::convert_to_radian(double in)
{
    return in * G_PI / 200.0;
}

double
GonConversion::convert_from_radian(double in)
{
    return in * 200.0 / G_PI;
}
