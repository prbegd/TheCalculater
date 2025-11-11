/**
 * @file fraction.cpp
 * @author prbegd
 * @brief Fraction functions.
 * @date 2025-07-01
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#include "TheCalculater/math/fraction.hpp"
#include "TheCalculater/settings.hpp"
#include "TheCalculater/util.hpp"
#include "boost/rational.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <stdexcept>

namespace TheCalculater::math {
    Fraction reciprocal(const Fraction& x)
    {
        if (x.numerator() == 0) {
            throwEx(std::domain_error("Reciprocal of 0 is Infinity, which is undefined in rational domain."));
        } else if (x == 1 || x == -1)
            return x;
        return { x.denominator(), x.numerator() };
    }

    namespace { namespace _dPow {
        Fraction fastPow(Fraction x, boost::multiprecision::cpp_int n)
        {
            if (n < 0) {
                x = reciprocal(x);
                n = -n;
            }
            Fraction result = 1;
            while (n > 0) {
                if (n & 1) {
                    result *= x;
                }
                x *= x;
                n >>= 1;
            }
            return result;
        }
    }} // namespace ::_dPow

    Fraction pow(Fraction x, const Fraction& n)
    {
        if (n == 0)
            return 1;
        else if (n == 1)
            return x;
        else if (n == -1)
            return reciprocal(x);
        x = _dPow::fastPow(x, n.numerator());
        if (n.denominator() != 1) {
            x = root(x, n.denominator());
        }
        if (n.denominator() < 0) {
            x = reciprocal(x);
        }
        return x;
    }

    namespace {
    namespace _dMakeFraction_String {
        bool isValidInteger(const std::string& str)
        {
            if (str.empty())
                return false;

            size_t start = 0;
            if (str[0] == '+' || str[0] == '-') {
                start = 1;
                if (str.length() == 1)
                    return false; // Not a valid integer if only sign is present
            }

            for (size_t i = start; i < str.length(); ++i) {
                if (!std::isdigit(str[i]))
                    return false;
            }

            return true;
        }

        Fraction decimalToFraction(const std::string& str, size_t dotPos)
        {
            using boost::multiprecision::cpp_int;
            std::string integerPart = str.substr(0, dotPos);
            std::string decimalPart = str.substr(dotPos + 1);

            if (integerPart.empty() || decimalPart.empty()) {
                throwEx(std::invalid_argument("Invalid decimal format: " + str));
            }

            // Process like "+.123" or "-.456"
            if (integerPart == "+" || integerPart == "-") {
                integerPart += "0";
            }

            if (!isValidInteger(integerPart) || !isValidInteger(decimalPart)) {
                throwEx(std::invalid_argument("Invalid decimal format: " + str));
            }

            cpp_int numerator(integerPart + decimalPart);
            cpp_int denominator = boost::multiprecision::pow(cpp_int(10), decimalPart.length());

            return { numerator, denominator };
        }

        Fraction fractionToFraction(const std::string& str, size_t slashPos)
        {
            if (slashPos == 0 || slashPos == str.length() - 1) {
                throwEx(std::invalid_argument("Invalid fraction format: " + str));
            }

            std::string numeratorStr = str.substr(0, slashPos);
            std::string denominatorStr = str.substr(slashPos + 1);

            if (!isValidInteger(numeratorStr) || !isValidInteger(denominatorStr)) {
                throwEx(std::invalid_argument("Invalid fraction format: " + str));
            }

            boost::multiprecision::cpp_int numerator(numeratorStr);
            boost::multiprecision::cpp_int denominator(denominatorStr);

            return { numerator, denominator };
        }
    }
    } // namespace ::_dMakeFraction_String

    template <>
    Fraction makeFraction<std::string>(const std::string& rawStr)
    {
        using namespace _dMakeFraction_String;
        std::string str = boost::algorithm::trim_copy(rawStr);
        if (str.empty()) {
            throwEx(std::invalid_argument("Empty string cannot be converted to fraction"));
        }

        if (size_t slashPos = str.find('/'); slashPos != std::string::npos) {
            return fractionToFraction(str, slashPos);
        } else if (size_t dotPos = str.find('.'); dotPos != std::string::npos) {
            return decimalToFraction(str, dotPos);
        } else {
            if (!isValidInteger(str)) {
                throwEx(std::invalid_argument("Invalid integer format: " + str));
            }
            return { boost::multiprecision::cpp_int(str), 1 };
        }
    }

    Fraction pi()
    {
        return settings::readDecimal("calculating.pi");
    }
    Fraction e()
    {
        return settings::readDecimal("calculating.e");
    }

    static Fraction getTolerance()
    {
        return { 1, pow(boost::multiprecision::cpp_int(10), settings::readInteger("calculating.taylor_series_precision")) };
    }
    static unsigned getMaxIterations()
    {
        return settings::readInteger("calculating.taylor_series_max_iterations");
    }

    Fraction root(const Fraction& x, const boost::multiprecision::cpp_int& n)
    {
        if (n <= 0)
            throwEx(std::invalid_argument("Root index must be greater than 0."));
        if (x == 0)
            return 0;
        if (n == 1)
            return x;
        if (x < 0) {
            if (n % 2 == 1) {
                return -root(-x, n);
            }
            throwEx(std::domain_error("Cannot compute root of a negative number for even roots."));
        }

        const Fraction& tolerance = getTolerance();
        const unsigned max_iterations = getMaxIterations();

        Fraction y_prev = (x > 1) ? x : 1;
        Fraction y_next;

        for (unsigned i = 0; i < max_iterations; ++i) {
            Fraction pow(1);
            for (unsigned j = 0; j < n - 1; ++j)
                pow *= y_prev;

            y_next = ((n - 1) * y_prev + x / pow) / n;

            if (abs(y_next - y_prev) < tolerance)
                break;

            y_prev = y_next;
        }
        return y_next;
    }
    Fraction sin(const Fraction& x)
    {
        Fraction term = x;
        Fraction result = term;
        Fraction x_sq = x * x;
        int sign = -1;

        const Fraction& tolerance = getTolerance();
        const unsigned max_iterations = getMaxIterations();

        for (unsigned n = 1; n < max_iterations; ++n) {
            term = term * x_sq;
            term = term / ((2 * n) * (2 * n + 1));
            Fraction current_term = term * sign;
            result += current_term;
            sign *= -1;

            if (abs(current_term) < tolerance)
                break;
        }
        return result;
    }
    Fraction cos(const Fraction& x)
    {
        Fraction term(1);
        Fraction result = term;
        Fraction x_sq = x * x;
        int sign = -1;

        const unsigned max_iterations = getMaxIterations();
        const Fraction& tolerance = getTolerance();

        for (unsigned n = 1; n < max_iterations; ++n) {
            term = term * x_sq;
            term = term / ((2 * n - 1) * (2 * n));
            Fraction current_term = term * sign;
            result += current_term;
            sign *= -1;

            if (abs(current_term) < tolerance)
                break;
        }
        return result;
    }
    Fraction arcsin(const Fraction& x)
    {
        if (x < -1 || x > 1) {
            throwEx(std::domain_error("arcsin(x) is undefined for |x| > 1"));
        }

        Fraction term = x;
        Fraction result = term;
        Fraction x_sq = x * x;

        Fraction coeff(1);

        const unsigned max_iterations = getMaxIterations();
        const Fraction& tolerance = getTolerance();

        for (unsigned n = 1; n < max_iterations; ++n) {
            coeff = coeff * Fraction((2 * n) - 1, 2 * n);

            term = term * x_sq;
            Fraction next = coeff * term / ((2 * n) + 1);

            result += next;

            if (abs(next) < tolerance)
                break;
        }

        return result;
    }
    Fraction arctan(const Fraction& x)
    {
        if (x > Fraction(1)) {
            return pi() / 2 - arctan(Fraction(1) / x);
        }
        if (x < Fraction(-1)) {
            return -pi() / 2 - arctan(Fraction(1) / x);
        }

        Fraction term = x;
        Fraction result = term;
        Fraction x_sq = x * x;
        int sign = -1;

        const unsigned max_iterations = getMaxIterations();
        const Fraction& tolerance = getTolerance();

        for (unsigned n = 1; n < max_iterations; ++n) {
            term = term * x_sq;
            term = term * Fraction((2 * n) - 1, (2 * n) + 1);
            Fraction current_term = term * sign;
            result += current_term;
            sign *= -1;

            if (abs(current_term) < tolerance)
                break;
        }

        return result;
    }

    boost::multiprecision::cpp_int floor(const Fraction& x)
    {
        using namespace boost::multiprecision;

        cpp_int res = x.numerator() / x.denominator();
        if (x.numerator() < 0 && x.numerator() % x.denominator() != 0) {
            res -= 1;
        }
        return res;
    }
    boost::multiprecision::cpp_int ceil(const Fraction& x)
    {
        using namespace boost::multiprecision;

        cpp_int res = x.numerator() / x.denominator();
        if (x.numerator() > 0 && x.numerator() % x.denominator() != 0) {
            res += 1;
        }
        return res;
    }
    Fraction ln(const Fraction& x)
    {
        using namespace boost::multiprecision;
        if (x <= 0)
            throwEx(std::domain_error("natural logarithm of non-positive number"));
        if (x == 1)
            return 0;

        const Fraction& tolerance = getTolerance();

        size_t exp_num = (x.numerator() == 0) ? 0 : msb(x.numerator());
        size_t exp_den = (x.denominator() == 0) ? 0 : msb(x.denominator());

        cpp_int two_exp_num = cpp_int(1) << exp_num;
        Fraction f_num = { x.numerator(), two_exp_num };

        cpp_int two_exp_den = cpp_int(1) << exp_den;
        Fraction f_den = { x.denominator(), two_exp_den };

        Fraction f = f_num / f_den;
        cpp_int exp_val = static_cast<cpp_int>(exp_num) - static_cast<cpp_int>(exp_den);

        while (f < 1) {
            f *= 2;
            exp_val -= 1;
        }
        while (f >= 2) {
            f /= 2;
            exp_val += 1;
        }

        cpp_int exp_abs = abs(exp_val);

        Fraction tolerance1 = tolerance / (2 * (exp_abs + 1));

        Fraction ln_f = _ln_series_(f, tolerance1);
        Fraction ln2_val = _ln_series_(2, tolerance1);

        return ln_f + exp_val * ln2_val;
    }
    Fraction _ln_series_(const Fraction& x, const Fraction& tolerance)
    {
        using namespace boost::multiprecision;
        if (x <= 0)
            throwEx(std::domain_error("ln(x) is undefined for x <= 0"));
        if (x == 1)
            return 0;

        const unsigned max_iterations = getMaxIterations();

        Fraction y = (x - 1) / (x + 1);
        Fraction series = 0;
        Fraction term = y;
        cpp_int n = 1;

        for (unsigned i = 1; i < max_iterations; ++i) {
            Fraction current_term = term / n;
            if (abs(current_term) < tolerance)
                break;
            series += current_term;
            term = term * y * y;
            n += 2;
        }

        return 2 * series;
    }
} // namespace TheCalculater::math