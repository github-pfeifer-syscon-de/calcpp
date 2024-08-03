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

#include "GaussDialog.hpp"
#include "CalcppWin.hpp"

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
    auto label = Gtk::make_managed<Gtk::Label>(Glib::ustring::sprintf("Row%d", row));
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
    auto label = Gtk::make_managed<Gtk::Label>(Glib::ustring("Result"));
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
            throw std::invalid_argument("Invalid number");
        }
        std::vector<std::vector<double>> rows;
        for (int row = 1; row < m_n+1; ++row) {
            std::vector<double> colValue;
            for (int col = 1; col < m_n+2; ++col) {
                auto widget = m_grid->get_child_at(col, row);
                auto val = 0.0;
                if (auto entry = dynamic_cast<Gtk::Entry*>(widget)) {
                    val = parse(entry);
                }
                else {
                    std::cout << "No entry at " << row << " " << col  << std::endl;
                }
                colValue.push_back(val);
            }
            //std::cout << row << " col size" << colValue.size() << std::endl;
            rows.emplace_back(std::move(colValue));
        }
        //std::cout << "row size " << rows.size() << std::endl;
        gauss(rows);
        for (int row = 0; row < static_cast<int32_t>(rows.size()); ++row) {
            auto widget = m_grid->get_child_at(row+1, m_n+1);
            if (auto entry = dynamic_cast<Gtk::Entry*>(widget)) {
                //std::cout << "Setting row " << row
                //          << " col " << m_n
                //          << " row " << rows[row][m_n] << std::endl;
                entry->set_text(Glib::ustring::sprintf("%lf", rows[row][m_n]));
            }
            else {
                std::cout << "No out-entry at " << row+1 << " " << m_n+1 << std::endl;
            }
        }
    }
    catch (const std::exception& err) {
        m_parent->show_error(Glib::ustring::sprintf("Unable to calculate \"%s\"", err.what()));
    }
}

static void
swapRow(std::vector<double>& a, std::vector<double>& b) {
    for (int32_t l = 0; l < static_cast<int32_t>(a.size()); ++l) {
        double temp = a[l];
        a[l] = b[l];
        b[l] = temp;
    }
}

// simple evaluation might be inaccurate
void
GaussDialog::gauss(std::vector<std::vector<double>>& mat)
{
    // following wikipedia to avoid numeric instability (but may still have issues, but it is as far as i can manage this, even the numerical recepies won't solve any random matrix...)
    // https://en.wikipedia.org/wiki/Gaussian_elimination
	int h = 0; /* Initialization of the pivot row */
	int k = 0; /* Initialization of the pivot column */
	const int m = mat.size();
	const int n = mat[0].size();
	while (h < m && k < n) {
	    /* Find the k-th pivot: */
	    int i_max = 0;
	    double max = 0.0;
            for (int i = h; i < m; ++i) {
            double abs = std::abs(mat[i][k]);
            if (abs > max) {
                max = abs;
                i_max = i;
            }
	    }
	    if (mat[i_max][k] == 0.0) {
            ++k;    /* No pivot in this column, pass to next column */
	    }
	    else {
		swapRow(mat[h], mat[i_max]);
		/* Do for all rows below pivot: */
		for (int i = h + 1; i < m; ++i) {
		    double f = mat[i][k] / mat[h][k];
		    if (std::abs(f) > 0.0) {
                /* Fill with zeros the lower part of pivot column: */
                mat[i][k] = 0.0;
                /* Do for all remaining elements in current row: */
                for (int j = k + 1; j < n; ++j) {
                    mat[i][j] = mat[i][j] - mat[h][j] * f;
                }
		    }
		}
		++h;    /* Increase pivot row and column */
		++k;
	    }
	}
	// back substitution
	for (int row = m-1; row >= 0; --row) {
	    double sum = 0.0;
	    for (int col = row+1; col < m; ++col) {
            sum += mat[row][col] * mat[col][n-1];
            mat[row][col] = 0.0;	// as we canceling these out
	    }
	    mat[row][n-1] = (mat[row][n-1]-sum) / (mat[row][row]);
	    mat[row][row] = 1.0;
	}
}
