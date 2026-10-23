/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2026 RPf
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
#include <genericimg_config.h>
#include <format>
#include <psc_i18n.hpp>


#include "NoteDialog.hpp"

NoteDialog::NoteDialog(BaseObjectType* cobject
        , const Glib::RefPtr<Gtk::Builder>& builder
        , CalcppWin* parent
        , Glib::RefPtr<Gio::Settings> settings)
: Gtk::Dialog(cobject)
{
    builder->get_widget("notes", m_notes);
    Gtk::Button* addButton{};
    builder->get_widget("addTab", addButton);
    addButton->signal_clicked().connect([&] {
        askName("");
    });
    Glib::ustring info;
    // if more types are available -> add selection
    if (psc::sql::Database::isAvailable(psc::sql::DbType::Sqlite)) {
        auto userData = Glib::get_user_data_dir();
        //std::cout << "userData " << userData << std::endl;
        auto fUserData = Gio::File::create_for_path(userData);
        auto notesFile = fUserData->get_child("notes.sqlite");
        try {
            m_database = psc::sql::Database::create(psc::sql::DbType::Sqlite, notesFile->get_path());
            m_table = std::make_shared<psc::sql::SqlTable>("notes");
            m_nameCol = std::make_shared<psc::sql::SqlVarcharColumn>("name", 128, false, true);
            m_table->addColumns(m_nameCol);
            m_textCol = std::make_shared<psc::sql::SqlVarcharColumn>("text", 2048);
            m_table->addColumns(m_textCol);
            m_database->create(m_table);
            auto bldSelect = m_database->buildSelect(m_table);
            bldSelect->addSelect("*");
            auto select = bldSelect->build();
            while (select->next()) {
                auto name = Glib::ustring(select->getString(m_nameCol->getName()));
                auto text = Glib::ustring(select->getString(m_textCol->getName()));
                addPage(name, text, true);
            }
        }
        catch (const psc::sql::SqlException& exc) {
            info = Glib::ustring::sprintf("Error %s opening %s", exc.what(), fUserData->get_path());
        }
    }
    else {
        info = "No sqlite included, changes will not be permanent!";
    }
    if (m_notes->get_n_pages() == 0) {
        addPage(_("Default"), info);
    }
    show_all();
}

void
NoteDialog::askName(const Glib::ustring& name)
{
    auto dlg = Gtk::Dialog();
    dlg.add_button(Gtk::Stock::ADD, Gtk::RESPONSE_OK);
    dlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dlg.set_default_response(Gtk::RESPONSE_OK);
    dlg.set_transient_for(*this);
    auto box = dlg.get_content_area();
    auto textEntry = Gtk::make_managed<Gtk::Entry>();
    textEntry->set_placeholder_text(_("New tab name"));
    textEntry->set_text(name);
    box->pack_end(*textEntry, Gtk::PACK_EXPAND_WIDGET);
    textEntry->set_visible(true);
    if (dlg.run() == Gtk::RESPONSE_OK) {
        auto txt = textEntry->get_text();
        for (int i = 0; i < m_notes->get_n_pages(); ++i) {
            auto scrl = m_notes->get_nth_page(i);
            auto name = m_notes->get_tab_label_text(*scrl);
            if (name == txt) {
                showError(_("Text already used"), Gtk::MESSAGE_ERROR);
                askName(txt);
                return;
            }
        }
        addPage(txt, "");
    }
}

void
NoteDialog::addPage(const Glib::ustring& name, const Glib::ustring& text, bool activate)
{
    auto scrollWindow = Gtk::make_managed<Gtk::ScrolledWindow>();
    auto textWidget = Gtk::make_managed<Gtk::TextView>();
    scrollWindow->add(*textWidget);
    textWidget->get_buffer()->set_text(text);
    textWidget->set_visible(true);
    scrollWindow->set_visible(true);
    int n = m_notes->append_page(*scrollWindow, name);
    if (activate) {
        m_notes->set_current_page(n);
    }
}

void
NoteDialog::showError(const Glib::ustring& msg, Gtk::MessageType type)
{
    Gtk::MessageDialog messagedialog(*this, msg, FALSE, type);
    messagedialog.run();
    messagedialog.hide();
}

void
NoteDialog::on_response(int response)
{
    if (response == Gtk::RESPONSE_OK) {
        try {
            auto merge = psc::sql::SqlMerge(m_database, m_table);
            for (int i = 0; i < m_notes->get_n_pages(); ++i) {
                auto scrl = m_notes->get_nth_page(i);
                auto name = m_notes->get_tab_label_text(*scrl);
                auto cont = dynamic_cast<Gtk::Container*>(scrl);
                if (cont != nullptr) {
                    for (Widget* chld : cont->get_children()) {
                        auto textView = dynamic_cast<Gtk::TextView*>(chld);
                        if (textView != nullptr) {
                            auto text = textView->get_buffer()->get_text();
                            merge.prepare(name);
                            merge.setString(m_textCol->getName(), text);
                            merge.exec();
                        }
                        else {
                            std::cout << "Not the expected TextView found!" << std::endl;
                        }
                    }
                }
                else {
                    std::cout << "Not the expected container found!" << std::endl;
                }
            }
        }
        catch (const std::exception& exc) {
            auto msg = std::format("Error {} saving texts", exc.what());
            showError(msg, Gtk::MESSAGE_ERROR);
        }
    }
}