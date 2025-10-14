/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
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

#pragma once

#include <gtkmm.h>

#include "NumDialog.hpp"
#include "CalcppWin.hpp"

class Dimension;
class Unit;

class DimensionColumns
: public Gtk::TreeModel::ColumnRecord
{
public:
    Gtk::TreeModelColumn<Glib::ustring> m_name;
    Gtk::TreeModelColumn<std::shared_ptr<Dimension>> m_value;
    DimensionColumns()
    {
        add(m_name);
        add(m_value);
    }
};

class UnitColumns
: public Gtk::TreeModel::ColumnRecord
{
public:
    Gtk::TreeModelColumn<Glib::ustring> m_name;
    Gtk::TreeModelColumn<std::shared_ptr<Unit>> m_value;
    UnitColumns()
    {
        add(m_name);
        add(m_value);
    }
};

class UnitDialog
: public NumDialog
{
public:
    UnitDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent);
    explicit UnitDialog(const UnitDialog& orig) = delete;
    virtual ~UnitDialog() = default;

    double getValue(bool showError = false);
    void save();
protected:
    void evaluate() override;
    void update();
    void updateSelect(const Glib::ustring& selSrc, const Glib::ustring& selRes);

    DimensionColumns m_dimension_columns;
    UnitColumns m_unit_columns;

    static constexpr auto CONF_DIM{"conversion-dimension"};
    static constexpr auto CONF_SOURCE{"conversion-source"};
    static constexpr auto CONF_RESULT{"conversion-result"};
    static constexpr auto CONF_VALUE{"conversion-value"};

private:
    Gtk::ComboBox* m_dimension;
    Gtk::ComboBox* m_valueUnit;
    Gtk::ComboBox* m_resultUnit;
    Gtk::Entry* m_value;
    Gtk::Entry* m_result;
    CalcppWin* m_parent;
};

