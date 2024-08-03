/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#pragma once

#include "NumDialog.hpp"


class GaussDialog
: public NumDialog
{
public:
    GaussDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent);
    explicit GaussDialog(const GaussDialog& orig) = delete;
    virtual ~GaussDialog() = default;

protected:
    void build();
    void evaluate() override;
    void gauss(std::vector<std::vector<double>>& mat);
private:
    void buildHeadingRow(const int row);
    void buildEntryRow(const int row);
    void buildResultRow(const int row);

    static constexpr auto INITAL_ENTRIES = 2;
    static constexpr auto MIN_ROWS = 2;
    static constexpr auto MAX_ROWS = 6;
    Gtk::SpinButton* m_entries;
    Gtk::Grid* m_grid;
    int m_n;
};

