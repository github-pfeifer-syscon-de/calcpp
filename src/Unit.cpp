/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf
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
#include <psc_i18n.hpp>
#include <psc_format.hpp>
#include <JsonHelper.hpp>
#include <string.h>
#include <cstddef>
#include <cmath>

#include "Unit.hpp"
#include "NumDialog.hpp"


Dimensions::Dimensions(const std::string& exec_path)
: m_length{std::make_shared<Dimension>()}
, m_area{std::make_shared<Dimension>()}
, m_volume{std::make_shared<Dimension>()}
, m_temperature{std::make_shared<Dimension>()}
, m_speed{std::make_shared<Dimension>()}
, m_mass{std::make_shared<Dimension>()}
, m_time{std::make_shared<Dimension>()}
{
    m_dimensions.push_back(m_length);
    m_dimensions.push_back(m_area);
    m_dimensions.push_back(m_volume);
    m_dimensions.push_back(m_temperature);
    m_dimensions.push_back(m_speed);
    m_dimensions.push_back(m_mass);
    m_dimensions.push_back(m_time);

    loadJson(exec_path);
}


void
Dimensions::loadJsonUnits(const psc::json::PtrJsonObj& unitObj)
{
    auto nameVal = unitObj->getValue("name");
    auto unitsName = nameVal->getString();
    auto dimVal = unitObj->getValue("dim");
    auto unitsDim = dimVal->getString();
    std::shared_ptr<Dimension> dim;
    if (unitsDim == "L") {
        dim = m_length;
    }
    else if (unitsDim == "T") {
        dim = m_temperature;
    }
    else if (unitsDim == "a") {
        dim = m_area;
    }
    else if (unitsDim == "V") {
        dim = m_volume;
    }
    else if (unitsDim == "t") {
        dim = m_time;
    }
    else if (unitsDim == "m") {
        dim = m_mass;
    }
    else if (unitsDim == "v") {
        dim = m_speed;
    }
    else {
        dim = std::make_shared<Dimension>();
        m_dimensions.push_back(dim);
    }
    dim->setName(unitsName);
    dim->setDim(unitsDim);
    //std::cout << "Dimensions::loadJson found units " << unitsName << std::endl;
    auto unitArrVal = unitObj->getValue("unit");
    if (unitArrVal->isArray()) {
        auto unitArr = unitArrVal->getArray();
        dim->loadJsonUnits(unitArr);
    }
    else {
        std::cout << "Dimensions::loadJsonUnits  loading json unit expecting array for " << unitsName << std::endl;
    }
}

