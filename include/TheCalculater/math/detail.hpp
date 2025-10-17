/**
 * @file detail.hpp
 * @author prbegd
 * @brief Some utility functions.
 * @date 2025-07-01
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#pragma once
#include "TheCalculater/core.hpp"
#include "TheCalculater/util.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>
#include <cmath>

namespace TheCalculater::math {
    using Fraction = boost::rational<boost::multiprecision::cpp_int>;

    namespace fraction_convertor {
        /// parse decimal to rational. Only supports formats like -1.2345 or .12345
        THECALC_API Fraction parseDecimal(std::string str);
        /// convert float to string and then callparseDecimal().
        template <typename T>
        Fraction parseFloat(T value)
            requires(std::is_floating_point_v<T>)
        {
            if (std::isinf(value))
                throwEx(std::invalid_argument("Cannot convert ±∞ to rational"));
            if (std::isnan(value))
                throwEx(std::invalid_argument("Cannot convert NaN to rational"));
            std::ostringstream oss;
            oss << std::setprecision(17) << value;
            return parseDecimal(oss.str());
        }

        /**
         * @brief Parse a string to rational.
         *
         * @param str The string to parse
         * @throw std::invalid_argument If the string is not a valid rational number
         *
         * Like parseDecimal(), but supports fractions like -1/2.
         */
        THECALC_API Fraction parseRational(std::string str);
    }; // namespace fraction_convertor

    /// Get the value from settings.
    THECALC_API Fraction pi();
    /// Get the value from settings.
    THECALC_API Fraction e();

    /**
     * @brief Compute the nth root of a fraction.
     *
     * @param fraction The fraction to compute the nth root of
     * @param n The nth root to compute
     * @return _fraction The nth root of the fraction
     * @throw std::invalid_argument If n is 0 or negative.
     * @throw std::domain_error If the fraction is negative and n is even.
     *
     * Uses Newton-Raphson Method to compute.
     */
    THECALC_API Fraction root(const Fraction& x, const boost::multiprecision::cpp_int& n);

    /// Compute the square root of a fraction.
    /// @see root(const _fraction&, const boost::multiprecision::cpp_int&)
    inline Fraction sqrt(const Fraction& x) { return root(x, 2); }
    /// Compute the cube root of a fraction.
    /// @see root(const _fraction&, const boost::multiprecision::cpp_int&)
    inline Fraction cbrt(const Fraction& x) { return root(x, 3); }

    /// Uses Taylor Series to compute.
    THECALC_API Fraction sin(const Fraction& x);
    /// Uses Taylor Series to compute.
    THECALC_API Fraction cos(const Fraction& x);
    /// Uses Taylor Series to compute.
    inline Fraction tan(const Fraction& x) { return sin(x) / cos(x); }

    /// Uses Taylor Series to compute.
    /// @throw std::domain_error if |x| > 1
    THECALC_API Fraction arcsin(const Fraction& x);
    /// Uses Taylor Series to compute.
    inline Fraction arccos(const Fraction& x) { return pi() / 2 - arcsin(x); }
    /// Uses Taylor Series to compute.
    /// Values of |x| > 1 are supported.
    THECALC_API Fraction arctan(const Fraction& x);

    THECALC_API boost::multiprecision::cpp_int floor(const Fraction& x);
    THECALC_API boost::multiprecision::cpp_int ceil(const Fraction& x);

    THECALC_API Fraction ln(const Fraction& x);

    THECALC_API Fraction _ln_series_(const Fraction& x, const Fraction& tolerance);
} // namespace TheCalculater::math