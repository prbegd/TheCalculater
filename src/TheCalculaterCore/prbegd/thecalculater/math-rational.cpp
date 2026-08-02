/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 *
 */
module;
module prbegd.thecalculater.math;
import prbegd.thecalculater.util;
import thirdparty.core;
import std;

namespace thecalculater::math {
Integer makeInteger(std::string_view str)
{
    if (str.starts_with('+')) {
        str.remove_prefix(1);
    }
    return Integer(str);
}
namespace { namespace impl::make_rational::string {
    Rational decimalToRational(const boost::cmatch& match)
    {
        // match[3] is matched means that there is a repeating decimal part
        if (match[3].matched) {
            const Integer nonRepeating(match[2].str());
            const Integer repeating(match[3].str());
            const auto& nonRepeatingLength = match[2].length();
            const auto& repeatingLength = match[3].length();

            std::string integerStr = match[1].str();
            const Integer integerPart = makeInteger(integerStr);
            const Rational decimalPart(pow(Integer(10), repeatingLength) * nonRepeating + repeating - nonRepeating,
                                       (pow(Integer(10), repeatingLength) - 1) * pow(Integer(10), nonRepeatingLength));

            const Rational result = Integer(abs(integerPart)) + decimalPart;
            return integerStr[0] == '-' ? -result : result;
        }
        std::string integerPart = match[1].str();
        std::string decimalPart = match[2].str();
        if (decimalPart.empty()) {
            return { makeInteger(integerPart) };
        }

        Integer deno = pow(Integer(10), decimalPart.length());
        // Integer(std::string) consider number beginning with 0 as octal number, so we remove the leading 0 (when it exists)
        if (integerPart == "0") {
            return { makeInteger(decimalPart), deno };
        }
        if (integerPart == "-0") {
            return { -makeInteger(decimalPart), deno };
        }
        return { makeInteger(integerPart + decimalPart), deno };
    }
}} // namespace ::impl::make_rational::string

Rational makeRational(std::string_view str)
{
    static const boost::regex plainFractionRegex(R"(\s*([+-]?\d+)/(\d+)\s*)");
    boost::cmatch match;
    if (boost::regex_match(str.begin(), str.end(), match, plainFractionRegex)) {
        return {
            makeInteger(match[1].str()),
            makeInteger(match[2].str())
        };
    }
    static const boost::regex plainDecimalRegex(R"(\s*([+-]?\d+)(?:\.(\d*)(?:\{(\d+)\})?)?\s*)");
    if (boost::regex_match(str.begin(), str.end(), match, plainDecimalRegex)) {
        return impl::make_rational::string::decimalToRational(match);
    }
    static const boost::regex latexFractionRegex(R"(\s*([+-]?)\\frac\{([+-]?\d+)\}\{([+-]?\d+)\}\s*)");
    if (boost::regex_match(str.begin(), str.end(), match, latexFractionRegex)) {
        const Rational result = {
            makeInteger(match[2].str()),
            makeInteger(match[3].str())
        };
        return match[1] == "-" ? -result : result;
    }
    static const boost::regex latexDecimalRegex(R"(\s*([+-]?\d+)(?:\.(\d*)(?:\\overline\{(\d+)\})?)?\s*)");
    if (boost::regex_match(str.begin(), str.end(), match, latexDecimalRegex)) {
        return impl::make_rational::string::decimalToRational(match);
    }
    throwext(InvalidRationalParseException<std::string>(std::string(str)));
}
Rational makeRational(double value)
{

    if (value == 0.0) {
        return 0;
    }
    if (!std::isfinite(value)) {
        throwext(InvalidRationalParseException<double>(value));
    }

    int exp = 0;
    const double frac = std::frexp(value, &exp);

    const Integer significand = static_cast<Integer>(std::llround(std::ldexp(frac, 53)));

    const int shift = exp - 53;

    Integer numerator;
    Integer denominator;
    if (shift >= 0) {
        numerator = significand << shift;
        denominator = 1;
    } else {
        numerator = significand;
        denominator = Integer(1) << (-shift);
    }

    if (numerator == 0) {
        denominator = 1;
    }

    return { numerator, denominator };
}

namespace { namespace impl::format::rational {
    std::string alwaysFraction(const Rational& x, const ExpressionOutputFormat type)
    {
        switch (type) {
        case ExpressionOutputFormat::LaTeX:
            return std::format("\\frac{{{}}}{{{}}}", numerator(x), denominator(x));
        case ExpressionOutputFormat::PlainText:
            return std::format("{}/{}", numerator(x), denominator(x));
        }
    }
    bool decimalRepeated(const Rational& x)
    {
        std::map<Integer, Integer> denoPrimeFactors = primeFactorization(denominator(x));
        return std::ranges::any_of(denoPrimeFactors, [](const auto& n) { return n.first != 2 && n.first != 5; });
    }
    std::string preferDecimal(const Rational& x, const ExpressionOutputFormat type)
    {
        Integer numer = numerator(x);
        const Integer& denom = denominator(x);
        if (denom == 1) {
            return numer.str();
        }
        if (decimalRepeated(x)) {
            return alwaysFraction(x, type);
        }
        std::ostringstream result;

        if (numer < 0) {
            result << '-';
            numer = -numer;
        }
        const Integer integerPart = numer / denom;
        Integer remainder = numer % denom;
        result << integerPart.str() << '.';

        while (remainder != 0) {
            const Integer quotient = remainder * 10 / denom;
            result << quotient.str();

            remainder = (remainder * 10) % denom;
        }

        return result.str();
    }

