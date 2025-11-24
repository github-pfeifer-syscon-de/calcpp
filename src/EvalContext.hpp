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

#include <gtkmm.h>
#include <list>
#include <memory>

#include "OutputForm.hpp"
#include "AngleUnit.hpp"
#include "Token.hpp"
#include "Function.hpp"
#include "BaseEval.hpp"


/*
 * model of variable display
 */
class VariableColumns : public Gtk::TreeModel::ColumnRecord
{
public:
    Gtk::TreeModelColumn<Glib::ustring> m_name;
    Gtk::TreeModelColumn<double> m_value;
    VariableColumns()
    {
        add(m_name);
        add(m_value);
    }
};

/*
 * collects function needed mostly during evaluation of parser tree
 *   like output format, angle conversion and variable storage.
 *   Uses advanced setting types to store variable list.
 */
class EvalContext : public Glib::Object, public BaseEval // inherit Glib:object as we use properties
{
public:
    EvalContext();
    virtual ~EvalContext() = default;

    Glib::RefPtr<Gtk::ListStore> get_list();
    bool find(Glib::ustring name, Gtk::TreeModel::Row* ret);
    void remove(Glib::ustring name);
    void rename(Glib::ustring name, Glib::ustring newName);
    void set_value(Glib::ustring name, double val);
    double toRadian(double in) override;
    double fromRadian(double in) override;
    std::shared_ptr<Function> getFunction(const Glib::ustring& fun) override;    // from ConversionContext

    PtrAngleConversion get_angle_conv();
    PtrOutputForm get_output_format();
    bool get_variable(const Glib::ustring& name, double* val) override;
    void set_variable(const Glib::ustring& name, double val) override;

    Glib::PropertyProxy<Glib::ustring> property_angle_conv_id();
    Glib::PropertyProxy_ReadOnly<Glib::ustring> property_angle_conv_id() const;
    Glib::PropertyProxy<Glib::ustring> property_output_format_id();
    Glib::PropertyProxy_ReadOnly<Glib::ustring> property_output_format_id() const;

    void load(Glib::RefPtr<Gio::Settings> settings);
    void save(Glib::RefPtr<Gio::Settings> settings);
    VariableColumns m_variable_columns;

    static constexpr auto ANGLE_CONV_ID_PROPERTY = "angle_conv_id";
    static constexpr auto OUTPUT_FORMAT_ID_PROPERTY = "output_format_id";
    static constexpr auto VAR_CONFIG_GRP = "variables";
    static constexpr auto CONFIG_ANGLE_UNIT = "angle-unit";
    static constexpr auto CONFIG_OUTPUT_FORMAT = "output-format";
private:
    using FunctionMap = std::map<Glib::ustring, std::shared_ptr<Function>>;
    const FunctionMap& get_function_map();
    PtrAngleConversion m_angleConv;
    PtrOutputForm m_output_format;

    Glib::Property<Glib::ustring> property_angle_conv_id_;
    Glib::Property<Glib::ustring> output_format_id_;

    std::map<Glib::ustring, double> m_variables;
    // list a listStore to display variables
    Glib::RefPtr<Gtk::ListStore> m_list;
    FunctionMap m_functionMap;
};

using PtrEvalContext = std::shared_ptr<EvalContext>;