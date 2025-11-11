/**
 * @file fraction.hpp
 * @author prbegd
 * @brief Fraction class and related functions.
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

    /**
     * @brief Base template factory function for converting T to Fraction.
     *
     * If the conversion from T to Fraction is not specialized, this function will be deleted.
     *
     * @tparam T The type of the number to create a fraction from.
     * @return Fraction The fraction created from the number.
     */
    template <typename T>
    Fraction makeFraction(const T&) = delete;
    /**
     * @brief Convert a string to Fraction.
     *
     * This function accepts strings in the format of "1/2" or "-3.14".
     *
     * @return Fraction The fraction created from the string.
     * @throw std::invalid_argument If the string is not a valid fraction or decimal number.
     * @throw boost::bad_rational If the denominator is zero.
     */
    template <>
    Fraction makeFraction<std::string>(const std::string&);
    /**
     * @brief Convert a floating-point number to Fraction.
     *
     * @return Fraction The fraction created from the floating-point number.
     */
    template <typename T>
    Fraction makeFraction(const T& value)
        requires(std::is_floating_point_v<T>)
    {
        if (std::isinf(value))
            throwEx(std::invalid_argument("Cannot convert ±Infinity to rational"));
        if (std::isnan(value))
            throwEx(std::invalid_argument("Cannot convert NaN to rational"));
        std::ostringstream oss;
        oss << std::setprecision(17) << value;
        return makeFraction(oss.str());
    }

    THECALC_API Fraction pi();
    THECALC_API Fraction e();
    
    /**
     * @brief Compute the reciprocal of a fraction.
     * 
     * @param x The fraction to compute the reciprocal of.
     * @return Fraction The reciprocal of the fraction.
     8 @throw std::domain_error If the fraction is zero.
     */
    Fraction reciprocal(const Fraction& x);

    // Compute the power of a fraction.
    Fraction pow(Fraction x, const Fraction& n);

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