    std::string alwaysDecimal(const Rational& x, ExpressionOutputFormat type)
    {
        Integer numer = numerator(x);
        const Integer& denom = denominator(x);
        if (denom == 1) {
            return numer.str();
        }
        std::ostringstream result;

        const std::string_view repeatingBeginMarker = type == ExpressionOutputFormat::LaTeX ? "\\overline{" : "{";
        const char repeatingEndMarker = '}';

        if (numer < 0) {
            result << '-';
            numer = -numer;
        }
        const Integer integerPart = numer / denom;
        Integer remainder = numer % denom;
        result << integerPart.str() << '.';

        std::unordered_map<Integer, std::size_t> remainderPositions;
        std::string decimalPart;
        std::size_t position = 0;

        while (remainder != 0) {
            auto it = remainderPositions.find(remainder);
            if (it != remainderPositions.end()) {
                std::size_t cycle_start = it->second;

                std::string nonRepeating = decimalPart.substr(0, cycle_start);
                std::string repeating = decimalPart.substr(cycle_start);

                result << nonRepeating
                       << repeatingBeginMarker
                       << repeating
                       << repeatingEndMarker;

                return result.str();
            }

            remainderPositions[remainder] = position++;

            remainder *= 10;
            const Integer quotient = remainder / denom;
            remainder = remainder % denom;

            decimalPart += quotient.str();
        }

        result << decimalPart;
        return result.str();
    }
}} // namespace ::impl::format::rational
std::string format(const Rational& x, const RationalFormatOptions& options)
{
    switch (options.numeric) {
    case RationalNumericFormat::AlwaysFraction:
        return impl::format::rational::alwaysFraction(x, options.output);
    case RationalNumericFormat::PreferInteger: {
        if (denominator(x) == 1) {
            return numerator(x).str();
        }
        return impl::format::rational::alwaysFraction(x, options.output);
    }
    case RationalNumericFormat::PreferDecimal:
        return impl::format::rational::preferDecimal(x, options.output);
    case RationalNumericFormat::AlwaysDecimal:
        return impl::format::rational::alwaysDecimal(x, options.output);
    }
}

Rational reciprocal(const Rational& x)
{
    if (numerator(x) == 0) {
        throwext(std::overflow_error("Reciprocal of 0"));
    } else if (x == 1 || x == -1) {
        return x;
    }
    if (x < 0) {
        return { -denominator(x), -numerator(x) };
    }
    return { denominator(x), numerator(x) };
}

namespace { namespace impl::pow {
    Integer fastPow(Integer base, Integer exponent)
    {
        if (exponent == 1) {
            return base;
        }
        Integer result = 1;
        while (exponent > 0) {
            if (exponent & 1) {
                result *= base;
            }
            base *= base;
            exponent >>= 1;
        }
        return result;
    }
    Rational fastPow(const Rational& base, const Integer& exponent)
    {
        if (exponent < 0) {
            return fastPow(reciprocal(base), -exponent);
        }
        return { impl::pow::fastPow(numerator(base), exponent), impl::pow::fastPow(denominator(base), exponent) };
    }
}} // namespace ::impl::pow

namespace { namespace impl::root {
    Integer roughIntegerRoot(const Integer& radicand, const Integer& index)
    {
        Integer guess = 1;
        guess <<= (((msb(radicand) + index) / index) - 1).convert_to<std::uint64_t>();
        if (guess < 1) {
            guess = 1;
        }
        return guess;
    }

