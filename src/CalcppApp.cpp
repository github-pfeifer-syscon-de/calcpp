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

#include <iostream>
#include <exception>

#include <config.h>
#include "CalcppApp.hpp"

CalcppApp::CalcppApp(int argc, char **argv)
: Gtk::Application(argc, argv, "de.pfeifer_syscon.calcpp")
, m_calcppAppWindow{nullptr}
, m_exec{argv[0]}
{
}

CalcppApp::~CalcppApp()
{
}

void
CalcppApp::on_activate()
{
    auto builder = Gtk::Builder::create();
    try {
        builder->add_from_resource(get_resource_base_path() + "/calc-win.ui");
        builder->get_widget_derived("CalcppWin", m_calcppAppWindow, this);
        add_window(*m_calcppAppWindow);
        m_calcppAppWindow->show();
    }
    catch (const Glib::Error &ex) {
        std::cerr << "Unable to load calc-win: " << ex.what() << std::endl;
    }
}


void
CalcppApp::on_action_quit()
{
    m_calcppAppWindow->hide();

    // and we shoud delete appWindow if we were not going exit anyway
    // Not really necessary, when Gtk::Widget::hide() is called, unless
    // Gio::Application::hold() has been called without a corresponding call
    // to Gio::Application::release().
    quit();
}

void
CalcppApp::on_action_about() {
    auto builder = Gtk::Builder::create();
    try {
        builder->add_from_resource(get_resource_base_path() + "/abt-dlg.ui");
        auto dlgObj = builder->get_object("abt-dlg");
        auto dialog = Glib::RefPtr<Gtk::AboutDialog>::cast_dynamic(dlgObj);
        auto icon = Gdk::Pixbuf::create_from_resource(get_resource_base_path() + "/calcpp.png");
        dialog->set_logo(icon);
        dialog->set_transient_for(*m_calcppAppWindow);
        dialog->show_all();
        dialog->run();
        dialog->hide();
    }
    catch (const Glib::Error &ex) {
        std::cerr << "Unable to load about-dialog: " << ex.what() << std::endl;
    }
}

void
CalcppApp::on_action_help() {
    auto builder = Gtk::Builder::create();
    try {
        builder->add_from_resource(get_resource_base_path() + "/help-dlg.ui");
        auto dlgObj = builder->get_object("help-dlg");
        auto dialog = Glib::RefPtr<Gtk::Dialog>::cast_dynamic(dlgObj);
        auto textObj = builder->get_object("text");
        auto text = Glib::RefPtr<Gtk::TextView>::cast_dynamic(textObj);

        std::string fullPath = g_canonicalize_filename(m_exec.c_str(), Glib::get_current_dir().c_str());
        Glib::RefPtr<Gio::File> f = Gio::File::create_for_path(fullPath);
        auto dist_dir = f->get_parent()->get_parent();
        // this file identifies the development dir, beside executable
        auto readme = Glib::build_filename(dist_dir->get_path(), "README");
        if (!Glib::file_test(readme, Glib::FileTest::FILE_TEST_IS_REGULAR)) {
            // alternative search in distribution
            Glib::RefPtr<Gio::File> packageData = Gio::File::create_for_path(PACKAGE_DATA_DIR); // see Makefile.am
            std::string base_name = packageData->get_basename();
            readme = Glib::build_filename(packageData->get_parent()->get_path(), "doc", base_name, "README");
        }
        if (Glib::file_test(readme, Glib::FileTest::FILE_TEST_IS_REGULAR)) {
            Glib::ustring readmeText = Glib::file_get_contents(readme);
            text->get_buffer()->set_text(readmeText);
        } else {
            text->get_buffer()->set_text(Glib::ustring::sprintf("The README file %s coud not be located.\n", readme));
        }

        dialog->set_transient_for(*m_calcppAppWindow);
        dialog->show_all();
        dialog->run();
        dialog->hide();
    }
    catch (const Glib::Error &ex) {
        std::cerr << "Unable to load menubar: " << ex.what() << std::endl;
    }
}

Glib::RefPtr<Gtk::Builder>
CalcppApp::get_menu_builder()
{
    return m_builder;
}

Glib::ustring
CalcppApp::get_exec_path()
{
    return m_exec;
}

void
CalcppApp::on_startup()
{
    Gtk::Application::on_startup();

    add_action("quit", sigc::mem_fun(*this, &CalcppApp::on_action_quit));
    add_action("about", sigc::mem_fun(*this, &CalcppApp::on_action_about));
    add_action("help", sigc::mem_fun(*this, &CalcppApp::on_action_help));

    m_builder = Gtk::Builder::create();
    try {
        m_builder->add_from_resource(get_resource_base_path() + "/app-menu.ui");
        auto menuObj = m_builder->get_object("menubar");
        auto menuBar = Glib::RefPtr<Gio::Menu>::cast_dynamic(menuObj);
        if (menuBar)
            set_menubar(menuBar);
        else
            std::cerr << "Cound not find/cast menubar!" << std::endl;
    }
    catch (const Glib::FileError& ex) {
        std::cerr << "Unable to load menubar: " << ex.what() << std::endl;
    }
}

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");      // we depend on locale
    CalcppApp app(argc, argv);

    return app.run();
}