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
#include <unictype.h>

/*
 * hold unicode block info got from unistring
 */
class UnicodeBlock  {
public:
    UnicodeBlock(ucs4_t start, ucs4_t end, const Glib::ustring& name);
    UnicodeBlock(const UnicodeBlock& other);
    virtual ~UnicodeBlock();

    Glib::ustring get_name() const;

    ucs4_t get_start() const;

    ucs4_t get_end() const;
private:
    ucs4_t m_start;
    ucs4_t m_end;
    Glib::ustring m_name;
};

/*
 * column model for char display a name and 16 characters
 */
class CharColumns : public Gtk::TreeModel::ColumnRecord
{
public:
    static const ucs4_t CHARS_PER_ROW = 0x10;
    Gtk::TreeModelColumn<Glib::ustring> m_name;
    Gtk::TreeModelColumn<Glib::ustring> m_cell[CHARS_PER_ROW];
    CharColumns()
    {
        add(m_name);
        for (uint32_t i = 0; i < CHARS_PER_ROW; ++i) {
            add(m_cell[i]);
        }
    }
};

/*
 * model for unicode block selection only the first name will be visible
 *   the second is used get the unicode-block infos.
 */
class ComboColumns : public Gtk::TreeModel::ColumnRecord
{
public:
    Gtk::TreeModelColumn<Glib::ustring> m_name;
    Gtk::TreeModelColumn<std::shared_ptr<UnicodeBlock>> m_value;
    ComboColumns()
    {
        add(m_name);
        add(m_value);
    }
};

class BlockRef
: public Glib::Object
{
public:
    BlockRef(const Glib::ustring& name, ucs4_t uc)
    : Glib::ObjectBase(typeid (BlockRef))
    , m_name{name}
    , m_uc{uc}
    {
    }
    Glib::ustring getName()
    {
        return m_name;
    }
    ucs4_t getChar()
    {
        return m_uc;
    }
    static Glib::RefPtr<BlockRef> create(const Glib::ustring& name, ucs4_t uc)
    {
        return Glib::RefPtr<BlockRef> (new BlockRef(name, uc));
    }
private:
    Glib::ustring m_name;
    ucs4_t m_uc;

};

class CalcppWin;

/*
 * a dialog the displays characters in table for a selectalbe unicode blocks,
 *   as a quick and dirty solution  allows editing to copy characters.
 *   So it may be of some help to use greek chars for calculation.
 */
class CharDialog : public Gtk::Dialog {
public:
    CharDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent, Glib::RefPtr<Gio::Settings> settings);
    virtual ~CharDialog();

    Gtk::Widget* createBoxItem(const Glib::RefPtr<BlockRef>& blockRef);

private:
    void create_columns();
    Glib::RefPtr<Gtk::ListStore> create_list();
    void show(const std::shared_ptr<UnicodeBlock>& page);
    void append_combo_row(const std::shared_ptr<UnicodeBlock>& blk);
    Glib::RefPtr<Gtk::ListStore> get_pages();
    void fill_list();
    void char_info();

    CharColumns m_char_columns;
    ComboColumns m_combo_columns;
    Gtk::TreeView* m_table;
    Gtk::ComboBox* m_page;
    Gtk::Entry* m_info;
    Gtk::Entry* m_infoHtml;
    Gtk::FlowBox* m_box;
    Gtk::Entry* m_entry;
    Glib::RefPtr<Gtk::ListStore> m_list; // characters display
    Glib::RefPtr<Gtk::ListStore> m_pages; // unicode pages selection
    Glib::RefPtr<Gio::Settings> m_settings;


};