    Rational roughRationalRoot(const Rational& radicand, const Integer& index)
    {
        return { roughIntegerRoot(numerator(radicand), index),
                 roughIntegerRoot(denominator(radicand), index) };
    }

    std::optional<Integer> rootOfPerfectPower(const Integer& radicand, const Integer& index, const RationalCalculationOptions& config)
    {
        Integer y_prev = roughIntegerRoot(radicand, index);
        Integer y_next;

        for (unsigned i = 0; i < config.approximation.maxIterations; ++i) {
            const Integer power = impl::pow::fastPow(y_prev, index - 1);

            y_next = ((index - 1) * y_prev + radicand / power) / index;

            if (y_next == y_prev && impl::pow::fastPow(y_next, index) == radicand) {
                return y_next;
            }

            y_prev = y_next;
        }
        return std::nullopt;
    }

    // Expect x != 0, x != 1, x > 0, n > 1.
    Rational iterationApproximate(const Rational& radicand, const Integer& index, const RationalCalculationOptions& config)
    {
        if (!config.approximation.enabled) {
            throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult));
        }
        Rational y_prev = roughRationalRoot(radicand, index);
        Rational y_next;

        for (unsigned i = 0; i < config.approximation.maxIterations; ++i) {
            y_next = ((index - 1) * y_prev + radicand / impl::pow::fastPow(y_prev, index - 1)) / index;

            if (abs(y_next - y_prev) < config.approximation.tolerance) {
                break;
            }

            y_prev = y_next;
        }
        return y_next;
    }
    Rational root(const Rational& radicand, const Integer& index, const RationalCalculationOptions& config)
    {
        // Newton's method for root computation.
        // y_{k+1} = \frac{1}{n}[(n-1)y_{k}+\frac{x}{y^{n-1}_{k}}]
        if (index < 0) {
            return reciprocal(impl::root::root(radicand, -index, config));
        }
        if (index == 1) {
            return radicand;
        }
        if (radicand == 0) {
            return 0;
        }
        if (radicand == 1) {
            return 1;
        }
        if (radicand < 0 && index % 2 == 1) {
            return -impl::root::root(-radicand, index, config);
        }

        auto numerRoot = impl::root::rootOfPerfectPower(numerator(radicand), index, config);
        if (numerRoot) {
            if (denominator(radicand) == 1) {
                return *numerRoot;
            }
            auto denoRoot = impl::root::rootOfPerfectPower(denominator(radicand), index, config);
            if (denoRoot) {
                return { *numerRoot, *denoRoot };
            }
        }
        return impl::root::iterationApproximate(radicand, index, config);
    }
}} // namespace ::impl::root

