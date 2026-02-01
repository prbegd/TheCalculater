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
#include "TheCalculater/math/formatter.hpp"
#include <boost/multiprecision/fwd.hpp>

namespace boost {
    template <typename T>
    class rational;
}

namespace TheCalculater::math {
    using Fraction = boost::rational<boost::multiprecision::cpp_int>;

    /**
     * @brief Convert a string to Fraction.
     *
     * This function accepts both plain text and LaTeX format of decimals and fractions,
     * decimals can include repeating part marked start and end in "{}"(plain text) or "\overline{}"(LaTeX).
     *
     * Examples:
     *     - "1/2"
     *     - "\frac{3}{4}"
     *     - "0.{142857}"
     *     - "0.1\overline{6}"
     *
     * @param str The string to convert.
     * @return Fraction The fraction created from the string.
     * @throw std::invalid_argument If the string is not a valid fraction or decimal number.
     * @throw boost::bad_rational If the denominator is zero.
     */
    THECALC_API Fraction makeFraction(std::string_view str);
    /**
     * @brief Convert a double to Fraction.
     *
     * @return Fraction The fraction created from the double.
     */
    THECALC_API Fraction makeFraction(double value);

    struct FractionFormatOptions {
        enum class Style : int8_t {
            /// Always format as fraction.
            AlwaysFraction,
            /// If the fraction have decimal part, format as fraction. Otherwise, format as integer.
            FractionWhenDecimal,
            /// If the decimal part of the fraction is repeated, format as fraction. Otherwise, format as decimal.
            FractionWhenRepeatedDecimal,
            /// Always format as decimal. So when the fraction is a repeating decimal, it will mark the repeated part.
            AlwaysDecimal
        };
        FormatType type = FormatType::LaTeX;
        /// The style of fraction formatting.
        // TODO: make this read from settings
        Style style = Style::FractionWhenRepeatedDecimal;
    };

    /**
     * @brief Format a fraction in given format.
     *
     * @param frac The fraction to format.
     * @return std::string The formatted fraction in given format.
     */
    THECALC_API std::string format(const Fraction& frac, const FractionFormatOptions& options = {});

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
     * @throw std::domain_error If the fraction is k*pi, where k is an integer.
     */
    THECALC_API Fraction cot(const Fraction& rad);
    /**
     * @brief Compute the secant of a fraction.
     *
     * @param rad The RADIAN to compute the secant of.
     * @return Fraction The secant of the fraction.
     * @throw std::domain_error If the fraction is pi/2 + k*pi, where k is an integer.
     */
    THECALC_API Fraction sec(const Fraction& rad);
    /**
     * @brief Compute the cosecant of a fraction.
     *
     * @param rad The RADIAN to compute the cosecant of.
     * @return Fraction The cosecant of the fraction.
     * @throw std::domain_error If the fraction is k*pi, where k is an integer.
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
    /**
     * @brief Compute the arccotangent of a fraction.
     *
     * @param rad The RADIAN to compute the arccotangent of.
     * @return Fraction The arccotangent of the fraction.
     */
    THECALC_API Fraction arccot(const Fraction& rad);
    /**
     * @brief Compute the arcsecant of a fraction.
     *
     * @param rad The RADIAN to compute the arcsecant of.
     * @return Fraction The arcsecant of the fraction.
     * @throw std::domain_error If the fraction is in the range (-1, 1).
     */
    THECALC_API Fraction arcsec(const Fraction& rad);
    /**
     * @brief Compute the arccosecant of a fraction.
     *
     * @param rad The RADIAN to compute the arccosecant of.
     * @return Fraction The arccosecant of the fraction.
     * @throw std::domain_error If the fraction is in the range (-1, 1).
     */
    THECALC_API Fraction arccsc(const Fraction& rad);

    THECALC_API boost::multiprecision::cpp_int floor(const Fraction& x);
    THECALC_API boost::multiprecision::cpp_int ceil(const Fraction& x);

    /**
     * @brief Compute the natural logarithm of a fraction.
     *
     * @param x The fraction to compute the natural logarithm of.
     * @return Fraction The natural logarithm of the fraction.
     */
    THECALC_API Fraction ln(const Fraction& x);
    /**
     * @brief Compute the logarithm of a fraction with a specified base.
     *
     * @param x The fraction to compute the logarithm of.
     * @param base The base of the logarithm.
     * @return Fraction The logarithm of the fraction with the specified base.
     */
    THECALC_API Fraction log(const Fraction& x, const Fraction& base);
    /**
     * @brief Compute the logarithm of a fraction with base 10.
     *
     * @param x The fraction to compute the logarithm of.
     * @return Fraction The logarithm of the fraction with base 10.
     */
    THECALC_API Fraction lg(const Fraction& x);
} // namespace TheCalculater::math