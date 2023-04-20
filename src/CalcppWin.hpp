/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#include "EvalContext.hpp"
#include "OutputForm.hpp"
#include "CalcTextView.hpp"
#include "CalcTreeView.hpp"

class CalcppApp;
class Syntax;
class Token;

/*
 * Window that is the central visible piece of application,
 *   loaded from resource, uses subclassed components,
 *   uses settings to store and application state.
 */
class CalcppWin : public Gtk::ApplicationWindow {
public:
    CalcppWin(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, CalcppApp *appl);
    virtual ~CalcppWin();

    void on_hide() override;
    void show_error(Glib::ustring msg);
    void eval(Glib::ustring text, Gtk::TextIter& end);
    void apply_font(bool defaultFont);
protected:
private:
    void load_config();
    void save_config();
    void build_menu();
    void activate_actions();

    CalcppApp *m_application;
    EvalContext *m_evalContext;
    CalcTextView* m_textView;
    Glib::RefPtr<Gtk::Paned> m_paned;
    CalcTreeView* m_treeView;
    Glib::RefPtr<Gio::Settings> m_settings;
    Glib::RefPtr<Gtk::TextTag> m_fontTag;
};

static const char * const CONFIG_GRP = "General";
static const char * const CONFIG_TEXT = "text";
static const char * const CONFIG_PANED = "paned";
static const char * const CONFIG_POS = "pos";
static const char * const CONFIG_SIZE = "size";
static const char * const CONFIG_SCREEN = "screen";
static const char * const CONFIG_FONT = "font";
static const char * const CONFIG_DEFAULT_FONT = "defaultfont";