void
Dimensions::loadJson(const std::string& execPath)
{
    const std::string unitName{"unit.js"};
    auto userFullPath = Glib::canonicalize_filename("calcpp/", Glib::get_user_data_dir());
    auto userConfig = Gio::File::create_for_path(userFullPath);
    if (!userConfig->query_exists()) {
        userConfig->make_directory_with_parents();
    }
    auto jsonFile = userConfig->get_child(unitName);    // prefere a local copy if it exists
    if (!jsonFile->query_exists()) {
        // this effort is done to run from source dir (without installed package data)
        std::string fullPath = Glib::canonicalize_filename(execPath, Glib::get_current_dir());
        Glib::RefPtr<Gio::File> jsonDir = Gio::File::create_for_path(fullPath);
        auto bin_dir = jsonDir->get_parent();
        std::vector<std::string> relResPath;
        if (G_DIR_SEPARATOR == '\\') {
            relResPath.push_back("..");	// have to escape .libs on windows
        }
        relResPath.push_back("..");
        relResPath.push_back("res");
        std::string resRel = Glib::build_filename(relResPath);
        std::string resPath = Glib::canonicalize_filename(resRel, bin_dir->get_path());
        // this file identifies the development resources dir, beside executable

        jsonFile = Gio::File::create_for_path(resPath + "/" + unitName);
        //std::cout << "Dimensions::loadJson trying " << jsonFile->get_path() << std::boolalpha << " found " << jsonFile->query_exists() << std::endl;
        if (!jsonFile->query_exists()) {
            jsonFile = Gio::File::create_for_path(PACKAGE_DATA_DIR "/" + unitName);
        }
    }
    size_t len;
    std::string etag;
    char* data{nullptr};
    //std::cout << "Dimensions::loadJson loading " << jsonFile->get_path() << std::endl;
    if (jsonFile->load_contents(data, len, etag) && data) {
        //std::cout << "Length::Length len " << len << std::endl;
        try {
            JsonHelper helper;
            std::string localized = localizeJson(data);
            g_free(data);
            helper.load_data(localized.c_str());
            auto root = helper.getRootObj();
            auto unitsVal = root->getValue("units");
            if (unitsVal->isArray()) {
                auto units = unitsVal->getArray();
                for (guint us = 0; us < units->getSize(); ++us) {
                    auto unitVal = units->get(us);
                    if (unitVal->isObject()) {
                        auto unitObj = unitVal->getObject();
                        loadJsonUnits(unitObj);
                    }
                    else {
                        std::cout << "Dimensions::loadJson loading json units expecting object at idx " << us << std::endl;
                    }
                }
            }
            else {
                throw UnitException("Loading json units expected array not found!");
            }
        }
        catch (const std::exception& exc) {
            std::cout << "Error " << exc.what() << " loading json " << jsonFile->get_path() << std::endl;
            throw UnitException(Glib::ustring::sprintf(_("The file %s had errors %s (please check console output)"), exc.what(), jsonFile->get_path()));
        }
    }
    else {
        throw UnitException(Glib::ustring::sprintf(_("A usable %s was not found searching %s"), unitName, jsonFile->get_path()));
        //std::cout << "Length::Length the file " << jsonFile->get_path() << " could no be loaded!" << std::endl;
    }
}

// takes the Js syntax to allow recognition of translations
//   and returns the Json syntax so the parser will accept it
std::string
Dimensions::localizeJson(const char* data)
{
    std::string localized;
    localized.reserve(strlen(data));
    const char *start = strchr(data, '{'); // ignore the js declaration
    if (start) {
        const char *end = strrchr(start, '}');  // ignore the ; from js declaration
        if (end) {
            bool inString{false};
            const char* pos = start;
            while (pos <= end) {
                char c = *pos;
                if (c == '"' && pos > start && *(pos-1) != '\\')  {
                    localized += c;
                    inString = !inString;
                }
                else if (!inString && c == '_' && pos+1 < end && *(pos+1) == '(') {
                    auto chr = strchr(pos, '\"');
                    if (chr && chr < end) {
                        ++chr;
                        auto chrend = strchr(chr, '\"');
                        if (chrend && chrend < end) {
                            std::string str{chr, static_cast<size_t>(chrend - chr)};
                            //std::cout << "Dimension::localizeJson lookup " << str << std::endl;
                            const char* transl = gettext(str.c_str());
                            localized += '"';
                            if (transl) {
                                localized += transl;
                            }
                            else {
                                localized += str;
                            }
                            localized += '"';
                            pos = strchr(chrend, ')');
                        }
                        else {
                            std::cout << "Dimension::localizeJson translation call no close quote pos " << pos - data << "!" << std::endl;

                        }
                    }
                    else {
                        std::cout << "Dimension::localizeJson translation call no quote pos " << pos - data << "!" << std::endl;
                    }
                }
                else {
                    localized += c;
                }
                ++pos;
            }
        }
        else {
            std::cout << "Dimension::localizeJson the expected \"}\" was not found" << std::endl;
        }
    }
    else {
        std::cout << "Dimension::localizeJson the expected \"{\" was not found" << std::endl;
    }
    //std::cout << "Dimension::localizeJson convereted\n" << localized << std::endl;
    return localized;
}


//std::shared_ptr<Dimensions>
//Dimensions::get()
//{
//    if (!m_dimension) {
//        m_dimension = std::make_shared<Dimensions>();
//    }
//    return m_dimension;
//}

