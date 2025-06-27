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
#include "TheCalculater/math/detail.hpp"
#include "TheCalculater/util.hpp"


// （其实这个api还没写啦）
// #include "TheCalculater/settings.hpp"

namespace TheCalculater::math {
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

        static Complex fromPolar(const _fraction& magnitude, const _fraction& argument);

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
        Complex operator++(int)
        {
            Complex temp = *this;
            ++*this;
            return temp;
        }
        Complex operator--(int)
        {
            Complex temp = *this;
            --*this;
            return temp;
        }

        bool operator==(const Complex& other) const { return this == &other || (real_ == other.real_ && imaginary_ == other.imaginary_); }
        bool operator!=(const Complex& other) const { return !(*this == other); }
        bool operator<(const Complex& other) const { return magnitude() < other.magnitude(); }
        bool operator<=(const Complex& other) const { return magnitude() <= other.magnitude(); }
        bool operator>(const Complex& other) const { return magnitude() > other.magnitude(); }
        bool operator>=(const Complex& other) const { return magnitude() >= other.magnitude(); }

        operator bool() const { return real_ != 0 || imaginary_ != 0; }
        bool operator!() const { return real_ == 0 && imaginary_ == 0; }

        [[nodiscard]] _fraction argument();
        [[nodiscard]] _fraction magnitude() const { return sqrt(real_ * real_ + imaginary_ * imaginary_); }
        [[nodiscard]] _fraction abs() const { return sqrt(real_ * real_ + imaginary_ * imaginary_); }
        [[nodiscard]] Complex conjugate() const { return { real_, -imaginary_ }; }

        [[nodiscard]] std::string toString() const;
        [[nodiscard]] std::string toStringEx() const;

    private:
        _fraction real_;
        _fraction imaginary_;
    };
} // namespace TheCalculater::math

std::ostream& operator<<(std::ostream& ost, const TheCalculater::math::Complex& cpx);