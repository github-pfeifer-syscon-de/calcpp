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


#include "CalcppWin.hpp"
#include "CalcppApp.hpp"
#include "AngleUnit.hpp"
#include "Syntax.hpp"
#include "Token.hpp"
#include "CalcTextView.hpp"
#include "CharDialog.hpp"
#include "DateDialog.hpp"
#include "PrefDialog.hpp"

/*
 * slightly customized file chooser
 */
class CalcFileChooser : public Gtk::FileChooserDialog {
public:
    CalcFileChooser(Gtk::Window *win, bool save)
    : Gtk::FileChooserDialog(*win
                            , save
                            ? "Save File"
                            : "Open File"
                            , save
                            ? Gtk::FileChooserAction::FILE_CHOOSER_ACTION_SAVE
                            : Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN
                            , Gtk::DIALOG_MODAL | Gtk::DIALOG_DESTROY_WITH_PARENT)
    {
        add_button("_Cancel", Gtk::RESPONSE_CANCEL);
        add_button(save
                    ? "_Save"
                    : "_Open", Gtk::RESPONSE_ACCEPT);

        Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
        filter->set_name("Text");
        //filter->add_mime_type("text/plain");
        filter->add_pattern("*.txt");
        set_filter(filter);
    }

    virtual ~CalcFileChooser()
    {
    }
protected:
private:
};


// using templates seems the way we can use the same functions for different types ... vector cast seems difficult
// only used locally, placed here so this does not increase build time
//
// to reduce the number of duplicated references build menu from source entries
template <class T>
class MenuBuilder {
// assert not exactly needed, but nice to get a clear error for what base class this was build
//   (and a java lower-bound reminiscent)
static_assert(std::is_base_of<Named, T>::value, "Required template Named.");
public:
    void build(std::vector<T *> entries, Glib::RefPtr<Gio::Menu> section, Glib::ustring action)
    {
        for (T* named : entries) {
            //std::cout << "menu " << named->get_id() << std::endl;
            Glib::RefPtr<Gio::MenuItem> item = Gio::MenuItem::create(named->get_name(), action);
            item->set_action_and_target(action, Glib::Variant<Glib::ustring>::create(named->get_id()));
            section->append_item(item);
        }
    }
};

// loaded from calc-win.ui
CalcppWin::CalcppWin(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppApp *application)
: Gtk::ApplicationWindow(cobject)       //Calls the base class constructor
, m_application{application}
, m_evalContext{new EvalContext()}
, m_textView{nullptr}
, m_paned{nullptr}
, m_treeView{nullptr}
{
    set_title("Calculator");
    auto pix = Gdk::Pixbuf::create_from_resource(m_application->get_resource_base_path() + "/calcpp.png");
    set_icon(pix);
    // this allows using builder
    //     and create customized subclasses! that's all i ever wanted ;)
    builder->get_widget_derived("text_view", m_textView, this);
    auto panedObj = builder->get_object("paned");
    m_paned = Glib::RefPtr<Gtk::Paned>::cast_dynamic(panedObj);
    builder->get_widget_derived("tree_view", m_treeView, m_evalContext, this);

    load_config();
    activate_actions();

    build_menu();

    // not working see CalcTextView
    //set_events(Gdk::KEY_PRESS_MASK);
    //m_textView->signal_key_press_event().connect(sigc::mem_fun(*this, &CalcppWin::key_pressed));

    show_all_children();
}


CalcppWin::~CalcppWin()
{
    if (m_evalContext != nullptr) {
        delete m_evalContext;
        m_evalContext = nullptr;
    }
}


