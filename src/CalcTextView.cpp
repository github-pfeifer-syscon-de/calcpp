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

#include "CalcTextView.hpp"
#include "CalcppWin.hpp"

CalcTextView::CalcTextView(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* calccppWin)
: Gtk::TextView(cobject)
, m_calccppWin{calccppWin}
{
}


CalcTextView::~CalcTextView()
{
}


bool
CalcTextView::on_key_press_event(GdkEventKey* event)
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
        if (m_calccppWin)
            m_calccppWin->eval(text);
        else
            std::cerr <<  "Ptr for calc win not set! No eval can be done." << std::endl;
        return true;
    }
    return Gtk::TextView::on_key_press_event(event);
}

