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
#include <utility>

// （其实这个api还没写啦）
// #include "TheCalculater/settings.hpp"

namespace TheCalculater::math {
    using _fraction = boost::rational<boost::multiprecision::cpp_int>;

    class Complex;
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
        _fraction parseFloat(double value);
        _fraction parseDecimal(std::string str);

        /**
         * @brief parse a string to rational.
         *
         * @param str string to parse
         * @throw std::invalid_argument if the string is not a valid rational number
         */
        _fraction parseRational(std::string str);
    }; // namespace fraction_convertor

    /**
     * @brief compute the square root of a fraction.
     *
     * @param fraction the fraction to compute the square root of
     * @param n precision of the result
     * @return _fraction the square root of the fraction
     * @throw std::invalid_argument if the fraction is negative
     *
     * TODO: change '15' to settings::readInt("calc.float_precision", true) after settings is implemented
     */

    _fraction sqrt(const _fraction& fraction, int n = /* settings::readInt("calc.float_precision", true) */ 15);

    class Complex {
    public:
        Complex() noexcept
            : real_(0), imaginary_(0)
        { }

        Complex(const Complex&) = default;
        Complex(Complex&&) noexcept = default;
        ~Complex() = default;

        template <typename T, typename U>
        Complex(T real, U imag)
            requires(!util::is_string<T>::value && !util::is_string<U>::value)
            : real_(fraction_convertor::convert(std::move(real))), imaginary_(fraction_convertor::convert(std::move(imag)))
        { }

        template <typename T>
        Complex(T real)
            requires(!util::is_string<T>::value)
            : real_(fraction_convertor::convert(real)), imaginary_(0)
        { }

        template <typename S>
        explicit Complex(S&& str)
            requires(util::is_string<S>::value)
        {
            str.erase(std::remove(str.begin(), str.end(), ' '), str.end());

            if (str.empty()) {
                real_ = 0;
                imaginary_ = 0;
                return;
            }

            bool hasImag = (str.back() == 'i');
            if (hasImag) {
                str.pop_back();

                if (str.empty() || str == "+") {
                    real_ = 0;
                    imaginary_ = 1;
                    return;
                }
                if (str == "-") {
                    real_ = 0;
                    imaginary_ = -1;
                    return;
                }
            }

            if (!hasImag) {
                real_ = fraction_convertor::parseRational(str);
                imaginary_ = 0;
                return;
            }

            size_t pos = std::string::npos;
            size_t plus_pos = str.find_last_of('+');
            size_t minus_pos = str.find_last_of('-');

            if (plus_pos != std::string::npos && plus_pos > 0)
                pos = plus_pos;
            if (minus_pos != std::string::npos && minus_pos > 0) {
                if (pos == std::string::npos || minus_pos > pos)
                    pos = minus_pos;
            }

            if (pos == std::string::npos) {
                real_ = 0;
                imaginary_ = fraction_convertor::parseRational(str);
            } else {
                std::string realStr = str.substr(0, pos);
                std::string imagStr = str.substr(pos);

                real_ = realStr.empty() ? 0 : fraction_convertor::parseRational(realStr);

                if (imagStr == "+" || imagStr == "-") {
                    imaginary_ = (imagStr == "+") ? 1 : -1;
                } else {
                    imaginary_ = fraction_convertor::parseRational(imagStr);
                }
            }
        }

        [[nodiscard]] const _fraction& real() const { return real_; }
        [[nodiscard]] const _fraction& imaginary() const { return imaginary_; }
        void setReal(const _fraction& real) { real_ = real; }
        void setImaginary(const _fraction& imaginary) { imaginary_ = imaginary; }

        Complex operator+() const { return *this; }
        Complex operator-() const { return { -real_, -imaginary_ }; }

        Complex operator+(const Complex& other) const { return { real_ + other.real_, imaginary_ + other.imaginary_ }; }
        Complex operator-(const Complex& other) const { return { real_ - other.real_, imaginary_ - other.imaginary_ }; }
        Complex operator*(const Complex& other) const { return { real_ * other.real_ - imaginary_ * other.imaginary_, real_ * other.imaginary_ + imaginary_ * other.real_ }; }
        Complex operator/(const Complex& other) const
        {
            if (other.real_ == 0 && other.imaginary_ == 0)
                throw std::invalid_argument("Division by zero");
            return {
                (real_ * other.real_ + imaginary_ * other.imaginary_) / (other.real_ * other.real_ + other.imaginary_ * other.imaginary_),
                (imaginary_ * other.real_ - real_ * other.imaginary_) / (other.real_ * other.real_ + other.imaginary_ * other.imaginary_)
            };
        }

        Complex& operator=(const Complex& other) = default;
        Complex& operator=(Complex&&) noexcept = default;

        Complex& operator+=(const Complex& other) { return *this = *this + other; }
        Complex& operator-=(const Complex& other) { return *this = *this - other; }
        Complex& operator*=(const Complex& other) { return *this = *this * other; }
        Complex& operator/=(const Complex& other) { return *this = *this / other; }
        Complex& operator++() { return *this += 1; }
        Complex& operator--() { return *this -= 1; }
        Complex operator++(int) { Complex temp = *this; ++*this; return temp; }
        Complex operator--(int) { Complex temp = *this; --*this; return temp; }

        bool operator==(const Complex& other) const { return this == &other || (real_ == other.real_ && imaginary_ == other.imaginary_); }
        bool operator!=(const Complex& other) const { return !(*this == other); }
        bool operator<(const Complex& other) const { return modulus() < other.modulus(); }
        bool operator<=(const Complex& other) const { return modulus() <= other.modulus(); }
        bool operator>(const Complex& other) const { return modulus() > other.modulus(); }
        bool operator>=(const Complex& other) const { return modulus() >= other.modulus(); }

        operator bool() const { return real_ != 0 || imaginary_ != 0; }
        bool operator!() const { return real_ == 0 && imaginary_ == 0; }

        [[nodiscard]] _fraction modulus() const { return sqrt(real_ * real_ + imaginary_ * imaginary_); }
        [[nodiscard]] _fraction abs() const { return modulus(); }
        [[nodiscard]] Complex conjugate() const { return { real_, -imaginary_ }; }

        [[nodiscard]] std::string toString() const;
        [[nodiscard]] std::string toStringEx() const;

    private:
        _fraction real_;
        _fraction imaginary_;
    };
} // namespace TheCalculater::math

std::ostream& operator<<(std::ostream& ost, const TheCalculater::math::Complex& cpx);