void
CalcppWin::activate_actions()
{
    add_action(m_settings->create_action(CONFIG_ANGLE_UNIT));
    add_action(m_settings->create_action(CONFIG_OUTPUT_FORMAT));

    auto char_action = Gio::SimpleAction::create("chars");
    char_action->signal_activate().connect (
        [this] (const Glib::VariantBase& value)
		{
			try {
				auto builder = Gtk::Builder::create();
				CharDialog* charDialog;
				builder->add_from_resource(m_application->get_resource_base_path() + "/char-dlg.ui");
				builder->get_widget_derived("CharDialog", charDialog, this, m_settings);
				charDialog->run();
				delete charDialog;  // as this is a toplevel component shoud destroy -> works
			}
			catch (const Glib::Error &ex) {
				std::cerr << "Unable to load char-dlg: " << ex.what() << std::endl;
			}
		});
    add_action(char_action);

    auto calendar_action = Gio::SimpleAction::create("calendar");
    calendar_action->signal_activate().connect(
        [this]  (const Glib::VariantBase& value)
		{
			try {
				auto builder = Gtk::Builder::create();
				DateDialog* dateDialog;
				builder->add_from_resource(m_application->get_resource_base_path() + "/date-dlg.ui");
				builder->get_widget_derived("DateDialog", dateDialog, this, m_settings);
				dateDialog->run();
				delete dateDialog;  // as this is a toplevel component shoud destroy -> works
			}
			catch (const Glib::Error &ex) {
				std::cerr << "Unable to load date-dlg: " << ex.what() << std::endl;
			}
		});
    add_action (calendar_action);

    auto pref_action = Gio::SimpleAction::create("preferences");
    pref_action->signal_activate().connect(
        [this]  (const Glib::VariantBase& value)
		{
			try {
				auto builder = Gtk::Builder::create();
				PrefDialog* prefDialog;
				builder->add_from_resource(m_application->get_resource_base_path() + "/pref-dlg.ui");
				builder->get_widget_derived("PrefDialog", prefDialog, this, m_settings);
				prefDialog->run();
				delete prefDialog;  // as this is a toplevel component shoud destroy -> works
			}
			catch (const Glib::Error &ex) {
				std::cerr << "Unable to load pref-dlg: " << ex.what() << std::endl;
			}
		});
    add_action(pref_action);

    auto load_action = Gio::SimpleAction::create("load");
    load_action->signal_activate().connect(
        [this]  (const Glib::VariantBase& value)
		{
			try {
				CalcFileChooser file_chooser(this, false);
				if (file_chooser.run () == Gtk::ResponseType::RESPONSE_ACCEPT) {
					std::string text = Glib::file_get_contents(file_chooser.get_filename());
					m_textView->get_buffer()->set_text(text);
				}
			}
			catch (const Glib::Error &ex) {
				show_error(Glib::ustring::sprintf("Unable to load file %s", ex.what()));
			}
        });
    add_action (load_action);

    auto save_action = Gio::SimpleAction::create("save");
    save_action->signal_activate().connect (
        [this]  (const Glib::VariantBase& value)
		{
			try {
				CalcFileChooser file_chooser(this, true);
				if (file_chooser.run() == Gtk::ResponseType::RESPONSE_ACCEPT) {
					Glib::ustring text = m_textView->get_buffer()->get_text();
					Glib::file_set_contents(file_chooser.get_filename(), text);
				}
			}
			catch (const Glib::Error &ex) {
				show_error(Glib::ustring::sprintf("Unable to save file %s", ex.what()));
			}
		});
    add_action (save_action);

}

void
CalcppWin::apply_font(bool defaultFont )
{
    Glib::ustring font = m_settings->get_string(CONFIG_FONT);
    Glib::RefPtr<Gtk::TextBuffer> buffer = m_textView->get_buffer();
    Gtk::TextIter start_iter, end_iter;
    buffer->get_bounds(start_iter, end_iter);

    if (!defaultFont) {
		if (!m_fontTag) {
				m_fontTag = buffer->create_tag();
				m_settings->bind(CONFIG_FONT, m_fontTag.get(), "font");
				//std::cout << "Bind font " << font << std::endl;
		}
		buffer->apply_tag(m_fontTag, start_iter, end_iter);
			//std::cout << "   apply font " << font
			//          << " start " <<  start_iter.get_line()
			//          << " end " <<  end_iter.get_line()
			//          << std::endl;
    }
    else {
        if (m_fontTag) {
			buffer->remove_tag(m_fontTag, start_iter, end_iter);
			m_fontTag.clear();
            //std::cout << "   clear font " << font << std::endl;
        }
    }
}

