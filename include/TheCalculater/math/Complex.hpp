/**
 * @file Complex.hpp
 * @author prbegd
 * @brief Provides a Complex class for TheCalculater.
 * @date 2025-06-21
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#pragma once
#include "TheCalculater/util.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>
#include <string_view>

// （其实这个api还没写啦）
#ifdef THECALCULATER_SETTINGS
#include "TheCalculater/settings.hpp"
#endif

namespace TheCalculater::math {
    using _fraction = boost::rational<boost::multiprecision::cpp_int>;

    class Complex;

    class _fractionConvertor {
    public:
        template <typename T>
        static _fraction convert(T value)
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

        /**
         * @brief parse a string to Complex object.(a+bi)
         *
         * @param str string to parse
         * @param complex complex object to store the result
         * @throw std::invalid_argument if the string is not a valid complex number
         */
        static void parseString(std::string str, Complex& complex);

    private:
        static constexpr int getFloatPrecision()
        {
#ifdef THECALCULATER_SETTINGS
            // for future myself: true means force cache
            return settings::readInt("calc.float_precision", true);
#else
            return 15; // NOLINT
#endif
        }
        static _fraction parseFloat(double value);
        static _fraction parseDecimal(std::string_view str);
    };

    class Complex {
    private:
        _fraction real_;
        _fraction imaginary_;

    public:
        Complex()
            : real_(0), imaginary_(0)
        {
        }

        Complex(const Complex&) = default;
        Complex(Complex&&) = default;

        friend void _fractionConvertor::parseString(std::string str, Complex& complex);

        // [[nodiscard]] const _fraction &real() const { return real_; }
    };
} // namespace TheCalculater::math