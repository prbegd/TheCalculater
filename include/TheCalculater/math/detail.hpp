/**
 * @file detail.hpp
 * @author prbegd
 * @brief Some utility functions.
 * @date 2025-07-01
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>
#include "TheCalculater/core.hpp"

namespace TheCalculater::math {
    using _fraction = boost::rational<boost::multiprecision::cpp_int>;

    namespace fraction_convertor {
        template <typename T>
        _fraction convert(T value)
        {
            if constexpr (std::is_integral_v<T> || std::is_same_v<T, boost::multiprecision::cpp_int>) {
                return _fraction(value);
            } else if constexpr (std::is_same_v<T, _fraction>) {
                return value;
            } else if constexpr (std::is_floating_point_v<T>) {
                return parseFloat(value);
            } else {
                static_assert(std::is_void_v<T>, "Unsupported type");
                return 0;
            }
        }
        /// parse decimal to rational. Only supports formats like -1.2345 or .12345
        THECALC_API _fraction parseDecimal(std::string str);
        /// convert float to string and then callparseDecimal().
        template<typename T>
        _fraction parseFloat(T value)
            requires(std::is_floating_point_v<T>)
        {
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
        THECALC_API _fraction parseRational(std::string str);
    }; // namespace fraction_convertor

    /// Default value is 3.14159265358979323846
    THECALC_API _fraction pi();
    /// Default value is 2.71828182845904523536
    THECALC_API _fraction e();

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
    THECALC_API _fraction root(const _fraction& fraction, const boost::multiprecision::cpp_int& n);
    
    /// Compute the square root of a fraction.
    /// @see root(const _fraction&, const boost::multiprecision::cpp_int&)
    inline _fraction sqrt(const _fraction& fraction) { return root(fraction, 2); }
    /// Compute the cube root of a fraction.
    /// @see root(const _fraction&, const boost::multiprecision::cpp_int&)
    inline _fraction cbrt(const _fraction& fraction) { return root(fraction, 3); }

    /// Uses Taylor Series to compute.
    THECALC_API _fraction sin(const _fraction& fraction);
    /// Uses Taylor Series to compute.
    THECALC_API _fraction cos(const _fraction& fraction);
    /// Uses Taylor Series to compute.
    inline _fraction tan(const _fraction& fraction) { return sin(fraction) / cos(fraction); }

    /// Uses Taylor Series to compute.
    /// @throw std::domain_error if |x| > 1
    THECALC_API _fraction arcsin(const _fraction& fraction);
    /// Uses Taylor Series to compute.
    inline _fraction arccos(const _fraction& fraction) { return pi() / 2 - arcsin(fraction); }
    /// Uses Taylor Series to compute.
    /// Values of |x| > 1 are supported.
    THECALC_API _fraction arctan(const _fraction& fraction);

    THECALC_API boost::multiprecision::cpp_int floor(const _fraction& fraction);
    THECALC_API boost::multiprecision::cpp_int ceil(const _fraction& fraction);

    THECALC_API _fraction ln(const _fraction& fraction);
    
    
    THECALC_API _fraction _ln_series_(const _fraction& fraction, const _fraction& tolerance);
} // namespace TheCalculater::math