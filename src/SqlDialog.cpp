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
#include <format>
#include <psc_i18n.hpp>
#include <StringUtils.hpp>

#include "SqlDialog.hpp"
#include "CalcppApp.hpp"
#include "CalcppWin.hpp"

bool
PageTextView::on_key_press_event(GdkEventKey* event)
{
    if (false)
        std::cout << "got keychar state " << event->state
                  << " mask " << Gdk::ModifierType::CONTROL_MASK
                  << " keyv " << event->keyval
                  << " cntl  " << (int)(event->string[0])
                  << std::endl;
    if ((event->state & Gdk::ModifierType::CONTROL_MASK)
      && strcmp(event->string, "\r") == 0) {
        Glib::ustring text;
        Glib::RefPtr<Gtk::TextBuffer> buffer = get_buffer();
        Gtk::TextIter end;

        if (buffer->get_has_selection()) {
            Gtk::TextIter start;
            buffer->get_selection_bounds(start, end);
            text = buffer->get_slice(start, end, true);
            end.forward_to_line_end();
        }
        else {
            // read text from line cursor is on
            Glib::RefPtr<Gtk::TextMark> cursor = buffer->get_insert();
            Gtk::TextIter start = cursor->get_iter();
            end = cursor->get_iter();
            start.set_line_offset(0);
            if (end.get_char() != '\n') {
                end.forward_to_line_end();
            }
            text = buffer->get_slice(start, end, true);
        }
        end.forward_char(); // beginning of next line
        if (true) { // end != null
            buffer->place_cursor(end); // effectively kills selection
        }
        //std::cout << "Going to eval " << text << std::endl;
        if (m_sqlPage)
            m_sqlPage->eval(text);
        else
            std::cerr <<  "Ptr for calc win not set! No eval can be done." << std::endl;
        return true;
    }
    return Gtk::TextView::on_key_press_event(event);
}


SqlPage::SqlPage(SqlDialog* sqlDialog, const Glib::RefPtr<Gio::File>& file)
: m_sqlDialog{sqlDialog}
, m_file{file}
{
    auto builder = Gtk::Builder::create();
    builder->add_from_resource(m_sqlDialog->getCalcWin()->getApplication()->get_resource_base_path() + "/sql-comp.ui");
    builder->get_widget("paned", m_paned);
    builder->get_widget("objects", m_objects);
    builder->get_widget("columns", m_columns);
    builder->get_widget_derived("text", m_text, this);
    m_sqlDialog->addPage(m_file->get_basename(), m_paned, true);
    try {
        auto typeModel = Gtk::TreeStore::create(m_objectColumns);
        m_database = psc::sql::Database::create(psc::sql::DbType::Sqlite,m_file->get_path());
        auto metadata = m_database->getMetadata();
        auto selectTbl = metadata->getTableList();
        while (selectTbl->next()) {
            auto type = selectTbl->getString("type");
            auto tblName = selectTbl->getString("name");
            auto nameModelIter = typeModel->append();
            auto nameModelRow = *nameModelIter;
            nameModelRow.set_value(m_objectColumns.m_name, Glib::ustring(tblName));
            nameModelRow.set_value(m_objectColumns.m_type, Glib::ustring(type));
            nameModelRow.set_value(m_objectColumns.m_query,
                std::dynamic_pointer_cast<DetailQuery>(
                        std::make_shared<DetailQueryTable>("table", tblName)));
            auto selectChld = metadata->getObjectList(tblName);
            //selectChld->reset();
            //selectChld->setString(1, tblName);
            while (selectChld->next()) {
                auto idxType = selectChld->getString("type");
                auto idxName = selectChld->getString("name");
                if (/*idxName == tblName &&*/ idxType != type) {  // view links to view again, not interested
                    auto chldModelIter = typeModel->append(nameModelIter->children());
                    auto chldModelRow = *chldModelIter;
                    chldModelRow.set_value(m_objectColumns.m_name, Glib::ustring(idxName));
                    chldModelRow.set_value(m_objectColumns.m_type, Glib::ustring(idxType));
                    if (idxType == "index") {
                        chldModelRow.set_value(m_objectColumns.m_query,
                            std::dynamic_pointer_cast<DetailQuery>(
                                    std::make_shared<DetailQueryIndex>(tblName, idxName)));
                    }
                }
            }
        }
        m_objects->set_model(typeModel);
        m_objects->append_column_editable(_("Name"), m_objectColumns.m_name);
        m_objects->append_column_editable(_("Type"), m_objectColumns.m_type);
        m_objects->expand_all();
        m_objects->get_selection()->signal_changed().connect(
                sigc::mem_fun(*this, &SqlPage::object_selected));
    }
    catch (const std::exception& exc) {
        auto info = Glib::ustring::sprintf(_("Error %s opening %s"), exc.what(), m_file->get_path());
        m_sqlDialog->showError(info, Gtk::MESSAGE_ERROR);
    }
}

