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

#pragma once

#include <vector>

#include "OutputForm.hpp"

/*
 * base to allow conversion of angle units as the trigometric functions work on
 *   radian only. Used also to build menu so extra implementations just need a
 *   unique id and get added in get_conversions.
 */
class AngleConversion : public Named
{
public:
    virtual ~AngleConversion();

    virtual double convert_to_radian(double in) = 0;
    virtual double convert_from_radian(double in) = 0;
    Glib::ustring get_id();
    Glib::ustring get_name();

    static std::vector<AngleConversion *>get_conversions();
    static AngleConversion *get_conversion(Glib::ustring unit);
protected:
    AngleConversion(const char* id, const char* name);

private:
    static std::vector<AngleConversion *> conversions;
    Glib::ustring m_id;
    Glib::ustring m_name;
};

class RadianConversion : public AngleConversion
{
public:
    RadianConversion();

    double convert_to_radian(double in) override;
    double convert_from_radian(double in) override;
};

class DegreeConversion : public AngleConversion
{
public:
    DegreeConversion();

    double convert_to_radian(double in) override;
    double convert_from_radian(double in) override;
};

class GonConversion : public AngleConversion
{
public:
    GonConversion();

    double convert_to_radian(double in) override;
    double convert_from_radian(double in) override;
};