/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2024 RPf <gpl3@pfeifer-syscon.de>
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
#include <cstdlib>
#include <psc_i18n.hpp>
#include <psc_format.hpp>

#include "GaussDialog.hpp"
#include "CalcppWin.hpp"
#include "Matrix.hpp"

GaussDialog::GaussDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent)
: NumDialog(cobject,  builder, parent)
, m_n{INITAL_ENTRIES}
{
    builder->get_widget("entries", m_entries);
    m_entries->set_range(MIN_ROWS, MAX_ROWS);
    m_entries->set_increments(1.0, 2.0);
    m_entries->signal_changed().connect(
            sigc::mem_fun(*this, &GaussDialog::build));
    builder->get_widget("grid", m_grid);
    m_entries->set_value(m_n);
    build();
}

void
GaussDialog::buildHeadingRow(const int row)
{
    for (int col = 1; col < m_n+2; ++col) {
        auto title = col < m_n+1
                    ? Glib::ustring::sprintf("x%d", col)
                    : Glib::ustring("=");
        auto label = Gtk::make_managed<Gtk::Label>(title);
        m_grid->attach(*label, col, row);
    }
}

void
GaussDialog::buildEntryRow(const int row)
{
    auto label = Gtk::make_managed<Gtk::Label>(
        psc::fmt::vformat(_("Row{}")
        , psc::fmt::make_format_args(row)));
    m_grid->attach(*label, 0, row);
    for (int col = 1; col < m_n+2; ++col) {
        auto entry = Gtk::make_managed<Gtk::Entry>();
        entry->set_text("1");
        entry->set_alignment(1.0f);
        m_grid->attach(*entry, col, row);
    }
}

void
GaussDialog::buildResultRow(const int row)
{
    auto label = Gtk::make_managed<Gtk::Label>(_("Result"));
    m_grid->attach(*label, 0, row);
    for (int col = 1; col < m_n+1; ++col) {
        auto entry = Gtk::make_managed<Gtk::Entry>();
        entry->set_editable(false);
        entry->set_alignment(1.0f);
        m_grid->attach(*entry, col, row);
    }
}

void
GaussDialog::build()
{
    // clear existing
    for (auto widget : m_grid->get_children()) {
        m_grid->remove(*widget);
    }
    m_n = m_entries->get_value_as_int();
    //std::cout << "GaussDialog::build " << m_n << std::endl;
    for (int row = 0; row < m_n+2; ++row) {
        if (row == 0) {
            buildHeadingRow(row);
        }
        else if (row == m_n+1) {
            buildResultRow(row);
        }
        else {
            buildEntryRow(row);
        }
    }
    m_grid->show_all();
}


void
GaussDialog::evaluate()
{
    try {
        if (m_n < MIN_ROWS) {
            throw std::invalid_argument(_("Invalid number"));
        }
        psc::mat::MatrixU<double> m{static_cast<size_t>(m_n)};
        for (int row = 1; row < m_n+1; ++row) {
            for (int col = 1; col < m_n+2; ++col) {
                auto widget = m_grid->get_child_at(col, row);
                auto val = 0.0;
                if (auto entry = dynamic_cast<Gtk::Entry*>(widget)) {
                    val = parse(entry);
                }
                else {
                    std::cout << "No entry at " << row << " " << col  << std::endl;
                }
                m[row-1][col-1] = val;
            }
            //std::cout << row << " col size" << colValue.size() << std::endl;
            //rows.emplace_back(std::move(colValue));
        }
        //std::cout << "row size " << rows.size() << std::endl;
        psc::mat::Gauss::eliminate(m);
        for (int row = 0; row < static_cast<int32_t>(m.getRows()); ++row) {
            auto widget = m_grid->get_child_at(row+1, m_n+1);
            if (auto entry = dynamic_cast<Gtk::Entry*>(widget)) {
                //std::cout << "Setting row " << row
                //          << " col " << m_n
                //          << " row " << rows[row][m_n] << std::endl;
                entry->set_text(Glib::ustring::sprintf("%lf", m[row][m_n]));
            }
            else {
                std::cout << "No out-entry at " << row+1 << " " << m_n+1 << std::endl;
            }
        }
    }
    catch (const std::exception& err) {
        auto what = err.what();
        m_parent->show_error(psc::fmt::vformat(_("Unable to calculate \"{}\""),
                                               psc::fmt::make_format_args(what)));
    }
}

