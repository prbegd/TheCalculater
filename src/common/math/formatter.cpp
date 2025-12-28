/**
 * @file formatter.cpp
 * @author prbegd
 * @date 2025-12-20
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
#include "TheCalculater/math/formatter.hpp"
#include "TheCalculater/math/analytic_expression.hpp"
#include "TheCalculater/math/fraction.hpp"
#include "TheCalculater/math/prime_factorization.hpp"
#include <algorithm>
#include <cstddef>
#include <sstream>
#include <unordered_map>

namespace TheCalculater::math {
    namespace { namespace _d_format::fraction {
        std::string fractionString(const Fraction& frac, const FormatType type)
        {
            switch (type) {
            case FormatType::LaTeX:
                return "\\frac{" + frac.numerator().str() + "}{" + frac.denominator().str() + "}";
            case FormatType::PlainText:
                return frac.numerator().str() + "/" + frac.denominator().str();
            }
        }
        bool isRepeatedDecimal(const Fraction& frac)
        {
            // Check if the denominator contains any prime factor other than 2 and 5.
            auto denoPrimeFactors = primeFactorization(frac.denominator());
            return std::any_of(denoPrimeFactors.begin(), denoPrimeFactors.end(), [](const auto& n) {
                return n != 2 && n != 5;
            });
        }
        std::string fractionWhenRepeatedDecimal(const Fraction& frac, const FormatType type)
        {
            boost::multiprecision::cpp_int numerator = frac.numerator();
            const boost::multiprecision::cpp_int& denominator = frac.denominator();
            if (denominator == 1)
                return numerator.str();
            if (isRepeatedDecimal(frac))
                return fractionString(frac, type);
            std::ostringstream result;

            if (numerator < 0) {
                result << '-';
                numerator = -numerator;
            }
            boost::multiprecision::cpp_int integerPart = numerator / denominator;
            boost::multiprecision::cpp_int remainder = numerator % denominator;
            result << integerPart.str() << '.';

            while (remainder != 0) {
                boost::multiprecision::cpp_int quotient = remainder * 10 / denominator;
                result << quotient.str();

                remainder = (remainder * 10) % denominator;
            }

            return result.str();
        }

        std::tuple<std::function<void()>, std::function<void()>> getRepeatingMarker(std::ostringstream& output, FormatType type)
        {
            switch (type) {
            case FormatType::LaTeX:
                return { [&output] { output << "\\overline{"; }, [&output] { output << "}"; } };
            case FormatType::PlainText:
                return { [&output] { output << "{"; }, [&output] { output << "}"; } };
            }
        }

        std::string alwaysDecimal(const Fraction& frac, FormatType type)
        {
            boost::multiprecision::cpp_int numerator = frac.numerator();
            const boost::multiprecision::cpp_int& denominator = frac.denominator();
            if (denominator == 1)
                return numerator.str();
            std::ostringstream result;

            const auto [markRepeatingBegin, markRepeatingEnd] = getRepeatingMarker(result, type);

            if (numerator < 0) {
                result << '-';
                numerator = -numerator;
            }
            boost::multiprecision::cpp_int integerPart = numerator / denominator;
            boost::multiprecision::cpp_int remainder = numerator % denominator;
            result << integerPart.str() << '.';

            std::unordered_map<boost::multiprecision::cpp_int, size_t> remainderPositions;
            std::string decimalPart;
            size_t position = 0;

            while (remainder != 0) {
                auto it = remainderPositions.find(remainder);
                if (it != remainderPositions.end()) {
                    size_t cycle_start = it->second;

                    std::string non_repeating = decimalPart.substr(0, cycle_start);
                    std::string repeating = decimalPart.substr(cycle_start);

                    result << non_repeating;
                    markRepeatingBegin();
                    result << repeating;
                    markRepeatingEnd();

                    return result.str();
                }

                remainderPositions[remainder] = position++;

                remainder *= 10;
                boost::multiprecision::cpp_int quotient = remainder / denominator;
                remainder = remainder % denominator;

                decimalPart += quotient.str();
            }

            result << decimalPart;
            return result.str();
        }
    }} // namespace ::_d_format::fraction
    std::string format(const Fraction& frac, const FractionFormatOptions& options)
    {
        switch (options.style) {
        case FractionFormatOptions::Style::AlwaysFraction:
            return _d_format::fraction::fractionString(frac, options.type);
        case FractionFormatOptions::Style::FractionWhenDecimal: {
            if (frac.denominator() == 1)
                return frac.numerator().str();
            return _d_format::fraction::fractionString(frac, options.type);
        }
        case FractionFormatOptions::Style::FractionWhenRepeatedDecimal:
            return _d_format::fraction::fractionWhenRepeatedDecimal(frac, options.type);
        case FractionFormatOptions::Style::AlwaysDecimal:
            return _d_format::fraction::alwaysDecimal(frac, options.type);
        }
    }

    namespace { namespace _d_format::analytic_expression {

    }}
    std::string format(const AnalyticExpression& expr)
    {
        // TODO: implement this function.
    }
    namespace { namespace _d_format_tree {
        std::string format(const AnalyticExpression::AbstractNode& node)
        {
            switch (node.type()) {
            case AnalyticExpression::NodeType::Undefined:
                return "Undefined";
            case AnalyticExpression::NodeType::Infinity:
                return "Infinity";
            case AnalyticExpression::NodeType::Constant:
                return math::format(static_cast<const AnalyticExpression::Constant&>(node).value, { .type = FormatType::PlainText });
            case AnalyticExpression::NodeType::Pi:
                return "Pi";
            case AnalyticExpression::NodeType::Euler:
                return "e";
            case AnalyticExpression::NodeType::Variable:
                return static_cast<const AnalyticExpression::Variable&>(node).name;
            case AnalyticExpression::NodeType::Addition: {
                const auto& add = static_cast<const AnalyticExpression::Addition&>(node);
                return std::format("(+ {} {})", format(add.firstOperand()), format(add.secondOperand()));
            }
            case AnalyticExpression::NodeType::Subtraction: {
                const auto& sub = static_cast<const AnalyticExpression::Subtraction&>(node);
                return std::format("(- {} {})", format(sub.firstOperand()), format(sub.secondOperand()));
            }
            case AnalyticExpression::NodeType::Multiplication: {
                const auto& mul = static_cast<const AnalyticExpression::Multiplication&>(node);
                return std::format("(* {} {})", format(mul.firstOperand()), format(mul.secondOperand()));
            }
            case AnalyticExpression::NodeType::Division: {
                const auto& div = static_cast<const AnalyticExpression::Division&>(node);
                return std::format("(/ {} {})", format(div.firstOperand()), format(div.secondOperand()));
            }
            case AnalyticExpression::NodeType::Negation: {
                const auto& neg = static_cast<const AnalyticExpression::Negation&>(node);
                return std::format("(- {})", format(neg.firstOperand()));
            }
            case AnalyticExpression::NodeType::Affirmation: {
                const auto& aff = static_cast<const AnalyticExpression::Affirmation&>(node);
                return std::format("(+ {})", format(aff.firstOperand()));
            }
            case AnalyticExpression::NodeType::Power: {
                const auto& pow = static_cast<const AnalyticExpression::Power&>(node);
                return std::format("(^ {} {})", format(pow.firstOperand()), format(pow.secondOperand()));
            }
            // TODO: Uncomment the following lines when implementing them.
            // case AnalyticExpression::NodeType::Root: {
            //     const auto& root = static_cast<const AnalyticExpression::Root&>(node);
            //     return std::format("(√ {} {})", format(root.firstOperand()), format(root.secondOperand()));
            // }
            // case AnalyticExpression::NodeType::Factorial: {
            //     const auto& fac = static_cast<const AnalyticExpression::Factorial&>(node);
            //     return std::format("(! {})", format(fac.firstOperand()));
            // }
            // case AnalyticExpression::NodeType::AbsoluteValue: {
            //     const auto& abs = static_cast<const AnalyticExpression::AbsoluteValue&>(node);
            //     return std::format("(| {})", format(abs.firstOperand()));
            // }
            // case AnalyticExpression::NodeType::Modulus: {
            //     const auto& mod = static_cast<const AnalyticExpression::Modulus&>(node);
            //     return std::format("(mod {} {})", format(mod.firstOperand()), format(mod.secondOperand()));
            // }
            // case AnalyticExpression::NodeType::Logarithm: {
            //     const auto& log = static_cast<const AnalyticExpression::Logarithm&>(node);
            //     return std::format("(log {} {})", format(log.firstOperand()), format(log.secondOperand()));
            // }
            // case AnalyticExpression::NodeType::NaturalLogarithm: {
            //     const auto& nlog = static_cast<const AnalyticExpression::NaturalLogarithm&>(node);
            //     return std::format("(ln {})", format(nlog.firstOperand()));
            // }
            // case AnalyticExpression::NodeType::Sine: {
            //     const auto& sin = static_cast<const AnalyticExpression::Sine&>(node);
            //     return std::format("(sin {})", format(sin.firstOperand()));
            // }
            // case AnalyticExpression::NodeType::Cosine: {
            //     const auto& cos = static_cast<const AnalyticExpression::Cosine&>(node);
            //     return std::format("(cos {})", format(cos.firstOperand()));
            // }
            // case AnalyticExpression::NodeType::Tangent: {
            //     const auto& tan = static_cast<const AnalyticExpression::Tangent&>(node);
            //     return std::format("(tan {})", format(tan.firstOperand()));
            // }
            // case AnalyticExpression::NodeType::Arcsine: {
            //     const auto& asin = static_cast<const AnalyticExpression::Arcsine&>(node);
            //     return std::format("(arcsin {})", format(asin.firstOperand()));
            // }
            // case AnalyticExpression::NodeType::Arccosine: {
            //     const auto& acos = static_cast<const AnalyticExpression::Arccosine&>(node);
            //     return std::format("(arccos {})", format(acos.firstOperand()));
            // }
            // case AnalyticExpression::NodeType::Arctangent: {
            //     const auto& atan = static_cast<const AnalyticExpression::Arctangent&>(node);
            //     return std::format("(arctan {})", format(atan.firstOperand()));
            // }
            case AnalyticExpression::NodeType::ImaginaryUnit:
                return "i";
            default:
                return "?";
            }
        }
    }} // namespace ::_d_format_tree
    std::string formatTree(const AnalyticExpression& expr)
    {
        return _d_format_tree::format(*expr.root);
    }
} // namespace TheCalculater::math