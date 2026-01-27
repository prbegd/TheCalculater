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
#include "TheCalculater/math/prime_factorization.hpp"
#include "TheCalculater/settings.hpp"
#include "TheCalculater/util.hpp"
#include "boost/multiprecision/fwd.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>
#include <boost/regex/v5/regex.hpp>
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

    namespace { namespace _d_pow {
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
    }} // namespace ::_d_pow

    Fraction pow(Fraction x, const Fraction& n)
    {
        if (n == 0)
            return 1;
        else if (n == 1)
            return x;
        else if (n == -1)
            return reciprocal(x);
        x = _d_pow::fastPow(x, n.numerator());
        if (n.denominator() != 1) {
            x = root(x, n.denominator());
        }
        if (n.denominator() < 0) {
            x = reciprocal(x);
        }
        return x;
    }

    Fraction factorial(const Fraction& x)
    {
        if (x.denominator() != 1 || x < 0) {
            throwEx(std::invalid_argument("Factorial of non-positive integer is not supported for now."));
        }
        boost::multiprecision::cpp_int result = 1;
        for (boost::multiprecision::cpp_int i = 2; i <= x.numerator(); ++i) {
            result *= i;
        }
        return result;
    }

    Fraction operator%(const Fraction& x, const Fraction& y)
    {
        return x - floor(x / y) * y;
    }
    Fraction mod(const Fraction& x, const Fraction& y)
    {
        return x - floor(x / y) * y;
    }

    namespace { namespace _d_make_fraction_string {
        Fraction decimalToFraction(const boost::smatch& match)
        {
            using boost::multiprecision::cpp_int;
            // match[3] is matched means that there is a repeating decimal part
            if (match[3].matched) {
                cpp_int nonRepeating(match[2].str());
                cpp_int repeating(match[3].str());
                const auto& nonRepeatingLength = match[2].length();
                const auto& repeatingLength = match[3].length();

                std::string integerStr = match[1].str();
                cpp_int integerPart(integerStr);
                Fraction decimalPart(pow(cpp_int(10), repeatingLength) * nonRepeating + repeating - nonRepeating,
                    (pow(cpp_int(10), repeatingLength) - 1) * pow(cpp_int(10), nonRepeatingLength));

                Fraction result = abs(integerPart) + decimalPart;
                return integerStr[0] == '-' ? -result : result;
            } else {
                return { cpp_int(match[1].str() + match[2].str()),
                    cpp_int(pow(cpp_int(10), match[2].length())) };
            }
        }
    }} // namespace ::_d_make_fraction_string

    template <>
    Fraction makeFraction<std::string>(const std::string& str)
    {
        static const boost::regex plainFractionRegex(R"(([+-]?\d+)/(\d+))");
        if (boost::smatch match; boost::regex_match(str, match, plainFractionRegex)) {
            return {
                boost::multiprecision::cpp_int(match[1].str()),
                boost::multiprecision::cpp_int(match[2].str())
            };
        }
        static const boost::regex plainDecimalRegex(R"(([+-]?\d+)(?:\.(\d*)(?:\{(\d+)\})?)?)");
        if (boost::smatch match; boost::regex_match(str, match, plainDecimalRegex)) {
            return _d_make_fraction_string::decimalToFraction(match);
        }
        static const boost::regex latexFractionRegex(R"(([+-]?)\\frac\{([+-]?\d+)\}\{([+-]?\d+)\})");
        if (boost::smatch match; boost::regex_match(str, match, latexFractionRegex)) {
            Fraction result = {
                boost::multiprecision::cpp_int(match[2].str()),
                boost::multiprecision::cpp_int(match[3].str())
            };
            return match[1] == "-" ? -result : result;
        }
        static const boost::regex latexDecimalRegex(R"(([+-]?\d+)(?:\.(\d*)(?:\\overline\{(\d+)\})?)?)");
        if (boost::smatch match; boost::regex_match(str, match, latexDecimalRegex)) {
            return _d_make_fraction_string::decimalToFraction(match);
        }
        throwEx(std::invalid_argument("Invalid fraction format: " + str));
    }
    Fraction makeFraction(double value)
    {
        if (std::isinf(value))
            throwEx(std::invalid_argument("Cannot convert ±Infinity to rational"));
        if (std::isnan(value))
            throwEx(std::invalid_argument("Cannot convert NaN to rational"));
        std::ostringstream oss;
        oss << std::setprecision(17) << value;
        return makeFraction(oss.str());
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
        // Newton's method for root computation.
        // y_{k+1} = \frac{1}{n}[(n-1)y_{k}+\frac{x}{y^{n-1}_{k}}]

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
            y_next = ((n - 1) * y_prev + x / _d_pow::fastPow(y_prev, n - 1)) / n;

            if (abs(y_next - y_prev) < tolerance)
                break;

            y_prev = y_next;
        }
        return y_next;
    }
    Fraction sqrt(const Fraction& x) { return root(x, 2); }
    Fraction cbrt(const Fraction& x) { return root(x, 3); }
    namespace { namespace _d_trigonometric {
        Fraction shrinkRange(Fraction rad)
        {
            const Fraction pi = settings::readDecimal("calculating.pi");
            const Fraction twoPi = 2 * pi;
            rad = rad % twoPi;
            if (rad > pi) {
                rad -= twoPi;
            }
            return rad;
        }
    }} // namespace ::_d_trigonometric
    Fraction sin(const Fraction& rad)
    {
        // Maclaurin series for sine function:
        // \sin x = \sum^{\infty}_{n=0}(-1)^{n}\frac{x^{2n+1}}{(2n+1)!}
        using boost::multiprecision::cpp_int;
        // \sin -x = -\sin x
        if (rad < 0) {
            return -sin(-rad);
        }
        const Fraction x = _d_trigonometric::shrinkRange(rad);

        Fraction term = x;
        Fraction result = term;
        const Fraction xSq = x * x;
        int sign = -1;

        cpp_int denominator(1);

        const Fraction& tolerance = getTolerance();
        const unsigned maxIterations = getMaxIterations();

        for (cpp_int n = 1; n < maxIterations; ++n) {
            const cpp_int k = 2 * n;
            denominator = denominator * k * (k + 1);

            term = term * xSq;

            Fraction currentTerm = term / denominator * sign;

            result += currentTerm;
            sign *= -1;

            if (abs(currentTerm) < tolerance)
                break;
        }
        return result;
    }
    Fraction cos(const Fraction& rad)
    {
        // Maclaurin series for cosine function:
        // \cos x = \sum^{\infty}_{n=0}(-1)^{n}\frac{x^{2n}}{(2n)!}
        using boost::multiprecision::cpp_int;
        // \cos -x = \cos x, we directly remove the sign from x
        const Fraction x = _d_trigonometric::shrinkRange(abs(rad));

        const Fraction xSq = x * x;
        Fraction term = xSq;
        Fraction result = 1 - term;
        int sign = 1;

        cpp_int denominator = 2;

        const Fraction& tolerance = getTolerance();
        const unsigned maxIterations = getMaxIterations();

        for (cpp_int n = 2; n < maxIterations; ++n) {
            const cpp_int k = 2 * n;
            denominator = denominator * (k - 1) * k;

            term = term * xSq;

            Fraction currentTerm = term / denominator * sign;

            result += currentTerm;
            sign *= -1;

            if (abs(currentTerm) < tolerance)
                break;
        }
        return result;
    }
    Fraction tan(const Fraction& rad)
    {
        const Fraction deno = cos(rad);
        if (deno == 0) {
            throwEx(std::domain_error("tan(x) is undefined for x = pi/2 + k*pi, where k is an integer"));
        }
        return sin(rad) / deno;
    }
    Fraction cot(const Fraction& rad)
    {
        const Fraction deno = sin(rad);
        if (deno == 0) {
            throwEx(std::domain_error("cot(x) is undefined for x = k*pi, where k is an integer"));
        }
        return cos(rad) / deno;
    }
    Fraction sec(const Fraction& rad)
    {
        const Fraction deno = cos(rad);
        if (deno == 0) {
            throwEx(std::domain_error("sec(x) is undefined for x = pi/2 + k*pi, where k is an integer"));
        }
        return 1 / deno;
    }
    Fraction csc(const Fraction& rad)
    {
        const Fraction deno = sin(rad);
        if (deno == 0) {
            throwEx(std::domain_error("csc(x) is undefined for x = k*pi, where k is an integer"));
        }
        return 1 / deno;
    }
    namespace { namespace _d_inverse_trigonometric {
        template <util::ConstexprString TFuncName>
        void checkArcDomain(const Fraction& x)
        {
            if (x < -1 || x > 1) {
                throwEx(std::domain_error(std::string(TFuncName.v) += "(x) is undefined for |x| > 1"));
            }
        }
    }} // namespace ::_d_inverse_trigonometric

    Fraction arcsin(const Fraction& rad)
    {
        // Maclaurin series for arcsine function:
        // \arcsin x=x+\sum_{n=1}^{\infty}\frac{(2n)!}{4^n\,(n!)^2\,(2n+1)}x^{2n+1},\quad x\in[-1,1]
        _d_inverse_trigonometric::checkArcDomain<"arcsin">(rad);

        // arcsin(-x) = -arcsin(x)
        if (rad < 0) {
            return -arcsin(-rad);
        }

        if (rad == 0)
            return 0;
        if (rad == 1)
            return settings::readDecimal("calculating.pi").fraction() / 2;

        // arcsin(x) = 2 * arcsin(sqrt((1-x)/2)) for x -> 1. Currently it triggers when rad > 0.9.
        // TODO: Consider change this condition dynamically according to getTolerance().
        if (rad > Fraction { 9, 10 }) {
            Fraction x = sqrt((1 - rad) / 2);
            return settings::readDecimal("calculating.pi").fraction() / 2 - 2 * arcsin(x);
        }

        using boost::multiprecision::cpp_int;

        Fraction term = rad;
        Fraction result = term;
        Fraction xSq = rad * rad;

        cpp_int numerator(1);
        cpp_int denominator(1);
        cpp_int nFactorial(1);
        cpp_int twoN(0);

        const Fraction& tolerance = getTolerance();
        const unsigned maxIterations = getMaxIterations();

        for (cpp_int n = 1; n < maxIterations; ++n) {
            twoN = 2 * n;
            numerator = numerator * (twoN - 1) * twoN; // (2n)!
            denominator = denominator * 4; // 4^n
            nFactorial = nFactorial * n; // n!

            term = term * xSq;

            cpp_int coeffDenom = denominator * nFactorial * nFactorial * (twoN + 1);
            Fraction currentTerm = term * Fraction(numerator) / Fraction(coeffDenom);

            result += currentTerm;

            if (abs(currentTerm) < tolerance)
                break;
        }

        return result;
    }

    Fraction arccos(const Fraction& rad)
    {
        _d_inverse_trigonometric::checkArcDomain<"arccos">(rad);

        if (rad == 1)
            return 0;
        if (rad == 0)
            return settings::readDecimal("calculating.pi").fraction() / 2;
        if (rad == -1)
            return settings::readDecimal("calculating.pi");

        // arccos(x) = π/2 - arcsin(x)
        const Fraction pi = settings::readDecimal("calculating.pi");
        return pi / 2 - arcsin(rad);
    }

    Fraction arctan(const Fraction& rad)
    {
        // Maclaurin series for arctan function:
        // \arctan x=\sum_{n=0}^{\infty}\frac{(-1)^n}{2n+1}x^{2n+1},\quad x\in[-1,1]

        // arctan(-x) = -arctan(x)
        if (rad < 0) {
            return -arctan(-rad);
        }

        if (rad == 0)
            return 0;
        if (rad == 1)
            return settings::readDecimal("calculating.pi").fraction() / 4;

        // when x > 1, arctan(x) = π/2 - arctan(1/x)
        if (rad > 1)
            return settings::readDecimal("calculating.pi").fraction() / 2 - arctan(Fraction(1) / rad);

        using boost::multiprecision::cpp_int;

        Fraction term = rad;
        Fraction result = term;
        Fraction xSq = rad * rad;
        int sign = -1;

        const Fraction& tolerance = getTolerance();
        const unsigned maxIterations = getMaxIterations();

        for (cpp_int n = 1; n < maxIterations; ++n) {
            term = term * xSq;
            Fraction currentTerm = term / Fraction(2 * n + 1) * sign;

            result += currentTerm;
            sign *= -1;

            if (abs(currentTerm) < tolerance)
                break;
        }

        return result;
    }
    Fraction arccot(const Fraction& rad)
    {
        return settings::readDecimal("calculating.pi").fraction() / 2 - arctan(rad);
    }
    Fraction arcsec(const Fraction& rad)
    {
        if (abs(rad) < 1) {
            throwEx(std::domain_error("arcsec(x) is undefined for -1 < x < 1"));
        }
        return arccos(reciprocal(rad));
    }
    Fraction arccsc(const Fraction& rad)
    {
        if (abs(rad) < 1) {
            throwEx(std::domain_error("arccsc(x) is undefined for -1 < x < 1"));
        }
        return arcsin(reciprocal(rad));
    }

    boost::multiprecision::cpp_int floor(const Fraction& x)
    {
        using boost::multiprecision::cpp_int;

        cpp_int res = x.numerator() / x.denominator();
        if (x.numerator() < 0 && x.numerator() % x.denominator() != 0) {
            res -= 1;
        }
        return res;
    }
    boost::multiprecision::cpp_int ceil(const Fraction& x)
    {
        using boost::multiprecision::cpp_int;

        cpp_int res = x.numerator() / x.denominator();
        if (x.numerator() > 0 && x.numerator() % x.denominator() != 0) {
            res += 1;
        }
        return res;
    }
    namespace { namespace _d_ln {
        Fraction series(const Fraction& x, const Fraction& tolerance)
        {
            using boost::multiprecision::cpp_int;
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
    }} // namespace ::_d_ln
    Fraction ln(const Fraction& x)
    {
        using boost::multiprecision::cpp_int;
        if (x <= 0)
            throwEx(std::domain_error("Natural logarithm of non-positive number"));
        if (x == 1)
            return 0;

        const Fraction& tolerance = getTolerance();

        size_t expNumer = msb(x.numerator());
        size_t expDeno = msb(x.denominator());

        cpp_int twoExpNumer = cpp_int(1) << expNumer;
        Fraction fNumer = { x.numerator(), twoExpNumer };

        cpp_int twoExpDeno = cpp_int(1) << expDeno;
        Fraction fDeno = { x.denominator(), twoExpDeno };

        Fraction f = fNumer / fDeno;
        cpp_int expValue = static_cast<cpp_int>(expNumer) - static_cast<cpp_int>(expDeno);

        while (f < 1) {
            f *= 2;
            expValue -= 1;
        }
        while (f >= 2) {
            f /= 2;
            expValue += 1;
        }

        cpp_int expAbs = abs(expValue);

        Fraction tolerance1 = tolerance / (2 * (expAbs + 1));

        Fraction lnF = _d_ln::series(f, tolerance1);
        Fraction ln2Value = _d_ln::series(2, tolerance1);

        return lnF + expValue * ln2Value;
    }
    Fraction log(const Fraction& x, const Fraction& base)
    {
        if (base == 0 || base == 1) {
            throwEx(std::domain_error("Not unique solution for logarithm with base 0 or 1"));
        } else if (base < 0) {
            throwEx(std::domain_error("Logarithm with negative base"));
        } else if (x < 0) {
            throwEx(std::domain_error("Logarithm of negative number with positive base"));
        } else if (x == 0) {
            throwEx(std::domain_error("Logarithm of zero number"));
        }
        // log_b(M/N) = log_b(M) - log_b(N)
        if (x.denominator() != 1) {
            return log(x.numerator(), base) - log(x.denominator(), base);
        } else if (x == 1) {
            return 0;
        } else if (x == base) {
            return 1;
        }

        // log_b(a*b*c*...) = log_b(a) + log_b(b) + log_b(c) + ...
        // We use prime factorization to increase calculation speed (probably).
        const Fraction& lnBase = ln(base);
        auto factors = primeFactorization(x.numerator());
        Fraction result;
        for (const auto& factor : factors) {
            result += ln(factor) / lnBase;
        }

        return result;
    }
    Fraction lg(const Fraction& x)
    {
        return log(x, 10);
    }
} // namespace TheCalculater::math