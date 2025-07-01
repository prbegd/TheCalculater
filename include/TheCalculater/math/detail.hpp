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
#include "boost/multiprecision/fwd.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>

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
        /// convert float to string and then callparseDecimal().
        _fraction parseFloat(double value);
        /// parse decimal to rational. Only supports formats like -1.2345 or .12345
        _fraction parseDecimal(std::string str);

        /**
         * @brief Parse a string to rational.
         *
         * @param str The string to parse
         * @throw std::invalid_argument If the string is not a valid rational number
         *
         * Like parseDecimal(), but supports fractions like -1/2.
         */
        _fraction parseRational(std::string str);
    }; // namespace fraction_convertor

    /// Default value is 3.14159265358979323846
    _fraction pi();
    /// Default value is 2.71828182845904523536
    _fraction e();

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
    _fraction root(const _fraction& fraction, const boost::multiprecision::cpp_int& n);
    
    /// Compute the square root of a fraction.
    /// @see root(const _fraction&, const boost::multiprecision::cpp_int&)
    inline _fraction sqrt(const _fraction& fraction) { return root(fraction, 2); }
    /// Compute the cube root of a fraction.
    /// @see root(const _fraction&, const boost::multiprecision::cpp_int&)
    inline _fraction cbrt(const _fraction& fraction) { return root(fraction, 3); }

    /// Uses Taylor Series to compute.
    _fraction sin(const _fraction& fraction);
    /// Uses Taylor Series to compute.
    _fraction cos(const _fraction& fraction);
    /// Uses Taylor Series to compute.
    inline _fraction tan(const _fraction& fraction) { return sin(fraction) / cos(fraction); }

    /// Uses Taylor Series to compute.
    /// @throw std::domain_error if |x| > 1
    _fraction arcsin(const _fraction& fraction);
    /// Uses Taylor Series to compute.
    inline _fraction arccos(const _fraction& fraction) { return pi() / 2 - arcsin(fraction); }
    /// Uses Taylor Series to compute.
    /// Values of |x| > 1 are supported.
    _fraction arctan(const _fraction& fraction);

    boost::multiprecision::cpp_int floor(const _fraction& fraction);
    boost::multiprecision::cpp_int ceil(const _fraction& fraction);

} // namespace TheCalculater::math