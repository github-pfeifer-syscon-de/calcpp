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
#include <psc_format.hpp>
#include <psc_i18n.hpp>


#include "CalcppWin.hpp"
#include "PrimeDialog.hpp"
#include "Primes.hpp"

PrimeDialog::PrimeDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, CalcppWin* parent)
: NumDialog(cobject, builder, parent)
{
    builder->get_widget("max", m_entryMax);
    builder->get_widget("factor", m_entryFactor);
    builder->get_widget("text", m_text);

    m_dispPrimes.connect(sigc::mem_fun(*this, &PrimeDialog::displayPrime));
    m_dispFactors.connect(sigc::mem_fun(*this, &PrimeDialog::displayFactors));
}

std::vector<size_t>
PrimeDialog::computePrimes(size_t max)
{
    auto primes = psc::math::Primes::compute(max);
    m_dispPrimes.emit();
    return primes;
}

std::vector<size_t>
PrimeDialog::computeFactors(size_t n)
{
    auto factorization = psc::math::Primes::factorize(n);
    m_dispFactors.emit();
    return factorization;
}

void
PrimeDialog::evaluate()
{
    if (!m_entryMax->get_text().empty()) {
        m_maxPrime = static_cast<size_t>(parse(m_entryMax));
        if (m_maxPrime > PRIME_LIMIT) {
            m_parent->show_error(
                psc::fmt::vformat(
                  _("Number {} exceeds limit {}, (you may recompile to increase this limit)")
                , psc::fmt::make_format_args( m_maxPrime, PRIME_LIMIT)));
        }
        else {
            m_entryMax->set_text("");
            m_handlePrimes = std::async(std::launch::async, &PrimeDialog::computePrimes, this, m_maxPrime);
        }
    }
    if (!m_entryFactor->get_text().empty()) {
        m_factorize = static_cast<size_t>(parse(m_entryFactor));
        if (m_factorize > PRIME_LIMIT) {
            m_parent->show_error(
                psc::fmt::vformat(
                  _("Number {} exceeds limit {}, (you may recompile to increase this limit)")
                , psc::fmt::make_format_args( m_factorize, PRIME_LIMIT)));
        }
        else {
            m_entryFactor->set_text("");
            m_handleFactors = std::async(std::launch::async, &PrimeDialog::computeFactors, this, m_factorize);
        }
    }
}

void
PrimeDialog::displayPrime()
{
    auto buf = m_text->get_buffer();
    buf->set_text("");
    try {
        auto primes = m_handlePrimes.get();
        auto cnt = primes.size();
        // limit the number of displayed as textarea will not handle very large text nicely
        auto showPrimes = std::min(cnt, static_cast<size_t>(MAX_SHOW_PRIMES));
        std::string text = psc::fmt::vformat(_("Found {} upto {}"),
            psc::fmt::make_format_args(cnt, m_maxPrime));
        if (showPrimes < primes.size()) {
            text += psc::fmt::vformat(_(" only the last {} will be shown"),
                psc::fmt::make_format_args(MAX_SHOW_PRIMES));
        }
        text += "\n";
        auto ins = buf->get_insert();
        buf->insert(buf->get_iter_at_mark(ins), text);
        auto start = static_cast<size_t>(std::max(static_cast<int64_t>(0), static_cast<int64_t>(primes.size()) - MAX_SHOW_PRIMES));
        size_t n{};
        text.clear();
        for (size_t i = start; i < primes.size(); ++i) {
            auto p = primes[i];
            text += psc::fmt::format("{}, ", p);
            if (++n % 8 == 0) {
                text += "\n";
                ins = buf->get_insert();
                buf->insert(buf->get_iter_at_mark(ins), text);
                text.clear();
            }
        }
    }
    catch (const std::exception& ex) {
        auto err = ex.what();
        m_parent->show_error(
            psc::fmt::vformat(
              _("Error {} computing primes upto {}")
            , psc::fmt::make_format_args( err, m_maxPrime)));
    }
}

void
PrimeDialog::displayFactors()
{
    Glib::ustring text;
    try {
        auto fact = m_handleFactors.get();
        text.reserve(8192);
        text = psc::fmt::format("{} = ", m_factorize);
        auto start = text.size();
        for (auto f : fact) {
            if (text.size() > start) {
                text += " * ";
            }
            text += psc::fmt::format("{}", f);
        }
    }
    catch (const std::exception& ex) {
        auto err = ex.what();
        m_parent->show_error(
            psc::fmt::vformat(
              _("Error {} computing factors {}")
            , psc::fmt::make_format_args( err, m_factorize)));
    }
    m_text->get_buffer()->set_text(text);
}