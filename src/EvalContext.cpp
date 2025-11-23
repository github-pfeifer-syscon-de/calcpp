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

#include <math.h>
#include <iostream>
#include <StringUtils.hpp>
#include <psc_i18n.hpp>

#include "EvalContext.hpp"
#include "config.h"

EvalContext::EvalContext()
: Glib::ObjectBase(typeid (EvalContext))
, m_variable_columns()
, m_angleConv{RadianConversion::get_conversion("")} // use search with invalid key so we wont create addition references
, m_output_format{OutformDecimal::get_form("")} // as above, avoid the hassle to correctly free it afterwards
, property_angle_conv_id_(*this, ANGLE_CONV_ID_PROPERTY, m_angleConv->get_id())
, output_format_id_(*this, OUTPUT_FORMAT_ID_PROPERTY, m_output_format->get_id())
, m_variables()
, m_list{Gtk::ListStore::create(m_variable_columns)}
, m_functionMap{
          {"sqrt",   std::make_shared<FunctionSqrt>()}
    	, {"cbrt",   std::make_shared<FunctionCbrt>()}
        , {"exp",    std::make_shared<FunctionExp>()}
        , {"sin",    std::make_shared<FunctionSin>()}
        , {"cos",    std::make_shared<FunctionCos>()}
        , {"tan",    std::make_shared<FunctionTan>()}
        , {"log2",   std::make_shared<FunctionLog2>()}
        , {"abs",    std::make_shared<FunctionAbs>()}
        , {"fac",    std::make_shared<FunctionFactorial>()}
    }
{
    auto functLog = std::make_shared<FunctionLog>();
    m_functionMap.insert(std::make_pair("log",    functLog));
    m_functionMap.insert(std::make_pair("ln",     functLog));
    auto functAsin = std::make_shared<FunctionAsin>();
    m_functionMap.insert(std::make_pair("asin",   functAsin));
    m_functionMap.insert(std::make_pair("arcsin", functAsin));
    auto functAcos = std::make_shared<FunctionAcos>();
    m_functionMap.insert(std::make_pair("acos",   functAcos));
    m_functionMap.insert(std::make_pair("arccos", functAcos));
    auto functAtan = std::make_shared<FunctionAtan>();
    m_functionMap.insert(std::make_pair("atan",   functAtan));
    m_functionMap.insert(std::make_pair("arctan", functAtan));
    auto functLog10 = std::make_shared<FunctionLog10>();
    m_functionMap.insert(std::make_pair("log10", functLog10));
    m_functionMap.insert(std::make_pair("lg",    functLog10));

    set_value(StringUtils::u8str(u8"\u03c0"), G_PI); // π or pi set some defaults
    set_value("e", G_E);
    set_value(StringUtils::u8str(u8"\u03d5"), (1.0 + sqrt(5.0)) / 2.0); // ϕ or phi
    // have to use proxy as it seems, to reflect property changes with functions
    //   custom setter,getter for property would be nice but the functions are not virtual...
    Glib::PropertyProxy<Glib::ustring> angle_proxy = property_angle_conv_id_.get_proxy();
    angle_proxy.signal_changed().connect(
        [this, angle_proxy] {
            Glib::ustring val = angle_proxy.get_value();
            //std::cout << "angle setting val " << val << " actual " << m_angleConv->get_id() << std::endl;
            if (val != m_angleConv->get_id()) {
                m_angleConv = AngleConversion::get_conversion(val);
            }
        });
    Glib::PropertyProxy<Glib::ustring> format_proxy = output_format_id_.get_proxy();
    format_proxy.signal_changed().connect(
        [this, format_proxy] {
            Glib::ustring val = format_proxy.get_value();
            //std::cout << "format setting val " << val << " actual " << m_output_format->get_id() << std::endl;
            if (val != m_output_format->get_id()) {
                m_output_format = OutputForm::get_form(val);
            }
        });
}

Glib::RefPtr<Gtk::ListStore>
EvalContext::get_list()
{
    return m_list;
}

bool
EvalContext::find(Glib::ustring name, Gtk::TreeModel::Row* ret)
{
    typedef Gtk::TreeModel::Children type_children; //minimise code length.
    type_children children = m_list->children();
    for (type_children::iterator iter = children.begin(); iter != children.end(); ++iter) {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring varName = row[m_variable_columns.m_name];
        if (varName == name) {
            //std::cout << "Existing row for " << varName << std::endl;
            *ret = row;
            return true;
        }
    }
    //std::cout << "No existing row for " << name << std::endl;
    return false;
}

