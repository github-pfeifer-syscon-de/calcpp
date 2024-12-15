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

#include <math.h>
#include <iostream>
#include <StringUtils.hpp>
#include <psc_i18n.hpp>

#include "EvalContext.hpp"

EvalContext::EvalContext()
: Glib::ObjectBase(typeid (EvalContext))
, m_variable_columns()
, m_angleConv{RadianConversion::get_conversion("")} // use search with invalid key so we wont create addition references
, m_output_format{OutformDecimal::get_form("")} // as above, avoid the hassle to correctly free it afterwards
, property_angle_conv_id_(*this, ANGLE_CONV_ID_PROPERTY, m_angleConv->get_id())
, output_format_id_(*this, OUTPUT_FORMAT_ID_PROPERTY, m_output_format->get_id())
, m_variables()
, m_list{Gtk::ListStore::create(m_variable_columns)}
, m_functionMap{
          {"sqrt",   std::make_shared<FunctionSqrt>()}
    	, {"cbrt",   std::make_shared<FunctionCbrt>()}
        , {"log",    std::make_shared<FunctionLog>()}
        , {"ln",     std::make_shared<FunctionLog>()}
        , {"exp",    std::make_shared<FunctionExp>()}
        , {"sin",    std::make_shared<FunctionSin>()}
        , {"cos",    std::make_shared<FunctionCos>()}
        , {"tan",    std::make_shared<FunctionTan>()}
        , {"asin",   std::make_shared<FunctionAsin>()}
        , {"arcsin", std::make_shared<FunctionAsin>()}
        , {"acos",   std::make_shared<FunctionAcos>()}
        , {"arccos", std::make_shared<FunctionAcos>()}
        , {"atan",   std::make_shared<FunctionAtan>()}
        , {"arctan", std::make_shared<FunctionAtan>()}
        , {"log2",   std::make_shared<FunctionLog2>()}
        , {"log10",  std::make_shared<FunctionLog10>()}
        , {"lg",     std::make_shared<FunctionLog10>()}
        , {"abs",    std::make_shared<FunctionAbs>()}
        , {"fac",    std::make_shared<FunctionFactorial>()}
    }
{
    set_value(StringUtils::u8str(u8"\u03c0"), G_PI); // π or pi set some defaults
    set_value("e", G_E);
    set_value(StringUtils::u8str(u8"\u03d5"), (1.0 + sqrt(5.0)) / 2.0); // ϕ or phi
    // have to use proxy as it seems, to reflect property changes with functions
    //   custom setter,getter for property would be nice but the functions are not virtual...
    Glib::PropertyProxy<Glib::ustring> angle_proxy = property_angle_conv_id_.get_proxy();
    angle_proxy.signal_changed().connect(
        [this, angle_proxy] {
            Glib::ustring val = angle_proxy.get_value();
            //std::cout << "angle setting val " << val << " actual " << m_angleConv->get_id() << std::endl;
            if (val != m_angleConv->get_id()) {
                m_angleConv = AngleConversion::get_conversion(val);
            }
        });
    Glib::PropertyProxy<Glib::ustring> format_proxy = output_format_id_.get_proxy();
    format_proxy.signal_changed().connect(
        [this, format_proxy] {
            Glib::ustring val = format_proxy.get_value();
            //std::cout << "format setting val " << val << " actual " << m_output_format->get_id() << std::endl;
            if (val != m_output_format->get_id()) {
                m_output_format = OutputForm::get_form(val);
            }
        });
}

EvalContext::~EvalContext()
{
}

Glib::RefPtr<Gtk::ListStore>
EvalContext::get_list()
{
    return m_list;
}

bool
EvalContext::find(Glib::ustring name, Gtk::TreeModel::Row* ret)
{
    typedef Gtk::TreeModel::Children type_children; //minimise code length.
    type_children children = m_list->children();
    for (type_children::iterator iter = children.begin(); iter != children.end(); ++iter) {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring varName = row[m_variable_columns.m_name];
        if (varName == name) {
            //std::cout << "Existing row for " << varName << std::endl;
            *ret = row;
            return true;
        }
    }
    //std::cout << "No existing row for " << name << std::endl;
    return false;
}

void
EvalContext::remove(Glib::ustring name)
{
    if (m_variables.erase(name)) { // remove old name
        Gtk::TreeModel::Row row;
        if (find(name, &row)) {
            m_list->erase(row);
        }
    }
}

