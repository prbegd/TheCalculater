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
#include "thecalculater/macros.hpp"

export module prbegd.thecalculater.math:rational;
import std;
import thirdparty.core;
import prbegd.thecalculater.util;
import :basis;

namespace thecalculater::math {
export using Integer = boost::multiprecision::mpz_int;
export using Rational = boost::multiprecision::mpq_rational;

/**
 * @note Handles some edge cases compared to directly constructing Integer objects.
 */
export TCAPI Integer makeInteger(std::string_view str);

/**
 * @attention Use this function instead of directly constructing Rational objects: this function accepts variant input formats. 
 * 
 * @details
 * - Accepts prefix and suffix white spaces, but not in between.
 * - Invoking this function with the result from @ref thecalculater::math::format(const thecalculater::math::Rational&, const thecalculater::math::RationalFormatOptions&) is always valid.
 *
 * @throw thecalculater::math::InvalidRationalParseException<std::string If `str` is not formatted as a valid rational number.
 * @throw std::overflow_error If the rational number represented by `str` has zero denominator.
 * */
export TCAPI Rational makeRational(std::string_view str);

/**
 * @throw thecalculater::math::InvalidRationalParseException<double> If `value` is not finite.
 */
export TCAPI Rational makeRational(double value);

export struct RationalFormatOptions {
    ExpressionOutputFormat output = ExpressionOutputFormat::LaTeX;
    RationalNumericFormat numeric = RationalNumericFormat::PreferDecimal;
};

/**
 * @details
 * - All rational numbers are valid.
 */
export TCAPI std::string format(const Rational& x, const RationalFormatOptions& options = { });

export struct TCAPI RationalCalculationOptions {
    ApproximationOptions<Rational> approximation = {
        .tolerance = Rational(1, 10000000000000000),
        .constants = {
            .pi = Rational(7853981633974483, 2500000000000000), .euler = Rational(6795704571147613, 2500000000000000), .gamma = Rational(5772156649015329, 10000000000000000), .phi = Rational(4045084971874737, 2500000000000000) }
    };
};

/**
 * @throw std::overflow_error If `x` is 0.
 */
export TCAPI Rational reciprocal(const Rational& x);

// TODO(P0): following approximation should round to the specified tolerance, otherwise the precision will grow to hell and blow up your computer!!
/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IndeterminateForm) When both `base` and `exponent` are 0.
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Domain) When `base` is negative and the denominator of `exponent` is even.
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Pole) When `base` is 0 and `exponent` is negative.
 */
export TCAPI Rational pow(Rational base,
                          const Rational& exponent,
                          const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Domain) When `index` is 0; or when `radicand` is negative and `index` is even.
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Pole) When `radicand` is 0 and `index` is negative.
 */
export TCAPI Rational root(const Rational& radicand,
                           const Rational& index,
                           const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Domain) When `radicand` is negative.
 */
export TCAPI Rational sqrt(const Rational& radicand,
                           const RationalCalculationOptions& config = { });

export TCAPI Rational cbrt(const Rational& radicand,
                           const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Pole) When `x` is negative.
 */
export TCAPI Integer factorial(const Integer& x);

/**
 * @throw std::overflow_error If `divisor` is 0.
 */
export TCAPI Rational mod(const Rational& dividend, const Rational& divisor);

/**
 * @throw std::overflow_error If `divisor` is 0.
 */
export TCAPI Rational operator%(const Rational& dividend, const Rational& divisor);

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 */
export TCAPI Rational sin(const Rational& rad,
                          const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 */
export TCAPI Rational cos(const Rational& rad,
                          const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Pole) When `rad` is pi/2 + k*pi, where k is an integer.
 */
export TCAPI Rational tan(const Rational& rad,
                          const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Pole) When `rad` is k*pi, where k is an integer.
 */
export TCAPI Rational cot(const Rational& rad,
                          const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Pole) When `rad` is pi/2 + k*pi, where k is an integer.
 */
export TCAPI Rational sec(const Rational& rad,
                          const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Pole) When `rad` is k*pi, where k is an integer.
 */
export TCAPI Rational csc(const Rational& rad,
                          const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Domain) When `rad` is not in the range [-1, 1].
 */
export TCAPI Rational arcsin(const Rational& rad,
                             const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Domain) When `rad` is not in the range [-1, 1].
 */
export TCAPI Rational arccos(const Rational& rad,
                             const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 */
export TCAPI Rational arctan(const Rational& rad,
                             const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 */
export TCAPI Rational arccot(const Rational& rad,
                             const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Domain) When `rad` is in the range (-1, 1).
 */
export TCAPI Rational arcsec(const Rational& rad,
                             const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Domain) When `rad` is in the range (-1, 1).
 */
export TCAPI Rational arccsc(const Rational& rad,
                             const RationalCalculationOptions& config = { });

export TCAPI Integer floor(const Rational& x);

export TCAPI Integer ceil(const Rational& x);

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Pole) When `argument` is 0.
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Domain) When `argument` is negative.
 */
export TCAPI Rational ln(const Rational& argument,
                         const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IndeterminateForm) When `base` is 1.
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Domain) When `base` is negative; or when `argument` is negative.
 */