Rational pow(Rational base, const Rational& exponent, const RationalCalculationOptions& config)
{
    if (base == 0 && exponent == 0) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IndeterminateForm, RationalCalculationException::Operation::Power, { base, exponent }));
    }
    if (base < 0 && denominator(exponent) % 2 == 0) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Domain, RationalCalculationException::Operation::Power, { base, exponent }));
    }
    if (base == 0 && exponent < 0) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Pole, RationalCalculationException::Operation::Power, { base, exponent }));
    }
    try {
        base = impl::pow::fastPow(base, numerator(exponent));
        if (denominator(exponent) != 1) {
            base = impl::root::root(base, denominator(exponent), config);
        }
        return base;
    } catch (const RationalCalculationException& e) {
        if (e.type == RationalCalculationException::Type::IrrationalResult) {
            throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Power, { base, exponent }));
        }
        throw;
    }
}
// OPTIMIZE(P3): current performance sucks when `index` is large.
Rational root(const Rational& radicand, const Rational& index, const RationalCalculationOptions& config)
{
    if (index == 0 || (radicand < 0 && index % 2 == 0)) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Domain, RationalCalculationException::Operation::Root, { radicand, index }));
    }
    if (radicand == 0 && index < 0) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Pole, RationalCalculationException::Operation::Root, { radicand, index }));
    }
    try {
        return impl::root::root(impl::pow::fastPow(radicand, denominator(index)), numerator(index), config);
    } catch (const RationalCalculationException& e) {
        if (e.type == RationalCalculationException::Type::IrrationalResult) {
            throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Root, { radicand, index }));
        }
        throw;
    }
}
Rational sqrt(const Rational& radicand, const RationalCalculationOptions& config) { return root(radicand, 2, config); }
Rational cbrt(const Rational& radicand, const RationalCalculationOptions& config) { return root(radicand, 3, config); }

// TODO(P0): Use some approximate method to support Rational case
Integer factorial(const Integer& x)
{
    if (x < 0) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Pole, RationalCalculationException::Operation::Factorial, { x }));
    }
    Integer result = 1;
    for (Integer i = 2; i <= x; i++) {
        result *= i;
    }
    return result;
}

Rational mod(const Rational& dividend, const Rational& divisor)
{
    if (divisor == 0) {
        throwext(std::overflow_error("Modulus by zero"));
    }
    if (dividend == 0) {
        return 0;
    }
    return dividend - floor(dividend / divisor) * divisor;
}
Rational operator%(const Rational& dividend, const Rational& divisor)
{
    if (divisor == 0) {
        throwext(std::overflow_error("Modulus by zero"));
    }
    if (dividend == 0) {
        return 0;
    }
    return dividend - floor(dividend / divisor) * divisor;
}
namespace { namespace impl::trigonometric {
    Rational shrinkRange(Rational rad, const RationalCalculationOptions& config)
    {
        const Rational twoPi = 2 * config.approximation.constants.pi;
        rad = rad % twoPi;
        if (rad > config.approximation.constants.pi) {
            rad -= twoPi;
        }
        return rad;
    }
}} // namespace ::impl::trigonometric
Rational sin(const Rational& rad, const RationalCalculationOptions& config)
{
    // Maclaurin series for sine function:
    // \sin x = \sum^{\infty}_{n=0}(-1)^{n}\frac{x^{2n+1}}{(2n+1)!}

    if (rad == 0) {
        return 0;
    }
    if (!config.approximation.enabled) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Sine, { rad }));
    }
    // \sin -x = -\sin x
    if (rad < 0) {
        return -sin(-rad, config);
    }
    const Rational x = impl::trigonometric::shrinkRange(rad, config);

    Rational term = x;
    Rational result = term;
    const Rational xSq = x * x;
    int sign = -1;

    Integer denominator(1);

    for (Integer n = 1; n < config.approximation.maxIterations; ++n) {
        const Integer k = 2 * n;
        denominator = denominator * k * (k + 1);

        term = term * xSq;

        const Rational currentTerm = term / denominator * sign;

        result += currentTerm;
        sign *= -1;

        if (abs(currentTerm) < config.approximation.tolerance) {
            break;
        }
    }
    return result;
}
Rational cos(const Rational& rad, const RationalCalculationOptions& config)
{
    // Maclaurin series for cosine function:
    // \cos x = \sum^{\infty}_{n=0}(-1)^{n}\frac{x^{2n}}{(2n)!}

    if (rad == 0) {
        return 1;
    }
    if (!config.approximation.enabled) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Cosine, { rad }));
    }
    // \cos -x = \cos x, we directly remove the sign from x
    const Rational x = impl::trigonometric::shrinkRange(abs(rad), config);

    const Rational xSq = x * x;
    Rational term = xSq;
    Integer denominator = 2;

    Rational result = 1 - term / denominator;
    int sign = 1;

    for (Integer n = 2; n < config.approximation.maxIterations; ++n) {
        const Integer k = 2 * n;
        denominator = denominator * (k - 1) * k;

        term = term * xSq;

        const Rational currentTerm = term / denominator * sign;

        result += currentTerm;
        sign *= -1;

        if (abs(currentTerm) < config.approximation.tolerance) {
            break;
        }
    }
    return result;
}
Rational tan(const Rational& rad, const RationalCalculationOptions& config)
{
    if (rad == 0) {
        return 0;
    }
    if (!config.approximation.enabled) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Tangent, { rad }));
    }
    const Rational deno = cos(rad, config);
    if (config.approximation.approxEqual(deno, 0)) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Pole, RationalCalculationException::Operation::Tangent, { rad }));
    }
    return sin(rad, config) / deno;
}
Rational cot(const Rational& rad, const RationalCalculationOptions& config)
{
    if (rad == 0) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Pole, RationalCalculationException::Operation::Cotangent, { rad }));
    }
    if (!config.approximation.enabled) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Cotangent, { rad }));
    }
    const Rational deno = sin(rad, config);
    if (config.approximation.approxEqual(deno, 0)) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Pole, RationalCalculationException::Operation::Cotangent, { rad }));
    }
    return cos(rad, config) / deno;
}
Rational sec(const Rational& rad, const RationalCalculationOptions& config)
{
    if (rad == 0) {
        return 1;
    }
    if (!config.approximation.enabled) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Secant, { rad }));
    }
    const Rational deno = cos(rad, config);
    if (config.approximation.approxEqual(deno, 0)) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Pole, RationalCalculationException::Operation::Secant, { rad }));
    }
    return 1 / deno;
}
Rational csc(const Rational& rad, const RationalCalculationOptions& config)
{
    if (!config.approximation.enabled) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Cosecant, { rad }));
    }
    const Rational deno = sin(rad, config);
    if (config.approximation.approxEqual(deno, 0)) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Pole, RationalCalculationException::Operation::Cosecant, { rad }));
    }
    return 1 / deno;
}

