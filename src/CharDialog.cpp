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
    builder->get_widget("infoHtml", m_infoHtml);
    builder->get_widget("box", m_box);
    builder->get_widget("entry", m_entry);
    set_transient_for(*parent);

    create_columns();
    m_table->set_model(m_list);
    fill_list();
    show_all();
	m_entry->signal_changed().connect(sigc::mem_fun(*this, &CharDialog::char_info));
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

// allow display of infos for chars
void
CharDialog::char_info()
{
    Glib::ustring text = m_entry->get_text();
    Glib::ustring info;
    Glib::ustring infoHtml;
    auto model = Gio::ListStore<BlockRef>::create();
    std::map<Glib::ustring, ucs4_t> m_infoMap;

    for (auto iter = text.begin(); iter != text.end(); ++iter) {
        gunichar c = *iter;
        ucs4_t uc{c};
        if (uc < 0x10000) {
            info += Glib::ustring::sprintf("\\u%04x", uc);
        }
        else {
            info += Glib::ustring::sprintf("\\U%08x", uc);
        }
        infoHtml += Glib::ustring::sprintf("&#%d;", uc);
        // the script (a.k.a. writing system) is a more informative concept,
        //   so here we remember all distinct scripts, and when clicking
        //   we navigate to the block with the char the script was discovered with
        //   best we can do as e.g. latin has multiple pages ...
        const uc_script_t* info = uc_script(uc);
        //const uc_block_t* block = uc_block(uc);
        Glib::ustring name{info->name};
        auto iterMap = m_infoMap.find(name);
        if (iterMap == m_infoMap.end()) {
            m_infoMap.insert(std::pair(name, uc));
            auto block = BlockRef::create(name, uc);
            model->append(block);
        }
    }
    m_info->set_text(info);
    m_infoHtml->set_text(infoHtml);
    m_box->bind_list_store(model, sigc::mem_fun(*this, &CharDialog::createBoxItem));
}

Gtk::Widget*
CharDialog::createBoxItem(const Glib::RefPtr<BlockRef>& blockRef)
{
    auto name = blockRef->getName();
    ucs4_t uc{blockRef->getChar()};
    auto pageButton = Gtk::make_managed<Gtk::Button>(name);
    pageButton->signal_clicked().connect(
        [this,uc] {
            auto model = m_page->get_model();
            auto chlds = model->children();
            for (auto iter = chlds.begin(); iter != chlds.end(); ++iter) {
                auto row = *iter;
                std::shared_ptr<UnicodeBlock> unicodePage = row[m_combo_columns.m_value];
                if (uc >= unicodePage->get_start()
                 && uc <= unicodePage->get_end()) {
                    m_page->set_active(row);		// and display
                    break;
                }
            }
        });
    return pageButton;
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
