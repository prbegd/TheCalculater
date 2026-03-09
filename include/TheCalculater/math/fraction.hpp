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
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>

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
     * @throw std::invalid_argument If `str` is not a valid fraction or decimal number.
     * @throw boost::bad_rational If the denominator is zero.
     */
    THECALC_API Fraction makeFraction(std::string_view str);
    /**
     * @brief Convert a double to Fraction.
     *
     * @param value The double to convert.
     * @return Fraction The fraction created from the double.
     * @throw std::invalid_argument If `value` is ±Infinity or NaN.
     */
    THECALC_API Fraction makeFraction(double value);

    /**
     * @brief Configuration structure used in Fraction formatting.
     */
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
        Style style = Style::FractionWhenRepeatedDecimal;
    };

    /**
     * @brief Format a fraction in given format.
     *
     * @param frac The fraction to format.
     * @return std::string The formatted fraction in given format.
     * @throw std::invalid_argument When value of `options.style` isn't a enumerator in enum FractionFormatOptions::Style.
     */
    THECALC_API std::string format(const Fraction& frac, const FractionFormatOptions& options = {});

    /**
     * @brief Configuration structure used in Fraction calculation.
     */
    struct FractionCalculationConfig {
        /// Pi value used in calculation.
        Fraction pi;
        /// Euler value used in calculation.
        Fraction e;
        struct ApproximateConfig {
            // When this is set to `false`, TheCalculater::math::IrrationalResultException may be thrown since cannot get ration result.
            bool useWhenNeeded = false;
            // e.g. we want 4 significant digits: 0.0001
            Fraction tolerance;
            // If approximation uses iterations, the iteration will stop after iteration count reach this value to prevent infinite loop.
            uint32_t maxIterations = 0;
        } approximation;

        /**
         * @brief Get the global default config instance
         *
         * This function should be thread safe.
         * This function is currently implemented in `TheCalculater.settings`.
         * 
         * @throw std::runtime_error If failed to create global default object.
         */
        THECALC_API static const FractionCalculationConfig& globalDefault();
    };
    THECALCULATER_DEFINE_EXCEPTION(FractionCalculationException, std::logic_error);
    THECALCULATER_DEFINE_EXCEPTION(IrrationalResultException, FractionCalculationException);

    /**
     * @brief Compute the reciprocal of a fraction.
     *
     * Same as 1 / x.
     * 
     * @param x The fraction to compute the reciprocal of.
     * @return Fraction The reciprocal of `x`.
     * @throw FractionCalculationException If `x` is 0.
     */
    THECALC_API Fraction reciprocal(const Fraction& x);

    /**
     * @brief Compute the power of a fraction.
     *
     * @param x The base.
     * @param n The exponent.
     * @param config The calculation configuration.
     * @return Fraction The `n`th power of `x`.
     */
    THECALC_API Fraction pow(Fraction x, const Fraction& n,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());

    /**
     * @brief Compute the factorial of a integer.
     *
     * @param x The integer to compute the factorial of. Must NOT less than 0.
     * @return Fraction The factorial of `x`.
     * @throw FractionCalculationException If `x` is not a integer or less than 0.
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
    /**
     * @brief Compute the modulo of two fractions.
     *
     * @param x The dividend.
     * @param y The divisor.
     * @return Fraction The modulo of the two fractions.
     */
    THECALC_API Fraction mod(const Fraction& x, const Fraction& y);

    /**
     * @brief Compute the nth root of a fraction.
     *
     * @param x The fraction to compute the nth root of.
     * @param n The nth root to compute. Must NOT equal to 0.
     * @param config The calculation configuration.
     * @return Fraction The `n`th root of the `x`.
     * @throw FractionCalculationException If `n` is 0.
     * @throw FractionCalculationException If trying to compute root of a negative number for even roots.
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction root(const Fraction& x, const Fraction& n,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());

    /**
     * @brief Compute the square root of a fraction.
     * 
     * Same as root(x, 2, config);
     * 
     * @see root(const Fraction&, const boost::multiprecision::cpp_int&)
     */
    THECALC_API Fraction sqrt(const Fraction& x,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());

    /**
     * @brief Compute the cube root of a fraction.
     * 
     * Same as root(x, 3, config);
     * 
     * @see root(const Fraction&, const boost::multiprecision::cpp_int&)
     */
    THECALC_API Fraction cbrt(const Fraction& x,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault()); 

    /**
     * @brief Compute the sine of a fraction.
     *
     * @param rad The RADIAN to compute the sine of.
     * @param config The calculation configuration.
     * @return Fraction The sine of `x`.
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction sin(const Fraction& rad,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());
    /**
     * @brief Compute the cosine of a fraction.
     *
     * @param rad The RADIAN to compute the cosine of.
     * @param config The calculation configuration.
     * @return Fraction The cosine of `x`.
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction cos(const Fraction& rad,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());
    /**
     * @brief Compute the tangent of a fraction.
     *
     * @param rad The RADIAN to compute the tangent of.
     * @param config The calculation configuration.
     * @return Fraction The tangent of `x`.
     * @throw FractionCalculationException If the fraction is pi/2 + k*pi, where k is an integer.
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction tan(const Fraction& rad,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());
    /**
     * @brief Compute the cotangent of a fraction.
     *
     * @param rad The RADIAN to compute the cotangent of.
     * @param config The calculation configuration.
     * @return Fraction The cotangent of `x`.
     * @throw FractionCalculationException If the fraction is k*pi, where k is an integer.
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction cot(const Fraction& rad,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());
    /**
     * @brief Compute the secant of a fraction.
     *
     * @param rad The RADIAN to compute the secant of.
     * @param config The calculation configuration.
     * @return Fraction The secant of `x`.
     * @throw FractionCalculationException If the fraction is pi/2 + k*pi, where k is an integer.
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction sec(const Fraction& rad,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());
    /**
     * @brief Compute the cosecant of a fraction.
     *
     * @param rad The RADIAN to compute the cosecant of.
     * @param config The calculation configuration.
     * @return Fraction The cosecant of `x`.
     * @throw FractionCalculationException If the fraction is k*pi, where k is an integer.
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction csc(const Fraction& rad,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());

    /**
     * @brief Compute the arcsine of a fraction.
     *
     * @param rad The RADIAN to compute the arcsine of.
     * @param config The calculation configuration.
     * @return Fraction The arcsine of `x`.
     * @throw FractionCalculationException If the fraction is not in the range [-1, 1].
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction arcsin(const Fraction& rad,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());
    /**
     * @brief Compute the arccosine of a fraction.
     *
     * @param rad The RADIAN to compute the arccosine of.
     * @param config The calculation configuration.
     * @return Fraction The arccosine of `x`.
     * @throw FractionCalculationException If the fraction is not in the range [-1, 1].
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction arccos(const Fraction& rad,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());
    /**
     * @brief Compute the arctangent of a fraction.
     *
     * @param rad The RADIAN to compute the arctangent of.
     * @param config The calculation configuration.
     * @return Fraction The arctangent of `x`.
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction arctan(const Fraction& rad,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());
    /**
     * @brief Compute the arccotangent of a fraction.
     *
     * @param rad The RADIAN to compute the arccotangent of.
     * @param config The calculation configuration.
     * @return Fraction The arccotangent of `x`.
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction arccot(const Fraction& rad,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());
    /**
     * @brief Compute the arcsecant of a fraction.
     *
     * @param rad The RADIAN to compute the arcsecant of.
     * @param config The calculation configuration.
     * @return Fraction The arcsecant of `x`.
     * @throw FractionCalculationException If the fraction is in the range (-1, 1).
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction arcsec(const Fraction& rad,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());
    /**
     * @brief Compute the arccosecant of a fraction.
     *
     * @param rad The RADIAN to compute the arccosecant of.
     * @param config The calculation configuration.
     * @return Fraction The arccosecant of `x`.
     * @throw FractionCalculationException If the fraction is in the range (-1, 1).
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction arccsc(const Fraction& rad,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());

    /**
     * @brief Compute the floor of the Fraction.
     * 
     * @param x The Fraction to compute floor for.
     * @return boost::multiprecision::cpp_int The floor of `x`
     */
    THECALC_API boost::multiprecision::cpp_int floor(const Fraction& x);
    /**
     * @brief Compute the ceiling of the Fraction.
     * 
     * @param x The Fraction to compute ceiling for,
     * @return boost::multiprecision::cpp_int The ceiling of `x`.
     */
    THECALC_API boost::multiprecision::cpp_int ceil(const Fraction& x);

    /**
     * @brief Compute the natural logarithm of a fraction.
     *
     * @param x The fraction to compute the natural logarithm of. Must be greater than 0.
     * @param config The calculation configuration.
     * @return Fraction The natural logarithm of `x`.
     * @throw FractionCalculationException If the Fraction is less than or equal to 0.
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction ln(const Fraction& x,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());
    /**
     * @brief Compute the logarithm of a fraction with a specified base.
     *
     * @param x The fraction to compute the logarithm of. Must be greater than 0.
     * @param base The base of the logarithm. Must NOT be 0 or 1. Must NOT be negative.
     * @param config The calculation configuration.
     * @return Fraction The logarithm of `x` with the specified base `base`.
     * @throw FractionCalculationException If `x` is less than or equal to 0.
     * @throw FractionCalculationException If `base` is 0 or 1 or negative.
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction log(const Fraction& x, const Fraction& base,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());
    /**
     * @brief Compute the logarithm of a fraction with base 10.
     *
     * Same as log(x, 10, config)
     * 
     * @param x The fraction to compute the logarithm of.
     * @param config The calculation configuration.
     * @return Fraction The logarithm of `x` with base 10.
     * @throw FractionCalculationException If `x` is less than or equal to 0.
     * @throw IrrationalResultException See FractionCalculationConfig::ApproximateConfig::useWhenNeeded
     */
    THECALC_API Fraction lg(const Fraction& x,
        const FractionCalculationConfig& config = FractionCalculationConfig::globalDefault());
} // namespace TheCalculater::math

inline std::ostream& operator<<(std::ostream& os, const TheCalculater::math::Fraction& fraction)
{
    os << TheCalculater::math::format(fraction);
    return os;
}
template <>
struct std::formatter<TheCalculater::math::Fraction> {
    template <typename TCharT>
    constexpr auto parse(basic_format_parse_context<TCharT>& ctx) {
        return ctx.begin();
    }
    template <typename TOut, typename TCharT>
    auto format(const TheCalculater::math::Fraction& fraction, basic_format_context<TOut, TCharT>& ctx) const {
        std::ostringstream oss;
        oss << fraction;
        return std::format_to(ctx.out(), "{}", oss.view());
    }
};
template <>
struct std::formatter<boost::multiprecision::cpp_int> {
    template <typename TCharT>
    constexpr auto parse(basic_format_parse_context<TCharT>& ctx) {
        return ctx.begin();
    }
    template <typename TOut, typename TCharT>
    auto format(const boost::multiprecision::cpp_int& value, basic_format_context<TOut, TCharT>& ctx) const {
        return std::format_to(ctx.out(), "{}", value.str());
    }
};