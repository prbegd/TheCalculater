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
#include "TheCalculater/math/fraction.hpp"
#include <memory>

namespace TheCalculater::math {
    class AnalyticExpression {
    public:
        class AbstractNode;
        enum class NodeType;

        class UnaryOperatorInterface;
        class BinaryOperatorInterface;

        class Constant;
        class Variable;
        class Infinity;
        class Pi;
        class Euler;
        class ImaginaryUnit;

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
        class Arcsine;
        class Arccosine;
        class Arctangent;

        using VariableContext = std::map<std::string, std::unique_ptr<AbstractNode>>;
        struct SimplifyConfig;

        std::unique_ptr<AbstractNode> root;
    };

    THECALCULATER_DEFINE_EXCEPTION(AnalyticExpressionEvaluateException, std::logic_error);
    
    enum class AnalyticExpression::NodeType { Constant, Variable, Infinity, Pi, Euler, ImaginaryUnit,
            Addition, Subtraction, Multiplication, Division,
            Negation, Affirmation, Power, Root, Factorial,
            AbsoluteValue, Modulus,
            Logarithm, NaturalLogarithm, Sine, Cosine, Tangent,
            Arcsine, Arccosine, Arctangent };

    class AnalyticExpression::AbstractNode {
    public:
        virtual ~AbstractNode() = default;

        /**
         * @brief Simplify the expression.
         * 
         * @param context The context of variables.
         * @param config The configuration for simplification.
         * @return std::unique_ptr<AbstractNode> The simplified expression.
         * @throw AnalyticExpressionEvaluateException If something goes wrong during simplification. Check derived classes for specific reasons.
         */
        [[nodiscard]] virtual std::unique_ptr<AbstractNode> simplify(const VariableContext& context, const SimplifyConfig& config) const = 0;
        [[nodiscard]] virtual std::unique_ptr<AbstractNode> clone() const = 0;
        [[nodiscard]] virtual NodeType type() const = 0;
    };

    class AnalyticExpression::UnaryOperatorInterface {
    public:
        virtual ~UnaryOperatorInterface() = default;

        [[nodiscard]] virtual std::unique_ptr<AbstractNode> operand() const = 0;
    };

    class AnalyticExpression::BinaryOperatorInterface {
    public:
        virtual ~BinaryOperatorInterface() = default;

        [[nodiscard]] virtual std::unique_ptr<AbstractNode> firstOperand() const = 0;
        [[nodiscard]] virtual std::unique_ptr<AbstractNode> secondOperand() const = 0;
    };

