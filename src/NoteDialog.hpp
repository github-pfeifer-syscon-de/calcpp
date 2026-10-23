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
#pragma once

#include <gtkmm.h>
#include <Database.hpp>


class CalcppWin;

class NoteDialog
: public Gtk::Dialog
{
public:
    NoteDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent, Glib::RefPtr<Gio::Settings> settings);
    explicit NoteDialog(const NoteDialog& other) = delete;
    virtual ~NoteDialog() = default;

    void on_response(int response) override;

protected:
    void addPage(const Glib::ustring& name, const Glib::ustring& text, bool activate = false);
    void askName(const Glib::ustring& name);
    void showError(const Glib::ustring& msg, Gtk::MessageType type);

private:
    Gtk::Notebook* m_notes;
    std::shared_ptr<psc::sql::Database> m_database;
    std::shared_ptr<psc::sql::SqlTable> m_table;
    std::shared_ptr<psc::sql::SqlVarcharColumn> m_nameCol;
    std::shared_ptr<psc::sql::SqlVarcharColumn> m_textCol;

};
