/**
 * @file Complex.cpp
 * @author prbegd
 * @brief Provides a Complex class for TheCalculater.
 * @date 2025-06-23
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#include "TheCalculater/math/Complex.hpp"
#include "TheCalculater/math/detail.hpp"
#include <sstream>
#include <stdexcept>

namespace TheCalculater::math {
    Complex Complex::fromPolar(const _fraction& magnitude, const _fraction& argument) // NOLINT
    {
        return { magnitude * cos(argument), magnitude * sin(argument) };
    }
    Complex Complex::pow(const _fraction& other) const
    {
    }
    _fraction Complex::argument()
    {
        if (real_ > 0)
            return arctan(imaginary_ / real_);
        if (real_ < 0) {
            if (imaginary_ >= 0)
                return arctan(imaginary_ / real_) + pi();
            return arctan(imaginary_ / real_) - pi();
        }
        if (imaginary_ > 0)
            return pi() / 2;
        if (imaginary_ < 0)
            return -pi() / 2;
        throw std::logic_error("Complex number 0 has no definite argument.");
    }
    std::string Complex::toString() const
    {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }
    // std::string Complex::toStringEx() const
    // {
    // }
} // namespace TheCalculater::math

std::ostream& operator<<(std::ostream& ost, const TheCalculater::math::Complex& cpx)
{
    bool hasReal = cpx.real() != 0;
    bool hasImaginary = cpx.imaginary() != 0;
    if (hasReal) {
        ost << cpx.real().numerator();
        if (cpx.real().denominator() != 1)
            ost << "/" << cpx.real().denominator();
    }
    if (hasImaginary) {
        if (hasReal && cpx.imaginary() > 0)
            ost << "+";

        if (cpx.imaginary() == -1)
            ost << "-";
        else if (cpx.imaginary() != 1) {
            ost << cpx.imaginary().numerator();
            if (cpx.imaginary().denominator() != 1)
                ost << "/" << cpx.imaginary().denominator();
        }

        ost << "i";
    }
    if (!hasReal && !hasImaginary)
        ost << "0"; // 防止输出空字符串，例如 Complex(0, -1) 应该显示为 "-i" 而非 ""
    return ost;
}