void
EvalContext::remove(Glib::ustring name)
{
    if (m_variables.erase(name)) { // remove old name
        Gtk::TreeModel::Row row;
        if (find(name, &row)) {
            m_list->erase(row);
        }
    }
    else {
        std::cout << "EvalContext::remove name " << name << " not found."  << std::endl;
    }
}

void
EvalContext::rename(Glib::ustring name, Glib::ustring newName)
{
    double val = 0.0;
    std::map<Glib::ustring, double>::iterator it = m_variables.find(name);
    if (it != m_variables.end()) {
        val = it->second;
        m_variables.erase(it);
    }
    else {
        std::cerr << "Coud not find name " << name << " in map, to rename  to " << newName << std::endl;
    }
    m_variables.insert(std::pair<Glib::ustring, double>(newName, val)); // create new entry
    Gtk::TreeModel::Row row;
    if (find(name, &row)) {
        row.set_value<Glib::ustring>(m_variable_columns.m_name, newName);
    }
}

void
EvalContext::set_value(Glib::ustring name, double val)
{
    Gtk::TreeModel::Row row;
    if (!find(name, &row)) {
        Gtk::TreeIter iter = m_list->append();
        row = *iter;
        row.set_value(m_variable_columns.m_name, name);
    }
    row.set_value<double>(m_variable_columns.m_value, val);
    std::map<Glib::ustring, double>::iterator it = m_variables.find(name);
    if (it != m_variables.end())
        m_variables.erase(it);
    m_variables.insert(std::pair<Glib::ustring, double>(name, val));
}

bool
EvalContext::get_variable(const Glib::ustring& name, double* val)
{
    std::map<Glib::ustring, double>::iterator it = m_variables.find(name);
    if (it != m_variables.end()) {
        *val = it->second;
        return true;
    }
    *val = 0.0;
    return false;
}

void
EvalContext::set_variable(const Glib::ustring& name, double val)
{
    set_value(name, val);
}

double
EvalContext::toRadian(double in)
{
    return m_angleConv->convert_to_radian(in);
}

double EvalContext::fromRadian(double in)
{
	return m_angleConv->convert_from_radian(in);
}

PtrAngleConversion
EvalContext::get_angle_conv()
{
    return m_angleConv;
}

PtrOutputForm
EvalContext::get_output_format()
{
    return m_output_format;
}

Glib::PropertyProxy<Glib::ustring>
EvalContext::property_angle_conv_id()
{
    return property_angle_conv_id_.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring>
EvalContext::property_angle_conv_id() const
{
    return property_angle_conv_id_.get_proxy();
}

Glib::PropertyProxy<Glib::ustring>
EvalContext::property_output_format_id()
{
    return output_format_id_.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring>
EvalContext::property_output_format_id() const
{
    return output_format_id_.get_proxy();
}

void
EvalContext::load(Glib::RefPtr<Gio::Settings> settings)
{
    Glib::Variant<std::map < Glib::ustring, double>> values;
    settings->get_value(VAR_CONFIG_GRP, values);
    //std::cout << "load " << values.print(true) << std::endl;

    Glib::VariantIter iter = values.get_iter();
    Glib::Variant<std::tuple < Glib::ustring, double>> entry;
    while (iter.next_value(entry)) {
        auto entryTup = entry.get();
        //            std::cout << "load name" << std::get<0>(entryTup) << "=" << std::get<1>(entryTup) << std::endl;
        set_value(std::get<0>(entryTup), std::get<1>(entryTup));
    }

    settings->bind(CONFIG_ANGLE_UNIT,
                   this, ANGLE_CONV_ID_PROPERTY, Gio::SettingsBindFlags::SETTINGS_BIND_GET);
    settings->bind(CONFIG_OUTPUT_FORMAT,
                   this, OUTPUT_FORMAT_ID_PROPERTY, Gio::SettingsBindFlags::SETTINGS_BIND_GET);

}

void
EvalContext::save(Glib::RefPtr<Gio::Settings> settings)
{
    auto values = Glib::Variant<std::map < Glib::ustring, double>>::create(m_variables);
    //std::cout << "save " << values.print(true) << std::endl;
    settings->set_value(VAR_CONFIG_GRP, values);
}

std::shared_ptr<Function>
EvalContext::getFunction(const Glib::ustring& fun)
{
    auto& map = get_function_map();
    auto iter = map.find(fun);
    if (iter != map.end()) {
        return iter->second;
    }
    return std::shared_ptr<Function>();
}

const EvalContext::FunctionMap &
EvalContext::get_function_map()
{
    return m_functionMap;
}