Rational arcsin(const Rational& rad, const RationalCalculationOptions& config)
{
    // Maclaurin series for arcsine function:
    // \arcsin x=x+\sum_{n=1}^{\infty}\frac{(2n)!}{4^n\,(n!)^2\,(2n+1)}x^{2n+1},\quad x\in[-1,1]
    if (rad == 0) {
        return 0;
    }
    if (!config.approximation.enabled) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Arcsine, { rad }));
    }
    if (rad < -1 || rad > 1) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Domain, RationalCalculationException::Operation::Arcsine, { rad }));
    }
    // arcsin(-x) = -arcsin(x)
    if (rad < 0) {
        return -arcsin(-rad, config);
    }

    if (rad == 1) {
        return config.approximation.constants.pi / 2;
    }

    // arcsin(x) = 2 * arcsin(sqrt((1-x)/2)) for x -> 1. Currently it triggers when rad > 0.9.
    // HACK(P3): Consider change this condition dynamically according to getTolerance().
    if (rad > Rational { 9, 10 }) {
        const Rational x = sqrt((1 - rad) / 2, config);
        return config.approximation.constants.pi / 2 - 2 * arcsin(x, config);
    }

    Rational term = rad;
    Rational result = term;
    const Rational xSq = rad * rad;

    Integer numerator(1);
    Integer denominator(1);
    Integer nFactorial(1);
    Integer twoN(0);

    for (Integer n = 1; n < config.approximation.maxIterations; ++n) {
        twoN = 2 * n;
        numerator = numerator * (twoN - 1) * twoN; // (2n)!
        denominator = denominator * 4; // 4^n
        nFactorial = nFactorial * n; // n!

        term = term * xSq;

        const Integer coeffDenom = denominator * nFactorial * nFactorial * (twoN + 1);
        const Rational currentTerm = term * Rational(numerator) / Rational(coeffDenom);

        result += currentTerm;

        if (abs(currentTerm) < config.approximation.tolerance) {
            break;
        }
    }

    return result;
}