void
CalcppWin::load_config()
{
    Glib::ustring execPath = m_application->get_exec_path();
    // this effort is done to run from source dir (without installed schema)
    Glib::RefPtr<Gio::SettingsSchemaSource> schema_source = Gio::SettingsSchemaSource::get_default();
    std::string fullPath = g_canonicalize_filename(execPath.c_str(), Glib::get_current_dir().c_str());
    Glib::RefPtr<Gio::File> f = Gio::File::create_for_path(fullPath);
    auto bin_dir = f->get_parent();
	std::vector<std::string> relResPath;
	if (G_DIR_SEPARATOR == '\\') {
		relResPath.push_back("..");	// have to escape .libs on windows
	}
	relResPath.push_back("..");
	relResPath.push_back("res");
    std::string resRel = Glib::build_filename(relResPath);
    std::string resPath = g_canonicalize_filename(resRel.c_str(), bin_dir->get_path().c_str());
    std::string resSchema = Glib::build_filename(resPath, m_application->get_id() + ".gschema.xml");
    // this file identifies the development resources dir, beside executable
    try {
        if (Glib::file_test(resSchema, Glib::FileTest::FILE_TEST_IS_REGULAR)) {
            // for development run without global schema, and again the documented function is missing
            schema_source = Glib::wrap(
                    g_settings_schema_source_new_from_directory(
                        resPath.c_str(), g_settings_schema_source_get_default(), FALSE, nullptr));
        }
        Glib::RefPtr<Gio::SettingsSchema> schema = schema_source->lookup(m_application->get_id(), false);
        // Somehow this function was not ported to glibmm
        m_settings = Glib::wrap(g_settings_new_full(schema->gobj(), nullptr, nullptr), false);
    }
    catch (const Glib::Error &exc) {
        show_error(Glib::ustring::sprintf("No settings coud be read! Error %s\nSettings will not work !", exc.what()));
    }
    if (m_settings) {
        Glib::ustring text = m_settings->get_string(CONFIG_TEXT);
        m_textView->get_buffer()->set_text(text);

        int width = 400, height = 300;
        Glib::VariantContainerBase size;
        m_settings->get_value(CONFIG_SIZE, size);
       	Glib::VariantIter sizeIter(size);
        Glib::Variant<gint32> varWidth,varHeight;
        if (sizeIter.next_value(varWidth)
         && sizeIter.next_value(varHeight)) {
            width = varWidth.get();
            height = varHeight.get();
            //std::cout << "Got size " << width << "," << height << std::endl;
        }
        else {
            std::cerr << "Got " << size.get_n_children() << " values for size (expecting 2)" << std::endl;
		}
        set_default_size(width, height);
        Glib::VariantContainerBase pos;
        m_settings->get_value(CONFIG_POS, pos);
        //std::cout << "Loaded V pos " << pos.print(true) << std::endl;
        Glib::VariantIter posIter(pos);
        Glib::Variant<gint32> varX,varY;
        if (posIter.next_value(varX)
         && posIter.next_value(varY)) {
            move(varX.get(), varY.get());
            //std::cout << "Got pos " << varX.get() << "," << varY.get() << std::endl;
        }
        else {
            std::cerr << "Got " << pos.get_n_children() << " values for pos (expecting 2)" << std::endl;
            set_position(Gtk::WindowPosition::WIN_POS_CENTER);
        }
        m_settings->bind(CONFIG_PANED, m_paned->property_position());

        bool defaultFont = m_settings->get_boolean(CONFIG_DEFAULT_FONT);
        apply_font(defaultFont);

        m_evalContext->load(m_settings);
    }
    else {
        set_default_size(400, 300);
        set_position(Gtk::WindowPosition::WIN_POS_CENTER);
    }
}