export TCAPI Rational log(const Rational& argument,
                          const Rational& base,
                          const RationalCalculationOptions& config = { });

/**
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::IrrationalResult) When the result is irrational and and `config.approximation.enabled` is set to `false`
 * @throw thecalculater::math::RationalCalculationException (with @ref thecalculater::math::RationalCalculationException::Type::Domain) When `argument` is negative.
 */
export TCAPI Rational lg(const Rational& argument,
                         const RationalCalculationOptions& config = { });
} // namespace thecalculater::math

export inline std::ostream& operator<<(std::ostream& os, const thecalculater::math::Rational& rational)
{
    os << thecalculater::math::format(rational);
    return os;
}
template <>
struct std::formatter<thecalculater::math::Rational> {
    thecalculater::math::RationalFormatOptions options;

    template <typename TParseContext>
    constexpr auto parse(TParseContext& ctx)
    {
        auto it = ctx.begin();
        while (it != ctx.end() && *it != '}') {
            switch (*it) {
            case 'L':
                this->options.output = thecalculater::math::ExpressionOutputFormat::LaTeX;
                break;
            case 'P':
                this->options.output = thecalculater::math::ExpressionOutputFormat::PlainText;
                break;
            case 'f':
                this->options.numeric = thecalculater::math::RationalNumericFormat::AlwaysFraction;
                break;
            case 'i':
                this->options.numeric = thecalculater::math::RationalNumericFormat::PreferInteger;
                break;
            case 'm':
                this->options.numeric = thecalculater::math::RationalNumericFormat::PreferDecimal;
                break;
            case 'd':
                this->options.numeric = thecalculater::math::RationalNumericFormat::AlwaysDecimal;
                break;
            default:
                thecalculater::throwext(std::format_error(std::format("Unsupported format specifier '{}' for std::format(thecalculater::math::Rational)", *it)));
            }
            it++;
        }
        return it;
    }
    template <typename TOut, typename TCharT>
    auto format(const thecalculater::math::Rational& rational, basic_format_context<TOut, TCharT>& ctx) const
    {
        return std::format_to(ctx.out(), "{}", thecalculater::math::format(rational, options));
    }
};
template <>
struct std::formatter<thecalculater::math::Integer> {
    template <typename TParseContext>
    constexpr auto parse(TParseContext& ctx)
    {
        return ctx.begin();
    }
    template <typename TFmtContext>
    auto format(const thecalculater::math::Integer& value, TFmtContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}", value.str());
    }
};

namespace thecalculater::math {
export template <typename T>
class InvalidRationalParseException : public std::exception, public boost::exception { // NOLINT(misc-multiple-inheritance)
public:
    T origin;

    explicit InvalidRationalParseException(T origin)
        : origin(origin)
    { }
    [[nodiscard]]
    const char* what() const noexcept override
    {
        if (message_.empty()) {
            message_ = std::format("Try to parse an invalid value into a Rational: {}", this->origin);
        }
        return message_.c_str();
    }

private:
    mutable std::string message_;
};
export class TCAPI RationalCalculationException : public std::exception, public boost::exception { // NOLINT(misc-multiple-inheritance
public:
    enum class Type : std::uint8_t {
        Unspecified,
        Domain,
        Pole,
        IndeterminateForm,
        IrrationalResult,
    };
    enum class Operation : std::uint8_t {
        Unspecified,
        Power,
        Factorial,
        Root,
        Sine,
        Cosine,
        Tangent,
        Cotangent,
        Secant,
        Cosecant,
        Arcsine,
        Arccosine,
        Arctangent,
        Arccotangent,
        Arcsecant,
        Arccosecant,
        NaturalLogarithm,
        Logarithm,
    };

    Type type;
    Operation operation;
    std::vector<Rational> operands;

    explicit RationalCalculationException(Type type, Operation operation = Operation::Unspecified, std::vector<Rational> operands = { });

    [[nodiscard]]
    const char* what() const noexcept override;

    [[nodiscard]]
    std::string stringifyOperation() const noexcept;

protected:
    mutable std::string message_;
};
} // namespace thecalculater::math