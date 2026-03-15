/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2024 RPf 
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
#include "CalcppWin.hpp"


class FractDialog
: public NumDialog
{
public:
    FractDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent);
    explicit FractDialog(const FractDialog& orig) = delete;
    virtual ~FractDialog() = default;

protected:
    void evaluate() override;
private:
    Gtk::Entry* m_entryDecimal;
    Gtk::Entry* m_entryNum;
    Gtk::Entry* m_entryDenom;
};

