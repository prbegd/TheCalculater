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
        class Arcsine;
        class Arccosine;
        class Arctangent;

        using VariableContext = std::map<std::string, std::unique_ptr<AbstractNode>>;
        struct SimplifyConfig;

        std::unique_ptr<AbstractNode> root;
    };

    THECALCULATER_DEFINE_EXCEPTION(AnalyticExpressionEvaluateException, std::logic_error);

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
        Arcsine,
        Arccosine,
        Arctangent,
        ImaginaryUnit
    };

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
         * @brief Compare two expressions for sorting. 
         *
         * @warning This function is NOT for checking equality of two expressions. (or the result may be confusing.)
         * 
         * @param a The first expression.
         * @param b The second expression.
         * @return int Negative if a < b, 0 if a == b, positive if a > b.
         */
        [[nodiscard]] static int sortCompare(const AbstractNode& a, const AbstractNode& b);
    };

    class AnalyticExpression::UnaryOperatorInterface {
    public:
        virtual ~UnaryOperatorInterface() = default;

        [[nodiscard]] virtual const AbstractNode& firstOperand() const = 0;
    };

    class AnalyticExpression::BinaryOperatorInterface {
    public:
        using FlatTerms = std::vector<std::pair<std::unique_ptr<AnalyticExpression::AbstractNode>,bool>>; 

        virtual ~BinaryOperatorInterface() = default;

        [[nodiscard]] virtual const AbstractNode& firstOperand() const = 0;
        [[nodiscard]] virtual const AbstractNode& secondOperand() const = 0;
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

        [[nodiscard]] size_t hash() const override;

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Constant>(value); }
        [[nodiscard]] NodeType type() const override { return NodeType::Constant; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Constant; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig&) const override { return clone(); }
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

        [[nodiscard]] size_t hash() const override;

        Variable(const AnalyticExpression::Variable& other) = delete;
        Variable(AnalyticExpression::Variable&& other) = default;
        Variable& operator=(const AnalyticExpression::Variable& other) = delete;
        Variable& operator=(AnalyticExpression::Variable&& other) = default;
        ~Variable() override = default;

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Variable>(name); }
        [[nodiscard]] NodeType type() const override { return NodeType::Variable; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Variable; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext& context, const SimplifyConfig&) const override;
    };

    class AnalyticExpression::Infinity : public AbstractNode {
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

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig&) const override { return clone(); }
    };

    class AnalyticExpression::Pi : public AbstractNode {
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

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig&) const override;
    };

    class AnalyticExpression::Euler : public AbstractNode {
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

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig&) const override;
    };

    class AnalyticExpression::ImaginaryUnit : public AbstractNode {
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

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig&) const override { return clone(); }
    };

    class AnalyticExpression::Undefined : public AbstractNode {
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
        

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig&) const override { return clone(); }
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

        [[nodiscard]] const AbstractNode& firstOperand() const override { return *left; }
        [[nodiscard]] const AbstractNode& secondOperand() const override { return *right; }

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Addition>(left->clone(), right->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Addition; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Addition; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig&) const override;
    
    private:
        static void simplify_combineConstantTerms(std::vector<std::unique_ptr<AnalyticExpression::AbstractNode>>& terms);
        static std::vector<std::unique_ptr<AbstractNode>>::iterator simplify_combineVariableTerms(std::vector<std::unique_ptr<AbstractNode>>& terms);

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

        [[nodiscard]] const AbstractNode& firstOperand() const override { return *left; }
        [[nodiscard]] const AbstractNode& secondOperand() const override { return *right; }

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Subtraction>(left->clone(), right->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Subtraction; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Subtraction; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig&) const override;
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

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] const AbstractNode& firstOperand() const override { return *left; }
        [[nodiscard]] const AbstractNode& secondOperand() const override { return *right; }

        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Multiplication>(left->clone(), right->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Multiplication; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Multiplication; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig&) const override;
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

        [[nodiscard]] const AbstractNode& firstOperand() const override { return *numerator; }
        [[nodiscard]] const AbstractNode& secondOperand() const override { return *denominator; }

        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] std::unique_ptr<AbstractNode> clone() const override { return std::make_unique<Division>(numerator->clone(), denominator->clone()); }
        [[nodiscard]] NodeType type() const override { return NodeType::Division; }
        [[nodiscard]] constexpr static NodeType staticType() { return NodeType::Division; }

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig&) const override;
    };

    class AnalyticExpression::Negation : public AbstractNode, public UnaryOperatorInterface {
    public:
        std::unique_ptr<AbstractNode> operand;

        Negation(const std::unique_ptr<AbstractNode>& operand)
            : operand(operand->clone())
        { }
        Negation(std::unique_ptr<AbstractNode>&& operand)
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

        [[nodiscard]] std::unique_ptr<AbstractNode> simplify(const VariableContext&, const SimplifyConfig&) const override;
    };

    struct AnalyticExpression::SimplifyConfig {
        /// @brief Whether to use approximation for irrational numbers (e.g. pi, e).
        /// If false, only rational numbers will be calculated.
        bool irrationalUseApproximation = false;
    };

} // namespace TheCalculater::math