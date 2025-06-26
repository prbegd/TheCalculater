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
#include <sstream>

namespace TheCalculater::math::fraction_convertor {

    static bool processNegative(std::string& str)
    {
        if (str.empty())
            return false;

        bool negative = (str[0] == '-');
        if (str[0] == '+' || str[0] == '-') {
            str.erase(0, 1);
        }
        return negative;
    }

    _fraction parseDecimal(std::string str)
    {
        using boost::multiprecision::cpp_int;

        if (str.empty())
            return { cpp_int(0), cpp_int(1) };

        bool negative = processNegative(str);

        size_t pos = str.find('.');
        if (pos == std::string::npos) {
            cpp_int num(str);
            if (negative)
                num = -num;
            return { num, cpp_int(1) };
        }

        if (str == ".")
            return { cpp_int(0), cpp_int(1) };
        if (pos == 0)
            str.insert(0, "0");
        if (pos == str.size() - 1)
            str.push_back('0');

        std::string integer = str.substr(0, pos);
        std::string fractional = str.substr(pos + 1);

        cpp_int num = cpp_int(integer) * cpp_int("1" + std::string(fractional.size(), '0'))
            + cpp_int(fractional.empty() ? "0" : fractional);

        if (negative)
            num = -num;
        cpp_int denom = cpp_int("1" + std::string(fractional.size(), '0'));

        return { num, denom };
    }
    _fraction parseRational(std::string str)
    {
        using boost::multiprecision::cpp_int;

        if (str.empty())
            return { cpp_int(0), cpp_int(1) };

        bool negative = processNegative(str);

        size_t pos = str.find('/');
        if (pos == std::string::npos)
            return parseDecimal(str);

        if (pos == 0 || pos == str.size() - 1)
            throw std::invalid_argument("Invalid rational format: " + str);

        std::string numStr = str.substr(0, pos);
        std::string denomStr = str.substr(pos + 1);

        cpp_int numerator = numStr.empty() ? cpp_int(0) : cpp_int(numStr);
        cpp_int denominator = denomStr.empty() ? cpp_int(1) : cpp_int(denomStr);

        if (denominator == 0)
            throw std::invalid_argument("Denominator cannot be zero: " + str);

        if (negative)
            numerator = -numerator;

        return { numerator, denominator };
    }
    _fraction parseFloat(double value)
    {
        std::ostringstream oss;
        // TODO: change '15' to settings::readInt("calc.float_precision", true) after settings is implemented
        // for future myself: true means cache
        oss << std::setprecision(/* settings::readInt("calc.precision", true) */ 15) << value;
        return parseDecimal(oss.str());
    }
} // namespace TheCalculater::math::fraction_convertor
namespace TheCalculater::math {
    _fraction sqrt(const _fraction& fra, int n)
    {
        using namespace boost::multiprecision;
        if (fra < 0)
            throw std::invalid_argument("Cannot compute square root of a negative number.");

        if (fra == 0)
            return 0;

        _fraction threshold(1, pow(cpp_int(10), n));

        _fraction x_prev = fra;
        _fraction x_next;
        // 最大迭代次数防止不收敛
        const int max_iterations = 1000;
        int iterations = 0;

        while (true) {

            // 应用巴比伦法迭代公式: x_{k+1} = (x_k + s / x_k) / 2
            x_next = (x_prev + fra / x_prev) / 2;

            // 检查是否满足精度要求
            _fraction diff = abs(x_next - x_prev);
            if (diff < threshold || iterations >= max_iterations) {
                break;
            }

            x_prev = x_next;
            ++iterations;
        }

        return x_next;
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