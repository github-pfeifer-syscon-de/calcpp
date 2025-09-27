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
#include <charconv>
#include <optional>
#include <system_error>
#include <StringUtils.hpp>
#include <psc_i18n.hpp>
#include <psc_format.hpp>

#include "PlotDialog.hpp"
#include "Syntax.hpp"

PlotDialog::PlotDialog(BaseObjectType* cobject
                    , const Glib::RefPtr<Gtk::Builder>& builder
                    , const std::shared_ptr<EvalContext>& evalContext)
: Gtk::Dialog(cobject)
, m_evalContext{evalContext}
{
    builder->get_widget("grid", m_grid);
    builder->get_widget("fun1", m_fun1);
    builder->get_widget("fun2", m_fun2);
    builder->get_widget("fun3", m_fun3);
    builder->get_widget("col1", m_col1);
    builder->get_widget("col2", m_col2);
    builder->get_widget("col3", m_col3);
    builder->get_widget("apply", m_apply);
    builder->get_widget("min", m_min);
    builder->get_widget("max", m_max);
    builder->get_widget("scroll", m_scroll);
    builder->get_widget_derived<psc::ui::PlotDrawing>("drawing", m_drawing);

    m_apply->signal_clicked().connect(sigc::mem_fun(*this, &PlotDialog::apply));
}

void
PlotDialog::apply()
{
    auto& xAxis = m_drawing->getXAxis();
    xAxis.setMinMax(m_min->get_value(), m_max->get_value());
    std::vector<std::shared_ptr<psc::ui::PlotView>> views;
    try {
        auto fun1 = m_fun1->get_text();
        try {
            addExpression(fun1, m_col1->get_rgba(), views);
        }
        catch (const std::exception& exc) {
            auto msg = exc.what();
            show_error(psc::fmt::vformat(
                        _("Parse error {} for {}" ),
                          psc::fmt::make_format_args(msg, fun1)));
        }
        auto fun2 = m_fun2->get_text();
        try {
            addExpression(fun2, m_col2->get_rgba(), views);
        }
        catch (const std::exception& exc) {
            auto msg = exc.what();
            show_error(psc::fmt::vformat(
                        _("Parse error {} for {}" ),
                          psc::fmt::make_format_args(msg, fun2)));
        }
        auto fun3 = m_fun3->get_text();
        try {
            addExpression(fun3, m_col3->get_rgba(), views);
        }
        catch (const std::exception& exc) {
            auto msg = exc.what();
            show_error(psc::fmt::vformat(
                        _("Parse error {} for {}" ),
                          psc::fmt::make_format_args(msg, fun3)));
        }
        m_drawing->setPlot(views);
        m_drawing->refresh();
    }
    catch (const std::exception& exc) {
            auto msg = exc.what();
            show_error(psc::fmt::vformat(
                        _("Error evaluating {}" ),
                          psc::fmt::make_format_args(msg)));
    }
    m_evalContext->remove("x");
}


void
PlotDialog::show_error(const Glib::ustring& msg, Gtk::MessageType type)
{
    Gtk::MessageDialog messagedialog(*this, msg, false, type);
    messagedialog.run();
    messagedialog.hide();
}

void
PlotDialog::addExpression(const Glib::ustring& fun
                        , Gdk::RGBA color
                        , std::vector<std::shared_ptr<psc::ui::PlotView>>& views)
{
    auto fn{fun};
    StringUtils::trim(fn);
    if (!fn.empty()) {
        auto expr = std::make_shared<PlotExpression>(fn, m_evalContext);
        expr->setPlotColor(color);
        views.push_back(expr);
    }
}



PlotExpression::PlotExpression(Glib::ustring& fun, const std::shared_ptr<EvalContext>& evalContext)
: m_evalContext{evalContext}
{
    m_evalContext->set_variable("x", 0.0);    // ensure variable is found
    Syntax syntax(m_evalContext->get_output_format(), m_evalContext);
    m_list = syntax.parse(fun);
#   ifdef DEBUG
    std::cout << "PlotExpression::PlotExpression" << std::endl;
    for (auto tok : m_list1) {
        std::cout << tok->show() << std::endl;
    }
#   endif
}



double
PlotExpression::calculate(double x)
{
    m_evalContext->set_variable("x", x);
    double y = m_evalContext->eval(m_list);
#   ifdef DEBUG
    std::cout << "PlotExpression::calculate"
              << " x " << x
              << " y " << y << std::endl;
#   endif
    return y;
}
