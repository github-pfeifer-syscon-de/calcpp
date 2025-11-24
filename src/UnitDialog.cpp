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
#include "EvalContext.hpp"
#include "Syntax.hpp"

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
        auto settings = m_parent->getSettings();
        auto selectDimId = settings->get_string(CONF_DIM);
        auto dims = std::make_shared<Dimensions>(execPath);
        int n{};
        int selectDim{};
        auto dimList = Gtk::ListStore::create(m_dimension_columns);
        for (auto& dim : dims->getDimensions()) {
            Gtk::TreeIter iter = dimList->append();
            Gtk::TreeModel::Row row = *iter;
            // use column with text and value
            row[m_dimension_columns.m_name] = dim->getName();
            row[m_dimension_columns.m_value] = dim;
            if (selectDimId == dim->getDim()) {
                selectDim = n;
            }
            ++n;
            //std::cout << "UnitDialog::UnitDialog add row " << dim->getName() << std::endl;
        }
        m_dimension->set_model(dimList);
        m_dimension->set_active(selectDim);
        m_dimension->set_entry_text_column(m_dimension_columns.m_name);
        m_valueUnit->set_entry_text_column(m_unit_columns.m_name);
        m_resultUnit->set_entry_text_column(m_unit_columns.m_name);
        auto selectSourceId = settings->get_string(CONF_SOURCE);
        auto selectResultId = settings->get_string(CONF_RESULT);
        updateSelect(selectSourceId, selectResultId);
        auto expr = settings->get_string(CONF_EXPR);
        m_value->set_text(expr);
        evaluate();
        m_dimension->signal_changed().connect(sigc::mem_fun(*this, &UnitDialog::update));
        m_valueUnit->signal_changed().connect(sigc::mem_fun(*this, &UnitDialog::evaluate));
        m_resultUnit->signal_changed().connect(sigc::mem_fun(*this, &UnitDialog::evaluate));
    }
    catch (const std::exception& exc) {
        show_error(exc.what(), Gtk::MessageType::MESSAGE_ERROR);
    }
}

void
UnitDialog::update()
{
    updateSelect("", "");
}

void
UnitDialog::updateSelect(const Glib::ustring& selSrc, const Glib::ustring& selRes)
{
    auto iter = m_dimension->get_active();
    if (!iter) {
        std::cout << "UnitDialog::update not selected" << std::endl;
        return;
    }
    auto row = *iter;
    auto dim = row.get_value(m_dimension_columns.m_value);
    //std::cout << "UnitDialog::updateSelect src " << selSrc
    //          << " res " << selRes << std::endl;
    if (dim) {
        int prefSrc{-1};
        int prefRes{-1};
        int prefDefl{-1};
        double lastFactor = std::numeric_limits<double>::max();
        auto unitList = Gtk::ListStore::create(m_unit_columns);
        auto all = dim->getUnits();
        for (auto& unit : all) {
            int row = static_cast<int>(unitList->children().size());
            //std::cout << "UnitDialog::update add row " << unit->getName() << std::endl;
            Gtk::TreeIter valIter = unitList->append();
            Gtk::TreeModel::Row valRow = *valIter;
            valRow[m_unit_columns.m_name] = unit->getName();
            valRow[m_unit_columns.m_value] = unit;
            //std::cout << "UnitDialog::updateSelect unit " << unit->getId() << std::endl;
            if (!selSrc.empty()) {
                if (unit->getId() == selSrc) {
                    prefSrc = row;
                }
            }
            if (!selRes.empty()) {
                if (unit->getId() == selRes) {
                    prefRes = row;
                }
            }
            auto diff = std::abs(1.0 - unit->getFactor());
            if (diff < lastFactor) {
                prefDefl = row;
                lastFactor = diff;
            }
        }
        m_valueUnit->set_model(unitList);
        m_valueUnit->set_active((prefSrc >= 0 ? prefSrc : prefDefl));
        m_resultUnit->set_model(unitList);
        m_resultUnit->set_active((prefRes >= 0 ? prefRes : prefDefl));
    }
}

PtrEvalContext
UnitDialog::getEvalContext()
{
    return m_parent->getEvalContext();
}

Glib::ustring
UnitDialog::getValue(bool showError)
{
    Glib::ustring result;
    auto iterVal = m_valueUnit->get_active();
    auto iterRes = m_resultUnit->get_active();
    if (iterVal && iterRes) {
        auto valRow = *iterVal;
        auto valDim = valRow.get_value(m_unit_columns.m_value);
        auto resRow = *iterRes;
        auto resDim = resRow.get_value(m_unit_columns.m_value);
        try {

            double base{};
            if (valDim->useOwnParse()) {
                base = valDim->fromUnit(m_value, this);
            }
            else {
                try {
                    // allow simple expressions as input
                    auto evalContext = getEvalContext();
                    auto outputForm = evalContext->get_output_format();
                    Syntax syntax(outputForm, evalContext);
                    auto txt = m_value->get_text();
                    auto stack = syntax.parse(txt);
                    auto val  = evalContext->eval(stack);
                    base = valDim->fromUnit(val);
                }
                catch (const ParseError& err) {
                    auto what = err.what();
                    show_error(psc::fmt::vformat(_("Parse error: {}")
                                , psc::fmt::make_format_args(what)));
                }
            }
            result = resDim->toUnit(base, this);
        }
        catch (const std::exception& err) {
            auto what = err.what();
            if (showError) {
                show_error(psc::fmt::vformat(_("Unable to calculate \"{}\""),
                                                       psc::fmt::make_format_args(what)));
            }

        }
    }
    return result;
}

void
UnitDialog::show_error(const Glib::ustring& msg, Gtk::MessageType type)
{
    m_parent->show_error(msg, type);
}

void
UnitDialog::save()
{
    auto settings = m_parent->getSettings();
    auto iter = m_dimension->get_active();
    if (iter) {
        auto row = *iter;
        auto dim = row.get_value(m_dimension_columns.m_value);
        settings->set_string(CONF_DIM, dim->getDim());
    }
    auto itrSrc = m_valueUnit->get_active();
    if (itrSrc) {
        auto srcRow = *itrSrc;
        auto srcUnit = srcRow.get_value(m_unit_columns.m_value);
        //std::cout << "UnitDialog::save src " << srcUnit->getId() << std::endl;
        settings->set_string(CONF_SOURCE, srcUnit->getId());
    }
    else {
        std::cout << "UnitDialog::save no srcSel" << std::endl;
    }
    auto iterRes = m_resultUnit->get_active();
    if (iterRes) {
        auto resRow = *iterRes;
        auto resUnit = resRow.get_value(m_unit_columns.m_value);
        //std::cout << "UnitDialog::save res " << resUnit->getId() << std::endl;
        settings->set_string(CONF_RESULT, resUnit->getId());
    }
    else {
        std::cout << "UnitDialog::save no resSel" << std::endl;
    }
    settings->set_string(CONF_EXPR, m_value->get_text());
}


void
UnitDialog::evaluate()
{
    m_result->set_text(getValue(true));
}