std::shared_ptr<Dimension>
Dimensions::getLength()
{
    return m_length;
}

std::shared_ptr<Dimension>
Dimensions::getArea()
{
    return m_area;
}

std::shared_ptr<Dimension>
Dimensions::getVolume()
{
    return m_volume;
}

std::shared_ptr<Dimension>
Dimensions::getTemperature()
{
    return m_temperature;
}

std::shared_ptr<Dimension>
Dimensions::getSpeed()
{
    return m_speed;
}

std::shared_ptr<Dimension>
Dimensions::getMass()
{
    return m_mass;
}

std::shared_ptr<Dimension>
Dimensions::getTime()
{
    return m_time;
}

std::list<std::shared_ptr<Dimension>>
Dimensions::getDimensions()
{
    return m_dimensions;
}


Dimension::Dimension()
{
    m_units.reserve(16);
}

void
Dimension::add(std::shared_ptr<Unit> unit)
{
    m_units.push_back(unit);
}

std::vector<std::shared_ptr<Unit>>
Dimension::getUnits()
{
    return m_units;
}

Glib::ustring
Dimension::getName()
{
    return m_name;
}

void
Dimension::setName(const Glib::ustring& name)
{
    m_name = name;
}

Glib::ustring
Dimension::getDim()
{
    return m_dim;
}

void
Dimension::setDim(const Glib::ustring& dim)
{
    m_dim = dim;
}

std::shared_ptr<Unit>
Dimension::findById(const Glib::ustring& id)
{
    std::shared_ptr<Unit> unit;
    auto entry = m_ids.find(id);
    if (entry != m_ids.end()) {
        unit = entry->second;
    }
    return unit;
}

void
Dimension::loadJsonUnits(const psc::json::PtrJsonArr& unitArr)
{
    for (guint u = 0; u < unitArr->getSize(); ++u) {
        auto uVal = unitArr->get(u);
        if (uVal->isObject()) {
            auto uObj = uVal->getObject();
            loadJsonUnit(uObj);
        }
        else {
            std::cout << "Dimension::loadJsonUnits json unit expecting object for " << getName() << " at " << u << std::endl;
        }
    }
}

void
Dimension::loadJsonUnit(const psc::json::PtrJsonObj& uObj)
{
    auto uNameVal = uObj->getValue("name");
    auto uName = uNameVal->getString();
    //std::cout << "Dimension::loadJsonUnit found unit " << uName << std::endl;
    auto ofUnitVal = uObj->getValue("ofUnit");
    auto ofValueVal = uObj->getValue("ofValue");
    double fact{1.0};
    if (ofUnitVal && ofValueVal) {
        auto ofUnit = ofUnitVal->getString();
        auto ofValue = ofValueVal->getDouble();
        auto ofUnitEntry = m_ids.find(ofUnit);
        if (ofUnitEntry != m_ids.end()) {
            auto unit = ofUnitEntry->second;
            fact = unit->fromUnit(ofValue);
            //std::cout << "Dimension::loadJsonUnit " << uName << " using " << ofUnit << " from " << ofValue << " fact " << fact << std::endl;
        }
        else {
            std::cout << "Dimension::loadJsonUnit referenced unit " << ofUnit << " not found!"  << std::endl;
            return;
        }
    }
    else {
        auto uFactVal = uObj->getValue("fact");
        fact = uFactVal->getDouble();
    }
    double offs{0.0};
    auto uOffsVal = uObj->getValue("offs");
    if (uOffsVal) {
        offs = uOffsVal->getDouble();
    }
    Glib::ustring id;
    auto unitId = uObj->getValue("id");
    if (unitId) {
        id = unitId->getString();
    }
    std::shared_ptr<Unit> unit;
    if (id == "hms") {
        unit = std::make_shared<UnitHms>(uName, fact, offs);
    }
    else {
        unit = std::make_shared<Unit>(uName, fact, offs);
    }
    if (!id.empty()) {
        m_ids.insert(std::pair(id, unit));
        unit->setId(id);
    }
    else {
        std::cout << "Id is expected, this may not work ... " <<  std::endl;
    }
    add(unit);
}


