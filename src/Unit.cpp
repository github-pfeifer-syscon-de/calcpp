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

#include <giomm.h>
#include <iostream>
#include <psc_i18n.hpp>
#include <JsonHelper.hpp>
#include <string.h>
#include <cstddef>

#include "Unit.hpp"


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
    // this effort is done to run from source dir (without installed package data)
    std::string fullPath = g_canonicalize_filename(execPath.c_str(), Glib::get_current_dir().c_str());
    Glib::RefPtr<Gio::File> jsonDir = Gio::File::create_for_path(fullPath);
    auto bin_dir = jsonDir->get_parent();
    std::vector<std::string> relResPath;
    if (G_DIR_SEPARATOR == '\\') {
        relResPath.push_back("..");	// have to escape .libs on windows
    }
    relResPath.push_back("..");
    relResPath.push_back("res");
    std::string resRel = Glib::build_filename(relResPath);
    std::string resPath = g_canonicalize_filename(resRel.c_str(), bin_dir->get_path().c_str());
    // this file identifies the development resources dir, beside executable

    std::string unitName{"unit.js"};
    auto jsonFile = Gio::File::create_for_path(resPath + "/" + unitName);
    //std::cout << "Dimensions::loadJson trying " << jsonFile->get_path() << std::boolalpha << " found " << jsonFile->query_exists() << std::endl;
    if (!jsonFile->query_exists()) {
        jsonFile = Gio::File::create_for_path(PACKAGE_DATA_DIR "/" + unitName);
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
    auto unit = std::make_shared<Unit>(uName, fact, offs);
    auto unitId = uObj->getValue("id");
    if (unitId) {
        auto id = unitId->getString();
        m_ids.insert(std::pair(id, unit));
        unit->setId(id);
    }
    else {
        std::cout << "Id is ecpected this may not work ... " <<  std::endl;
    }
    add(unit);
}


// Length
//    auto mm = std::make_shared<Unit>("mm", 1.0/1000.0);
//    add(mm);
//    auto cm = std::make_shared<Unit>("cm", 1.0/100.0);
//    add(cm);
//    auto m = std::make_shared<Unit>("m", 1.0);
//    add(m);
//    auto km = std::make_shared<Unit>("km", 1000.0);
//    add(km);
//    auto in = std::make_shared<Unit>("in", 25.4/1000.0);
//    add(in);
//    auto ft = std::make_shared<Unit>("ft", 0.3048);
//    add(ft);
//    auto mi = std::make_shared<Unit>("mi", 1609.344);
//    add(mi);
//    auto nmi = std::make_shared<Unit>("naut.mi", 1852.0);
//    add(nmi);
//    auto au = std::make_shared<Unit>("AU", 149597870700.0 );
//    add(au);
//    auto lj = std::make_shared<Unit>("lj", 9460730472580800.0);
//    add(lj);
//
//
//Area
//    auto mm2 = std::make_shared<Unit>("mm²", 1.0/1000000.0);
//    add(mm2);
//    auto cm2 = std::make_shared<Unit>("cm²", 1.0/10000.0);
//    add(cm2);
//    auto m2 = std::make_shared<Unit>("m²", 1.0);
//    add(m2);
//    auto km2 = std::make_shared<Unit>("km²", 1000000.0);
//    add(km2);
//    auto in2 = std::make_shared<Unit>("in²", 0.00064516);
//    add(in2);
//    auto ft2 = std::make_shared<Unit>("ft²", 0.09290304);
//    add(ft2);
//    auto ya2 = std::make_shared<Unit>("ya²", 0.83612736);
//    add(ya2);
//    auto mi2 = std::make_shared<Unit>("mi²", 2589988.110336 );
//    add(mi2);
//    auto ha = std::make_shared<Unit>("ha", 10000.0);
//    add(ha);
//
//Volume
//    auto mm3 = std::make_shared<Unit>("mm³", 1.0/1000000000.0);
//    add(mm3);
//    auto cm3 = std::make_shared<Unit>("cm³", 1.0/1000000.0);
//    add(cm3);
//    auto m3 = std::make_shared<Unit>("m³", 1.0);
//    add(m3);
//    auto L = std::make_shared<Unit>("L", 1.0/1000.0);
//    add(L);
//    auto mL = std::make_shared<Unit>("mL", 1.0/1000000.0);
//    add(mL);
//    auto imFlUn = std::make_shared<Unit>("imper.fl.ounce", mL->fromUnit(28.4130625));
//    add(imFlUn);
//    auto imPint = std::make_shared<Unit>("imper.pint",  mL->fromUnit(568.26125));
//    add(imPint);
//    auto imGal = std::make_shared<Unit>("imper.gallon", L->fromUnit(4.54609));
//    add(imGal);
//    auto usFlUn = std::make_shared<Unit>("US.fl.ounce", mL->fromUnit(29.5735295625));
//    add(usFlUn);
//    auto usPint = std::make_shared<Unit>("US.liq.pint", mL->fromUnit(473.176473));
//    add(usPint);
//    auto usGal = std::make_shared<Unit>("US.gallon", L->fromUnit(3.785411784));
//    add(usGal);
//    auto km3 = std::make_shared<Unit>("km³", 1000000000.0);
//    add(km3);
//    auto in3 = std::make_shared<Unit>("in³", mL->fromUnit(16.387064));
//    add(in3);
//    auto ft3 = std::make_shared<Unit>("ft³", 0.028316846592);
//    add(ft3);
//    auto ya3 = std::make_shared<Unit>("ya³", 0.764554857984);
//    add(ya3);
//    auto mi3 = std::make_shared<Unit>("mi³", 1609.344 * 1609.344 * 1609.344);
//    add(mi3);
//
// Temperature
//    auto K = std::make_shared<Unit>("K", 1.0, 0.0); // Kelvin
//    add(K);
//    auto C = std::make_shared<Unit>("°C", 1.0, 273.15); // Celsius
//    add(C);
//    auto F = std::make_shared<Unit>("°F", 5.0 / 9.0, 459.67); // Fahrenheit
//    add(F);
//    auto R = std::make_shared<Unit>("°R", 5.0 / 9.0); // Rankine
//    add(R);
//
//Speed
//    auto ms = std::make_shared<Unit>("m/s", 1.0);
//    add(ms);
//    auto kmh = std::make_shared<Unit>("km/h", 1000.0/3600.0);
//    add(kmh);
//    auto mih = std::make_shared<Unit>("mi/h", 1609.344/3600.0);
//    add(mih);
//    auto knot = std::make_shared<Unit>("knot", 1852.0/3600.0);
//    add(knot);
//    auto ls = std::make_shared<Unit>("lightspeed", 299792458.0);
//    add(ls);
//
//Mass
//    auto kg = std::make_shared<Unit>("kg", 1.0);
//    add(kg);
//    auto t = std::make_shared<Unit>("c", 1000.0);
//    add(t);
//    auto p = std::make_shared<Unit>("pound", 0.45359237);
//    add(p);
//
//Time
//   auto ns = std::make_shared<Unit>("ns", 1.0E-9);
//   add(ns);
//   auto us = std::make_shared<Unit>("µs", 1.0E-6);
//   add(us);
//   auto ms = std::make_shared<Unit>("ms", 1.0E-3);
//   add(ms);
//   auto s = std::make_shared<Unit>("second", 1.0);
//   add(s);
//   auto mi = std::make_shared<Unit>("minute", 60.0);
//   add(mi);
//   auto h = std::make_shared<Unit>("hour", 60.0*60.0);
//   add(h);
//   auto day = std::make_shared<Unit>("day", 24.0*60.0*60.0);
//   add(day);


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


