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
#include <iomanip>
#include <cstdlib>
#include <charconv>
#include <optional>
#include <system_error>
#include <psc_format.hpp>

#include "CalcppApp.hpp"
#include "calc_test.hpp"
#include "Syntax.hpp"
#include "Unit.hpp"

static constexpr auto VALUE_LIMIT{0.000001};
static constexpr auto VALUE_COARS{0.1};

// eval basic expression
static bool
testEval()
{
    auto testEval = std::make_shared<TestEval>();
    auto testFormat = std::make_shared<TestFormat>();
    Syntax syntax(testFormat, testEval);
    Glib::ustring expr{"3.1+4*5"};
    auto list = syntax.parse(expr);
    double res = testEval->eval(list);
    std::cout << "testEval " << res << std::endl;
    return std::abs(res - 23.1) < VALUE_LIMIT;
}

static bool
testEvalBraced()
{
    auto testEval = std::make_shared<TestEval>();
    auto testFormat = std::make_shared<TestFormat>();
    Syntax syntax(testFormat, testEval);
    Glib::ustring expr{"(3+4.1)*5"};
    auto list = syntax.parse(expr);
    double res = testEval->eval(list);
    std::cout << "testEvalBraced " << res << std::endl;
    return std::abs(res - 35.5) < VALUE_LIMIT;
}

static bool
testLen(Dimensions& dims)
{
    auto len = dims.getLength();
    auto km = len->findById("km");
    auto mi = len->findById("mi");
    auto valMi = mi->toUnit(km->fromUnit(1.609344));
    std::cout << "testLen " << valMi << "mi" << std::endl;
    if (std::abs(valMi - 1.0) > VALUE_LIMIT) {
        return false;
    }
    return true;
}

static bool
testArea(Dimensions& dims)
{
    auto area = dims.getArea();
    auto km = area->findById("km2");
    auto mi = area->findById("mi2");
    auto valMi = mi->toUnit(km->fromUnit(1.609344 * 1.609344));
    std::cout << "testArea " << valMi << "mi_sqr" << std::endl;
    if (std::abs(valMi - 1.0) > VALUE_LIMIT) {
        return false;
    }
    return true;
}

static bool
testVol(Dimensions& dims)
{
    auto vol = dims.getVolume();
    auto km = vol->findById("km3");
    auto mi = vol->findById("mi3");
    auto valMi = mi->toUnit(km->fromUnit(1.609344 * 1.609344 * 1.609344));
    std::cout << "testVol " << valMi << "mi_cubic" << std::endl;
    if (std::abs(valMi - 1.0) > VALUE_LIMIT) {
        return false;
    }
    auto ya = vol->findById("ya3");
    auto valYa = ya->toUnit(mi->fromUnit(1.0));
    std::cout << "testVol " << valMi << "ya_cubic" << std::endl;
    if (std::abs(valYa - 5451776000.0) > VALUE_LIMIT) { // see wiki https://en.wikipedia.org/wiki/Cubic_mile
        return false;
    }
    auto in = vol->findById("in3");
    auto ft = vol->findById("ft3");
    auto valFt = ft->toUnit(in->fromUnit(1728.0));
    std::cout << "testVol " << valFt << "ft_cubic" << std::endl;
    if (std::abs(valFt - 1.0) > VALUE_LIMIT) {
        return false;
    }
    auto gal = vol->findById("us.ga");
    auto flOunce = vol->findById("us.fl.ou");
    auto valGal = gal->toUnit(flOunce->fromUnit(128.0));
    std::cout << "testVol " << valGal << "US.gallon" << std::endl;
	if (std::abs(valGal - 1.0) > VALUE_LIMIT) {
        return false;
    }

    return true;
}

static bool
testTemp(Dimensions& dims)
{
    auto temp = dims.getTemperature();
    auto K = temp->findById("K");
    auto C = temp->findById("C");
    auto valK = K->toUnit(C->fromUnit(0.0));
    std::cout << "testTemp " << valK << "K" << std::endl;
    if (std::abs(valK - 273.15) > VALUE_LIMIT) {
        return false;
    }
    auto F = temp->findById("F");
    auto valC = C->toUnit(F->fromUnit(212.0));
    std::cout << "testTemp " << valC << "C" << std::endl;
    if (std::abs(valC - 100.0) > VALUE_LIMIT) {
        return false;
    }
    valC = C->toUnit(F->fromUnit(32.0));
    std::cout << "testTemp " << valC << "C" << std::endl;
    if (std::abs(valC - 0.0) > VALUE_LIMIT) {
        return false;
    }
    auto R = temp->findById("R");
    valC = C->toUnit(R->fromUnit(491.67));
    std::cout << "testTemp " << valC << "C" << std::endl;
    if (std::abs(valC - 0.0) > VALUE_LIMIT) {
        return false;
    }

    return true;
}

static bool
testSpeed(Dimensions& dims)
{
    auto speed = dims.getSpeed();
    auto ms = speed->findById("m/s");
    auto kmh = speed->findById("km/h");
    auto valMs = ms->toUnit(kmh->fromUnit(3.6));
    std::cout << "testSpeed " << valMs << "m/s" << std::endl;
    if (std::abs(valMs - 1.0) > VALUE_LIMIT) {
        return false;
    }
    auto mih = speed->findById("mi/h");
    auto valKmh = kmh->toUnit(mih->fromUnit(1));
    std::cout << "testSpeed " << valKmh << "km/h" << std::endl;
    if (std::abs(valKmh - 1.609344) > VALUE_LIMIT) {
        return false;
    }
    auto kn = speed->findById("knot");
    valKmh = kmh->toUnit(kn->fromUnit(1));
    std::cout << "testSpeed " << valKmh << "km/h" << std::endl;
    if (std::abs(valKmh - 1.852) > VALUE_LIMIT) {
        return false;
    }

    return true;
}

static bool
testMass(Dimensions& dims)
{
    auto mass = dims.getMass();
    auto kg = mass->findById("kg");
    auto pound = mass->findById("pound");
    auto valKg = kg->toUnit(pound->fromUnit(1.0));
    std::cout << "testMass " << valKg << "kg" << std::endl;
    if (std::abs(valKg - 0.45359237) > VALUE_LIMIT) {
        return false;
    }
    return true;
}


static bool
testTime(Dimensions& dims)
{
    auto time = dims.getTime();
    auto h = time->findById("h");
    auto min = time->findById("mi");
    auto valMin = min->toUnit(h->fromUnit(1.0));
    std::cout << "testTime " << valMin << "min" << std::endl;
    if (std::abs(valMin - 60.0) > VALUE_LIMIT) {
        return false;
    }
    return true;
}

/*
 *
 */
int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");      // make locale dependent, and make glib accept u8 const !!!
    Glib::init();

    Dimensions dims{argv[0]};

    if (!testEval()) {
        return 1;
    }
    if (!testEvalBraced()) {
        return 2;
    }
    if (!testLen(dims)) {
        return 3;
    }
    if (!testArea(dims)) {
        return 4;
    }
    if (!testVol(dims)) {
        return 5;
    }
    if (!testTemp(dims)) {
        return 6;
    }
    if (!testSpeed(dims)) {
        return 7;
    }
    if (!testMass(dims)) {
        return 8;
    }
    if (!testTime(dims)) {
        return 9;
    }

    return 0;
}

