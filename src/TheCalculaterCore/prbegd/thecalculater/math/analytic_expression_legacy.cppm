/**
 * @file analytic_expression_legacy.cppm
 * @author prbegd
 * @brief Declaration of analytic expression class and related functions.
 * @date 2025-11-12
 *
 * @brief Analytic Expression class and related functionalities.
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
#include "thecalculater/macros.hpp"

export module prbegd.thecalculater.math.analytic_expression_legacy;
import prbegd.thecalculater.math.fraction;
import thirdparty.core;
import std;

namespace thecalculater::math {
    /**
     * @brief A class representing the mathematical concept of the analytic expression.
     *
     * An analytic expression is a symbolic combination of constants, variables, mathematical operators, elementary functions and parentheses. Unlike an equation, it does not contain an equal sign or inequality symbols.
     *
     * TODO: crush the current simplify logic into small pieces because current method is a bunch of shit. After rebuilding the whole structure, finish the documentation here: add some examples.
     */
    export class TCAPI AnalyticExpression {
    public:
        /* abstract */ class Node;

        AnalyticExpression() = default;
        AnalyticExpression(const Node& node);
        AnalyticExpression(const std::unique_ptr<Node>& node);
        AnalyticExpression(std::unique_ptr<Node>&& node);

        enum class NodeType : std::int8_t;

        /* interface */ class IUnaryOperator;
        /* interface */ class IBinaryOperator;

        class Constant;
        class Variable;
        class Infinity;
        class Pi;
        class Euler;
        class ImaginaryUnit;
        class Undefined;

        class Addition;
        class Subtraction;
        class Multiplication;
        class Division;
        class Negation;
        class Affirmation;
        class Power;
        class Root;
        class Factorial;
        class AbsoluteValue;
        class Modulus;

        class Logarithm;
        class NaturalLogarithm;
        class Degree;
        class Sine;
        class Cosine;
        class Tangent;
        class Cotangent;
        class Secant;
        class Cosecant;
        class Arcsine;
        class Arccosine;
        class Arctangent;
        class Arccotangent;
        class Arcsecant;
        class Arccosecant;

        struct SimplifyContext;

        // * Factory methods

        static AnalyticExpression constant(const Fraction& value);
        static AnalyticExpression constant(Fraction&& value);
        static AnalyticExpression variable(std::string_view name);
        static AnalyticExpression variable(std::string&& name);
        static AnalyticExpression infinity();
        static AnalyticExpression pi();
        static AnalyticExpression euler();
        static AnalyticExpression imaginaryUnit();
        static AnalyticExpression undefined();
        static AnalyticExpression addition(const AnalyticExpression& left, const AnalyticExpression& right);
        static AnalyticExpression addition(AnalyticExpression&& left, AnalyticExpression&& right);
        static AnalyticExpression subtraction(const AnalyticExpression& left, const AnalyticExpression& right);
        static AnalyticExpression subtraction(AnalyticExpression&& left, AnalyticExpression&& right);
        static AnalyticExpression multiplication(const AnalyticExpression& left, const AnalyticExpression& right);
        static AnalyticExpression multiplication(AnalyticExpression&& left, AnalyticExpression&& right);
        static AnalyticExpression division(const AnalyticExpression& numerator, const AnalyticExpression& denominator);
        static AnalyticExpression division(AnalyticExpression&& numerator, AnalyticExpression&& denominator);
        static AnalyticExpression negation(const AnalyticExpression& operand);
        static AnalyticExpression negation(AnalyticExpression&& operand);
        static AnalyticExpression affirmation(const AnalyticExpression& operand);
        static AnalyticExpression affirmation(AnalyticExpression&& operand);
        static AnalyticExpression power(const AnalyticExpression& base, const AnalyticExpression& exponent);
        static AnalyticExpression power(AnalyticExpression&& base, AnalyticExpression&& exponent);
        static AnalyticExpression root(const AnalyticExpression& radicand, const AnalyticExpression& degree);
        static AnalyticExpression root(AnalyticExpression&& radicand, AnalyticExpression&& degree);
        static AnalyticExpression factorial(const AnalyticExpression& operand);
        static AnalyticExpression factorial(AnalyticExpression&& operand);
        static AnalyticExpression absoluteValue(const AnalyticExpression& operand);
        static AnalyticExpression absoluteValue(AnalyticExpression&& operand);
        static AnalyticExpression modulus(const AnalyticExpression& dividend, const AnalyticExpression& divisor);
        static AnalyticExpression modulus(AnalyticExpression&& dividend, AnalyticExpression&& divisor);
        static AnalyticExpression logarithm(const AnalyticExpression& base, const AnalyticExpression& operand);
        static AnalyticExpression logarithm(AnalyticExpression&& base, AnalyticExpression&& operand);
        static AnalyticExpression naturalLogarithm(const AnalyticExpression& operand);
        static AnalyticExpression naturalLogarithm(AnalyticExpression&& operand);
        static AnalyticExpression degree(const AnalyticExpression& operand);
        static AnalyticExpression degree(AnalyticExpression&& operand);
        static AnalyticExpression sine(const AnalyticExpression& operand);
        static AnalyticExpression sine(AnalyticExpression&& operand);
        static AnalyticExpression cosine(const AnalyticExpression& operand);
        static AnalyticExpression cosine(AnalyticExpression&& operand);
        static AnalyticExpression tangent(const AnalyticExpression& operand);
        static AnalyticExpression tangent(AnalyticExpression&& operand);
        static AnalyticExpression cotangent(const AnalyticExpression& operand);
        static AnalyticExpression cotangent(AnalyticExpression&& operand);
        static AnalyticExpression secant(const AnalyticExpression& operand);
        static AnalyticExpression secant(AnalyticExpression&& operand);
        static AnalyticExpression cosecant(const AnalyticExpression& operand);
        static AnalyticExpression cosecant(AnalyticExpression&& operand);
        static AnalyticExpression arcsine(const AnalyticExpression& operand);
        static AnalyticExpression arcsine(AnalyticExpression&& operand);
        static AnalyticExpression arccosine(const AnalyticExpression& operand);
        static AnalyticExpression arccosine(AnalyticExpression&& operand);
        static AnalyticExpression arctangent(const AnalyticExpression& operand);
        static AnalyticExpression arctangent(AnalyticExpression&& operand);
        static AnalyticExpression arccotangent(const AnalyticExpression& operand);
        static AnalyticExpression arccotangent(AnalyticExpression&& operand);
        static AnalyticExpression arcsecant(const AnalyticExpression& operand);
        static AnalyticExpression arcsecant(AnalyticExpression&& operand);
        static AnalyticExpression arccosecant(const AnalyticExpression& operand);
        static AnalyticExpression arccosecant(AnalyticExpression&& operand);

        std::unique_ptr<Node> base;

        /* abstract */ class Node {
        public:
            virtual ~Node() = default;

            /**
             * @brief Simplify the expression.
             *
             * @param context The context for simplification.
             * @return std::unique_ptr<Node> The simplified expression.
             * @throw AnalyticExpressionEvaluateException If something goes wrong during simplification. Check derived classes for specific reasons.
             */
            [[nodiscard]] virtual std::unique_ptr<Node> simplify(const SimplifyContext& context) const = 0;

            /**
             * @brief Calculate the hash value of the expression.
             *
             * @warning Do NOT use it to compare two expressions for equality.
             * Only use it to sort expressions.
             *
             * @return std::size_t The hash value.
             */
            [[nodiscard]] virtual std::size_t hash() const = 0;

            [[nodiscard]] virtual std::unique_ptr<Node> clone() const = 0;
            [[nodiscard]] virtual NodeType type() const = 0;

            /**
             * @brief Check whether two expressions are equal.
             *
             * This function is "raw" because it only compares the structure of expressions. e.g. "a + b" is not equal to "b + a" even though they are mathematically equal.
             * @note You may want to call simplify() first.
             *
             * @param other The other expression to compare with.
             * @return Whether the two expressions are equal.
             */
            [[nodiscard]] bool virtual rawEqualTo(const Node& other) const = 0;
        };
    };

    /**
     * @brief Format an analytic expression in LaTeX format.
     *
     * Work in progress.
     *
     * @param expr The expression to format.
     * @return std::string The formatted expression in LaTeX format.
     */
    export TCAPI std::string format(const AnalyticExpression& expr);

    /**
     * @brief Format an analytic expression in tree format (less readable)
     *
     * @param expr The expression to format.
     * @return std::string The formatted expression in tree format.
     */
    export TCAPI std::string formatTree(const AnalyticExpression& expr);

    export TCAPI AnalyticExpression operator+(const AnalyticExpression& left, const AnalyticExpression& right);
    export TCAPI AnalyticExpression operator+(AnalyticExpression&& left, AnalyticExpression&& right);
    export TCAPI AnalyticExpression operator-(const AnalyticExpression& left, const AnalyticExpression& right);
    export TCAPI AnalyticExpression operator-(AnalyticExpression&& left, AnalyticExpression&& right);
    export TCAPI AnalyticExpression operator*(const AnalyticExpression& left, const AnalyticExpression& right);
    export TCAPI AnalyticExpression operator*(AnalyticExpression&& left, AnalyticExpression&& right);
    export TCAPI AnalyticExpression operator/(const AnalyticExpression& numerator, const AnalyticExpression& denominator);
    export TCAPI AnalyticExpression operator/(AnalyticExpression&& numerator, AnalyticExpression&& denominator);
    export TCAPI AnalyticExpression operator-(const AnalyticExpression& operand);
    export TCAPI AnalyticExpression operator-(AnalyticExpression&& operand);
    export TCAPI AnalyticExpression operator+(const AnalyticExpression& operand);
    export TCAPI AnalyticExpression operator+(AnalyticExpression&& operand);
    export TCAPI AnalyticExpression operator%(const AnalyticExpression& left, const AnalyticExpression& right);
    export TCAPI AnalyticExpression operator%(AnalyticExpression&& left, AnalyticExpression&& right);

    export TCAPI AnalyticExpression& operator+=(AnalyticExpression& left, const AnalyticExpression& right);
    export TCAPI AnalyticExpression& operator+=(AnalyticExpression& left, AnalyticExpression&& right);
    export TCAPI AnalyticExpression& operator-=(AnalyticExpression& left, const AnalyticExpression& right);
    export TCAPI AnalyticExpression& operator-=(AnalyticExpression& left, AnalyticExpression&& right);
    export TCAPI AnalyticExpression& operator*=(AnalyticExpression& left, const AnalyticExpression& right);
    export TCAPI AnalyticExpression& operator*=(AnalyticExpression& left, AnalyticExpression&& right);
    export TCAPI AnalyticExpression& operator/=(AnalyticExpression& left, const AnalyticExpression& right);
    export TCAPI AnalyticExpression& operator/=(AnalyticExpression& left, AnalyticExpression&& right);
    export TCAPI AnalyticExpression& operator%=(AnalyticExpression& left, const AnalyticExpression& right);
    export TCAPI AnalyticExpression& operator%=(AnalyticExpression& left, AnalyticExpression&& right);

    enum class AnalyticExpression::NodeType : std::int8_t {
        Undefined,
        Infinity,
        Constant,
        Pi,
        Euler,
        Variable,
        Addition,
        Subtraction,
        Multiplication,
        Division,
        Negation,
        Affirmation,
        Power,
        Root,
        Factorial,
        AbsoluteValue,
        Modulus,
        Logarithm,
        NaturalLogarithm,
        Degree,
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
        ImaginaryUnit
    };

    /* interface */ class AnalyticExpression::IUnaryOperator {
    public:
        virtual ~IUnaryOperator() = default;

        [[nodiscard]] virtual const Node& firstOperand() const = 0;
    };

    /* interface */ class AnalyticExpression::IBinaryOperator {
    public:
        virtual ~IBinaryOperator() = default;

        [[nodiscard]] virtual const Node& firstOperand() const = 0;
        [[nodiscard]] virtual const Node& secondOperand() const = 0;
    };

    class AnalyticExpression::Constant : public Node {
    public:
        Fraction value;

        explicit Constant(const Fraction& value)
            : value(value)
        { }
        explicit Constant(Fraction&& value)
            : value(std::move(value))
        { }

        Constant(const AnalyticExpression::Constant& other) = delete;
        Constant(AnalyticExpression::Constant&& other) = default;
        Constant& operator=(const AnalyticExpression::Constant& other) = delete;
        Constant& operator=(AnalyticExpression::Constant&& other) = default;
        ~Constant() override = default;

        [[nodiscard]] std::size_t hash() const override;

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Constant>(value); }
        [[nodiscard]] NodeType type() const override { return NodeType::Constant; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Constant; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext&) const override { return clone(); }

        [[nodiscard]] bool rawEqualTo(const Node& other) const override;

        // Stop creating garbage object for simple comparisons. That's just suffering. These constants are here to help you and the compiler have a good day!
        static const Constant ZERO;
        static const Constant ONE;
    };

    class AnalyticExpression::Variable : public Node {
    public:
        std::string name;

        explicit Variable(std::string_view name)
            : name(name)
        { }
        explicit Variable(std::string&& name)
            : name(std::move(name))
        { }

        [[nodiscard]] std::size_t hash() const override;

        Variable(const AnalyticExpression::Variable& other) = delete;
        Variable(AnalyticExpression::Variable&& other) = default;
        Variable& operator=(const AnalyticExpression::Variable& other) = delete;
        Variable& operator=(AnalyticExpression::Variable&& other) = default;
        ~Variable() override = default;

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Variable>(name); }
        [[nodiscard]] NodeType type() const override { return NodeType::Variable; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Variable; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Infinity : public Node {
    public:
        Infinity() = default;
        Infinity(const Infinity& other) = delete;
        Infinity(Infinity&& other) = default;
        Infinity& operator=(const Infinity& other) = delete;
        Infinity& operator=(Infinity&& other) = default;
        ~Infinity() override = default;

        [[nodiscard]] std::size_t hash() const override { return 0xc3dc0c723e73cbc3; } // Hash of 'thecalculater::math::AnalyticExpression::Infinity'

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Infinity>(); }
        [[nodiscard]] NodeType type() const override { return NodeType::Infinity; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Infinity; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext&) const override { return clone(); }
        [[nodiscard]] bool rawEqualTo(const Node& other) const override { return other.type() == NodeType::Infinity; }
    };

    class AnalyticExpression::Pi : public Node {
    public:
        Pi() = default;
        Pi(const Pi& other) = delete;
        Pi(Pi&& other) = default;
        Pi& operator=(const Pi& other) = delete;
        Pi& operator=(Pi&& other) = default;
        ~Pi() override = default;

        [[nodiscard]] std::size_t hash() const override { return 0x8c18f600b6867066; } // Hash of 'thecalculater::math::AnalyticExpression::Pi'

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Pi>(); }
        [[nodiscard]] NodeType type() const override { return NodeType::Pi; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Pi; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override { return other.type() == NodeType::Pi; }
    };

    class AnalyticExpression::Euler : public Node {
    public:
        Euler() = default;
        Euler(const Euler& other) = delete;
        Euler(Euler&& other) = default;
        Euler& operator=(const Euler& other) = delete;
        Euler& operator=(Euler&& other) = default;
        ~Euler() override = default;

        [[nodiscard]] std::size_t hash() const override { return 0x573ab0792d7b9fca; } // Hash of 'thecalculater::math::AnalyticExpression::Euler'

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Euler>(); }
        [[nodiscard]] NodeType type() const override { return NodeType::Euler; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Euler; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override { return other.type() == NodeType::Euler; }
    };

    class AnalyticExpression::ImaginaryUnit : public Node {
    public:
        ImaginaryUnit() = default;
        ImaginaryUnit(const ImaginaryUnit& other) = delete;
        ImaginaryUnit(ImaginaryUnit&& other) = default;
        ImaginaryUnit& operator=(const ImaginaryUnit& other) = delete;
        ImaginaryUnit& operator=(ImaginaryUnit&& other) = default;
        ~ImaginaryUnit() override = default;

        [[nodiscard]] std::size_t hash() const override { return 0x99506ad9db02af43; } // Hash of 'thecalculater::math::AnalyticExpression::ImaginaryUnit'

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<ImaginaryUnit>(); }
        [[nodiscard]] NodeType type() const override { return NodeType::ImaginaryUnit; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::ImaginaryUnit; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext&) const override { return clone(); }
        [[nodiscard]] bool rawEqualTo(const Node& other) const override { return other.type() == NodeType::ImaginaryUnit; }
    };

    class AnalyticExpression::Undefined : public Node {
    public:
        Undefined() = default;
        Undefined(const Undefined& other) = delete;
        Undefined(Undefined&& other) = default;
        Undefined& operator=(const Undefined& other) = delete;
        Undefined& operator=(Undefined&& other) = default;
        ~Undefined() override = default;

        [[nodiscard]] std::size_t hash() const override { return 0x1e2e18b597856397; } // Hash of 'thecalculater::math::AnalyticExpression::Undefined'

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Undefined>(); }
        [[nodiscard]] NodeType type() const override { return NodeType::Undefined; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Undefined; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext&) const override { return clone(); }
        [[nodiscard]] bool rawEqualTo(const Node& other) const override { return other.type() == NodeType::Undefined; }
    };

    class AnalyticExpression::Addition : public Node, public IBinaryOperator {
    public:
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        explicit Addition(const std::unique_ptr<Node>& left, const std::unique_ptr<Node>& right)
            : left(left->clone()), right(right->clone())
        { }
        explicit Addition(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
            : left(std::move(left)), right(std::move(right))
        { }

        Addition(const AnalyticExpression::Addition& other) = delete;
        Addition(AnalyticExpression::Addition&& other) = default;
        Addition& operator=(const AnalyticExpression::Addition& other) = delete;
        Addition& operator=(AnalyticExpression::Addition&& other) = default;
        ~Addition() override = default;

        [[nodiscard]] const Node& firstOperand() const override { return *left; }
        [[nodiscard]] const Node& secondOperand() const override { return *right; }

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Addition>(left->clone(), right->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Addition; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Addition; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Subtraction : public Node, public IBinaryOperator {
    public:
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        explicit Subtraction(const std::unique_ptr<Node>& left, const std::unique_ptr<Node>& right)
            : left(left->clone()), right(right->clone())
        { }
        explicit Subtraction(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
            : left(std::move(left)), right(std::move(right))
        { }

        Subtraction(const AnalyticExpression::Subtraction& other) = delete;
        Subtraction(AnalyticExpression::Subtraction&& other) = default;
        Subtraction& operator=(const AnalyticExpression::Subtraction& other) = delete;
        Subtraction& operator=(AnalyticExpression::Subtraction&& other) = default;
        ~Subtraction() override = default;

        [[nodiscard]] const Node& firstOperand() const override { return *left; }
        [[nodiscard]] const Node& secondOperand() const override { return *right; }

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Subtraction>(left->clone(), right->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Subtraction; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Subtraction; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Multiplication : public Node, public IBinaryOperator {
    public:
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        explicit Multiplication(const std::unique_ptr<Node>& left, const std::unique_ptr<Node>& right)
            : left(left->clone()), right(right->clone())
        { }
        explicit Multiplication(std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right)
            : left(std::move(left)), right(std::move(right))
        { }

        Multiplication(const AnalyticExpression::Multiplication& other) = delete;
        Multiplication(AnalyticExpression::Multiplication&& other) = default;
        Multiplication& operator=(const AnalyticExpression::Multiplication& other) = delete;
        Multiplication& operator=(AnalyticExpression::Multiplication&& other) = default;
        ~Multiplication() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *left; }
        [[nodiscard]] const Node& secondOperand() const override { return *right; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Multiplication>(left->clone(), right->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Multiplication; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Multiplication; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    // We use division to represent fractions.
    class AnalyticExpression::Division : public Node, public IBinaryOperator {
    public:
        std::unique_ptr<Node> numerator;
        std::unique_ptr<Node> denominator;

        explicit Division(const std::unique_ptr<Node>& numerator, const std::unique_ptr<Node>& denominator)
            : numerator(numerator->clone()), denominator(denominator->clone())
        { }
        explicit Division(std::unique_ptr<Node>&& numerator, std::unique_ptr<Node>&& denominator)
            : numerator(std::move(numerator)), denominator(std::move(denominator))
        { }

        Division(const AnalyticExpression::Division& other) = delete;
        Division(AnalyticExpression::Division&& other) = default;
        Division& operator=(const AnalyticExpression::Division& other) = delete;
        Division& operator=(AnalyticExpression::Division&& other) = default;
        ~Division() override = default;

        [[nodiscard]] const Node& firstOperand() const override { return *numerator; }
        [[nodiscard]] const Node& secondOperand() const override { return *denominator; }

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Division>(numerator->clone(), denominator->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Division; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Division; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Negation : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Negation(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Negation(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Negation(const AnalyticExpression::Negation& other) = delete;
        Negation(AnalyticExpression::Negation&& other) = default;
        Negation& operator=(const AnalyticExpression::Negation& other) = delete;
        Negation& operator=(AnalyticExpression::Negation&& other) = default;
        ~Negation() override = default;

        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Negation>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Negation; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Negation; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Affirmation : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Affirmation(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Affirmation(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Affirmation(const AnalyticExpression::Affirmation& other) = delete;
        Affirmation(AnalyticExpression::Affirmation&& other) = default;
        Affirmation& operator=(const AnalyticExpression::Affirmation& other) = delete;
        Affirmation& operator=(AnalyticExpression::Affirmation&& other) = default;
        ~Affirmation() override = default;

        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Affirmation>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Affirmation; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Affirmation; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Power : public Node, public IBinaryOperator {
    public:
        std::unique_ptr<Node> base;
        std::unique_ptr<Node> exponent;

        explicit Power(const std::unique_ptr<Node>& base, const std::unique_ptr<Node>& exponent)
            : base(base->clone()), exponent(exponent->clone())
        { }
        explicit Power(std::unique_ptr<Node>&& base, std::unique_ptr<Node>&& exponent)
            : base(std::move(base)), exponent(std::move(exponent))
        { }

        Power(const AnalyticExpression::Power& other) = delete;
        Power(AnalyticExpression::Power&& other) = default;
        Power& operator=(const AnalyticExpression::Power& other) = delete;
        Power& operator=(AnalyticExpression::Power&& other) = default;
        ~Power() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *base; }
        [[nodiscard]] const Node& secondOperand() const override { return *exponent; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Power>(base->clone(), exponent->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Power; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Power; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Root : public Node, public IBinaryOperator {
    public:
        std::unique_ptr<Node> radicand;
        std::unique_ptr<Node> index;

        explicit Root(const std::unique_ptr<Node>& radicand, const std::unique_ptr<Node>& index)
            : radicand(radicand->clone()), index(index->clone())
        { }
        explicit Root(std::unique_ptr<Node>&& radicand, std::unique_ptr<Node>&& index)
            : radicand(std::move(radicand)), index(std::move(index))
        { }

        Root(const AnalyticExpression::Root& other) = delete;
        Root(AnalyticExpression::Root&& other) = default;
        Root& operator=(const AnalyticExpression::Root& other) = delete;
        Root& operator=(AnalyticExpression::Root&& other) = default;
        ~Root() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *radicand; }
        [[nodiscard]] const Node& secondOperand() const override { return *index; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Root>(radicand->clone(), index->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Root; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Root; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Factorial : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Factorial(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Factorial(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Factorial(const AnalyticExpression::Factorial& other) = delete;
        Factorial(AnalyticExpression::Factorial&& other) = default;
        Factorial& operator=(const AnalyticExpression::Factorial& other) = delete;
        Factorial& operator=(AnalyticExpression::Factorial&& other) = default;
        ~Factorial() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Factorial>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Factorial; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Factorial; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::AbsoluteValue : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit AbsoluteValue(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit AbsoluteValue(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        AbsoluteValue(const AnalyticExpression::AbsoluteValue& other) = delete;
        AbsoluteValue(AnalyticExpression::AbsoluteValue&& other) = default;
        AbsoluteValue& operator=(const AnalyticExpression::AbsoluteValue& other) = delete;
        AbsoluteValue& operator=(AnalyticExpression::AbsoluteValue&& other) = default;
        ~AbsoluteValue() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<AbsoluteValue>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::AbsoluteValue; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::AbsoluteValue; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Modulus : public Node, public IBinaryOperator {
    public:
        std::unique_ptr<Node> dividend;
        std::unique_ptr<Node> divisor;

        explicit Modulus(const std::unique_ptr<Node>& dividend, const std::unique_ptr<Node>& divisor)
            : dividend(dividend->clone()), divisor(divisor->clone())
        { }
        explicit Modulus(std::unique_ptr<Node>&& dividend, std::unique_ptr<Node>&& divisor)
            : dividend(std::move(dividend)), divisor(std::move(divisor))
        { }

        Modulus(const AnalyticExpression::Modulus& other) = delete;
        Modulus(AnalyticExpression::Modulus&& other) = default;
        Modulus& operator=(const AnalyticExpression::Modulus& other) = delete;
        Modulus& operator=(AnalyticExpression::Modulus&& other) = default;
        ~Modulus() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *dividend; }
        [[nodiscard]] const Node& secondOperand() const override { return *divisor; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Modulus>(dividend->clone(), divisor->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Modulus; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Modulus; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Logarithm : public Node, public IBinaryOperator {
    public:
        std::unique_ptr<Node> base;
        std::unique_ptr<Node> operand;

        explicit Logarithm(const std::unique_ptr<Node>& base, const std::unique_ptr<Node>& operand)
            : base(base->clone()), operand(operand->clone())
        { }
        explicit Logarithm(std::unique_ptr<Node>&& base, std::unique_ptr<Node>&& operand)
            : base(std::move(base)), operand(std::move(operand))
        { }

        Logarithm(const AnalyticExpression::Logarithm& other) = delete;
        Logarithm(AnalyticExpression::Logarithm&& other) = default;
        Logarithm& operator=(const AnalyticExpression::Logarithm& other) = delete;
        Logarithm& operator=(AnalyticExpression::Logarithm&& other) = default;
        ~Logarithm() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *base; }
        [[nodiscard]] const Node& secondOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Logarithm>(base->clone(), operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Logarithm; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Logarithm; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Degree : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Degree(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Degree(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Degree(const AnalyticExpression::Degree& other) = delete;
        Degree(AnalyticExpression::Degree&& other) = default;
        Degree& operator=(const AnalyticExpression::Degree& other) = delete;
        Degree& operator=(AnalyticExpression::Degree&& other) = default;
        ~Degree() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Degree>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Degree; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Degree; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Sine : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Sine(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Sine(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Sine(const AnalyticExpression::Sine& other) = delete;
        Sine(AnalyticExpression::Sine&& other) = default;
        Sine& operator=(const AnalyticExpression::Sine& other) = delete;
        Sine& operator=(AnalyticExpression::Sine&& other) = default;
        ~Sine() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Sine>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Sine; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Sine; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Cosine : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Cosine(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Cosine(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Cosine(const AnalyticExpression::Cosine& other) = delete;
        Cosine(AnalyticExpression::Cosine&& other) = default;
        Cosine& operator=(const AnalyticExpression::Cosine& other) = delete;
        Cosine& operator=(AnalyticExpression::Cosine&& other) = default;
        ~Cosine() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Cosine>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Cosine; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Cosine; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Tangent : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Tangent(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Tangent(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Tangent(const AnalyticExpression::Tangent& other) = delete;
        Tangent(AnalyticExpression::Tangent&& other) = default;
        Tangent& operator=(const AnalyticExpression::Tangent& other) = delete;
        Tangent& operator=(AnalyticExpression::Tangent&& other) = default;
        ~Tangent() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Tangent>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Tangent; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Tangent; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Cotangent : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Cotangent(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Cotangent(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Cotangent(const AnalyticExpression::Cotangent& other) = delete;
        Cotangent(AnalyticExpression::Cotangent&& other) = default;
        Cotangent& operator=(const AnalyticExpression::Cotangent& other) = delete;
        Cotangent& operator=(AnalyticExpression::Cotangent&& other) = default;
        ~Cotangent() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Cotangent>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Cotangent; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Cotangent; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Secant : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Secant(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Secant(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Secant(const AnalyticExpression::Secant& other) = delete;
        Secant(AnalyticExpression::Secant&& other) = default;
        Secant& operator=(const AnalyticExpression::Secant& other) = delete;
        Secant& operator=(AnalyticExpression::Secant&& other) = default;
        ~Secant() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Secant>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Secant; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Secant; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Cosecant : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Cosecant(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Cosecant(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Cosecant(const AnalyticExpression::Cosecant& other) = delete;
        Cosecant(AnalyticExpression::Cosecant&& other) = default;
        Cosecant& operator=(const AnalyticExpression::Cosecant& other) = delete;
        Cosecant& operator=(AnalyticExpression::Cosecant&& other) = default;
        ~Cosecant() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Cosecant>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Cosecant; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Cosecant; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Arcsine : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Arcsine(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Arcsine(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Arcsine(const AnalyticExpression::Arcsine& other) = delete;
        Arcsine(AnalyticExpression::Arcsine&& other) = default;
        Arcsine& operator=(const AnalyticExpression::Arcsine& other) = delete;
        Arcsine& operator=(AnalyticExpression::Arcsine&& other) = default;
        ~Arcsine() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Arcsine>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Arcsine; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Arcsine; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Arccosine : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Arccosine(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Arccosine(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Arccosine(const AnalyticExpression::Arccosine& other) = delete;
        Arccosine(AnalyticExpression::Arccosine&& other) = default;
        Arccosine& operator=(const AnalyticExpression::Arccosine& other) = delete;
        Arccosine& operator=(AnalyticExpression::Arccosine&& other) = default;
        ~Arccosine() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Arccosine>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Arccosine; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Arccosine; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Arctangent : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Arctangent(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Arctangent(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Arctangent(const AnalyticExpression::Arctangent& other) = delete;
        Arctangent(AnalyticExpression::Arctangent&& other) = default;
        Arctangent& operator=(const AnalyticExpression::Arctangent& other) = delete;
        Arctangent& operator=(AnalyticExpression::Arctangent&& other) = default;
        ~Arctangent() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Arctangent>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Arctangent; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Arctangent; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Arccotangent : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Arccotangent(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Arccotangent(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Arccotangent(const AnalyticExpression::Arccotangent& other) = delete;
        Arccotangent(AnalyticExpression::Arccotangent&& other) = default;
        Arccotangent& operator=(const AnalyticExpression::Arccotangent& other) = delete;
        Arccotangent& operator=(AnalyticExpression::Arccotangent&& other) = default;
        ~Arccotangent() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Arccotangent>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Arccotangent; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Arccotangent; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Arcsecant : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Arcsecant(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Arcsecant(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Arcsecant(const AnalyticExpression::Arcsecant& other) = delete;
        Arcsecant(AnalyticExpression::Arcsecant&& other) = default;
        Arcsecant& operator=(const AnalyticExpression::Arcsecant& other) = delete;
        Arcsecant& operator=(AnalyticExpression::Arcsecant&& other) = default;
        ~Arcsecant() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Arcsecant>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Arcsecant; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Arcsecant; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::Arccosecant : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit Arccosecant(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit Arccosecant(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        Arccosecant(const AnalyticExpression::Arccosecant& other) = delete;
        Arccosecant(AnalyticExpression::Arccosecant&& other) = default;
        Arccosecant& operator=(const AnalyticExpression::Arccosecant& other) = delete;
        Arccosecant& operator=(AnalyticExpression::Arccosecant&& other) = default;
        ~Arccosecant() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<Arccosecant>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Arccosecant; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Arccosecant; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    class AnalyticExpression::NaturalLogarithm : public Node, public IUnaryOperator {
    public:
        std::unique_ptr<Node> operand;

        explicit NaturalLogarithm(const std::unique_ptr<Node>& operand)
            : operand(operand->clone())
        { }
        explicit NaturalLogarithm(std::unique_ptr<Node>&& operand)
            : operand(std::move(operand))
        { }

        NaturalLogarithm(const AnalyticExpression::NaturalLogarithm& other) = delete;
        NaturalLogarithm(AnalyticExpression::NaturalLogarithm&& other) = default;
        NaturalLogarithm& operator=(const AnalyticExpression::NaturalLogarithm& other) = delete;
        NaturalLogarithm& operator=(AnalyticExpression::NaturalLogarithm&& other) = default;
        ~NaturalLogarithm() override = default;

        [[nodiscard]] std::size_t hash() const override;
        [[nodiscard]] const Node& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<Node> clone() const override { return std::make_unique<NaturalLogarithm>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::NaturalLogarithm; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::NaturalLogarithm; }

        [[nodiscard]] std::unique_ptr<Node> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const Node& other) const override;
    };

    struct AnalyticExpression::SimplifyContext {
        using VariableContext = std::map<std::string, std::unique_ptr<Node>>;
        struct Config {
            /// @brief Whether to use approximation for irrational numbers (e.g. pi, e).
            /// If false, only rational numbers will be calculated.
            bool irrationalUseApproximation = false;
            // e.g. we want 4 significant digits: 0.0001
            Fraction approximationTolerance;
            // If approximation uses iterations, the iteration will stop after iteration count reach this value to prevent infinite loop.
            std::uint32_t approximationMaxIterations = 0;
            /// The pi value used in calculation.
            Fraction pi;
            /// The euler value used in calculation.
            Fraction e;
        };

        VariableContext vars;
        Config config;
        class CalculationLogger {
        public:
            using LogFunction = std::function<void(std::string_view)>;

            CalculationLogger() = default;
            explicit CalculationLogger(LogFunction logger)
                : logger_(std::move(logger))
            { }

            template <typename... Args>
            void operator()(std::format_string<Args...> format, Args&&... args) const
            {
                if (logger_)
                    logger_(std::format(format, std::forward<Args>(args)...));
            }

        private:
            LogFunction logger_;
        } logger;
    };
} // namespace thecalculater::math