void
EvalContext::rename(Glib::ustring name, Glib::ustring newName)
{
    double val = 0.0;
    std::map<Glib::ustring, double>::iterator it = m_variables.find(name);
    if (it != m_variables.end()) {
        val = it->second;
        m_variables.erase(it);
    }
    else {
        std::cerr << "Coud not find name " << name << " in map, to rename  to " << newName << std::endl;
    }
    m_variables.insert(std::pair<Glib::ustring, double>(newName, val)); // create new entry
    Gtk::TreeModel::Row row;
    if (find(name, &row)) {
        row.set_value<Glib::ustring>(m_variable_columns.m_name, newName);
    }
}

void
EvalContext::set_value(Glib::ustring name, double val)
{
    Gtk::TreeModel::Row row;
    if (!find(name, &row)) {
        Gtk::TreeIter iter = m_list->append();
        row = *iter;
        row.set_value(m_variable_columns.m_name, name);
    }
    row.set_value<double>(m_variable_columns.m_value, val);
    std::map<Glib::ustring, double>::iterator it = m_variables.find(name);
    if (it != m_variables.end())
        m_variables.erase(it);
    m_variables.insert(std::pair<Glib::ustring, double>(name, val));
}

bool
EvalContext::get_variable(Glib::ustring name, double* val)
{
    std::map<Glib::ustring, double>::iterator it = m_variables.find(name);
    if (it != m_variables.end()) {
        *val = it->second;
        return true;
    }
    *val = 0.0;
    return false;
}

void
EvalContext::set_variable(Glib::ustring name, double val)
{
    set_value(name, val);
}

double
EvalContext::toRadian(double in)
{
    return m_angleConv->convert_to_radian(in);
}

double EvalContext::fromRadian(double in)
{
	return m_angleConv->convert_from_radian(in);
}

//void
//EvalContext::set_angle_conv(AngleConversion* angleConv)
//{
//    m_angleConv = angleConv;
//}

AngleConversion *
EvalContext::get_angle_conv()
{
    return m_angleConv;
}

OutputForm*
EvalContext::get_output_format()
{
    return m_output_format;
}

