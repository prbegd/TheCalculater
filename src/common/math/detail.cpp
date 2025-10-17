/**
 * @file detail.cpp
 * @author prbegd
 * @brief Some utility functions.
 * @date 2025-07-01
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#include "TheCalculater/math/detail.hpp"
#include "TheCalculater/settings.hpp"
#include "TheCalculater/util.hpp"
#include <stdexcept>

namespace TheCalculater::math {
    namespace fraction_convertor {
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

        Fraction parseDecimal(std::string str)
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
        Fraction parseRational(std::string str)
        {
            using boost::multiprecision::cpp_int;

            if (str.empty())
                return { cpp_int(0), cpp_int(1) };

            bool negative = processNegative(str);

            size_t pos = str.find('/');
            if (pos == std::string::npos)
                return parseDecimal(str);

            if (pos == 0 || pos == str.size() - 1)
                throwEx(std::invalid_argument("Invalid rational format: " + str));

            std::string numStr = str.substr(0, pos);
            std::string denomStr = str.substr(pos + 1);

            cpp_int numerator = numStr.empty() ? cpp_int(0) : cpp_int(numStr);
            cpp_int denominator = denomStr.empty() ? cpp_int(1) : cpp_int(denomStr);

            if (denominator == 0)
                throwEx(std::domain_error("Denominator cannot be zero: " + str));

            if (negative)
                numerator = -numerator;

            return { numerator, denominator };
        }
    } // namespace fraction_convertor

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