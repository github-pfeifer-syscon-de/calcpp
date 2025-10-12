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
#include <limits>
#include <psc_i18n.hpp>
#include <psc_format.hpp>

#include "UnitDialog.hpp"
#include "Unit.hpp"

UnitDialog::UnitDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent)
: NumDialog(cobject, builder, parent)
, m_parent{parent}
{
    builder->get_widget("dimension", m_dimension);
    builder->get_widget("valueUnit", m_valueUnit);
    builder->get_widget("resultUnit", m_resultUnit);
    builder->get_widget("value", m_value);
    builder->get_widget("result", m_result);

    auto calcppApp = m_parent->getApplication();
    auto execPath = calcppApp->get_exec_path();
    try {
        auto dims = std::make_shared<Dimensions>(execPath);
        auto dimList = Gtk::ListStore::create(m_dimension_columns);
        for (auto& dim : dims->getDimensions()) {
            Gtk::TreeIter iter = dimList->append();
            Gtk::TreeModel::Row row = *iter;
            // use column with text and value
            row[m_dimension_columns.m_name] = dim->getName();
            row[m_dimension_columns.m_value] = dim;
            //std::cout << "UnitDialog::UnitDialog add row " << dim->getName() << std::endl;
        }
        m_dimension->set_model(dimList);
        m_dimension->set_active(0);
        m_dimension->set_entry_text_column(m_dimension_columns.m_name);
        m_valueUnit->set_entry_text_column(m_unit_columns.m_name);
        m_resultUnit->set_entry_text_column(m_unit_columns.m_name);
        update();
        m_dimension->signal_changed().connect(sigc::mem_fun(*this, &UnitDialog::update));
        m_valueUnit->signal_changed().connect(sigc::mem_fun(*this, &UnitDialog::evaluate));
        m_resultUnit->signal_changed().connect(sigc::mem_fun(*this, &UnitDialog::evaluate));
    }
    catch (const std::exception& exc) {
        m_parent->show_error(exc.what(), Gtk::MessageType::MESSAGE_ERROR);
    }
}

void
UnitDialog::update()
{
    auto iter = m_dimension->get_active();
    if (!iter) {
        std::cout << "UnitDialog::update not selected" << std::endl;
        return;
    }
    auto row = *iter;
    auto dim = row.get_value(m_dimension_columns.m_value);
    if (dim) {
        int prefVal = -1;
        double lastFactor = std::numeric_limits<double>::max();
        auto unitList = Gtk::ListStore::create(m_unit_columns);
        auto all = dim->getUnits();
        for (auto entry : all) {
            auto& unit = entry.second;
            int row = static_cast<int>(unitList->children().size());
            //std::cout << "UnitDialog::update add row " << unit->getName() << std::endl;
            Gtk::TreeIter valIter = unitList->append();
            Gtk::TreeModel::Row valRow = *valIter;
            valRow[m_unit_columns.m_name] = unit->getName();
            valRow[m_unit_columns.m_value] = unit;
            if (std::abs(1.0 - unit->getFactor()) < lastFactor) {
                prefVal = row;
                lastFactor = unit->getFactor();
            }
        }
        m_valueUnit->set_model(unitList);
        m_valueUnit->set_active(std::max(prefVal, 0));
        m_resultUnit->set_model(unitList);
        m_resultUnit->set_active(std::max(prefVal, 0));
    }
}

void
UnitDialog::evaluate()
{
    auto iterVal = m_valueUnit->get_active();
    auto iterRes = m_resultUnit->get_active();
    if (iterVal && iterRes) {
        auto valRow = *iterVal;
        auto valDim = valRow.get_value(m_unit_columns.m_value);
        auto resRow = *iterRes;
        auto resDim = resRow.get_value(m_unit_columns.m_value);
        double res{0.0};
        try {
            double val = parse(m_value);
            res = resDim->toUnit(valDim->fromUnit(val));
        }
        catch (const std::exception& err) {
            auto what = err.what();
            m_parent->show_error(psc::fmt::vformat(_("Unable to calculate \"{}\""),
                                                   psc::fmt::make_format_args(what)));

        }
        m_result->set_text(Glib::ustring::sprintf("%lf", res));
    }
}