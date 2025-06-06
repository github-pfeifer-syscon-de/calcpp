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

#include <stdio.h>
#include <cstdint>
#include <string>
#include <psc_i18n.hpp>

#include "OutputForm.hpp"

// static collection of all  output formats
std::vector<OutputForm *> OutputForm::forms;

OutputForm::OutputForm(const char* id, const char* name)
: m_id{id}
, m_name{name}
{
}

OutputForm::~OutputForm()
{
}

std::vector<OutputForm *>
OutputForm::get_forms()
{
    if (forms.empty()) {
        forms.push_back(new OutformDecimal());
        forms.push_back(new OutformScientific());
        forms.push_back(new OutformExponential());
        forms.push_back(new OutformHex());
        forms.push_back(new OutformHexFp());
        forms.push_back(new OutformOctal());
    }
    return forms;
}

OutputForm *
OutputForm::get_form(Glib::ustring id)
{
    std::vector<OutputForm *> forms = get_forms();
    for (OutputForm* form : forms) {
        if (form->get_id() == id)
            return form;
    }
    return (*forms.begin());
}

Glib::ustring
OutputForm::get_id()
{
    return m_id;
}

Glib::ustring
OutputForm::get_name()
{
    return m_name;
}

bool
OutputForm::parse(const Glib::ustring& remain, double& value, std::string::size_type* offs) const
{
	try {
		value = std::stod(remain, offs);	// c++ way ;), honors local, parses hex (prefix 0x), still no thousands separator
		return true;
	}
	catch (const std::invalid_argument& ex) {
	}
	return false;
}

OutformHex::OutformHex()
: OutputForm("hex", _("Hexadecimal (integer)"))
{
}


Glib::ustring
OutformHex::format(double val)
{
    return Glib::ustring::sprintf("0x%llx", (gint64) val);
}

OutformOctal::OutformOctal()
: OutputForm("oct", _("Octal (integer)"))
{
}

Glib::ustring
OutformOctal::format(double val)
{
    return Glib::ustring::sprintf("0%llo", (gint64) val);
}

// for symetric input output processing
//   allow parsing octals with 010 = 8 (decimal) that might be unexpected in other cases
bool
OutformOctal::parse(const Glib::ustring& remain, double& value, std::string::size_type* offs) const
{
    //std::stod, std::stol, ...
    std::string::size_type fconv, iconv = -1;
    double fval;
    OutputForm::parse(remain, fval, &fconv);
	int64_t lout = 0l;
	try {
		lout = std::stoll(remain, &iconv, 8);	// c++ way
	}
	catch (const std::invalid_argument& ex) {
	}
    if (fconv > iconv) { // if floating conversion length is bigger this seems to be a floating point number
        *offs = fconv;
        value = fval;
    }
    else {	// if both length are equal prefere integer that shoud include octal numbers
        *offs = iconv;
        value = lout;
    }
	return *offs > 0;
}

OutformHexFp::OutformHexFp()
: OutputForm("hxf", _("Hexadecimal (floating point)"))
{
}

Glib::ustring
OutformHexFp::format(double val)
{
    return Glib::ustring::sprintf("%la", val);
}

OutformScientific::OutformScientific()
: OutputForm("sci", _("Scientific"))
{
}


Glib::ustring
OutformScientific::format(double val)
{
    return Glib::ustring::sprintf("%lg", val);
}

OutformDecimal::OutformDecimal()
: OutputForm("dec", _("Decimal"))
{
}


Glib::ustring OutformDecimal::format(double val)
{
    return Glib::ustring::sprintf("%lf", val);
}

OutformExponential::OutformExponential()
: OutputForm("exp", _("Exponential"))
{
}


Glib::ustring OutformExponential::format(double val)
{
    return Glib::ustring::sprintf("%le", val);
}