Rational arccos(const Rational& rad, const RationalCalculationOptions& config)
{
    if (rad == 1) {
        return 0;
    }
    if (!config.approximation.enabled) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Arccosine, { rad }));
    }
    if (rad < -1 || rad > 1) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Domain, RationalCalculationException::Operation::Arccosine, { rad }));
    }
    if (rad == 0) {
        return config.approximation.constants.pi / 2;
    }
    if (rad == -1) {
        return config.approximation.constants.pi;
    }

    // arccos(x) = π/2 - arcsin(x)
    return config.approximation.constants.pi / 2 - arcsin(rad, config);
}

Rational arctan(const Rational& rad, const RationalCalculationOptions& config)
{
    // Maclaurin series for arctan function:
    // \arctan x=\sum_{n=0}^{\infty}\frac{(-1)^n}{2n+1}x^{2n+1},\quad x\in[-1,1]

    if (rad == 0) {
        return 0;
    }
    if (!config.approximation.enabled) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Arctangent, { rad }));
    }

    // arctan(-x) = -arctan(x)
    if (rad < 0) {
        return -arctan(-rad, config);
    }

    if (rad == 1) {
        return config.approximation.constants.pi / 4;
    }

    // when x > 1, arctan(x) = π/2 - arctan(1/x)
    if (rad > 1) {
        return config.approximation.constants.pi / 2 - arctan(Rational(1) / rad, config);
    }

    Rational term = rad;
    Rational result = term;
    const Rational xSq = rad * rad;
    int sign = -1;

    for (Integer n = 1; n < config.approximation.maxIterations; ++n) {
        term = term * xSq;
        const Rational currentTerm = term / Rational(2 * n + 1) * sign;

        result += currentTerm;
        sign *= -1;

        if (abs(currentTerm) < config.approximation.tolerance) {
            break;
        }
    }

    return result;
}
Rational arccot(const Rational& rad, const RationalCalculationOptions& config)
{
    if (!config.approximation.enabled) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Arccotangent, { rad }));
    }
    return config.approximation.constants.pi / 2 - arctan(rad, config);
}
Rational arcsec(const Rational& rad, const RationalCalculationOptions& config)
{
    if (rad == 1) {
        return 0;
    }
    if (abs(rad) < 1) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Domain, RationalCalculationException::Operation::Arcsecant, { rad }));
    }
    if (!config.approximation.enabled) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Arcsecant, { rad }));
    }
    return arccos(reciprocal(rad), config);
}
Rational arccsc(const Rational& rad, const RationalCalculationOptions& config)
{
    if (abs(rad) < 1) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Domain, RationalCalculationException::Operation::Arccosecant, { rad }));
    }
    if (!config.approximation.enabled) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Arccosecant, { rad }));
    }
    return arcsin(reciprocal(rad), config);
}