    class AnalyticExpression::Constant : public AbstractNode {
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

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig&) const override
        { return clone(); }
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Constant>(value); }
        [[nodiscard]] NodeType type() const override { return NodeType::Constant; }
    };

    class AnalyticExpression::Variable : public AbstractNode {
    public:
        std::string name;

        explicit Variable(const std::string& name)
            : name(name)
        { }
        explicit Variable(std::string&& name)
            : name(std::move(name))
        { }

        Variable(const AnalyticExpression::Variable& other) = delete;
        Variable(AnalyticExpression::Variable&& other) = default;
        Variable& operator=(const AnalyticExpression::Variable& other) = delete;
        Variable& operator=(AnalyticExpression::Variable&& other) = default;
        ~Variable() override = default;

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext& context, const SimplifyConfig&) const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Variable>(name); }
        [[nodiscard]] NodeType type() const override { return NodeType::Variable; }
    };

    class AnalyticExpression::Infinity : public AbstractNode {
    public:
        Infinity() = default;
        Infinity(const Infinity& other) = delete;
        Infinity(Infinity&& other) = default;
        Infinity& operator=(const Infinity& other) = delete;
        Infinity& operator=(Infinity&& other) = default;
        ~Infinity() override = default;

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig&) const override { return clone(); }
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Infinity>(); }
        [[nodiscard]] NodeType type() const override { return NodeType::Infinity; }
    };

    class AnalyticExpression::Pi : public AbstractNode {
    public:
        Pi() = default;
        Pi(const Pi& other) = delete;
        Pi(Pi&& other) = default;
        Pi& operator=(const Pi& other) = delete;
        Pi& operator=(Pi&& other) = default;
        ~Pi() override = default;

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig& config) const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Pi>(); }
        [[nodiscard]] NodeType type() const override { return NodeType::Pi; }
    };

    class AnalyticExpression::Euler : public AbstractNode {
    public:
        Euler() = default;
        Euler(const Euler& other) = delete;
        Euler(Euler&& other) = default;
        Euler& operator=(const Euler& other) = delete;
        Euler& operator=(Euler&& other) = default;
        ~Euler() override = default;

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig& config) const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Euler>(); }
        [[nodiscard]] NodeType type() const override { return NodeType::Euler; }
    };

    class AnalyticExpression::ImaginaryUnit : public AbstractNode {
    public:
        ImaginaryUnit() = default;
        ImaginaryUnit(const ImaginaryUnit& other) = delete;
        ImaginaryUnit(ImaginaryUnit&& other) = default;
        ImaginaryUnit& operator=(const ImaginaryUnit& other) = delete;
        ImaginaryUnit& operator=(ImaginaryUnit&& other) = default;
        ~ImaginaryUnit() override = default;

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig&) const override { return clone(); }
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<ImaginaryUnit>(); }
        [[nodiscard]] NodeType type() const override { return NodeType::ImaginaryUnit; }
    };

    class AnalyticExpression::Addition : public AbstractNode, public BinaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> left;
        std::unique_ptr<AbstractNode> right;

        Addition(const std::unique_ptr<AbstractNode>& left, const std::unique_ptr<AbstractNode>& right)
            : left(left->clone()), right(right->clone())
        { }
        Addition(std::unique_ptr<AbstractNode>&& left, std::unique_ptr<AbstractNode>&& right)
            : left(std::move(left)), right(std::move(right))
        { }

        Addition(const AnalyticExpression::Addition& other) = delete;
        Addition(AnalyticExpression::Addition&& other) = default;
        Addition& operator=(const AnalyticExpression::Addition& other) = delete;
        Addition& operator=(AnalyticExpression::Addition&& other) = default;
        ~Addition() override = default;

        [[nodiscard]] std::unique_ptr<AbstractNode> firstOperand() const override { return left->clone(); }
        [[nodiscard]] std::unique_ptr<AbstractNode> secondOperand() const override { return right->clone(); }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext& context, const SimplifyConfig& config) const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Addition>(left->clone(), right->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Addition; }
    };

    class AnalyticExpression::Subtraction : public AbstractNode, public BinaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> left;
        std::unique_ptr<AbstractNode> right;

        Subtraction(const std::unique_ptr<AbstractNode>& left, const std::unique_ptr<AbstractNode>& right)
            : left(left->clone()), right(right->clone())
        { }
        Subtraction(std::unique_ptr<AbstractNode>&& left, std::unique_ptr<AbstractNode>&& right)
            : left(std::move(left)), right(std::move(right))
        { }

        Subtraction(const AnalyticExpression::Subtraction& other) = delete;
        Subtraction(AnalyticExpression::Subtraction&& other) = default;
        Subtraction& operator=(const AnalyticExpression::Subtraction& other) = delete;
        Subtraction& operator=(AnalyticExpression::Subtraction&& other) = default;
        ~Subtraction() override = default;
        
        [[nodiscard]] std::unique_ptr<AbstractNode> firstOperand() const override { return left->clone(); }
        [[nodiscard]] std::unique_ptr<AbstractNode> secondOperand() const override { return right->clone(); }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext& context, const SimplifyConfig& config) const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Subtraction>(left->clone(), right->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Subtraction; }
    };

    class AnalyticExpression::Multiplication : public AbstractNode, public BinaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> left;
        std::unique_ptr<AbstractNode> right;

        Multiplication(const std::unique_ptr<AbstractNode>& left, const std::unique_ptr<AbstractNode>& right)
            : left(left->clone()), right(right->clone())
        { }
        Multiplication(std::unique_ptr<AbstractNode>&& left, std::unique_ptr<AbstractNode>&& right)
            : left(std::move(left)), right(std::move(right))
        { }

        Multiplication(const AnalyticExpression::Multiplication& other) = delete;
        Multiplication(AnalyticExpression::Multiplication&& other) = default;
        Multiplication& operator=(const AnalyticExpression::Multiplication& other) = delete;
        Multiplication& operator=(AnalyticExpression::Multiplication&& other) = default;
        ~Multiplication() override = default;

        [[nodiscard]] std::unique_ptr<AbstractNode> firstOperand() const override { return left->clone(); }
        [[nodiscard]] std::unique_ptr<AbstractNode> secondOperand() const override { return right->clone(); }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext& context, const SimplifyConfig& config) const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Multiplication>(left->clone(), right->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Multiplication; }
    };

    // We use division to represent fractions.
    class AnalyticExpression::Division : public AbstractNode, public BinaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> numerator;
        std::unique_ptr<AbstractNode> denominator;

        Division(const std::unique_ptr<AbstractNode>& numerator, const std::unique_ptr<AbstractNode>& denominator)
            : numerator(numerator->clone()), denominator(denominator->clone())
        { }
        Division(std::unique_ptr<AbstractNode>&& numerator, std::unique_ptr<AbstractNode>&& denominator)
            : numerator(std::move(numerator)), denominator(std::move(denominator))
        { }

        Division(const AnalyticExpression::Division& other) = delete;
        Division(AnalyticExpression::Division&& other) = default;
        Division& operator=(const AnalyticExpression::Division& other) = delete;
        Division& operator=(AnalyticExpression::Division&& other) = default;
        ~Division() override = default;

        [[nodiscard]] std::unique_ptr<AbstractNode> firstOperand() const override { return numerator->clone(); }
        [[nodiscard]] std::unique_ptr<AbstractNode> secondOperand() const override { return denominator->clone(); }

        /// @throw AnalyticExpressionEvaluateException If encountered 0/0.
        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext& context, const SimplifyConfig& config) const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Division>(numerator->clone(), denominator->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Division; }
    };

    struct AnalyticExpression::SimplifyConfig {
        /// @brief Whether to use approximation for irrational numbers (e.g. pi, e).
        /// If false, only rational numbers will be calculated.
        bool irrationalUseApproximation = false;
    };

} // namespace TheCalculater::math