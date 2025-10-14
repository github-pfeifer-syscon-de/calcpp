/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
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

#pragma once

#include <glibmm.h>
#include <map>
#include <vector>
#include <memory>
#include <JsonObj.hpp>
#include <exception>

#include "CalcppApp.hpp"


class Unit;
class Dimension;

class UnitException
: public std::exception
{
public:
    UnitException(std::string error)
    : std::exception()
    , swhat(error)
    {
    }

    virtual const char * what() const noexcept
    {
        return swhat.c_str();
    }
private:
    std::string swhat;
};

class Dimensions
{
public:
    Dimensions(const std::string& exec_path);
    explicit Dimensions(const Dimensions& orig) = delete;
    virtual ~Dimensions() = default;
    //static std::shared_ptr<Dimensions> get();
    std::shared_ptr<Dimension> getLength();
    std::shared_ptr<Dimension> getArea();
    std::shared_ptr<Dimension> getVolume();
    std::shared_ptr<Dimension> getTemperature();
    std::shared_ptr<Dimension> getSpeed();
    std::shared_ptr<Dimension> getMass();
    std::shared_ptr<Dimension> getTime();


    std::list<std::shared_ptr<Dimension>> getDimensions();
protected:
    void loadJson(const std::string& exec_path);
    void loadJsonUnits(const psc::json::PtrJsonObj& unitObj);
    std::string localizeJson(const char* data);

private:
    //static std::shared_ptr<Dimensions> m_dimension;
    std::shared_ptr<Dimension> m_length;
    std::shared_ptr<Dimension> m_area;
    std::shared_ptr<Dimension> m_volume;
    std::shared_ptr<Dimension> m_temperature;
    std::shared_ptr<Dimension> m_speed;
    std::shared_ptr<Dimension> m_mass;
    std::shared_ptr<Dimension> m_time;
    std::list<std::shared_ptr<Dimension>> m_dimensions;

};


class Dimension
{
public:
    Dimension();
    explicit Dimension(const Dimension& orig) = delete;
    virtual ~Dimension() = default;
    void add(std::shared_ptr<Unit> unit);
    std::vector<std::shared_ptr<Unit>> getUnits();

    Glib::ustring getName();
    void setName(const Glib::ustring& name);
    Glib::ustring getDim();
    void setDim(const Glib::ustring& dim);
    std::shared_ptr<Unit> findById(const Glib::ustring& name);
    void loadJsonUnits(const psc::json::PtrJsonArr& unitArr);
    void loadJsonUnit(const psc::json::PtrJsonObj& unitVal);
protected:

    std::vector<std::shared_ptr<Unit>> m_units;
    std::map<Glib::ustring, std::shared_ptr<Unit>> m_ids;
private:
    Glib::ustring m_name;
    Glib::ustring m_dim;
};



class Unit
{
public:
    Unit(const Glib::ustring& name, double factor, double offset = 0.0);
    explicit Unit(const Unit& orig) = delete;
    virtual ~Unit() = default;
    Glib::ustring getName() const;
    Glib::ustring getId() const;
    void setId(const Glib::ustring& id);
    double getFactor() const;
    double getOffset() const;
    double toUnit(double val) const;
    double fromUnit(double val) const;
protected:
    Glib::ustring m_name;
    Glib::ustring m_id;
    double m_factor;
    double m_offset;
};