Integer floor(const Rational& x)
{

    Integer res = numerator(x) / denominator(x);
    if (numerator(x) < 0 && numerator(x) % denominator(x) != 0) {
        res -= 1;
    }
    return res;
}
Integer ceil(const Rational& x)
{

    Integer res = numerator(x) / denominator(x);
    if (numerator(x) > 0 && numerator(x) % denominator(x) != 0) {
        res += 1;
    }
    return res;
}
namespace { namespace impl::ln {
    Rational series(const Rational& argument, const Rational& tolerance, const RationalCalculationOptions& config)
    {

        if (argument == 1) {
            return 0;
        }

        const Rational y = (argument - 1) / (argument + 1);
        Rational series = 0;
        Rational term = y;
        Integer n = 1;

        for (unsigned i = 1; i < config.approximation.maxIterations; ++i) {
            const Rational current_term = term / n;
            if (abs(current_term) < tolerance) {
                break;
            }
            series += current_term;
            term = term * y * y;
            n += 2;
        }

        return 2 * series;
    }
}} // namespace ::impl::natural_logarithm
Rational ln(const Rational& argument, const RationalCalculationOptions& config)
{

    if (argument == 0) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Pole, RationalCalculationException::Operation::NaturalLogarithm, { argument }));
    }
    if (argument < 0) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Domain, RationalCalculationException::Operation::NaturalLogarithm, { argument }));
    }
    if (argument == 1) {
        return 0;
    }
    if (!config.approximation.enabled) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::NaturalLogarithm, { argument }));
    }

    std::size_t expNumer = msb(numerator(argument));
    std::size_t expDeno = msb(denominator(argument));

    Integer twoExpNumer = Integer(1) << expNumer;
    const Rational fNumer = { numerator(argument), twoExpNumer };

    Integer twoExpDeno = Integer(1) << expDeno;
    const Rational fDeno = { denominator(argument), twoExpDeno };

    Rational f = fNumer / fDeno;
    Integer expValue = static_cast<Integer>(expNumer) - static_cast<Integer>(expDeno);

    while (f < 1) {
        f *= 2;
        expValue -= 1;
    }
    while (f >= 2) {
        f /= 2;
        expValue += 1;
    }

    const Integer expAbs = abs(expValue);

    const Rational tolerance1 = config.approximation.tolerance / (2 * (expAbs + 1));

    const Rational lnF = impl::ln::series(f, tolerance1, config);
    const Rational ln2Value = impl::ln::series(2, tolerance1, config);

    return lnF + expValue * ln2Value;
}
namespace { namespace impl::log {
    std::optional<Rational> calculateRational(const Rational& argument, const Rational& base)
    {
        if (argument == 1) {
            return 0;
        }
        if (argument == base) {
            return 1;
        }

        const bool positive = (argument > 1 && base > 1) || (argument < 1 && base < 1);
        const Rational transformedBase = positive ? base : reciprocal(base);

        using Factors = decltype(primeFactorization<Integer>(0));
        const Factors transformedBaseNumerFactors = primeFactorization(numerator(transformedBase));
        const Factors argumentNumerFactors = primeFactorization(numerator(argument));
        const Factors transformedBaseDenomFactors = primeFactorization(denominator(transformedBase));
        const Factors argumentDenomFactors = primeFactorization(denominator(argument));

        if (!(std::ranges::equal(transformedBaseNumerFactors | std::views::keys, argumentNumerFactors | std::views::keys) && std::ranges::equal(transformedBaseDenomFactors | std::views::keys, argumentDenomFactors | std::views::keys))) {
            return std::nullopt;
        }

        const auto calculateFactorsExponentRatio = [](const Factors& transformedBaseFactors, const Factors& argumentFactors) -> std::optional<Rational> {
            Rational candidate = Rational(argumentFactors.begin()->second, transformedBaseFactors.at(argumentFactors.begin()->first));
            for (const auto& [argumentFactor, argumentFactorExponent] : argumentFactors) {
                const Rational ratio = Rational(argumentFactorExponent, transformedBaseFactors.at(argumentFactor));
                if (ratio != candidate) {
                    return std::nullopt;
                }
            }
            return candidate;
        };

        std::optional<Rational> numerRatio;
        if (numerator(transformedBase) != 1 && numerator(argument) != 1) {
            numerRatio = calculateFactorsExponentRatio(transformedBaseNumerFactors, argumentNumerFactors);
            if (!numerRatio) {
                return std::nullopt;
            }
            if (!positive) {
                *numerRatio = -(*numerRatio);
            }
        }
        std::optional<Rational> denomRatio;
        if (denominator(transformedBase) != 1 && denominator(argument) != 1) {
            denomRatio = calculateFactorsExponentRatio(transformedBaseDenomFactors, argumentDenomFactors);
            if (!denomRatio) {
                return std::nullopt;
            }
            if (!positive) {
                *denomRatio = -(*denomRatio);
            }
        }
        if (numerRatio && denomRatio && *numerRatio != *denomRatio) {
            return std::nullopt;
        }
        return numerRatio ? numerRatio : denomRatio;
    }
}} // namespace ::impl::logarithm
Rational log(const Rational& argument, const Rational& base, const RationalCalculationOptions& config)
{
    if (base == 1) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IndeterminateForm, RationalCalculationException::Operation::Logarithm, { argument, base }));
    }
    if (base <= 0 || argument <= 0) {
        throwext(RationalCalculationException(RationalCalculationException::Type::Domain, RationalCalculationException::Operation::Logarithm, { argument, base }));
    }
    if (std::optional<Rational> rationalResult = impl::log::calculateRational(argument, base); rationalResult) {
        return *rationalResult;
    }

    if (!config.approximation.enabled) {
        throwext(RationalCalculationException(RationalCalculationException::Type::IrrationalResult, RationalCalculationException::Operation::Logarithm, { argument, base }));
    }
    // log_b(M/N) = log_b(M) - log_b(N)
    if (denominator(argument) != 1) {
        return log(numerator(argument), base, config) - log(denominator(argument), base, config);
    }
    // log_b(a*b*c*...) = log_b(a) + log_b(b) + log_b(c) + ...
    // We use prime factorization to increase calculation speed (probably).
    const Rational& lnBase = ln(base);
    return std::ranges::fold_left(primeFactors(numerator(argument)), Rational(),
                                  [&](const auto& acc, const auto& factor) {
                                      return acc + ln(factor, config) / lnBase;
                                  });
}
Rational lg(const Rational& argument, const RationalCalculationOptions& config)
{
    return log(argument, 10, config);
}

