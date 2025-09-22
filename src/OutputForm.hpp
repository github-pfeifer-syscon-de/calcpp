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

#pragma once

#include <vector>
#include <glibmm.h>

#include "NumberFormat.hpp"

/*
 * base to build menu for format and angle-unit
 */
class Named // purely virtual -> interface in java speak
{
public:

    virtual Glib::ustring get_id() = 0;
    virtual Glib::ustring get_name() = 0;
protected:

};

/*
 * output format, used to build menu if you want to add your own implementation
 *   extends this class, assign a unique id, and add in get_forms.
 */
class OutputForm : public Named, public NumberFormat {
public:
    virtual ~OutputForm();

    static std::vector<OutputForm *> get_forms();
    static OutputForm* get_form(Glib::ustring id);
    Glib::ustring get_id() override;
    Glib::ustring get_name() override;
    virtual Glib::ustring format(double val) = 0;

    virtual bool parse(const Glib::ustring& remain, double& value, std::string::size_type* offs) const override;
protected:
    OutputForm(const char* id, const char* name);
private:
    static std::vector<OutputForm *> forms;

    Glib::ustring m_id;
    Glib::ustring m_name;
};

class OutformHex : public OutputForm {
public:
    OutformHex();

    Glib::ustring format(double val) override;
};

class OutformOctal : public OutputForm {
public:
    OutformOctal();

    Glib::ustring format(double val) override;
    virtual bool parse(const Glib::ustring& remain, double& value, std::string::size_type* offs) const override;
};

class OutformHexFp : public OutputForm {
public:
    OutformHexFp();

    Glib::ustring format(double val) override;
};

class OutformScientific : public OutputForm {
public:
    OutformScientific();

    Glib::ustring format(double val) override;
};

class OutformDecimal : public OutputForm {
public:
    OutformDecimal();

    Glib::ustring format(double val) override;
};

class OutformExponential : public OutputForm {
public:
    OutformExponential();

    Glib::ustring format(double val) override;
};