Glib::PropertyProxy<Glib::ustring>
EvalContext::property_angle_conv_id()
{
    return property_angle_conv_id_.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring>
EvalContext::property_angle_conv_id() const
{
    return property_angle_conv_id_.get_proxy();
}

Glib::PropertyProxy<Glib::ustring>
EvalContext::property_output_format_id()
{
    return output_format_id_.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring>
EvalContext::property_output_format_id() const
{
    return output_format_id_.get_proxy();
}

void
EvalContext::load(Glib::RefPtr<Gio::Settings> settings)
{
    Glib::Variant<std::map < Glib::ustring, double>> values;
    settings->get_value(VAR_CONFIG_GRP, values);
    //std::cout << "load " << values.print(true) << std::endl;

    Glib::VariantIter iter = values.get_iter();
    Glib::Variant<std::tuple < Glib::ustring, double>> entry;
    while (iter.next_value(entry)) {
        auto entryTup = entry.get();
        //            std::cout << "load name" << std::get<0>(entryTup) << "=" << std::get<1>(entryTup) << std::endl;
        set_value(std::get<0>(entryTup), std::get<1>(entryTup));
    }

    settings->bind(CONFIG_ANGLE_UNIT,
                   this, ANGLE_CONV_ID_PROPERTY, Gio::SettingsBindFlags::SETTINGS_BIND_GET);
    settings->bind(CONFIG_OUTPUT_FORMAT,
                   this, OUTPUT_FORMAT_ID_PROPERTY, Gio::SettingsBindFlags::SETTINGS_BIND_GET);

}

void
EvalContext::save(Glib::RefPtr<Gio::Settings> settings)
{
    auto values = Glib::Variant<std::map < Glib::ustring, double>>::create(m_variables);
    //std::cout << "save " << values.print(true) << std::endl;
    settings->set_value(VAR_CONFIG_GRP, values);
}

// simple check if expression will have one result, and no underrun
int
EvalContext::validate(std::list<std::shared_ptr<Token>>& stack)
{
	int cnt = 0;
	for (auto token : stack) {
		std::shared_ptr<NumToken> numToken = std::dynamic_pointer_cast<NumToken>(token);
		if (numToken) {
			++cnt;
		}
		std::shared_ptr<IdToken> idToken = std::dynamic_pointer_cast<IdToken>(token);
		if (idToken) {
			if (!idToken->is_function()) {
				++cnt;
			}
			else {	// function will consume 1 and and add 1, at least for in the momentary state
				if (cnt < 1) {
					return 0;
				}
			}
		}
		std::shared_ptr<OpToken> op = std::dynamic_pointer_cast<OpToken>(token);
		if (op) {
			int opCnt = op->is_binary() ? 2 : 1;
			if (cnt < opCnt) {
				return 0;
			}
			cnt -= opCnt - 1;	// -1 as we add a result
		}
		std::shared_ptr<AssignToken> assignToken = std::dynamic_pointer_cast<AssignToken>(token);
		if (assignToken) {
			throw EvalError(_("Assignment operator only allowed once"));
		}
	}
	return cnt;
}

// if this is an assigment return the id it shoud be assigned to.
//   the stack on return in this case will only contain the expression
std::shared_ptr<IdToken>
EvalContext::assign_token(std::list<std::shared_ptr<Token>>& stack)
{
	std::shared_ptr<IdToken> idAssignToken;
	auto iter = stack.begin();
	if (iter != stack.end()) {
		idAssignToken = std::dynamic_pointer_cast<IdToken>(*iter);
		if (idAssignToken) {
			++iter;
			if (iter != stack.end()) {
				std::shared_ptr<AssignToken> assignToken = std::dynamic_pointer_cast<AssignToken>(*iter);
				if (assignToken) {
					stack.pop_front();	// keep just expression
					stack.pop_front();
				}
				else {
					idAssignToken.reset();	// allow simple check later
				}
			}
		}
	}
	return idAssignToken;
}

double
EvalContext::eval(std::list<std::shared_ptr<Token>> stack)
{
	if (DEBUG) {
		std::cout << "Stack -------------" << std::endl;
		for (auto token : stack) {
			std::cout << token->show() << std::endl;
		}
		std::cout << "-------------------" << std::endl;
	}
	std::shared_ptr<IdToken> idAssignToken = assign_token(stack);
	int cnt = validate(stack);
	if (cnt != 1) {
		throw EvalError(psc::fmt::vformat(
                _("The calculation is not balanced {} (expect 1)")
                , psc::fmt::make_format_args(cnt)));
	}
	//#pragma GCC diagnostic push
	//#pragma GCC diagnostic ignored "-Wno-psabi"		// only supported as compile option
	std::list<double> values;
	for (auto token : stack) {
		std::shared_ptr<NumToken> numToken = std::dynamic_pointer_cast<NumToken>(token);
		if (numToken) {
			values.push_back(numToken->getValue());
		}
		std::shared_ptr<IdToken> idToken = std::dynamic_pointer_cast<IdToken>(token);
		if (idToken) {
			if (idToken->is_function()) {
				//if (values.empty()) {	consistency was checked before
				double valueR = values.back();
				values.pop_back();
				auto map = get_function_map();
				auto entry = map.find(idToken->getId());
                if (entry != map.end()) {
					double result = entry->second->eval(valueR, this);
					values.push_back(result);
                }
                else {
                    auto idTokenName = idToken->show();
					throw EvalError(psc::fmt::vformat(
                            _("No function named {}")
                            , psc::fmt::make_format_args(idTokenName)));
				}
			}
			else {
				double val = 0.0;
				if (!get_variable(idToken->getId(), &val)) {
                    auto idTokenName = idToken->show();
					throw EvalError(psc::fmt::vformat(
                            _("No variable named {}")
                            , psc::fmt::make_format_args(idTokenName)));
				}
				values.push_back(val);
			}
		}
		std::shared_ptr<OpToken> op = std::dynamic_pointer_cast<OpToken>(token);
		if (op) {
			//if (values.empty()) {	consistency was checked before
			double valueR = values.back();
			values.pop_back();
			double valueL = valueR;
			if (op->is_binary()) {
				//if (values.empty()) {	consistency was checked before
				valueL = values.back();
				values.pop_back();
			}
			double result = op->eval(valueL, valueR);
			values.push_back(result);
		}
	}
	//if (values.empty()) {	consistency was checked before
	double total = values.back();
	if (idAssignToken) {	// if this was a assignment assign value
		if (DEBUG)
			std::cout << "Set " << idAssignToken->getId() << " = " << total << std::endl;
		set_variable(idAssignToken->getId(), total);
	}
	//#pragma GCC diagnostic pop
	return total;
}

bool
EvalContext::is_function(const Glib::ustring& fun)
{
    auto map = get_function_map();
    auto iter = map.find(fun);
    return iter != map.end();
}

const std::map<Glib::ustring, std::shared_ptr<Function>>&
EvalContext::get_function_map()
{
    return m_functionMap;
}
