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

#include "CharDialog.hpp"
#include "CalcppWin.hpp"

UnicodeBlock::UnicodeBlock(const UnicodeBlock& other)
: m_start{other.m_start}
, m_end{other.m_end}
, m_name{other.m_name}
{
}

UnicodeBlock::UnicodeBlock(ucs4_t start, ucs4_t end, const Glib::ustring& name)
: m_start{start}
, m_end{end}
, m_name{name}
{
}

UnicodeBlock::~UnicodeBlock()
{
}

Glib::ustring
UnicodeBlock::get_name() const
{
    return m_name;
}

ucs4_t
UnicodeBlock::get_start() const
{
    return m_start;
}

ucs4_t
UnicodeBlock::get_end() const
{
    return m_end;
}


CharDialog::CharDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent, Glib::RefPtr<Gio::Settings> settings)
: Gtk::Dialog(cobject)
, m_char_columns()
, m_combo_columns()
, m_table{nullptr}
, m_page{nullptr}
, m_info{nullptr}
, m_list(Gtk::ListStore::create(m_char_columns))
, m_pages()
, m_settings{settings}
{
    builder->get_widget("table", m_table);
    builder->get_widget("page", m_page);
    builder->get_widget("info", m_info);
    builder->get_widget("entry", m_entry);

    set_transient_for(*parent);

    create_columns();
    m_table->set_model(m_list);
    fill_list();
    show_all();
	m_entry->signal_changed().connect(	// allow display of info for char
		[this] {
			Glib::ustring text = m_entry->get_text();
			Glib::ustring info;
			auto iter = text.begin();
			gunichar last = 0;
			while (iter != text.end()) {
				gunichar c = *iter;
				info += char_info(c);
				++iter;
				last = c;
			}
            m_info->set_text(info);
			if (last != 0) {
				auto iter = m_page->get_active();
			    auto row = *iter;
			    std::shared_ptr<UnicodeBlock> unicodePage = row[m_combo_columns.m_value];
				if (last < unicodePage->get_start()
				 || last > unicodePage->get_end()) {	// if page does not match char
					auto model = m_page->get_model();
					for (auto i : model->children()) {	// find it
						auto r = *i;
					    std::shared_ptr<UnicodeBlock> uniPage = r[m_combo_columns.m_value];
						if (last >= uniPage->get_start()
						 && last <= uniPage->get_end() 	) {
							m_page->set_active(i);		// and display
							break;
						}
					}
				}
			}
		}
	);
    //m_table->set_headers_clickable(true);
    // seems to be no column selection
    //Glib::RefPtr<TreeSelection> selection = m_table->get_selection();
    //selection->set_mode(Gtk::SelectionMode::SELECTION_SINGLE);
    //selection->signal_changed().connect(
    //[ = ] {
    //    Gtk::TreeIter iter = m_page->get_active();
    //    Gtk::TreeModel::Row row = *iter;
    //    std::shared_ptr<UnicodeBlock> unicodePage = row[m_combo_columns.m_value];
    //    add_rows(unicodePage);
    //});
}


CharDialog::~CharDialog()
{
}

void
CharDialog::create_columns()
{
    m_table->append_column("", m_char_columns.m_name);
    for (ucs4_t id = 0; id < CharColumns::CHARS_PER_ROW; ++id) {
        Glib::ustring name = Glib::ustring::sprintf("%x", id);
        m_table->append_column_editable(name, m_char_columns.m_cell[id]);    // _editable
        Gtk::CellRendererText* cell = (Gtk::CellRendererText*)m_table->get_column_cell_renderer(id+1);
        // use same font as on text
        if (!m_settings->get_boolean(CONFIG_DEFAULT_FONT)) {
            cell->property_font() = m_settings->get_string(CONFIG_FONT);
        }
        //cell.editable = true;		// simple hack to allow copy
        cell->signal_edited().connect(
            [this, id](Glib::ustring path, Glib::ustring data)
			{
                Gtk::TreePath tPath(path);
                auto model = m_table->get_model();
                Gtk::TreeIter iter = model->get_iter(tPath);
                Gtk::TreeModel::Row row = *iter;
                Glib::ustring cellVal = row[m_char_columns.m_cell[id]];
				m_entry->set_text(m_entry->get_text() + cellVal);		// let entry event handler do work
                //std::cout << "Cell value " << cellVal << std::endl;
				//if (cellVal.length() > 0) {
				//	ucs4_t uc = cellVal.at(0);
				//	m_info->set_text(char_info(uc));
				//}
            });
    }
}

Glib::ustring
CharDialog::char_info(ucs4_t uc)
{
	const uc_script_t* info = uc_script(uc);
    if (uc < 0x10000) {
        return Glib::ustring::sprintf("%s \\u%04x &#%d;", info->name, uc, uc);
    }
    return Glib::ustring::sprintf("%s \\U%08x &#%d;", info->name, uc, uc);
}

void
CharDialog::show(const std::shared_ptr<UnicodeBlock>& page)
{
    m_list->clear();
    for (ucs4_t r = page->get_start(); r < page->get_end(); r += CharColumns::CHARS_PER_ROW) {
        Gtk::TreeIter iter = m_list->append();
        Gtk::TreeModel::Row row = *iter;
        row[m_char_columns.m_name] = Glib::ustring::sprintf("%03x_", r >> 4u);
        for (ucs4_t c = 0; c < CharColumns::CHARS_PER_ROW; ++c) {
            row[m_char_columns.m_cell[c]] = Glib::ustring::sprintf("%lc", r + c);
        }
    }
}

void
CharDialog::append_combo_row(const std::shared_ptr<UnicodeBlock>& blk)
{
    Gtk::TreeIter iter = m_pages->append();
    Gtk::TreeModel::Row row = *iter;
    // use column with text and value
    row[m_combo_columns.m_name] = blk->get_name();
    row[m_combo_columns.m_value] = blk;
}

Glib::RefPtr<Gtk::ListStore>
CharDialog::get_pages()
{
    if (!m_pages) {
        m_pages = Gtk::ListStore::create(m_combo_columns);
        // depend on libunistring
        // see https://unicode-table.com/en/ for infos
        const uc_block_t *blocks;
        size_t count;
        uc_all_blocks(&blocks, &count);
        for (size_t i = 0; i < count; ++i) {
            const uc_block_t *block = &blocks[i];
			auto row = std::make_shared<UnicodeBlock>(block->start, block->end, block->name);
            append_combo_row(row);
        }
    }
    return m_pages;
}

void
CharDialog::fill_list()
{
    m_page->set_model(get_pages());
    m_page->set_entry_text_column(m_combo_columns.m_name);

    m_page->signal_changed().connect(
        [this] {
			Gtk::TreeIter iter = m_page->get_active();
			Gtk::TreeModel::Row row = *iter;
			auto unicodePage = row[m_combo_columns.m_value];
			show(unicodePage);
		}
	);
    m_page->set_active(0); // preselect latin
}