void
SqlPage::eval(Glib::ustring text)
{
    StringUtils::trim(text);
    try {
        //if (text.substr(0, 6).lowercase() == "select") {
        auto stmt = m_database->prepareStatement(text);
        auto ret = stmt->exec();
        if (ret == psc::sql::Result::ConstraintViolated) {
            auto info = Glib::ustring::sprintf(_("Error executing %s constraint violated"), text);
            m_sqlDialog->showError(info, Gtk::MESSAGE_ERROR);
            return;
        }
        if (ret == psc::sql::Result::Done) {
            return;
        }
        auto colNames = stmt->getColumnNames();
        Gtk::TreeModelColumnRecord dynamicRecord;
        std::vector<Gtk::TreeModelColumn<Glib::ustring>> tableCols;
        tableCols.reserve(colNames.size());
        for (size_t n = 0; n < colNames.size(); ++n) {
            Gtk::TreeModelColumn<Glib::ustring> tableCol;
            dynamicRecord.add(tableCol);
            tableCols.emplace_back(std::move(tableCol));
        }
        auto columnModel = Gtk::ListStore::create(dynamicRecord);
        m_columns->remove_all_columns();
        m_columns->set_model(columnModel);
        for (size_t n = 0; n < tableCols.size(); ++n) {
            auto& col = colNames[n];
            auto& tableCol = tableCols[n];
            m_columns->append_column_editable(col, tableCol);
        }
        do {
            auto types = stmt->getResultTypes();
            auto iter = columnModel->append();
            auto row = *iter;
            for (size_t col = 0; col < types.size(); ++col) {
                auto& tableCol = tableCols[col];
                auto iCol{static_cast<int>(col)};
                if (types[col] ==  psc::sql::Datatype::Integer) {
                    int64_t val = stmt->getLong(iCol);
                    Glib::ustring sval = std::to_string(val);
                    row.set_value(tableCol, sval);
                }
                else if (types[col] ==  psc::sql::Datatype::Numeric) {
                    double val = stmt->getDouble(iCol);
                    Glib::ustring sval = Glib::ustring::sprintf("%.3lf", val);
                    row.set_value(tableCol, sval);
                }
                else if (types[col] == psc::sql::Datatype::Text) {
                    Glib::ustring val = stmt->getString(iCol);
                    row.set_value(tableCol, val);
                }
            }
        } while (stmt->next());
        //}
        //else {
        //    m_database->execute(text);
        //}
    }
    catch (const std::exception& exc) {
        auto info = Glib::ustring::sprintf(_("Error %s opening %s"), exc.what(), m_file->get_path());
        m_sqlDialog->showError(info, Gtk::MESSAGE_ERROR);
    }
}

void
SqlPage::object_selected()
{
    m_columns->remove_all_columns();
    m_columns->append_column_editable(_("Name"), m_detailColumns.m_name);
    m_columns->append_column_editable(_("Type"), m_detailColumns.m_type);
    //auto toggleNullable = Gtk::make_managed<Gtk::CellRendererToggle>();
    m_columns->append_column_editable(_("Nullable"), m_detailColumns.m_nullable);
    //auto togglePrimary = Gtk::make_managed<Gtk::CellRendererToggle>();
    m_columns->append_column_editable(_("Primary-key"), m_detailColumns.m_primaryKey);
    m_columns->append_column_editable(_("DefaultValue"), m_detailColumns.m_defaultValue);

    auto iterObj = m_objects->get_selection()->get_selected();
    auto rowObj = *iterObj;
    auto query = rowObj.get_value(m_objectColumns.m_query);
    auto name = rowObj.get_value(m_objectColumns.m_name);
    auto columnModel = Gtk::ListStore::create(m_detailColumns);
    if (query) {
        try {
            query->query(m_database, m_detailColumns, columnModel);
        }
        catch (const std::exception& exc) {
            auto info = Glib::ustring::sprintf(_("Error %s query cols %s"), exc.what(), name);
            m_sqlDialog->showError(info, Gtk::MESSAGE_ERROR);
        }        //auto typeIter = rowObj.parent();
        //auto rowType = *typeIter;
        //auto type = rowType.get_value(m_objectColumns.m_type);
        //if (!type.empty()) {
        //}
    }
    m_columns->set_model(columnModel);
}

SqlDialog::SqlDialog(BaseObjectType* cobject
        , const Glib::RefPtr<Gtk::Builder>& builder
        , CalcppWin* calcWin
        , Glib::RefPtr<Gio::Settings> settings)
: Gtk::Dialog(cobject)
, m_calcWin{calcWin}
{
    builder->get_widget("notes", m_notes);
    Gtk::Button* addButton{};
    builder->get_widget("addTab", addButton);
    addButton->signal_clicked().connect([&] {
        askOpen(m_lastOpen);
    });
#   ifndef USE_SQLITE3
    showError(_("No usable database connection found"), Gtk::MESSAGE_ERROR);
#   endif
    show_all();
}

void
SqlDialog::askOpen(const Glib::RefPtr<Gio::File>& file)
{
    Gtk::FileChooserDialog fileChooser(*this
        , _("Open Database")
        , Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN
        , Gtk::DIALOG_MODAL | Gtk::DIALOG_DESTROY_WITH_PARENT);
    fileChooser.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_ACCEPT);
    fileChooser.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    if (file) {
        fileChooser.set_file(file);
    }
    if (fileChooser.run() == Gtk::RESPONSE_ACCEPT) {
        m_lastOpen = fileChooser.get_file();
        auto sqlPage = std::make_shared<SqlPage>(this, m_lastOpen);
        m_pages.emplace_back(std::move(sqlPage));
    }
}

void
SqlDialog::addPage(const Glib::ustring& name, Gtk::Widget* widget, bool activate)
{
    int n = m_notes->append_page(*widget, name);
    if (activate) {
        m_notes->set_current_page(n);
    }
}

void
SqlDialog::showError(const Glib::ustring& msg, Gtk::MessageType type)
{
    Gtk::MessageDialog messagedialog(*this, msg, FALSE, type);
    messagedialog.run();
    messagedialog.hide();
}

void
SqlDialog::on_response(int response)
{
    if (response == Gtk::RESPONSE_OK) {
    }
}