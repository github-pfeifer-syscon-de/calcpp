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
#include <genericimg_config.h>
#include <Database.hpp>

class CalcppWin;
class SqlDialog;


class DetailColumns
: public Gtk::TreeModel::ColumnRecord
{
public:
    Gtk::TreeModelColumn<Glib::ustring> m_name;
    Gtk::TreeModelColumn<Glib::ustring> m_type;
    Gtk::TreeModelColumn<Glib::ustring> m_defaultValue;
    Gtk::TreeModelColumn<bool> m_nullable;
    Gtk::TreeModelColumn<bool> m_primaryKey;
    DetailColumns()
    {
        add(m_name);
        add(m_type);
        add(m_defaultValue);
        add(m_nullable);
        add(m_primaryKey);
    }
};

class DetailQuery {
public:
    virtual ~DetailQuery() = default;

    virtual Glib::RefPtr<Gtk::ListStore> query(
        const psc::sql::PtrDatabase& db
        , DetailColumns& detailColumns
        , const Glib::RefPtr<Gtk::ListStore>& columnModel) = 0;
};


class DetailQueryTable
: public DetailQuery
{
public:
    DetailQueryTable(const std::string& type, const std::string& name)
    : m_type{type}
    , m_name{name}
    {
    }
    virtual ~DetailQueryTable() = default;
    Glib::RefPtr<Gtk::ListStore>
    query(const psc::sql::PtrDatabase& db
        , DetailColumns& detailColumns
        , const Glib::RefPtr<Gtk::ListStore>& columnModel) override {
        auto colStmt = db->getMetadata()->getObjectDetail(m_type, m_name);
        auto column = colStmt->getColumnNames();
        while (colStmt->next()) {
            auto cid = colStmt->getString("cid");
            auto name = colStmt->getString("name");
            auto type = colStmt->getString("type");
            auto notnull = colStmt->getLong("notnull");
            auto dflt_value = colStmt->getString("dflt_value");
            auto pk = colStmt->getLong("pk");
            auto iter = columnModel->append();
            auto row = *iter;
            row.set_value(detailColumns.m_name, Glib::ustring(name));
            row.set_value(detailColumns.m_type, Glib::ustring(type));
            row.set_value(detailColumns.m_defaultValue, Glib::ustring(dflt_value));
            row.set_value(detailColumns.m_nullable, (notnull == 0));
            row.set_value(detailColumns.m_primaryKey, (pk != 0));
        }
        return columnModel;
    }
protected:
    std::string m_type;
    std::string m_name;
};

class DetailQueryIndex
: public DetailQuery {
public:
    DetailQueryIndex(const std::string& table, const std::string& index)
    : m_table{table}
    , m_index{index}
    {
    }
    virtual ~DetailQueryIndex() = default;
    Glib::RefPtr<Gtk::ListStore>
    query(const psc::sql::PtrDatabase& db
        , DetailColumns& detailColumns
        , const Glib::RefPtr<Gtk::ListStore>& columnModel) override {
        auto colStmt = db->getMetadata()->getObjectDetail("index", m_index);
        auto column = colStmt->getColumnNames();
        while (colStmt->next()) {
            //auto seq = colStmt->getLong("seqon");
            //auto cid = colStmt->getLong("cid");
            auto name = colStmt->getString("name");
            auto iter = columnModel->append();
            auto row = *iter;
            row.set_value(detailColumns.m_name, Glib::ustring(name));
        }
        return columnModel;
    }
protected:
    std::string m_table;
    std::string m_index;
};


class ObjectColumns
: public Gtk::TreeModel::ColumnRecord
{
public:
    Gtk::TreeModelColumn<Glib::ustring> m_type;
    Gtk::TreeModelColumn<Glib::ustring> m_name;
    Gtk::TreeModelColumn<std::shared_ptr<DetailQuery>> m_query;
    ObjectColumns()
    {
        add(m_type);
        add(m_name);
        add(m_query);
    }
};

class SqlPage;

class PageTextView
: public Gtk::TextView
{
public:
    PageTextView(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, SqlPage* sqlPage)
    : Gtk::TextView(cobject)
    , m_sqlPage{sqlPage}
    {
        set_events(Gdk::KEY_PRESS_MASK);
    }
    virtual ~PageTextView() = default;

    bool on_key_press_event(GdkEventKey* event) override;

private:
    SqlPage* m_sqlPage;
};

class SqlPage
{
public:
    SqlPage(SqlDialog* sqlDialog, const Glib::RefPtr<Gio::File>& file);
    explicit SqlPage(const SqlPage& other) = delete;
    virtual ~SqlPage() = default;

    void object_selected();
    void eval(Glib::ustring text);
protected:
    SqlDialog* m_sqlDialog;
    Glib::RefPtr<Gio::File> m_file;
    psc::sql::PtrDatabase m_database;
    Gtk::Paned* m_paned;
    Gtk::TreeView* m_objects;
    Gtk::TreeView* m_columns;
    PageTextView* m_text;
    ObjectColumns m_objectColumns;
    DetailColumns m_detailColumns;
private:

};

class SqlDialog
: public Gtk::Dialog
{
public:
    SqlDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent, Glib::RefPtr<Gio::Settings> settings);
    explicit SqlDialog(const SqlDialog& other) = delete;
    virtual ~SqlDialog() = default;

    void on_response(int response) override;
    void addPage(const Glib::ustring& name, Gtk::Widget* widget, bool activate = false);
    void showError(const Glib::ustring& msg, Gtk::MessageType type);
    CalcppWin*getCalcWin() {
        return m_calcWin;
    }
protected:
    void askOpen(const Glib::RefPtr<Gio::File>& file);

private:
    CalcppWin* m_calcWin;
    Gtk::Notebook* m_notes;
    Glib::RefPtr<Gio::File> m_lastOpen;
    std::vector<std::shared_ptr<SqlPage>> m_pages;
};
