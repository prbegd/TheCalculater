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
#include <boost/multiprecision/fwd.hpp>
namespace boost {template <typename T> class rational; }

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
    THECALC_API Fraction makeFraction<std::string>(const std::string&);
    /**
     * @brief Convert a double to Fraction.
     *
     * @return Fraction The fraction created from the double.
     */
    THECALC_API Fraction makeFraction(double value);
    
    /**
     * @brief Compute the reciprocal of a fraction.
     * 
     * @param x The fraction to compute the reciprocal of.
     * @return Fraction The reciprocal of the fraction.
     8 @throw std::domain_error If the fraction is zero.
     */
    THECALC_API Fraction reciprocal(const Fraction& x);

    /**
     * @brief Compute the power of a fraction.
     * 
     * @param x The base.
     * @param n The exponent.
     * @return Fraction The power of the fraction.
     */
    THECALC_API Fraction pow(Fraction x, const Fraction& n);

    /**
     * @brief Compute the factorial of a fraction.
     *
     * @warning x MUST be a integer greater than or equal to 0.
     * 
     * @param x The fraction to compute the factorial of.
     * @return Fraction The factorial of the fraction.
     */
    THECALC_API Fraction factorial(const Fraction& x);

    /**
     * @brief Compute the modulo of two fractions.
     * 
     * @param x The dividend.
     * @param y The divisor.
     * @return Fraction The modulo of the two fractions.
     */
    THECALC_API Fraction operator%(const Fraction& x, const Fraction& y);
    THECALC_API Fraction mod(const Fraction& x, const Fraction& y);

    /**
     * @brief Compute the nth root of a fraction.
     *
     * @param fraction The fraction to compute the nth root of
     * @param n The nth root to compute
     * @return _fraction The nth root of the fraction
     * @throw std::invalid_argument If n is 0 or negative.
     * @throw std::domain_error If the fraction is negative and n is even.
     */
    THECALC_API Fraction root(const Fraction& x, const boost::multiprecision::cpp_int& n);

    /// Compute the square root of a fraction.
    /// @see root(const _fraction&, const boost::multiprecision::cpp_int&)
    THECALC_API Fraction sqrt(const Fraction& x) /* { return root(x, 2); } */;
    /// Compute the cube root of a fraction.
    /// @see root(const _fraction&, const boost::multiprecision::cpp_int&)
    THECALC_API Fraction cbrt(const Fraction& x) /* { return root(x, 3); } */;

    /**
     * @brief Compute the sine of a fraction.
     * 
     * @param rad The RADIAN to compute the sine of.
     * @return Fraction The sine of the fraction.
     */
    THECALC_API Fraction sin(const Fraction& rad);
    /**
     * @brief Compute the cosine of a fraction.
     * 
     * @param rad The RADIAN to compute the cosine of.
     * @return Fraction The cosine of the fraction.
     */
    THECALC_API Fraction cos(const Fraction& rad);
    /**
     * @brief Compute the tangent of a fraction.
     * 
     * @param rad The RADIAN to compute the tangent of.
     * @return Fraction The tangent of the fraction.
     * @throw std::domain_error If the fraction is pi/2 + k*pi, where k is an integer.
     */
    THECALC_API Fraction tan(const Fraction& rad);
    /**
     * @brief Compute the cotangent of a fraction.
     * 
     * @param rad The RADIAN to compute the cotangent of.
     * @return Fraction The cotangent of the fraction.
     
     */
    THECALC_API Fraction cot(const Fraction& rad);
    /**
     * @brief Compute the secant of a fraction.
     * 
     * @param rad The RADIAN to compute the secant of.
     * @return Fraction The secant of the fraction.
     */
    THECALC_API Fraction sec(const Fraction& rad);
    /**
     * @brief Compute the cosecant of a fraction.
     * 
     * @param rad The RADIAN to compute the cosecant of.
     * @return Fraction The cosecant of the fraction.
     */
    THECALC_API Fraction csc(const Fraction& rad);

    /**
     * @brief Compute the arcsine of a fraction.
     * 
     * @param rad The RADIAN to compute the arcsine of.
     * @return Fraction The arcsine of the fraction.
     * @throw std::domain_error If the fraction is not in the range [-1, 1].
     */
    THECALC_API Fraction arcsin(const Fraction& rad);
    /**
     * @brief Compute the arccosine of a fraction.
     * 
     * @param rad The RADIAN to compute the arccosine of.
     * @return Fraction The arccosine of the fraction.
     * @throw std::domain_error If the fraction is not in the range [-1, 1].
     */
    THECALC_API Fraction arccos(const Fraction& rad);
    /**
     * @brief Compute the arctangent of a fraction.
     * 
     * @param rad The RADIAN to compute the arctangent of.
     * @return Fraction The arctangent of the fraction.
     */
    THECALC_API Fraction arctan(const Fraction& rad);

    THECALC_API boost::multiprecision::cpp_int floor(const Fraction& x);
    THECALC_API boost::multiprecision::cpp_int ceil(const Fraction& x);

    THECALC_API Fraction ln(const Fraction& x);

    THECALC_API Fraction _ln_series_(const Fraction& x, const Fraction& tolerance);
} // namespace TheCalculater::math