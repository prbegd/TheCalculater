/**
 * @file analytic_expression_legacy.cpp
 * @author prbegd
 * @brief Implementation of analytic expression class and related functions.
 * @date 2025-11-12
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 *
 */
module;

module TheCalculater.math.analytic_expression_legacy;
import TheCalculater.math.fraction;
import TheCalculater.math.formatter;
import tpmm.boost;
import std;

namespace TheCalculater::math {
    AnalyticExpression::AnalyticExpression(const Node& node)
        : base(node.clone())
    { }
    AnalyticExpression::AnalyticExpression(const std::unique_ptr<Node>& node)
        : base(node->clone())
    { }
    AnalyticExpression::AnalyticExpression(std::unique_ptr<Node>&& node)
        : base(std::move(node))
    { }

    AnalyticExpression AnalyticExpression::constant(const Fraction& value)
    {
        return { std::make_unique<Constant>(value) };
    }
    AnalyticExpression AnalyticExpression::constant(Fraction&& value)
    {
        return { std::make_unique<Constant>(std::move(value)) };
    }
    AnalyticExpression AnalyticExpression::variable(std::string_view name)
    {
        return { std::make_unique<Variable>(name) };
    }
    AnalyticExpression AnalyticExpression::variable(std::string&& name)
    {
        return { std::make_unique<Variable>(std::move(name)) };
    }
    AnalyticExpression AnalyticExpression::infinity()
    {
        return { std::make_unique<Infinity>() };
    }
    AnalyticExpression AnalyticExpression::pi()
    {
        return { std::make_unique<Pi>() };
    }
    AnalyticExpression AnalyticExpression::euler()
    {
        return { std::make_unique<Euler>() };
    }
    AnalyticExpression AnalyticExpression::imaginaryUnit()
    {
        return { std::make_unique<ImaginaryUnit>() };
    }
    AnalyticExpression AnalyticExpression::undefined()
    {
        return { std::make_unique<Undefined>() };
    }
    AnalyticExpression AnalyticExpression::addition(const AnalyticExpression& left, const AnalyticExpression& right)
    {
        return { std::make_unique<Addition>(left.base, right.base) };
    }
    AnalyticExpression AnalyticExpression::addition(AnalyticExpression&& left, AnalyticExpression&& right)
    {
        return { std::make_unique<Addition>(std::move(left.base), std::move(right.base)) };
    }
    AnalyticExpression AnalyticExpression::subtraction(const AnalyticExpression& left, const AnalyticExpression& right)
    {
        return { std::make_unique<Subtraction>(left.base, right.base) };
    }
    AnalyticExpression AnalyticExpression::subtraction(AnalyticExpression&& left, AnalyticExpression&& right)
    {
        return { std::make_unique<Subtraction>(std::move(left.base), std::move(right.base)) };
    }
    AnalyticExpression AnalyticExpression::multiplication(const AnalyticExpression& left, const AnalyticExpression& right)
    {
        return { std::make_unique<Multiplication>(left.base, right.base) };
    }
    AnalyticExpression AnalyticExpression::multiplication(AnalyticExpression&& left, AnalyticExpression&& right)
    {
        return { std::make_unique<Multiplication>(std::move(left.base), std::move(right.base)) };
    }
    AnalyticExpression AnalyticExpression::division(const AnalyticExpression& numerator, const AnalyticExpression& denominator)
    {
        return { std::make_unique<Division>(numerator.base, denominator.base) };
    }
    AnalyticExpression AnalyticExpression::division(AnalyticExpression&& numerator, AnalyticExpression&& denominator)
    {
        return { std::make_unique<Division>(std::move(numerator.base), std::move(denominator.base)) };
    }
    AnalyticExpression AnalyticExpression::negation(const AnalyticExpression& operand)
    {
        return { std::make_unique<Negation>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::negation(AnalyticExpression&& operand)
    {
        return { std::make_unique<Negation>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::affirmation(const AnalyticExpression& operand)
    {
        return { std::make_unique<Affirmation>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::affirmation(AnalyticExpression&& operand)
    {
        return { std::make_unique<Affirmation>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::power(const AnalyticExpression& base, const AnalyticExpression& exponent)
    {
        return { std::make_unique<Power>(base.base, exponent.base) };
    }
    AnalyticExpression AnalyticExpression::power(AnalyticExpression&& base, AnalyticExpression&& exponent)
    {
        return { std::make_unique<Power>(std::move(base.base), std::move(exponent.base)) };
    }
    AnalyticExpression AnalyticExpression::root(const AnalyticExpression& radicand, const AnalyticExpression& degree)
    {
        return { std::make_unique<Root>(radicand.base, degree.base) };
    }
    AnalyticExpression AnalyticExpression::root(AnalyticExpression&& radicand, AnalyticExpression&& degree)
    {
        return { std::make_unique<Root>(std::move(radicand.base), std::move(degree.base)) };
    }
    AnalyticExpression AnalyticExpression::factorial(const AnalyticExpression& operand)
    {
        return { std::make_unique<Factorial>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::factorial(AnalyticExpression&& operand)
    {
        return { std::make_unique<Factorial>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::absoluteValue(const AnalyticExpression& operand)
    {
        return { std::make_unique<AbsoluteValue>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::absoluteValue(AnalyticExpression&& operand)
    {
        return { std::make_unique<AbsoluteValue>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::modulus(const AnalyticExpression& dividend, const AnalyticExpression& divisor)
    {
        return { std::make_unique<Modulus>(dividend.base, divisor.base) };
    }
    AnalyticExpression AnalyticExpression::modulus(AnalyticExpression&& dividend, AnalyticExpression&& divisor)
    {
        return { std::make_unique<Modulus>(std::move(dividend.base), std::move(divisor.base)) };
    }
    AnalyticExpression AnalyticExpression::logarithm(const AnalyticExpression& base, const AnalyticExpression& operand)
    {
        return { std::make_unique<Logarithm>(base.base, operand.base) };
    }
    AnalyticExpression AnalyticExpression::logarithm(AnalyticExpression&& base, AnalyticExpression&& operand)
    {
        return { std::make_unique<Logarithm>(std::move(base.base), std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::naturalLogarithm(const AnalyticExpression& operand)
    {
        return { std::make_unique<NaturalLogarithm>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::naturalLogarithm(AnalyticExpression&& operand)
    {
        return { std::make_unique<NaturalLogarithm>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::degree(const AnalyticExpression& operand)
    {
        return { std::make_unique<Degree>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::degree(AnalyticExpression&& operand)
    {
        return { std::make_unique<Degree>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::sine(const AnalyticExpression& operand)
    {
        return { std::make_unique<Sine>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::sine(AnalyticExpression&& operand)
    {
        return { std::make_unique<Sine>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::cosine(const AnalyticExpression& operand)
    {
        return { std::make_unique<Cosine>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::cosine(AnalyticExpression&& operand)
    {
        return { std::make_unique<Cosine>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::tangent(const AnalyticExpression& operand)
    {
        return { std::make_unique<Tangent>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::tangent(AnalyticExpression&& operand)
    {
        return { std::make_unique<Tangent>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::cotangent(const AnalyticExpression& operand)
    {
        return { std::make_unique<Cotangent>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::cotangent(AnalyticExpression&& operand)
    {
        return { std::make_unique<Cotangent>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::secant(const AnalyticExpression& operand)
    {
        return { std::make_unique<Secant>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::secant(AnalyticExpression&& operand)
    {
        return { std::make_unique<Secant>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::cosecant(const AnalyticExpression& operand)
    {
        return { std::make_unique<Cosecant>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::cosecant(AnalyticExpression&& operand)
    {
        return { std::make_unique<Cosecant>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::arcsine(const AnalyticExpression& operand)
    {
        return { std::make_unique<Arcsine>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::arcsine(AnalyticExpression&& operand)
    {
        return { std::make_unique<Arcsine>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::arccosine(const AnalyticExpression& operand)
    {
        return { std::make_unique<Arccosine>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::arccosine(AnalyticExpression&& operand)
    {
        return { std::make_unique<Arccosine>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::arctangent(const AnalyticExpression& operand)
    {
        return { std::make_unique<Arctangent>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::arctangent(AnalyticExpression&& operand)
    {
        return { std::make_unique<Arctangent>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::arccotangent(const AnalyticExpression& operand)
    {
        return { std::make_unique<Arccotangent>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::arccotangent(AnalyticExpression&& operand)
    {
        return { std::make_unique<Arccotangent>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::arcsecant(const AnalyticExpression& operand)
    {
        return { std::make_unique<Arcsecant>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::arcsecant(AnalyticExpression&& operand)
    {
        return { std::make_unique<Arcsecant>(std::move(operand.base)) };
    }
    AnalyticExpression AnalyticExpression::arccosecant(const AnalyticExpression& operand)
    {
        return { std::make_unique<Arccosecant>(operand.base) };
    }
    AnalyticExpression AnalyticExpression::arccosecant(AnalyticExpression&& operand)
    {
        return { std::make_unique<Arccosecant>(std::move(operand.base)) };
    }

    namespace { namespace _d_format::analytic_expression {

    }}
    std::string format(const AnalyticExpression&)
    {
        // TODO: implement this function.
        return { };
    }
    namespace { namespace _d_format_tree {
        std::string format(const AnalyticExpression::Node& node)
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
            case AnalyticExpression::NodeType::Root: {
                const auto& root = static_cast<const AnalyticExpression::Root&>(node);
                return std::format("(rt {} {})", format(root.firstOperand()), format(root.secondOperand()));
            }
            case AnalyticExpression::NodeType::Factorial: {
                const auto& fac = static_cast<const AnalyticExpression::Factorial&>(node);
                return std::format("(! {})", format(fac.firstOperand()));
            }
            case AnalyticExpression::NodeType::AbsoluteValue: {
                const auto& abs = static_cast<const AnalyticExpression::AbsoluteValue&>(node);
                return std::format("(abs {})", format(abs.firstOperand()));
            }
            case AnalyticExpression::NodeType::Modulus: {
                const auto& mod = static_cast<const AnalyticExpression::Modulus&>(node);
                return std::format("(mod {} {})", format(mod.firstOperand()), format(mod.secondOperand()));
            }
            case AnalyticExpression::NodeType::Logarithm: {
                const auto& log = static_cast<const AnalyticExpression::Logarithm&>(node);
                return std::format("(log {} {})", format(log.firstOperand()), format(log.secondOperand()));
            }
            case AnalyticExpression::NodeType::NaturalLogarithm: {
                const auto& nlog = static_cast<const AnalyticExpression::NaturalLogarithm&>(node);
                return std::format("(ln {})", format(nlog.firstOperand()));
            }
            case AnalyticExpression::NodeType::Degree: {
                const auto& deg = static_cast<const AnalyticExpression::Degree&>(node);
                return std::format("(deg {})", format(deg.firstOperand()));
            }
            case AnalyticExpression::NodeType::Sine: {
                const auto& sin = static_cast<const AnalyticExpression::Sine&>(node);
                return std::format("(sin {})", format(sin.firstOperand()));
            }
            case AnalyticExpression::NodeType::Cosine: {
                const auto& cos = static_cast<const AnalyticExpression::Cosine&>(node);
                return std::format("(cos {})", format(cos.firstOperand()));
            }
            case AnalyticExpression::NodeType::Tangent: {
                const auto& tan = static_cast<const AnalyticExpression::Tangent&>(node);
                return std::format("(tan {})", format(tan.firstOperand()));
            }
            case AnalyticExpression::NodeType::Cotangent: {
                const auto& cot = static_cast<const AnalyticExpression::Cotangent&>(node);
                return std::format("(cot {})", format(cot.firstOperand()));
            }
            case AnalyticExpression::NodeType::Secant: {
                const auto& sec = static_cast<const AnalyticExpression::Secant&>(node);
                return std::format("(sec {})", format(sec.firstOperand()));
            }
            case AnalyticExpression::NodeType::Cosecant: {
                const auto& csc = static_cast<const AnalyticExpression::Cosecant&>(node);
                return std::format("(csc {})", format(csc.firstOperand()));
            }
            case AnalyticExpression::NodeType::Arcsine: {
                const auto& asin = static_cast<const AnalyticExpression::Arcsine&>(node);
                return std::format("(arcsin {})", format(asin.firstOperand()));
            }
            case AnalyticExpression::NodeType::Arccosine: {
                const auto& acos = static_cast<const AnalyticExpression::Arccosine&>(node);
                return std::format("(arccos {})", format(acos.firstOperand()));
            }
            case AnalyticExpression::NodeType::Arctangent: {
                const auto& atan = static_cast<const AnalyticExpression::Arctangent&>(node);
                return std::format("(arctan {})", format(atan.firstOperand()));
            }
            case AnalyticExpression::NodeType::Arccotangent: {
                const auto& acot = static_cast<const AnalyticExpression::Arccotangent&>(node);
                return std::format("(arccot {})", format(acot.firstOperand()));
            }
            case AnalyticExpression::NodeType::Arcsecant: {
                const auto& asec = static_cast<const AnalyticExpression::Arcsecant&>(node);
                return std::format("(arcsec {})", format(asec.firstOperand()));
            }
            case AnalyticExpression::NodeType::Arccosecant: {
                const auto& acsc = static_cast<const AnalyticExpression::Arccosecant&>(node);
                return std::format("(arccsc {})", format(acsc.firstOperand()));
            }
            case AnalyticExpression::NodeType::ImaginaryUnit:
                return "i";
            default:
                return "?";
            }
        }
    }} // namespace ::_d_format_tree
    std::string formatTree(const AnalyticExpression& expr)
    {
        return _d_format_tree::format(*expr.base);
    }

    AnalyticExpression operator+(const AnalyticExpression& left, const AnalyticExpression& right)
    {
        return AnalyticExpression::addition(left, right);
    }
    AnalyticExpression operator+(AnalyticExpression&& left, AnalyticExpression&& right)
    {
        return AnalyticExpression::addition(std::move(left), std::move(right));
    }
    AnalyticExpression operator-(const AnalyticExpression& left, const AnalyticExpression& right)
    {
        return AnalyticExpression::subtraction(left, right);
    }
    AnalyticExpression operator-(AnalyticExpression&& left, AnalyticExpression&& right)
    {
        return AnalyticExpression::subtraction(std::move(left), std::move(right));
    }
    AnalyticExpression operator*(const AnalyticExpression& left, const AnalyticExpression& right)
    {
        return AnalyticExpression::multiplication(left, right);
    }
    AnalyticExpression operator*(AnalyticExpression&& left, AnalyticExpression&& right)
    {
        return AnalyticExpression::multiplication(std::move(left), std::move(right));
    }
    AnalyticExpression operator/(const AnalyticExpression& numerator, const AnalyticExpression& denominator)
    {
        return AnalyticExpression::division(numerator, denominator);
    }
    AnalyticExpression operator/(AnalyticExpression&& numerator, AnalyticExpression&& denominator)
    {
        return AnalyticExpression::division(std::move(numerator), std::move(denominator));
    }
    AnalyticExpression operator-(const AnalyticExpression& operand)
    {
        return AnalyticExpression::negation(operand);
    }
    AnalyticExpression operator-(AnalyticExpression&& operand)
    {
        return AnalyticExpression::negation(std::move(operand));
    }
    AnalyticExpression operator+(const AnalyticExpression& operand)
    {
        return AnalyticExpression::affirmation(operand);
    }
    AnalyticExpression operator+(AnalyticExpression&& operand)
    {
        return AnalyticExpression::affirmation(std::move(operand));
    }
    AnalyticExpression operator%(const AnalyticExpression& left, const AnalyticExpression& right)
    {
        return AnalyticExpression::modulus(left, right);
    }
    AnalyticExpression operator%(AnalyticExpression&& left, AnalyticExpression&& right)
    {
        return AnalyticExpression::modulus(std::move(left), std::move(right));
    }

    AnalyticExpression& operator+=(AnalyticExpression& left, const AnalyticExpression& right)
    {
        left = AnalyticExpression::addition(left, right);
        return left;
    }
    AnalyticExpression& operator+=(AnalyticExpression& left, AnalyticExpression&& right)
    {
        left = AnalyticExpression::addition(std::move(left), std::move(right));
        return left;
    }
    AnalyticExpression& operator-=(AnalyticExpression& left, const AnalyticExpression& right)
    {
        left = AnalyticExpression::subtraction(left, right);
        return left;
    }
    AnalyticExpression& operator-=(AnalyticExpression& left, AnalyticExpression&& right)
    {
        left = AnalyticExpression::subtraction(std::move(left), std::move(right));
        return left;
    }
    AnalyticExpression& operator*=(AnalyticExpression& left, const AnalyticExpression& right)
    {
        left = AnalyticExpression::multiplication(left, right);
        return left;
    }
    AnalyticExpression& operator*=(AnalyticExpression& left, AnalyticExpression&& right)
    {
        left = AnalyticExpression::multiplication(std::move(left), std::move(right));
        return left;
    }
    AnalyticExpression& operator/=(AnalyticExpression& left, const AnalyticExpression& right)
    {
        left = AnalyticExpression::division(left, right);
        return left;
    }
    AnalyticExpression& operator/=(AnalyticExpression& left, AnalyticExpression&& right)
    {
        left = AnalyticExpression::division(std::move(left), std::move(right));
        return left;
    }
    AnalyticExpression& operator%=(AnalyticExpression& left, const AnalyticExpression& right)
    {
        left = AnalyticExpression::modulus(left, right);
        return left;
    }
    AnalyticExpression& operator%=(AnalyticExpression& left, AnalyticExpression&& right)
    {
        left = AnalyticExpression::modulus(std::move(left), std::move(right));
        return left;
    }

    const AnalyticExpression::Constant AnalyticExpression::Constant::ZERO(0);
    const AnalyticExpression::Constant AnalyticExpression::Constant::ONE(1);
    namespace {
        [[nodiscard]] int sortingCompare(const AnalyticExpression::Node& a, const AnalyticExpression::Node& b) // NOLINT
        {
            if (a.type() != b.type()) {
                return static_cast<int>(a.type()) - static_cast<int>(b.type());
            }

            auto aHash = a.hash();
            auto bHash = b.hash();
            if (aHash < bHash)
                return -1;
            else if (aHash > bHash)
                return 1;
            else
                return 0;
        }
    } // namespace

    [[nodiscard]] std::size_t AnalyticExpression::Constant::hash() const
    {
        std::size_t seed = 0x3361e811604a8be7; // Hash of 'TheCalculater::math::AnalyticExpression::Constant'
        boost::hash_combine(seed, value);
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Variable::hash() const
    {
        std::size_t seed = 0xe60cbdcfe41d881a; // Hash of 'TheCalculater::math::AnalyticExpression::Variable
        boost::hash_combine(seed, name);
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Addition::hash() const
    {
        std::size_t seed = 0x26b57e0cad6d1c3; // Hash of 'TheCalculater::math::AnalyticExpression::Addition'
        boost::hash_combine(seed, left->hash());
        boost::hash_combine(seed, right->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Subtraction::hash() const
    {
        std::size_t seed = 0xf1563e6038e40c35; // Hash of 'TheCalculater::math::AnalyticExpression::Subtraction'
        boost::hash_combine(seed, left->hash());
        boost::hash_combine(seed, right->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Multiplication::hash() const
    {
        std::size_t seed = 0x95d1ec6364d57dc8; // Hash of 'TheCalculater::math::AnalyticExpression::Multiplication'
        boost::hash_combine(seed, left->hash());
        boost::hash_combine(seed, right->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Division::hash() const
    {
        std::size_t seed = 0x2d76e0229be33792; // Hash of 'TheCalculater::math::AnalyticExpression::Division'
        boost::hash_combine(seed, numerator->hash());
        boost::hash_combine(seed, denominator->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Negation::hash() const
    {
        std::size_t seed = 0xe09ccb459549b2d; // Hash of 'TheCalculater::math::AnalyticExpression::Negation'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Affirmation::hash() const
    {
        std::size_t seed = 0xcbf06d47db3ef7bd; // Hash of 'TheCalculater::math::AnalyticExpression::Affirmation'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Power::hash() const
    {
        std::size_t seed = 0xf709b05f78a07dcb; // Hash of 'TheCalculater::math::AnalyticExpression::Power'
        boost::hash_combine(seed, base->hash());
        boost::hash_combine(seed, exponent->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Root::hash() const
    {
        std::size_t seed = 0xf0070dea6c9ea427; // Hash of 'TheCalculater::math::AnalyticExpression::Root'
        boost::hash_combine(seed, radicand->hash());
        boost::hash_combine(seed, index->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Factorial::hash() const
    {
        std::size_t seed = 0x724892ddfc402102; // Hash of 'TheCalculater::math::AnalyticExpression::Factorial'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::AbsoluteValue::hash() const
    {
        std::size_t seed = 0xb6520fc18810bef7; // Hash of 'TheCalculater::math::AnalyticExpression::AbsoluteValue'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Modulus::hash() const
    {
        std::size_t seed = 0x46e0bc3ac0eb3723; // Hash of 'TheCalculater::math::AnalyticExpression::Modulus'
        boost::hash_combine(seed, dividend->hash());
        boost::hash_combine(seed, divisor->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Logarithm::hash() const
    {
        std::size_t seed = 0xfa76de7ccdb3659d; // Hash of 'TheCalculater::math::AnalyticExpression::Logarithm'
        boost::hash_combine(seed, base->hash());
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::NaturalLogarithm::hash() const
    {
        std::size_t seed = 0xffb7367750971651; // Hash of 'TheCalculater::math::AnalyticExpression::NaturalLogarithm'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Degree::hash() const
    {
        std::size_t seed = 0x4352923209ab86d4; // Hash of 'TheCalculater::math::AnalyticExpression::Degree'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Sine::hash() const
    {
        std::size_t seed = 0x682422b47671e928; // Hash of 'TheCalculater::math::AnalyticExpression::Sine'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Cosine::hash() const
    {
        std::size_t seed = 0x3bfcfb15956054ad; // Hash of 'TheCalculater::math::AnalyticExpression::Cosine'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Tangent::hash() const
    {
        std::size_t seed = 0x6da758eca579e7ae; // Hash of 'TheCalculater::math::AnalyticExpression::Tangent'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Cotangent::hash() const
    {
        std::size_t seed = 0x3078937dde9c0317; // Hash of 'TheCalculater::math::AnalyticExpression::Cotangent'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Secant::hash() const
    {
        std::size_t seed = 0xa91022ca62cbcccb; // Hash of 'TheCalculater::math::AnalyticExpression::Secant'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Cosecant::hash() const
    {
        std::size_t seed = 0x79c10674bec54b9d; // Hash of 'TheCalculater::math::AnalyticExpression::Cosecant'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Arcsine::hash() const
    {
        std::size_t seed = 0x9c05a7b5a7b29fca; // Hash of 'TheCalculater::math::AnalyticExpression::Arcsine'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Arccosine::hash() const
    {
        std::size_t seed = 0x27e3b1fde1e5166e; // Hash of 'TheCalculater::math::AnalyticExpression::Arccosine'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Arctangent::hash() const
    {
        std::size_t seed = 0xe467b7f655c81cc8; // Hash of 'TheCalculater::math::AnalyticExpression::Arctangent'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Arccotangent::hash() const
    {
        std::size_t seed = 0x15c2996b91a0adfd; // Hash of 'TheCalculater::math::AnalyticExpression::Arccotangent'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Arcsecant::hash() const
    {
        std::size_t seed = 0x8865bb38e15d8b94; // Hash of 'TheCalculater::math::AnalyticExpression::Arcsecant'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] std::size_t AnalyticExpression::Arccosecant::hash() const
    {
        std::size_t seed = 0xc43b3d0796a9c04c; // Hash of 'TheCalculater::math::AnalyticExpression::Arccosecant'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }

    bool AnalyticExpression::Constant::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Constant) {
            return false;
        }
        const auto& o = static_cast<const Constant&>(other);
        return value == o.value;
    }
    bool AnalyticExpression::Variable::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Variable) {
            return false;
        }
        const auto& o = static_cast<const Variable&>(other);
        return name == o.name;
    }
    bool AnalyticExpression::Addition::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Addition) {
            return false;
        }
        const auto& o = static_cast<const Addition&>(other);
        return left->rawEqualTo(*o.left) && right->rawEqualTo(*o.right);
    }
    bool AnalyticExpression::Subtraction::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Subtraction) {
            return false;
        }
        const auto& o = static_cast<const Subtraction&>(other);
        return left->rawEqualTo(*o.left) && right->rawEqualTo(*o.right);
    }
    bool AnalyticExpression::Multiplication::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Multiplication) {
            return false;
        }
        const auto& o = static_cast<const Multiplication&>(other);
        return left->rawEqualTo(*o.left) && right->rawEqualTo(*o.right);
    }
    bool AnalyticExpression::Division::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Division) {
            return false;
        }
        const auto& o = static_cast<const Division&>(other);
        return numerator->rawEqualTo(*o.numerator) && denominator->rawEqualTo(*o.denominator);
    }
    bool AnalyticExpression::Negation::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Negation) {
            return false;
        }
        const auto& o = static_cast<const Negation&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Affirmation::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Affirmation) {
            return false;
        }
        const auto& o = static_cast<const Affirmation&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Power::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Power) {
            return false;
        }
        const auto& o = static_cast<const Power&>(other);
        return base->rawEqualTo(*o.base) && exponent->rawEqualTo(*o.exponent);
    }
    bool AnalyticExpression::Root::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Root) {
            return false;
        }
        const auto& o = static_cast<const Root&>(other);
        return radicand->rawEqualTo(*o.radicand) && index->rawEqualTo(*o.index);
    }
    bool AnalyticExpression::Factorial::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Factorial) {
            return false;
        }
        const auto& o = static_cast<const Factorial&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::AbsoluteValue::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::AbsoluteValue) {
            return false;
        }
        const auto& o = static_cast<const AbsoluteValue&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Modulus::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Modulus) {
            return false;
        }
        const auto& o = static_cast<const Modulus&>(other);
        return dividend->rawEqualTo(*o.dividend) && divisor->rawEqualTo(*o.divisor);
    }
    bool AnalyticExpression::Logarithm::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Logarithm) {
            return false;
        }
        const auto& o = static_cast<const Logarithm&>(other);
        return base->rawEqualTo(*o.base) && operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::NaturalLogarithm::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::NaturalLogarithm) {
            return false;
        }
        const auto& o = static_cast<const NaturalLogarithm&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Degree::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Degree) {
            return false;
        }
        const auto& o = static_cast<const Degree&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Sine::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Sine) {
            return false;
        }
        const auto& o = static_cast<const Sine&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Cosine::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Cosine) {
            return false;
        }
        const auto& o = static_cast<const Cosine&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Tangent::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Tangent) {
            return false;
        }
        const auto& o = static_cast<const Tangent&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Cotangent::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Cotangent) {
            return false;
        }
        const auto& o = static_cast<const Cotangent&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Secant::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Secant) {
            return false;
        }
        const auto& o = static_cast<const Secant&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Cosecant::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Cosecant) {
            return false;
        }
        const auto& o = static_cast<const Cosecant&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Arcsine::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Arcsine) {
            return false;
        }
        const auto& o = static_cast<const Arcsine&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Arccosine::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Arccosine) {
            return false;
        }
        const auto& o = static_cast<const Arccosine&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Arctangent::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Arctangent) {
            return false;
        }
        const auto& o = static_cast<const Arctangent&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Arccotangent::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Arccotangent) {
            return false;
        }
        const auto& o = static_cast<const Arccotangent&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Arcsecant::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Arcsecant) {
            return false;
        }
        const auto& o = static_cast<const Arcsecant&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Arccosecant::rawEqualTo(const Node& other) const
    {
        if (other.type() != NodeType::Arccosecant) {
            return false;
        }
        const auto& o = static_cast<const Arccosecant&>(other);
        return operand->rawEqualTo(*o.operand);
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Variable::simplify(const SimplifyContext& context) const
    {
        auto it = context.vars.find(name);
        if (it == context.vars.end()) {
            return clone();
        }
        context.logger("Replacing variable {} with its value.", name);
        return it->second->clone();
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Pi::simplify(const SimplifyContext& context) const
    {
        if (context.config.irrationalUseApproximation) {
            return std::make_unique<Constant>(context.config.pi);
        }
        return clone();
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Euler::simplify(const SimplifyContext& context) const
    {
        if (context.config.irrationalUseApproximation) {
            return std::make_unique<Constant>(context.config.e);
        }
        return clone();
    }

    namespace { namespace _d_simplify {
        using Terms = std::list<std::unique_ptr<AnalyticExpression::Node>>;

        template <typename TOperation>
        Terms collectTermsFor(const AnalyticExpression::Node& node) = delete;
        template <>
        Terms collectTermsFor<AnalyticExpression::Addition>(const AnalyticExpression::Node& node)
        {
            static std::function<void(const AnalyticExpression::Node&, Terms&, bool)> collect = [&](const AnalyticExpression::Node& node, Terms& result, bool inverted) {
                if (node.type() == AnalyticExpression::NodeType::Addition) {
                    const auto& binOp = static_cast<const AnalyticExpression::Addition&>(node);
                    collect(binOp.firstOperand(), result, inverted);
                    collect(binOp.secondOperand(), result, inverted);
                } else if (node.type() == AnalyticExpression::NodeType::Subtraction) {
                    const auto& binOp = static_cast<const AnalyticExpression::Subtraction&>(node);
                    collect(binOp.firstOperand(), result, inverted);
                    collect(binOp.secondOperand(), result, !inverted);
                } else {
                    result.push_back(inverted ? AnalyticExpression::Negation(node.clone()).simplify({ }) : node.clone());
                }
            };

            Terms result;
            collect(node, result, false);
            return result;
        }
        template <>
        Terms collectTermsFor<AnalyticExpression::Multiplication>(const AnalyticExpression::Node& node)
        {
            static std::function<void(const AnalyticExpression::Node&, Terms&, bool)> collect = [&](const AnalyticExpression::Node& node, Terms& result, bool inverted) {
                if (node.type() == AnalyticExpression::NodeType::Multiplication) {
                    const auto& binOp = static_cast<const AnalyticExpression::Multiplication&>(node);
                    collect(binOp.firstOperand(), result, inverted);
                    collect(binOp.secondOperand(), result, inverted);
                } else if (node.type() == AnalyticExpression::NodeType::Division) {
                    const auto& binOp = static_cast<const AnalyticExpression::Division&>(node);
                    collect(binOp.firstOperand(), result, inverted);
                    collect(binOp.secondOperand(), result, !inverted);
                } else {
                    result.push_back(inverted ? AnalyticExpression::Division(std::make_unique<AnalyticExpression::Constant>(1), node.clone()).simplify({ }) : node.clone());
                }
            };

            Terms result;
            collect(node, result, false);
            return result;
        }

        template <typename TOperation>
        Terms flatten(const AnalyticExpression::Node& node)
        {
            auto terms = collectTermsFor<TOperation>(node);
            terms.sort([](const auto& a, const auto& b) { return sortingCompare(*a, *b) < 0; });
            return terms;
        }
        template <typename TOperation>
        std::unique_ptr<AnalyticExpression::Node> rebuildTree(Terms&& terms)
        {
            if (terms.empty()) {
                return std::make_unique<AnalyticExpression::Constant>(0);
            }

            std::unique_ptr<AnalyticExpression::Node> result = std::move(terms.front());

            for (auto& term : terms | std::ranges::views::drop(1)) {
                result = std::make_unique<TOperation>(std::move(result), std::move(term));
            }

            return result;
        }
        template <>
        std::unique_ptr<AnalyticExpression::Node> rebuildTree<AnalyticExpression::Addition>(Terms&& terms)
        {
            if (terms.empty()) {
                return std::make_unique<AnalyticExpression::Constant>(0);
            }

            std::unique_ptr<AnalyticExpression::Node> result = std::move(terms.front());

            for (auto& term : terms | std::ranges::views::drop(1)) {
                if (term->type() == AnalyticExpression::NodeType::Negation) {
                    const auto& neg = static_cast<const AnalyticExpression::Negation&>(*term);
                    result = std::make_unique<AnalyticExpression::Subtraction>(std::move(result), std::move(neg.operand));
                    continue;
                }
                result = std::make_unique<AnalyticExpression::Addition>(std::move(result), std::move(term));
            }

            return result;
        }
    }} // namespace ::_d_simplify
    namespace { namespace _d_simplify::addition {
        // Assume both side is already simplified. IF FAILED, return nullptr.
        std::unique_ptr<AnalyticExpression::Node> tryCombine(const AnalyticExpression::Node& a, const AnalyticExpression::Node& b)
        {
            // Handle constants first.
            if (a.type() == AnalyticExpression::NodeType::Constant && b.type() == AnalyticExpression::NodeType::Constant) {
                const auto& aConst = static_cast<const AnalyticExpression::Constant&>(a);
                const auto& bConst = static_cast<const AnalyticExpression::Constant&>(b);
                return std::make_unique<AnalyticExpression::Constant>(aConst.value + bConst.value);
            }
            // Handle common cases here.
            if (a.rawEqualTo(b)) {
                return std::make_unique<AnalyticExpression::Multiplication>(
                    std::make_unique<AnalyticExpression::Constant>(2),
                    a.clone());
            } else if (a.rawEqualTo(AnalyticExpression::Constant::ZERO)) {
                return b.clone();
            } else if (b.rawEqualTo(AnalyticExpression::Constant::ZERO)) {
                return a.clone();
            }

            return nullptr;
        }
    }} // namespace ::_d_simplify::addition
    namespace { namespace _d_simplify::multiplication {
        // Assume both side is already simplified. IF FAILED, return nullptr.
        std::unique_ptr<AnalyticExpression::Node> tryCombine(const AnalyticExpression::Node& a, const AnalyticExpression::Node& b)
        {
            // Handle constants first.
            if (a.type() == AnalyticExpression::NodeType::Constant && b.type() == AnalyticExpression::NodeType::Constant) {
                const auto& aConst = static_cast<const AnalyticExpression::Constant&>(a);
                const auto& bConst = static_cast<const AnalyticExpression::Constant&>(b);
                return std::make_unique<AnalyticExpression::Constant>(aConst.value * bConst.value);
            }
            // Handle common cases here.
            if (a.rawEqualTo(b)) {
                return std::make_unique<AnalyticExpression::Power>(a.clone(), std::make_unique<AnalyticExpression::Constant>(2));
            } else if (a.rawEqualTo(AnalyticExpression::Constant::ONE)) {
                return b.clone();
            } else if (b.rawEqualTo(AnalyticExpression::Constant::ONE)) {
                return a.clone();
            } else if (a.rawEqualTo(AnalyticExpression::Constant::ZERO) || b.rawEqualTo(AnalyticExpression::Constant::ZERO)) {
                return std::make_unique<AnalyticExpression::Constant>(0);
            }

            return nullptr;
        }
    }} // namespace ::_d_simplify::multiplication
    namespace { namespace _d_simplify {
        template <decltype(_d_simplify::addition::tryCombine) TCombiner>
        void combineLikeTermsIn(Terms& terms)
        {
            for (auto i = terms.begin(); i != terms.end(); ++i) {
                for (auto j = std::next(i); j != terms.end();) {
                    auto combined = TCombiner(**i, **j);
                    if (!combined) {
                        ++j;
                        continue;
                    }
                    *i = std::move(combined);
                    j = terms.erase(j);
                }
            }
        }

        FractionCalculationConfig makeCalculationConfigFromContext(const AnalyticExpression::SimplifyContext& context)
        {
            FractionCalculationConfig config;
            config.pi = context.config.pi;
            config.e = context.config.e;
            config.approximation.useWhenNeeded = context.config.irrationalUseApproximation;
            config.approximation.tolerance = context.config.approximationTolerance;
            config.approximation.maxIterations = context.config.approximationMaxIterations;
            return config;
        }
    }} // namespace ::_d_simplify

    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Addition::simplify(const SimplifyContext& context) const
    {
        auto leftSimplified = firstOperand().simplify(context);
        auto rightSimplified = secondOperand().simplify(context);

        context.logger("Flattening addition expression.");
        _d_simplify::Terms terms = _d_simplify::flatten<Addition>(Addition(std::move(leftSimplified), std::move(rightSimplified)));

        // If there's a undefined term, it'll be the first one after sorting. Same for infinity.
        if (terms.front()->type() == NodeType::Undefined)
            return std::make_unique<Undefined>();
        if (terms.front()->type() == NodeType::Infinity)
            return std::make_unique<Infinity>();

        context.logger("Combining terms for addition.");
        _d_simplify::combineLikeTermsIn<_d_simplify::addition::tryCombine>(terms);

        context.logger("Rebuilding addition expression.");
        return _d_simplify::rebuildTree<Addition>(std::move(terms));
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Subtraction::simplify(const SimplifyContext& context) const
    {
        // Simple reuse of addition simplification.
        auto leftSimplified = firstOperand().simplify(context);
        auto rightSimplified = secondOperand().simplify(context);

        context.logger("Flattening subtraction expression.");
        _d_simplify::Terms terms = _d_simplify::flatten<Addition>(Subtraction(std::move(leftSimplified), std::move(rightSimplified)));

        if (terms.front()->type() == NodeType::Undefined)
            return std::make_unique<Undefined>();
        if (terms.front()->type() == NodeType::Infinity)
            return std::make_unique<Infinity>();

        context.logger("Combining terms for subtraction.");
        _d_simplify::combineLikeTermsIn<_d_simplify::addition::tryCombine>(terms);

        context.logger("Rebuilding subtraction expression.");
        return _d_simplify::rebuildTree<Addition>(std::move(terms));
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Multiplication::simplify(const SimplifyContext& context) const
    {
        // Similar to addition
        auto leftSimplified = firstOperand().simplify(context);
        auto rightSimplified = secondOperand().simplify(context);

        context.logger("Flattening multiplication expression.");
        _d_simplify::Terms terms = _d_simplify::flatten<Multiplication>(Multiplication(std::move(leftSimplified), std::move(rightSimplified)));

        if (terms.front()->type() == NodeType::Undefined)
            return std::make_unique<Undefined>();
        if (terms.front()->type() == NodeType::Infinity)
            return std::make_unique<Infinity>();

        context.logger("Combining factors for multiplication.");
        _d_simplify::combineLikeTermsIn<_d_simplify::multiplication::tryCombine>(terms);

        context.logger("Rebuilding multiplication expression.");
        return _d_simplify::rebuildTree<Multiplication>(std::move(terms));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Division::simplify(const SimplifyContext& context) const
    {
        auto numerSimplified = firstOperand().simplify(context);
        auto denoSimplified = secondOperand().simplify(context);

        if (numerSimplified->type() == NodeType::Constant && denoSimplified->type() == NodeType::Constant) {
            context.logger("Turning division into constant.");
            const auto& numerConst = static_cast<const AnalyticExpression::Constant&>(*numerSimplified);
            const auto& denoConst = static_cast<const AnalyticExpression::Constant&>(*denoSimplified);
            if (denoConst.value == 0) {
                if (numerConst.value == 0)
                    return std::make_unique<AnalyticExpression::Undefined>();
                return std::make_unique<AnalyticExpression::Infinity>();
            }
            return std::make_unique<AnalyticExpression::Constant>(numerConst.value / denoConst.value);
        }

        context.logger("Flattening division expression.");
        _d_simplify::Terms terms = _d_simplify::flatten<Multiplication>(Division(std::move(numerSimplified), std::move(denoSimplified)));

        if (terms.front()->type() == NodeType::Undefined)
            return std::make_unique<Undefined>();
        if (terms.front()->type() == NodeType::Infinity)
            return std::make_unique<Infinity>();

        context.logger("Combining factors for division.");
        _d_simplify::combineLikeTermsIn<_d_simplify::multiplication::tryCombine>(terms);

        context.logger("Rebuilding division expression.");
        return _d_simplify::rebuildTree<Multiplication>(std::move(terms));
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Negation::simplify(const SimplifyContext& context) const
    {
        auto simplified = firstOperand().simplify(context);

        if (simplified->type() == NodeType::Constant) {
            context.logger("Turning negation into constant.");
            const auto& constant = static_cast<const AnalyticExpression::Constant&>(*simplified);
            return std::make_unique<AnalyticExpression::Constant>(-constant.value);
        } else if (simplified->type() == NodeType::Negation) {
            context.logger("Removing nested negation.");
            const auto& neg = static_cast<const AnalyticExpression::Negation&>(*simplified);
            return neg.firstOperand().simplify(context);
        } else if (simplified->type() == NodeType::Addition || simplified->type() == NodeType::Subtraction) {
            context.logger("Flattening negation expression.");
            _d_simplify::Terms terms = _d_simplify::flatten<Addition>(Negation(std::move(simplified)));

            context.logger("Negating terms for negation.");
            for (auto& term : terms) {
                if (term->type() == NodeType::Negation)
                    term = static_cast<AnalyticExpression::Negation&>(*term).firstOperand().simplify(context);
                else if (term->type() == NodeType::Constant) {
                    auto& constant = static_cast<AnalyticExpression::Constant&>(*term);
                    constant.value = -constant.value;
                } else if (term->type() == NodeType::Undefined) {
                    return std::make_unique<AnalyticExpression::Undefined>();
                } else if (term->type() == NodeType::Infinity) {
                    return std::make_unique<AnalyticExpression::Infinity>();
                } else {
                    term = std::make_unique<AnalyticExpression::Negation>(std::move(term));
                }
            }

            if (terms.size() == 1) {
                return std::move(terms.front());
            }
            context.logger("Rebuilding negation expression.");
            return _d_simplify::rebuildTree<Addition>(std::move(terms));
        }

        return std::make_unique<AnalyticExpression::Negation>(std::move(simplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Affirmation::simplify(const SimplifyContext& context) const
    {
        return operand->simplify(context);
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Power::simplify(const SimplifyContext& context) const
    {
        auto baseSimplified = firstOperand().simplify(context);
        auto exponentSimplified = secondOperand().simplify(context);

        if (baseSimplified->type() == NodeType::Constant && exponentSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant power.");
            const auto& baseConst = static_cast<const AnalyticExpression::Constant&>(*baseSimplified);
            const auto& expConst = static_cast<const AnalyticExpression::Constant&>(*exponentSimplified);
            if (expConst.value == 0 && baseConst.value == 0) {
                return std::make_unique<AnalyticExpression::Undefined>();
            }
            try {
                return std::make_unique<AnalyticExpression::Constant>(pow(baseConst.value, expConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
            } catch (const IrrationalResultException&) {
                // ignored
            }
        } else if (exponentSimplified->type() == NodeType::Constant) {
            context.logger("Handling constant exponent.");
            const auto& expConst = static_cast<const AnalyticExpression::Constant&>(*exponentSimplified);
            if (expConst.value == 0) {
                if (baseSimplified->type() == NodeType::Constant && static_cast<const Constant&>(*baseSimplified).value == 0)
                    return std::make_unique<AnalyticExpression::Undefined>();
                return std::make_unique<AnalyticExpression::Constant>(1);
            } else if (expConst.value == 1) {
                return baseSimplified;
            } else if (expConst.value == -1) {
                return std::make_unique<AnalyticExpression::Division>(std::make_unique<Constant>(1), std::move(baseSimplified));
            }
        }

        return std::make_unique<AnalyticExpression::Power>(std::move(baseSimplified), std::move(exponentSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Root::simplify(const SimplifyContext& context) const
    {
        auto radSimplified = firstOperand().simplify(context);
        auto indSimplified = secondOperand().simplify(context);
        if (radSimplified->type() == NodeType::Constant && indSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant root.");
            const auto& radConst = static_cast<const AnalyticExpression::Constant&>(*radSimplified);
            const auto& indConst = static_cast<const AnalyticExpression::Constant&>(*indSimplified);
            if (indConst.value == 0) {
                if (radConst.value == 0)
                    return std::make_unique<AnalyticExpression::Undefined>();
                return std::make_unique<AnalyticExpression::Undefined>();
            }
            try {
                return std::make_unique<AnalyticExpression::Constant>(math::root(radConst.value, indConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
            } catch (const IrrationalResultException&) {
                // ignored
            } catch (const FractionCalculationException& e) {
                // TODO: Add processing logic for computing even root of negative number.
                // It's too complex for now to handle more than 2 roots. (has n different roots)
            }
        } else if (indSimplified->type() == NodeType::Constant) {
            context.logger("Handling constant index for root.");
            const auto& indConst = static_cast<const AnalyticExpression::Constant&>(*indSimplified);
            if (indConst.value == 0) {
                return std::make_unique<AnalyticExpression::Undefined>();
            } else if (indConst.value == 1) {
                return radSimplified;
            } else if (indConst.value == -1) {
                return std::make_unique<AnalyticExpression::Division>(std::make_unique<Constant>(1), std::move(radSimplified));
            }
        }

        return std::make_unique<AnalyticExpression::Root>(std::move(radSimplified), std::move(indSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Factorial::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant factorial.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            // Make sure that the operand is a positive integer.
            if (operandConst.value.denominator() == 1 && operandConst.value.numerator() >= 0) {
                return std::make_unique<AnalyticExpression::Constant>(math::factorial(operandConst.value));
            } // TODO: Use gamma function to calculate factorial.
        }

        return std::make_unique<AnalyticExpression::Factorial>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::AbsoluteValue::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant absolute value.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(abs(operandConst.value));
        }

        return std::make_unique<AnalyticExpression::AbsoluteValue>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Modulus::simplify(const SimplifyContext& context) const
    {
        auto dividendSimplified = firstOperand().simplify(context);
        auto divisorSimplified = secondOperand().simplify(context);

        if (dividendSimplified->type() == NodeType::Constant && divisorSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant modulus.");
            const auto& dividendConst = static_cast<const AnalyticExpression::Constant&>(*dividendSimplified);
            const auto& divisorConst = static_cast<const AnalyticExpression::Constant&>(*divisorSimplified);
            return std::make_unique<AnalyticExpression::Constant>(dividendConst.value % divisorConst.value);
        }

        return std::make_unique<AnalyticExpression::Modulus>(std::move(dividendSimplified), std::move(divisorSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Logarithm::simplify(const SimplifyContext& context) const
    {
        auto baseSimplified = firstOperand().simplify(context);
        auto operandSimplified = secondOperand().simplify(context);

        if (baseSimplified->type() == NodeType::Constant && operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant logarithm.");
            const auto& baseConst = static_cast<const AnalyticExpression::Constant&>(*baseSimplified);
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            if (baseConst.value != 0 && baseConst.value != 1 && baseConst.value > 0 && operandConst.value > 0) {
                try {
                    return std::make_unique<AnalyticExpression::Constant>(log(baseConst.value, operandConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
                } catch (const IrrationalResultException&) {
                    // ignored
                }
            }
        }

        return std::make_unique<AnalyticExpression::Logarithm>(std::move(baseSimplified), std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::NaturalLogarithm::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant natural logarithm.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            if (operandConst.value > 0) {
                try {
                    return std::make_unique<AnalyticExpression::Constant>(ln(operandConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
                } catch (const IrrationalResultException&) {
                    // ignored
                }
            }
        }

        return std::make_unique<AnalyticExpression::NaturalLogarithm>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Degree::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant && context.config.irrationalUseApproximation) {
            context.logger("Calculating constant degree to radian conversion.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(operandConst.value * context.config.pi / 180);
        }

        return std::make_unique<AnalyticExpression::Degree>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Sine::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant sine.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            try {
                return std::make_unique<AnalyticExpression::Constant>(sin(operandConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
            } catch (const IrrationalResultException&) {
                // ignored
            }
        }

        return std::make_unique<AnalyticExpression::Sine>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Cosine::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant cosine.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            try {
                return std::make_unique<AnalyticExpression::Constant>(cos(operandConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
            } catch (const IrrationalResultException&) {
                // ignored
            }
        }

        return std::make_unique<AnalyticExpression::Cosine>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Tangent::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant tangent.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            try {
                return std::make_unique<AnalyticExpression::Constant>(tan(operandConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
            } catch (const IrrationalResultException&) {
                // ignored
            }
        }

        return std::make_unique<AnalyticExpression::Tangent>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Cotangent::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant cotangent.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            try {
                return std::make_unique<AnalyticExpression::Constant>(cot(operandConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
            } catch (const IrrationalResultException&) {
                // ignored
            }
        }

        return std::make_unique<AnalyticExpression::Cotangent>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Secant::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant secant.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            try {
                return std::make_unique<AnalyticExpression::Constant>(sec(operandConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
            } catch (const IrrationalResultException&) {
                // ignored
            }
        }

        return std::make_unique<AnalyticExpression::Secant>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Cosecant::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant cosecant.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            try {
                return std::make_unique<AnalyticExpression::Constant>(csc(operandConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
            } catch (const IrrationalResultException&) {
                // ignored
            }
        }

        return std::make_unique<AnalyticExpression::Cosecant>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Arcsine::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant arcsine.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            try {
                return std::make_unique<AnalyticExpression::Constant>(arcsin(operandConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
            } catch (const IrrationalResultException&) {
                // ignored
            }
        }

        return std::make_unique<AnalyticExpression::Arcsine>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Arccosine::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant arccosine.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            try {
                return std::make_unique<AnalyticExpression::Constant>(arccos(operandConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
            } catch (const IrrationalResultException&) {
                // ignored
            }
        }

        return std::make_unique<AnalyticExpression::Arccosine>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Arctangent::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant arctangent.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            try {
                return std::make_unique<AnalyticExpression::Constant>(arctan(operandConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
            } catch (const IrrationalResultException&) {
                // ignored
            }
        }

        return std::make_unique<AnalyticExpression::Arctangent>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Arccotangent::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant arccotangent.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            try {
                return std::make_unique<AnalyticExpression::Constant>(arccot(operandConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
            } catch (const IrrationalResultException&) {
                // ignored
            }
        }

        return std::make_unique<AnalyticExpression::Arccotangent>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Arcsecant::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant arcsecant.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            try {
                return std::make_unique<AnalyticExpression::Constant>(arcsec(operandConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
            } catch (const IrrationalResultException&) {
                // ignored
            }
        }

        return std::make_unique<AnalyticExpression::Arcsecant>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::Node> AnalyticExpression::Arccosecant::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant arccosecant.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            try {
                return std::make_unique<AnalyticExpression::Constant>(arccsc(operandConst.value, _d_simplify::makeCalculationConfigFromContext(context)));
            } catch (const IrrationalResultException&) {
                // ignored
            }
        }

        return std::make_unique<AnalyticExpression::Arccosecant>(std::move(operandSimplified));
    }
} // namespace TheCalculater::math
