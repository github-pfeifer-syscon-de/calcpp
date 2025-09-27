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
#include <Plot.hpp>
#include <map>

#include "EvalContext.hpp"
#include "BaseEval.hpp"
#include "NumberFormat.hpp"

class PlotDialog
: public Gtk::Dialog
{
public:
    PlotDialog(BaseObjectType* cobject
            , const Glib::RefPtr<Gtk::Builder>& builder
            , const std::shared_ptr<EvalContext>& evalContext);
    explicit PlotDialog(const PlotDialog& orig) = delete;
    virtual ~PlotDialog() = default;

    void apply();
    bool get_variable(const Glib::ustring& name, double* val);
    void set_variable(const Glib::ustring& name, double val);
    NumberFormat* getFormat();

protected:
    void addExpression(const Glib::ustring& fun
            , Gdk::RGBA color
            , std::vector<std::shared_ptr<psc::ui::PlotView>>& views);
    void show_error(const Glib::ustring& msg
            , Gtk::MessageType type = Gtk::MessageType::MESSAGE_ERROR);

private:
    std::shared_ptr<EvalContext> m_evalContext;
    Gtk::ScrolledWindow* m_scroll;
    Gtk::Grid* m_grid;
    Gtk::Entry* m_fun1;
    Gtk::Entry* m_fun2;
    Gtk::Entry* m_fun3;
    Gtk::ColorButton* m_col1;
    Gtk::ColorButton* m_col2;
    Gtk::ColorButton* m_col3;
    Gtk::Button* m_apply;
    Gtk::SpinButton* m_min;
    Gtk::SpinButton* m_max;
    psc::ui::PlotDrawing* m_drawing;
};



class PlotExpression
: public psc::ui::PlotFunction
{
public:
    PlotExpression(Glib::ustring& fun, const std::shared_ptr<EvalContext>& evalContext);
    ~ PlotExpression() = default;

    double calculate(double x) override;

private:
    std::shared_ptr<EvalContext> m_evalContext;
    std::list<std::shared_ptr<Token>> m_list;
};