RationalCalculationException::RationalCalculationException(Type type, Operation operation, std::vector<Rational> operands)
    : type(type),
      operation(operation),
      operands(std::move(operands))
{ }
const char* RationalCalculationException::what() const noexcept
{
    if (message_.empty()) {
        switch (this->type) {
        case Type::Unspecified:
            message_ = std::format("Something went wrong while rational calculating: {}", this->stringifyOperation());
            break;
        case Type::Domain:
            message_ = std::format("The result has no definition in real domain: {}", this->stringifyOperation());
            break;
        case Type::Pole:
            message_ = std::format("The result tends to infinity: {}", this->stringifyOperation());
            break;
        case Type::IndeterminateForm:
            message_ = std::format("The result has indeterminate form: {}", this->stringifyOperation());
            break;
        case Type::IrrationalResult:
            message_ = std::format("The result is irrational but approximation is disabled in ApproximationOptions: {}", this->stringifyOperation());
            break;
        }
    }
    return message_.c_str();
}
std::string RationalCalculationException::stringifyOperation() const noexcept
{
    switch (this->operation) {
    case Operation::Power:
        return std::format("{:P} ^ {:P}", this->operands[0], this->operands[1]);
    case Operation::Factorial:
        return std::format("factorial({:P})", this->operands[0]);
    case Operation::Root:
        return std::format("root(radicand={:P}, index={:P})", this->operands[0], this->operands[1]);
    case Operation::Sine:
        return std::format("sin(radian={:P})", this->operands[0]);
    case Operation::Cosine:
        return std::format("cos(radian={:P})", this->operands[0]);
    case Operation::Tangent:
        return std::format("tan(radian={:P})", this->operands[0]);
    case Operation::Cotangent:
        return std::format("cot(radian={:P})", this->operands[0]);
    case Operation::Secant:
        return std::format("sec(radian={:P})", this->operands[0]);
    case Operation::Cosecant:
        return std::format("csc(radian={:P})", this->operands[0]);
    case Operation::Arcsine:
        return std::format("arcsin(radian={:P})", this->operands[0]);
    case Operation::Arccosine:
        return std::format("arccos(radian={:P})", this->operands[0]);
    case Operation::Arctangent:
        return std::format("arctan(radian={:P})", this->operands[0]);
    case Operation::Arccotangent:
        return std::format("arccot(radian={:P})", this->operands[0]);
    case Operation::Arcsecant:
        return std::format("arcsec(radian={:P})", this->operands[0]);
    case Operation::Arccosecant:
        return std::format("arccsc(radian={:P})", this->operands[0]);
    case Operation::NaturalLogarithm:
        return std::format("ln({:P})", this->operands[0]);
    case Operation::Logarithm:
        return std::format("log(argument={:P}, base={:P})", this->operands[0], this->operands[1]);
    case Operation::Unspecified:
        return "unspecified operation";
    }
}
} // namespace thecalculater::math