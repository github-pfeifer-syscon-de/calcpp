/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
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

#include "CalcppWin.hpp"

/*
 * get the application up and running
 *   about and help dialog
 */
class CalcppApp : public Gtk::Application {
public:
    CalcppApp(int arc, char **argv);
    virtual ~CalcppApp() = default;

    void on_activate() override;
    void on_startup() override;

    Glib::RefPtr<Gtk::Builder> get_menu_builder();
    Glib::ustring get_exec_path();
    Glib::ustring getReadmeText();
private:
    CalcppWin *m_calcppAppWindow;
    Glib::ustring m_exec;
    Glib::RefPtr<Gtk::Builder> m_builder;

    std::string get_file(const std::string& name);
    void on_action_quit();
    void on_action_about();
    void on_action_help();
};