Unit::Unit(const Glib::ustring& name, double factor, double offset)
: m_name{name}
, m_factor{factor}
, m_offset{offset}
{

}

Glib::ustring
Unit::getName() const
{
    return m_name;
}

void
Unit::setId(const Glib::ustring& id)
{
    m_id = id;
}

Glib::ustring
Unit::getId() const
{
    return m_id;
}

double
Unit::getFactor() const
{
    return m_factor;
}

double
Unit::getOffset() const
{
    return m_offset;
}

Glib::ustring
Unit::toUnit(double val, NumDialog* numDialog) const
{
    auto result = toUnit(val);
    return numDialog->format(result);
}

double
Unit::fromUnit(Gtk::Entry* entry, NumDialog* numDialog) const
{
    double val = numDialog->parse(entry);
    return fromUnit(val);
}

double
Unit::toUnit(double val) const
{
    return val / m_factor - m_offset;
}

double
Unit::fromUnit(double val) const
{
    return (val + m_offset) * m_factor;
}


UnitHms::UnitHms(const Glib::ustring& name, double factor, double offset)
: Unit(name, factor, offset)
{
}

Glib::ustring
UnitHms::toUnit(double val, NumDialog* numDialog) const
{
   auto result = Unit::toUnit(val);
   bool negative = false;
   if (result < 0.0) {
       result = std::abs(result);
       negative = true;
   }
   auto s = std::fmod(result, 60.0);
   Glib::ustring sval = psc::fmt::format(std::locale(""), "{:Lg}", s);
   result -= s;
   result /= 60.0;
   auto m = std::fmod(result, 60.0);
   sval.insert(0, psc::fmt::format("{:d}:", static_cast<uint32_t>(m)));
   result -= m;
   result /= 60.0;
   auto h = std::fmod(result, 24.0);
   sval.insert(0, psc::fmt::format("{:d}:", static_cast<uint32_t>(h)));
   result -= h;
   auto d = result / 24.0;
   if (d > 0.0) {
        sval.insert(0, psc::fmt::format("{:d}:", static_cast<uint32_t>(d)));
   }
   if (negative) {
       sval.insert(0, "-");
   }
   return sval;
}

double
UnitHms::splitPart(Gtk::Entry* entry, size_t& pos, Glib::ustring& sval) const
{
    sval = sval.substr(0, pos);
    pos = sval.rfind(':');
    if (pos == sval.npos) { // without separator consider all as value
        pos = 0;
    }
    else {
        ++pos;
    }
    auto spart = sval.substr(pos);
    std::string::size_type offs;
    unsigned long val = std::stoul(spart, &offs);
    if (offs != spart.size()) {
        entry->grab_focus();
        throw std::invalid_argument(_("Invalid number"));
    }
    return static_cast<double>(val);
}

double
UnitHms::fromUnit(Gtk::Entry* entry, NumDialog* numDialog) const
{
    Glib::ustring sval = entry->get_text();
    auto pos = sval.rfind(':');
    if (pos == sval.npos) { // without separator consider all as seconds
        pos = 0;
    }
    else {
        ++pos;
    }
    std::string::size_type offs;
    auto spart = sval.substr(pos);
    auto secVal = std::stod(spart, &offs);      // allow locale sensitive decimals e.g. 1.2
    if (offs != spart.size()) {
        entry->grab_focus();
        throw std::invalid_argument(_("Invalid number"));
    }
    if (pos > 0) {
        --pos;
        auto m = splitPart(entry, pos, sval);
        secVal += 60.0 * m;
        if (pos > 0) {
            --pos;
            auto h = splitPart(entry, pos, sval);
            secVal += 60.0 * 60.0 * h;
            if (pos > 0) {
                --pos;
                auto d = splitPart(entry, pos, sval);
                secVal += 24.0 * 60.0 * 60.0 * d;
            }
        }
    }
    return Unit::fromUnit(secVal);
}
