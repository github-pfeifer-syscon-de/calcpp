/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2026 RPf
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
#include <future>
#include "NumDialog.hpp"

class PrimeDialog
: public NumDialog
{
public:
    PrimeDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent);
    explicit PrimeDialog(const PrimeDialog& other) = delete;
    virtual ~PrimeDialog() = default;
    // if you increase this consider the implementation limits:
    //  - size_t is used for indexing so this should nto be exceeded
    //  - the sieve is keept in memory so PRIME_LIMIT/(8*2) should be allocateable
    static constexpr size_t PRIME_LIMIT{1024ul*1024ul*1024ul}; // limit memory usage to ~128MB, and indexes that work with size_t on 32bit plattforms
    static constexpr int64_t MAX_SHOW_PRIMES{10000};
protected:
    void evaluate() override;
    void displayPrime();
    void displayFactors();
    std::vector<size_t> computePrimes(size_t max);
    std::vector<size_t> computeFactors(size_t n);

private:
    Gtk::Entry* m_entryMax;
    Gtk::Entry* m_entryFactor;
    Gtk::TextView* m_text;
    std::future<std::vector<size_t>> m_handlePrimes;
    std::future<std::vector<size_t>> m_handleFactors;
    Glib::Dispatcher m_dispPrimes;
    Glib::Dispatcher m_dispFactors;
    size_t m_maxPrime{};
    size_t m_factorize{};
};