void
CalcppWin::save_config()
{
    if (m_settings) {
		m_evalContext->save(m_settings);
        Glib::ustring text = m_textView->get_buffer()->get_text();
        m_settings->set_string(CONFIG_TEXT, text);

        int width, height;
        get_size(width, height);
        int x,y;
        get_position(x, y);
        Glib::RefPtr<Gdk::Screen> screen = get_screen();
        Glib::RefPtr<Gdk::Display> display = screen->get_display();
        //settings.set_string (config_screen, display.get_name());
        auto posTup = std::make_tuple(x, y);
        auto pos = Glib::Variant<std::tuple<int, int>>::create(posTup);
        //std::cout << "Saving V pos " << pos.print(true) << std::endl;
        //stdout.printf ("Saving set pos %s\n", pos.print (true));
        m_settings->set_value(CONFIG_POS, pos);
        auto sizeTup = std::make_tuple(width, height);
        auto size = Glib::Variant<std::tuple<int, int>>::create(sizeTup);
        //stdout.printf ("Saving set size %s\n", size.print (true));
        m_settings->set_value(CONFIG_SIZE, size);
    }
}


void
CalcppWin::on_hide()
{
    save_config();
    Gtk::Window::on_hide();
}

void
CalcppWin::show_error(Glib::ustring msg)
{
    // this shoud automatically give some context
    g_warning("show_error %s", msg.c_str());
    Gtk::MessageDialog messagedialog(*this, msg, FALSE, Gtk::MessageType::MESSAGE_WARNING);

    messagedialog.run();
    messagedialog.hide();
}

void
CalcppWin::build_menu()
{
    Glib::RefPtr<Gtk::Builder> builder = m_application->get_menu_builder();
    // formats
    auto outFormObj = builder->get_object("output-format");
    auto outFormMenuItem = Glib::RefPtr<Gio::Menu>::cast_dynamic(outFormObj);

    std::vector<OutputForm *> forms = OutputForm::get_forms();
    MenuBuilder<OutputForm> outFormBuilder;
    outFormBuilder.build(forms, outFormMenuItem, "win.output-format");

    // angle
    auto angleConvObj = builder->get_object("angle-unit");
    auto angleConvMenuItem = Glib::RefPtr<Gio::Menu>::cast_dynamic(angleConvObj);

    std::vector<AngleConversion *> convs = AngleConversion::get_conversions();
    MenuBuilder<AngleConversion> angleConvBuilder;
    angleConvBuilder.build(convs, angleConvMenuItem, "win.angle-unit");
}


void
CalcppWin::eval(Glib::ustring text, Gtk::TextIter& end)
{
    Glib::RefPtr<Gtk::TextBuffer> buffer = m_textView->get_buffer();
    // use anonym tags as there woud be a error when we use names and add them again
    Glib::RefPtr<Gtk::TextTag> fg = buffer->create_tag();
    fg->property_foreground().set_value("blue");
    Glib::RefPtr<Gtk::TextTag> bg = buffer->create_tag();
    bg->property_background().set_value("gray");

    std::vector<Glib::RefPtr < Gtk::TextTag>> tags;
    tags.push_back(fg);
    tags.push_back(bg);

    OutputForm* outputForm = m_evalContext->get_output_format();

    try {
        std::vector<Glib::ustring> lines = StrUtil::split(text, '\n');

        double val;
        bool result = false;
        for (Glib::ustring& line : lines) {
            auto sline = StrUtil::strip(line);
            if (!sline.empty()) { // not worth starting the whole thing up
                if (DEBUG)
                    std::cout << "eval \"" << sline << "\"" << std::endl;
                Syntax syntax(outputForm, m_evalContext);
				auto stack = syntax.parse(line);
				val = m_evalContext->eval(stack);
                result = true;
            }
        }
        if (result) {         // only output last result for not clutter view
            Glib::ustring res = outputForm->format(val);
			res += "\n";
            buffer->insert_with_tags(end, res, tags);
        }
    }
    catch (const ParseError& err) {
        show_error(Glib::ustring::sprintf("Error: %s", err.what()));
    }
}

