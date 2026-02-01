/**
 * @file analytic_expression.hpp
 * @author prbegd
 * @brief Declaration of algebraic expression class and related functions.
 * @date 2025-11-12
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#pragma once
#include "TheCalculater/core.hpp"
#include "TheCalculater/core/logger_wrapper.hpp"
#include "TheCalculater/math/fraction.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>
#include <memory>
#include <string_view>

namespace TheCalculater::math {
    class THECALC_API AnalyticExpression {
    public:
        class AbstractNode;

        AnalyticExpression() = default;
        AnalyticExpression(const AbstractNode& node);
        AnalyticExpression(const std::unique_ptr<AbstractNode>& node);
        AnalyticExpression(std::unique_ptr<AbstractNode>&& node);

        enum class NodeType : int8_t;

        class UnaryOperatorInterface;
        class BinaryOperatorInterface;

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

        static AnalyticExpression constant(const Fraction& value);
        static AnalyticExpression constant(Fraction&& value);
        static AnalyticExpression variable(std::string_view name);
        static AnalyticExpression variable(std::string&& name);

        std::unique_ptr<AbstractNode> base;
    };

    THECALCULATER_DEFINE_EXCEPTION(AnalyticExpressionSimplifyCException, std::logic_error);

    /**
     * @brief Format an analytic expression in LaTeX format.
     *
     * @param expr The expression to format.
     * @return std::string The formatted expression in LaTeX format.
     */
    THECALC_API std::string format(const AnalyticExpression& expr);

    /**
     * @brief Format an analytic expression in tree format (less readable)
     *
     * @param expr The expression to format.
     * @return std::string The formatted expression in tree format.
     */
    THECALC_API std::string formatTree(const AnalyticExpression& expr);

    enum class AnalyticExpression::NodeType : int8_t {
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

    class AnalyticExpression::AbstractNode {
    public:
        virtual ~AbstractNode() = default;

        /**
         * @brief Simplify the expression.
         *
         * @param context The context for simplification.
         * @return std::unique_ptr<AbstractNode> The simplified expression.
         * @throw AnalyticExpressionEvaluateException If something goes wrong during simplification. Check derived classes for specific reasons.
         */
        [[nodiscard]] virtual std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const = 0;

        /**
         * @brief Calculate the hash value of the expression.
         *
         * @warning Do NOT use it to compare two expressions for equality.
         * Only use it to sort expressions.
         *
         * @return size_t The hash value.
         */
        [[nodiscard]] virtual size_t hash() const = 0;

        [[nodiscard]] virtual std::unique_ptr<AbstractNode> clone() const = 0;
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
        [[nodiscard]] bool virtual rawEqualTo(const AbstractNode& other) const = 0;
    };

    class AnalyticExpression::UnaryOperatorInterface {
    public:
        virtual ~UnaryOperatorInterface() = default;

        [[nodiscard]] virtual const AbstractNode& firstOperand() const = 0;
    };

    class AnalyticExpression::BinaryOperatorInterface {
    public:
        virtual ~BinaryOperatorInterface() = default;

        [[nodiscard]] virtual const AbstractNode& firstOperand() const = 0;
        [[nodiscard]] virtual const AbstractNode& secondOperand() const = 0;
    };

    class THECALC_API AnalyticExpression::Constant : public AbstractNode {
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

        [[nodiscard]] size_t hash() const override;

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Constant>(value); }
        [[nodiscard]] NodeType type() const override { return NodeType::Constant; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Constant; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext&) const override { return clone(); }

        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;

        // Stop creating garbage object for simple comparisons. That's just suffering. These constants are here to help you and the compiler have a good day!
        static const Constant ZERO;
        static const Constant ONE;
    };

    class THECALC_API AnalyticExpression::Variable : public AbstractNode {
    public:
        std::string name;

        explicit Variable(std::string_view name)
            : name(name)
        { }
        explicit Variable(std::string&& name)
            : name(std::move(name))
        { }

        [[nodiscard]] size_t hash() const override;

        Variable(const AnalyticExpression::Variable& other) = delete;
        Variable(AnalyticExpression::Variable&& other) = default;
        Variable& operator=(const AnalyticExpression::Variable& other) = delete;
        Variable& operator=(AnalyticExpression::Variable&& other) = default;
        ~Variable() override = default;

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Variable>(name); }
        [[nodiscard]] NodeType type() const override { return NodeType::Variable; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Variable; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Infinity : public AbstractNode {
    public:
        Infinity() = default;
        Infinity(const Infinity& other) = delete;
        Infinity(Infinity&& other) = default;
        Infinity& operator=(const Infinity& other) = delete;
        Infinity& operator=(Infinity&& other) = default;
        ~Infinity() override = default;

        [[nodiscard]] size_t hash() const override { return 0xc3dc0c723e73cbc3; } // Hash of 'TheCalculater::math::AnalyticExpression::Infinity'

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Infinity>(); }
        [[nodiscard]] NodeType type() const override { return NodeType::Infinity; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Infinity; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext&) const override { return clone(); }
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override { return other.type() == NodeType::Infinity; }
    };

    class THECALC_API AnalyticExpression::Pi : public AbstractNode {
    public:
        Pi() = default;
        Pi(const Pi& other) = delete;
        Pi(Pi&& other) = default;
        Pi& operator=(const Pi& other) = delete;
        Pi& operator=(Pi&& other) = default;
        ~Pi() override = default;

        [[nodiscard]] size_t hash() const override { return 0x8c18f600b6867066; } // Hash of 'TheCalculater::math::AnalyticExpression::Pi'

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Pi>(); }
        [[nodiscard]] NodeType type() const override { return NodeType::Pi; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Pi; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override { return other.type() == NodeType::Pi; }
    };

    class THECALC_API AnalyticExpression::Euler : public AbstractNode {
    public:
        Euler() = default;
        Euler(const Euler& other) = delete;
        Euler(Euler&& other) = default;
        Euler& operator=(const Euler& other) = delete;
        Euler& operator=(Euler&& other) = default;
        ~Euler() override = default;

        [[nodiscard]] size_t hash() const override { return 0x573ab0792d7b9fca; } // Hash of 'TheCalculater::math::AnalyticExpression::Euler'

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Euler>(); }
        [[nodiscard]] NodeType type() const override { return NodeType::Euler; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Euler; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override { return other.type() == NodeType::Euler; }
    };

    class THECALC_API AnalyticExpression::ImaginaryUnit : public AbstractNode {
    public:
        ImaginaryUnit() = default;
        ImaginaryUnit(const ImaginaryUnit& other) = delete;
        ImaginaryUnit(ImaginaryUnit&& other) = default;
        ImaginaryUnit& operator=(const ImaginaryUnit& other) = delete;
        ImaginaryUnit& operator=(ImaginaryUnit&& other) = default;
        ~ImaginaryUnit() override = default;

        [[nodiscard]] size_t hash() const override { return 0x99506ad9db02af43; } // Hash of 'TheCalculater::math::AnalyticExpression::ImaginaryUnit'

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<ImaginaryUnit>(); }
        [[nodiscard]] NodeType type() const override { return NodeType::ImaginaryUnit; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::ImaginaryUnit; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext&) const override { return clone(); }
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override { return other.type() == NodeType::ImaginaryUnit; }
    };

    class THECALC_API AnalyticExpression::Undefined : public AbstractNode {
    public:
        Undefined() = default;
        Undefined(const Undefined& other) = delete;
        Undefined(Undefined&& other) = default;
        Undefined& operator=(const Undefined& other) = delete;
        Undefined& operator=(Undefined&& other) = default;
        ~Undefined() override = default;

        [[nodiscard]] size_t hash() const override { return 0x1e2e18b597856397; } // Hash of 'TheCalculater::math::AnalyticExpression::Undefined'

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Undefined>(); }
        [[nodiscard]] NodeType type() const override { return NodeType::Undefined; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Undefined; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext&) const override { return clone(); }
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override { return other.type() == NodeType::Undefined; }
    };

    class THECALC_API AnalyticExpression::Addition : public AbstractNode, public BinaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> left;
        std::unique_ptr<AbstractNode> right;

        explicit Addition(const std::unique_ptr<AbstractNode>& left, const std::unique_ptr<AbstractNode>& right)
            : left(left->clone()), right(right->clone())
        { }
        explicit Addition(std::unique_ptr<AbstractNode>&& left, std::unique_ptr<AbstractNode>&& right)
            : left(std::move(left)), right(std::move(right))
        { }

        Addition(const AnalyticExpression::Addition& other) = delete;
        Addition(AnalyticExpression::Addition&& other) = default;
        Addition& operator=(const AnalyticExpression::Addition& other) = delete;
        Addition& operator=(AnalyticExpression::Addition&& other) = default;
        ~Addition() override = default;

        [[nodiscard]] const AbstractNode& firstOperand() const override { return *left; }
        [[nodiscard]] const AbstractNode& secondOperand() const override { return *right; }

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Addition>(left->clone(), right->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Addition; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Addition; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Subtraction : public AbstractNode, public BinaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> left;
        std::unique_ptr<AbstractNode> right;

        explicit Subtraction(const std::unique_ptr<AbstractNode>& left, const std::unique_ptr<AbstractNode>& right)
            : left(left->clone()), right(right->clone())
        { }
        explicit Subtraction(std::unique_ptr<AbstractNode>&& left, std::unique_ptr<AbstractNode>&& right)
            : left(std::move(left)), right(std::move(right))
        { }

        Subtraction(const AnalyticExpression::Subtraction& other) = delete;
        Subtraction(AnalyticExpression::Subtraction&& other) = default;
        Subtraction& operator=(const AnalyticExpression::Subtraction& other) = delete;
        Subtraction& operator=(AnalyticExpression::Subtraction&& other) = default;
        ~Subtraction() override = default;

        [[nodiscard]] const AbstractNode& firstOperand() const override { return *left; }
        [[nodiscard]] const AbstractNode& secondOperand() const override { return *right; }

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Subtraction>(left->clone(), right->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Subtraction; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Subtraction; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Multiplication : public AbstractNode, public BinaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> left;
        std::unique_ptr<AbstractNode> right;

        explicit Multiplication(const std::unique_ptr<AbstractNode>& left, const std::unique_ptr<AbstractNode>& right)
            : left(left->clone()), right(right->clone())
        { }
        explicit Multiplication(std::unique_ptr<AbstractNode>&& left, std::unique_ptr<AbstractNode>&& right)
            : left(std::move(left)), right(std::move(right))
        { }

        Multiplication(const AnalyticExpression::Multiplication& other) = delete;
        Multiplication(AnalyticExpression::Multiplication&& other) = default;
        Multiplication& operator=(const AnalyticExpression::Multiplication& other) = delete;
        Multiplication& operator=(AnalyticExpression::Multiplication&& other) = default;
        ~Multiplication() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *left; }
        [[nodiscard]] const AbstractNode& secondOperand() const override { return *right; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Multiplication>(left->clone(), right->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Multiplication; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Multiplication; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    // We use division to represent fractions.
    class THECALC_API AnalyticExpression::Division : public AbstractNode, public BinaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> numerator;
        std::unique_ptr<AbstractNode> denominator;

        explicit Division(const std::unique_ptr<AbstractNode>& numerator, const std::unique_ptr<AbstractNode>& denominator)
            : numerator(numerator->clone()), denominator(denominator->clone())
        { }
        explicit Division(std::unique_ptr<AbstractNode>&& numerator, std::unique_ptr<AbstractNode>&& denominator)
            : numerator(std::move(numerator)), denominator(std::move(denominator))
        { }

        Division(const AnalyticExpression::Division& other) = delete;
        Division(AnalyticExpression::Division&& other) = default;
        Division& operator=(const AnalyticExpression::Division& other) = delete;
        Division& operator=(AnalyticExpression::Division&& other) = default;
        ~Division() override = default;

        [[nodiscard]] const AbstractNode& firstOperand() const override { return *numerator; }
        [[nodiscard]] const AbstractNode& secondOperand() const override { return *denominator; }

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Division>(numerator->clone(), denominator->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Division; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Division; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Negation : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Negation(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Negation(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Negation(const AnalyticExpression::Negation& other) = delete;
        Negation(AnalyticExpression::Negation&& other) = default;
        Negation& operator=(const AnalyticExpression::Negation& other) = delete;
        Negation& operator=(AnalyticExpression::Negation&& other) = default;
        ~Negation() override = default;

        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Negation>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Negation; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Negation; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Affirmation : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Affirmation(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Affirmation(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Affirmation(const AnalyticExpression::Affirmation& other) = delete;
        Affirmation(AnalyticExpression::Affirmation&& other) = default;
        Affirmation& operator=(const AnalyticExpression::Affirmation& other) = delete;
        Affirmation& operator=(AnalyticExpression::Affirmation&& other) = default;
        ~Affirmation() override = default;

        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Affirmation>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Affirmation; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Affirmation; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Power : public AbstractNode, public BinaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> base;
        std::unique_ptr<AbstractNode> exponent;

        explicit Power(const std::unique_ptr<AbstractNode>& base, const std::unique_ptr<AbstractNode>& exponent)
            : base(base->clone()), exponent(exponent->clone())
        { }
        explicit Power(std::unique_ptr<AbstractNode>&& base, std::unique_ptr<AbstractNode>&& exponent)
            : base(std::move(base)), exponent(std::move(exponent))
        { }

        Power(const AnalyticExpression::Power& other) = delete;
        Power(AnalyticExpression::Power&& other) = default;
        Power& operator=(const AnalyticExpression::Power& other) = delete;
        Power& operator=(AnalyticExpression::Power&& other) = default;
        ~Power() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *base; }
        [[nodiscard]] const AbstractNode& secondOperand() const override { return *exponent; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Power>(base->clone(), exponent->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Power; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Power; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Root : public AbstractNode, public BinaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> radicand;
        std::unique_ptr<AbstractNode> index;

        explicit Root(const std::unique_ptr<AbstractNode>& radicand, const std::unique_ptr<AbstractNode>& index)
            : radicand(radicand->clone()), index(index->clone())
        { }
        explicit Root(std::unique_ptr<AbstractNode>&& radicand, std::unique_ptr<AbstractNode>&& index)
            : radicand(std::move(radicand)), index(std::move(index))
        { }

        Root(const AnalyticExpression::Root& other) = delete;
        Root(AnalyticExpression::Root&& other) = default;
        Root& operator=(const AnalyticExpression::Root& other) = delete;
        Root& operator=(AnalyticExpression::Root&& other) = default;
        ~Root() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *radicand; }
        [[nodiscard]] const AbstractNode& secondOperand() const override { return *index; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Root>(radicand->clone(), index->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Root; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Root; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Factorial : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Factorial(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Factorial(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Factorial(const AnalyticExpression::Factorial& other) = delete;
        Factorial(AnalyticExpression::Factorial&& other) = default;
        Factorial& operator=(const AnalyticExpression::Factorial& other) = delete;
        Factorial& operator=(AnalyticExpression::Factorial&& other) = default;
        ~Factorial() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Factorial>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Factorial; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Factorial; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::AbsoluteValue : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit AbsoluteValue(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit AbsoluteValue(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        AbsoluteValue(const AnalyticExpression::AbsoluteValue& other) = delete;
        AbsoluteValue(AnalyticExpression::AbsoluteValue&& other) = default;
        AbsoluteValue& operator=(const AnalyticExpression::AbsoluteValue& other) = delete;
        AbsoluteValue& operator=(AnalyticExpression::AbsoluteValue&& other) = default;
        ~AbsoluteValue() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<AbsoluteValue>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::AbsoluteValue; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::AbsoluteValue; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Modulus : public AbstractNode, public BinaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> dividend;
        std::unique_ptr<AbstractNode> divisor;

        explicit Modulus(const std::unique_ptr<AbstractNode>& dividend, const std::unique_ptr<AbstractNode>& divisor)
            : dividend(dividend->clone()), divisor(divisor->clone())
        { }
        explicit Modulus(std::unique_ptr<AbstractNode>&& dividend, std::unique_ptr<AbstractNode>&& divisor)
            : dividend(std::move(dividend)), divisor(std::move(divisor))
        { }

        Modulus(const AnalyticExpression::Modulus& other) = delete;
        Modulus(AnalyticExpression::Modulus&& other) = default;
        Modulus& operator=(const AnalyticExpression::Modulus& other) = delete;
        Modulus& operator=(AnalyticExpression::Modulus&& other) = default;
        ~Modulus() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *dividend; }
        [[nodiscard]] const AbstractNode& secondOperand() const override { return *divisor; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Modulus>(dividend->clone(), divisor->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Modulus; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Modulus; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Logarithm : public AbstractNode, public BinaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> base;
        std::unique_ptr<AbstractNode> operand;

        explicit Logarithm(const std::unique_ptr<AbstractNode>& base, const std::unique_ptr<AbstractNode>& operand)
            : base(base->clone()), operand(operand->clone())
        { }
        explicit Logarithm(std::unique_ptr<AbstractNode>&& base, std::unique_ptr<AbstractNode>&& operand)
            : base(std::move(base)), operand(std::move(operand))
        { }

        Logarithm(const AnalyticExpression::Logarithm& other) = delete;
        Logarithm(AnalyticExpression::Logarithm&& other) = default;
        Logarithm& operator=(const AnalyticExpression::Logarithm& other) = delete;
        Logarithm& operator=(AnalyticExpression::Logarithm&& other) = default;
        ~Logarithm() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *base; }
        [[nodiscard]] const AbstractNode& secondOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Logarithm>(base->clone(), operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Logarithm; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Logarithm; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Sine : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Sine(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Sine(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Sine(const AnalyticExpression::Sine& other) = delete;
        Sine(AnalyticExpression::Sine&& other) = default;
        Sine& operator=(const AnalyticExpression::Sine& other) = delete;
        Sine& operator=(AnalyticExpression::Sine&& other) = default;
        ~Sine() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Sine>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Sine; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Sine; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Cosine : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Cosine(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Cosine(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Cosine(const AnalyticExpression::Cosine& other) = delete;
        Cosine(AnalyticExpression::Cosine&& other) = default;
        Cosine& operator=(const AnalyticExpression::Cosine& other) = delete;
        Cosine& operator=(AnalyticExpression::Cosine&& other) = default;
        ~Cosine() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Cosine>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Cosine; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Cosine; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Tangent : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Tangent(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Tangent(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Tangent(const AnalyticExpression::Tangent& other) = delete;
        Tangent(AnalyticExpression::Tangent&& other) = default;
        Tangent& operator=(const AnalyticExpression::Tangent& other) = delete;
        Tangent& operator=(AnalyticExpression::Tangent&& other) = default;
        ~Tangent() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Tangent>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Tangent; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Tangent; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Cotangent : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Cotangent(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Cotangent(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Cotangent(const AnalyticExpression::Cotangent& other) = delete;
        Cotangent(AnalyticExpression::Cotangent&& other) = default;
        Cotangent& operator=(const AnalyticExpression::Cotangent& other) = delete;
        Cotangent& operator=(AnalyticExpression::Cotangent&& other) = default;
        ~Cotangent() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Cotangent>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Cotangent; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Cotangent; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Secant : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Secant(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Secant(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Secant(const AnalyticExpression::Secant& other) = delete;
        Secant(AnalyticExpression::Secant&& other) = default;
        Secant& operator=(const AnalyticExpression::Secant& other) = delete;
        Secant& operator=(AnalyticExpression::Secant&& other) = default;
        ~Secant() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Secant>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Secant; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Secant; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Cosecant : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Cosecant(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Cosecant(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Cosecant(const AnalyticExpression::Cosecant& other) = delete;
        Cosecant(AnalyticExpression::Cosecant&& other) = default;
        Cosecant& operator=(const AnalyticExpression::Cosecant& other) = delete;
        Cosecant& operator=(AnalyticExpression::Cosecant&& other) = default;
        ~Cosecant() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Cosecant>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Cosecant; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Cosecant; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Arcsine : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Arcsine(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Arcsine(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Arcsine(const AnalyticExpression::Arcsine& other) = delete;
        Arcsine(AnalyticExpression::Arcsine&& other) = default;
        Arcsine& operator=(const AnalyticExpression::Arcsine& other) = delete;
        Arcsine& operator=(AnalyticExpression::Arcsine&& other) = default;
        ~Arcsine() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Arcsine>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Arcsine; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Arcsine; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Arccosine : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Arccosine(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Arccosine(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Arccosine(const AnalyticExpression::Arccosine& other) = delete;
        Arccosine(AnalyticExpression::Arccosine&& other) = default;
        Arccosine& operator=(const AnalyticExpression::Arccosine& other) = delete;
        Arccosine& operator=(AnalyticExpression::Arccosine&& other) = default;
        ~Arccosine() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Arccosine>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Arccosine; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Arccosine; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Arctangent : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Arctangent(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Arctangent(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Arctangent(const AnalyticExpression::Arctangent& other) = delete;
        Arctangent(AnalyticExpression::Arctangent&& other) = default;
        Arctangent& operator=(const AnalyticExpression::Arctangent& other) = delete;
        Arctangent& operator=(AnalyticExpression::Arctangent&& other) = default;
        ~Arctangent() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Arctangent>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Arctangent; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Arctangent; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Arccotangent : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Arccotangent(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Arccotangent(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Arccotangent(const AnalyticExpression::Arccotangent& other) = delete;
        Arccotangent(AnalyticExpression::Arccotangent&& other) = default;
        Arccotangent& operator=(const AnalyticExpression::Arccotangent& other) = delete;
        Arccotangent& operator=(AnalyticExpression::Arccotangent&& other) = default;
        ~Arccotangent() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Arccotangent>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Arccotangent; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Arccotangent; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Arcsecant : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Arcsecant(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Arcsecant(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Arcsecant(const AnalyticExpression::Arcsecant& other) = delete;
        Arcsecant(AnalyticExpression::Arcsecant&& other) = default;
        Arcsecant& operator=(const AnalyticExpression::Arcsecant& other) = delete;
        Arcsecant& operator=(AnalyticExpression::Arcsecant&& other) = default;
        ~Arcsecant() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Arcsecant>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Arcsecant; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Arcsecant; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::Arccosecant : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit Arccosecant(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit Arccosecant(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        Arccosecant(const AnalyticExpression::Arccosecant& other) = delete;
        Arccosecant(AnalyticExpression::Arccosecant&& other) = default;
        Arccosecant& operator=(const AnalyticExpression::Arccosecant& other) = delete;
        Arccosecant& operator=(AnalyticExpression::Arccosecant&& other) = default;
        ~Arccosecant() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Arccosecant>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Arccosecant; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Arccosecant; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    class THECALC_API AnalyticExpression::NaturalLogarithm : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        explicit NaturalLogarithm(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        explicit NaturalLogarithm(std::unique_ptr<AbstractNode>&& operand)
            : operand(std::move(operand))
        { }

        NaturalLogarithm(const AnalyticExpression::NaturalLogarithm& other) = delete;
        NaturalLogarithm(AnalyticExpression::NaturalLogarithm&& other) = default;
        NaturalLogarithm& operator=(const AnalyticExpression::NaturalLogarithm& other) = delete;
        NaturalLogarithm& operator=(AnalyticExpression::NaturalLogarithm&& other) = default;
        ~NaturalLogarithm() override = default;

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *operand; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<NaturalLogarithm>(operand->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::NaturalLogarithm; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::NaturalLogarithm; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const SimplifyContext& context) const override;
        [[nodiscard]] bool rawEqualTo(const AbstractNode& other) const override;
    };

    struct AnalyticExpression::SimplifyContext {
        using VariableContext = std::map<std::string, std::unique_ptr<AbstractNode>>;
        struct Config {
            /// @brief Whether to use approximation for irrational numbers (e.g. pi, e).
            /// If false, only rational numbers will be calculated.
            bool irrationalUseApproximation = false;
        };

        VariableContext vars;
        Config config;
        core::LoggerWrapper logger;
    };
} // namespace TheCalculater::math