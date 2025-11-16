/**
 * @file analytic_expression.cpp
 * @author prbegd
 * @brief Implementation of analytic expression class and related functions.
 * @date 2025-11-12
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */

#include "TheCalculater/math/analytic_expression.hpp"
#include "TheCalculater/math/fraction.hpp"
#include <memory>

namespace TheCalculater::math {

    std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Variable::simplify(const VariableContext& context, const SimplifyConfig&) const
    {
        auto it = context.find(name);
        if (it == context.end()) {
            return clone();
        }
        return it->second->clone();
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Pi::simplify(const VariableContext&, const SimplifyConfig& config) const
    {
        if (config.irrationalUseApproximation) {
            return std::make_unique<Constant>(pi());
        }
        return clone();
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Euler::simplify(const VariableContext&, const SimplifyConfig& config) const
    {
        if (config.irrationalUseApproximation) {
            return std::make_unique<Constant>(e());
        }
        return clone();
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Addition::simplify(const VariableContext& context, const SimplifyConfig& config) const
    {
        auto leftSimplified = left->simplify(context, config);
        auto rightSimplified = right->simplify(context, config);

        // Both sides are constants, so we can perform the operation directly.
        if (leftSimplified->type() == NodeType::Constant && rightSimplified->type() == NodeType::Constant) {
            auto* leftConst = static_cast<Constant*>(leftSimplified.get());
            auto* rightConst = static_cast<Constant*>(rightSimplified.get());
            return std::make_unique<Constant>(leftConst->value + rightConst->value);
        }

        return std::make_unique<Addition>(std::move(leftSimplified), std::move(rightSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Subtraction::simplify(const VariableContext& context, const SimplifyConfig& config) const
    {
        auto leftSimplified = left->simplify(context, config);
        auto rightSimplified = right->simplify(context, config);

        // Both sides are constants, so we can perform the operation directly.
        if (leftSimplified->type() == NodeType::Constant && rightSimplified->type() == NodeType::Constant) {
            auto* leftConst = static_cast<Constant*>(leftSimplified.get());
            auto* rightConst = static_cast<Constant*>(rightSimplified.get());
            return std::make_unique<Constant>(leftConst->value - rightConst->value);
        }

        return std::make_unique<Subtraction>(std::move(leftSimplified), std::move(rightSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Multiplication::simplify(const VariableContext& context, const SimplifyConfig& config) const
    {
        auto leftSimplified = left->simplify(context, config);
        auto rightSimplified = right->simplify(context, config);

        // Both sides are constants, so we can perform the operation directly.
        if (leftSimplified->type() == NodeType::Constant && rightSimplified->type() == NodeType::Constant) {
            const auto* leftConst = static_cast<Constant*>(leftSimplified.get());
            const auto* rightConst = static_cast<Constant*>(rightSimplified.get());
            return std::make_unique<Constant>(leftConst->value * rightConst->value);
        }
        // Only one side is constant
        if (leftSimplified->type() == NodeType::Constant || rightSimplified->type() == NodeType::Constant) {
            const auto* constSide = static_cast<Constant*>((leftSimplified->type() == NodeType::Constant) ? leftSimplified.get() : rightSimplified.get());
            const auto* otherSide = (leftSimplified->type() == NodeType::Constant) ? rightSimplified.get() : leftSimplified.get();

            if (constSide->value == 0) {
                return std::make_unique<Constant>(0);
            } else if (constSide->value == 1) {
                return otherSide->clone();
            }

            switch (otherSide->type()) {
            // Process law of distributivity (a * (b + c) = a*b + a*c or a * (b - c) = a*b - a*c)
            case NodeType::Addition:
            case NodeType::Subtraction: {
                const BinaryOperatorInterface* addSubSide = nullptr;
                if (otherSide->type() == NodeType::Addition) {
                    addSubSide = static_cast<const Addition*>(otherSide);
                } else {
                    addSubSide = static_cast<const Subtraction*>(otherSide);
                }
                std::unique_ptr<AbstractNode> resultLeft = Multiplication(constSide->clone(), addSubSide->firstOperand().clone()).simplify(context, config);
                std::unique_ptr<AbstractNode> resultRight = Multiplication(constSide->clone(), addSubSide->secondOperand().clone()).simplify(context, config);
                if (otherSide->type() == NodeType::Addition) {
                    return Addition(std::move(resultLeft), std::move(resultRight)).simplify(context, config);
                } else {
                    return Subtraction(std::move(resultLeft), std::move(resultRight)).simplify(context, config);
                }
                break;
            }
            case NodeType::Multiplication: {
                const auto* multSide = static_cast<const Multiplication*>(otherSide);

                // Process law of associativity a * (b * c) = (a * b) * c
                if (multSide->left->type() == NodeType::Constant || multSide->right->type() == NodeType::Constant) {
                    const auto* multSideConst = static_cast<const Constant*>((multSide->left->type() == NodeType::Constant) ? multSide->left.get() : multSide->right.get());
                    const auto* multSideOther = (multSide->left->type() == NodeType::Constant) ? multSide->right.get() : multSide->left.get();

                    return Multiplication(Multiplication(constSide->clone(), multSideConst->clone()).simplify(context, config), multSideOther->clone()).simplify(context, config);
                }

                break;
            }
            case NodeType::Division: {
                const auto* divSide = static_cast<const AnalyticExpression::Division*>(otherSide);
                
                break;
            }
            default:
                break;
            }
        }

        return std::make_unique<Multiplication>(std::move(leftSimplified), std::move(rightSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Division::simplify(const VariableContext& context, const SimplifyConfig& config) const
    {
        auto numerSimplified = numerator->simplify(context, config);
        auto denoSimplified = denominator->simplify(context, config);

        // Both sides are constants, so we can perform the operation directly.
        if (numerSimplified->type() == NodeType::Constant && denoSimplified->type() == NodeType::Constant) {
            auto* numerConst = static_cast<Constant*>(numerSimplified.get());
            auto* denoConst = static_cast<Constant*>(denoSimplified.get());
            if (denoConst->value == 0) {
                if (numerConst->value == 0)
                    throwEx(AnalyticExpressionEvaluateException(("{Undefined} 0 / 0 is undefined.")));
                else
                    return std::make_unique<Infinity>();
            }
            return std::make_unique<Constant>(numerConst->value / denoConst->value);
        }

        return std::make_unique<Division>(std::move(numerSimplified), std::move(denoSimplified));
    }
} // namespace TheCalculater::math
