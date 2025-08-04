/**
 * @file detail.cpp
 * @author prbegd
 * @brief Some utility functions.
 * @date 2025-07-01
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#include "TheCalculater/math/detail.hpp"

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
    } // namespace fraction_convertor

    _fraction pi()
    {
        static const _fraction value = fraction_convertor::parseDecimal(
            /* settings::readStr("calc.pi", true) */
            "3.14159265358979323846");
        return value;
    }
    _fraction e()
    {
        static const _fraction value = fraction_convertor::parseDecimal(
            /* settings::readStr("calc.e", true) */
            "2.71828182845904523536");
        return value;
    }

    static _fraction getTolerance()
    {
        return { 1, pow(boost::multiprecision::cpp_int(10),
                        /* settings::readInt("calc.float_precision", true) */ 15) };
    }
    static unsigned getMaxIterations()
    {
        return /* settings::readInt("calc.max_iterations") */ 1000;
    }
    static unsigned getTaylorIterations()
    {
        return /* settings::readInt("calc.taylor_iterations") */ 15;
    }

    _fraction root(const _fraction& fra, const boost::multiprecision::cpp_int& n)
    {
        if (n <= 0)
            throw std::invalid_argument("Root index must be greater than 0.");
        if (fra == 0)
            return 0;
        if (n == 1)
            return fra;
        if (fra < 0) {
            if (n % 2 == 1) {
                return -root(-fra, n);
            }
            throw std::domain_error("Cannot compute root of a negative number for even roots.");
        }

        const _fraction& tolerance = getTolerance();
        const unsigned max_iterations = getMaxIterations();

        _fraction y_prev = (fra > 1) ? fra : 1;
        _fraction y_next;

        for (unsigned i = 0; i < max_iterations; ++i) {
            _fraction pow(1);
            for (unsigned j = 0; j < n - 1; ++j)
                pow *= y_prev;

            y_next = ((n - 1) * y_prev + fra / pow) / n;

            if (abs(y_next - y_prev) < tolerance)
                break;

            y_prev = y_next;
        }
        return y_next;
    }
    // TODO: change iterations end condition to when the tolerance is less than given tolerance
    _fraction sin(const _fraction& fra)
    {
        const unsigned iterations = getTaylorIterations();
        _fraction term = fra;
        _fraction result = term;
        _fraction x_sq = fra * fra;
        int sign = -1;

        for (unsigned n = 1; n < iterations; ++n) {
            term = term * x_sq;
            term = term / ((2 * n) * (2 * n + 1));
            _fraction current_term = term * sign;
            result += current_term;
            sign *= -1;
        }
        return result;
    }
    _fraction cos(const _fraction& fra)
    {
        const unsigned iterations = getTaylorIterations();
        _fraction term(1);
        _fraction result = term;
        _fraction x_sq = fra * fra;
        int sign = -1;

        for (unsigned n = 1; n < iterations; ++n) {
            term = term * x_sq;
            term = term / ((2 * n - 1) * (2 * n));
            _fraction current_term = term * sign;
            result += current_term;
            sign *= -1;
        }
        return result;
    }
    _fraction arcsin(const _fraction& fra)
    {
        const unsigned iterations = getTaylorIterations();
        if (fra < -1 || fra > 1) {
            throw std::domain_error("arcsin(x) is undefined for |x| > 1");
        }

        _fraction term = fra;
        _fraction result = term;
        _fraction x_sq = fra * fra;

        _fraction coeff(1);

        for (unsigned n = 1; n < iterations; ++n) {
            coeff = coeff * _fraction((2 * n) - 1, 2 * n);

            term = term * x_sq;
            _fraction next = coeff * term / ((2 * n) + 1);

            result += next;
        }

        return result;
    }
    _fraction arctan(const _fraction& fra)
    {
        const unsigned iterations = getTaylorIterations();
        if (fra > _fraction(1)) {
            return pi() / 2 - arctan(_fraction(1) / fra);
        }
        if (fra < _fraction(-1)) {
            return -pi() / 2 - arctan(_fraction(1) / fra);
        }

        _fraction term = fra;
        _fraction result = term;
        _fraction x_sq = fra * fra;
        int sign = -1;

        for (unsigned n = 1; n < iterations; ++n) {
            term = term * x_sq;
            term = term * _fraction((2 * n) - 1, (2 * n) + 1);
            _fraction current_term = term * sign;
            result += current_term;
            sign *= -1;
        }

        return result;
    }

    boost::multiprecision::cpp_int floor(const _fraction& fra)
    {
        using namespace boost::multiprecision;

        cpp_int res = fra.numerator() / fra.denominator();
        if (fra.numerator() < 0 && fra.numerator() % fra.denominator() != 0) {
            res -= 1;
        }
        return res;
    }
    boost::multiprecision::cpp_int ceil(const _fraction& fra)
    {
        using namespace boost::multiprecision;

        cpp_int res = fra.numerator() / fra.denominator();
        if (fra.numerator() > 0 && fra.numerator() % fra.denominator() != 0) {
            res += 1;
        }
        return res;
    }
    _fraction ln(const _fraction& fra)
    {
        using namespace boost::multiprecision;
        if (fra <= 0) 
            throw std::domain_error("natural logarithm of non-positive number");
        if (fra == 1) 
            return 0;
        
        const _fraction& tolerance = getTolerance();


        size_t exp_num = (fra.numerator() == 0) ? 0 : msb(fra.numerator());
        size_t exp_den = (fra.denominator() == 0) ? 0 : msb(fra.denominator());

        cpp_int two_exp_num = cpp_int(1) << exp_num;
        _fraction f_num = {fra.numerator(), two_exp_num};

        cpp_int two_exp_den = cpp_int(1) << exp_den;
        _fraction f_den = {fra.denominator(), two_exp_den};

        _fraction f = f_num / f_den;
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

        _fraction tolerance1 = tolerance / (2 * (exp_abs + 1));

        _fraction ln_f = _ln_series_(f, tolerance1);
        _fraction ln2_val = _ln_series_(2, tolerance1);

        return ln_f + exp_val * ln2_val;
    }
    _fraction _ln_series_(const _fraction& fra, const _fraction& tolerance)
    {
        using namespace boost::multiprecision;
        if (fra <= 0)
            throw std::domain_error("ln(x) is undefined for x <= 0");
        if (fra == 1)
            return 0;

        const unsigned max_iterations = getMaxIterations();

        _fraction x = (fra - 1) / (fra + 1);
        _fraction series = 0;
        _fraction term = x;
        cpp_int n = 1;

        for (unsigned i = 1; i < max_iterations; ++i) {
            _fraction current_term = term / n;
            if (abs(current_term) < tolerance)
                break;
            series += current_term;
            term = term * x * x;
            n += 2;
        }

        return 2 * series;
    }
} // namespace TheCalculater::math