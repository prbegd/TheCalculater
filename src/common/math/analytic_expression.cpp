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

            // x * 0 = 0, x * 1 = x
            if (constSide->value == 0) {
                return std::make_unique<Constant>(0);
            } else if (constSide->value == 1) {
                return otherSide->clone();
            } else if (constSide->value == -1) {
                return Negation(otherSide->clone()).simplify(context, config);
            }

            switch (otherSide->type()) {
            // If the other side is addition or subtraction, and it has a constant operand (we can directly multiply it), we can apply the distributive law.
            case NodeType::Addition: {
                const auto& addSide = static_cast<const Addition&>(*otherSide);
                if (addSide.left->type() == NodeType::Constant || addSide.right->type() == NodeType::Constant)
                    return lawOfDistributeSimplify(context, config, *constSide, addSide);
                break;
            }
            case NodeType::Subtraction: {
                const auto& subSide = static_cast<const Subtraction&>(*otherSide);
                if (subSide.left->type() == NodeType::Constant || subSide.right->type() == NodeType::Constant)
                    return lawOfDistributeSimplify(context, config, *constSide, static_cast<const Subtraction&>(*otherSide));
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

                return Division(Multiplication(constSide->clone(), divSide->numerator->clone()).simplify(context, config), divSide->denominator->clone()).simplify(context, config);
            }
            default:
                break;
            }
        }

        if (leftSimplified->type() == NodeType::Undefined || rightSimplified->type() == NodeType::Undefined) {
            return std::make_unique<Undefined>();
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
                    return std::make_unique<Undefined>();
                else
                    return std::make_unique<Infinity>();
            }
            return std::make_unique<Constant>(numerConst->value / denoConst->value);
        }

        return std::make_unique<Division>(std::move(numerSimplified), std::move(denoSimplified));
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Negation::simplify(const VariableContext& context, const SimplifyConfig& config) const
    {
        auto operandSimplified = operand->simplify(context, config);

        if (operandSimplified->type() == NodeType::Constant) {
            auto* operandConst = static_cast<Constant*>(operandSimplified.get());
            return std::make_unique<Constant>(-operandConst->value);
        }
    }
} // namespace TheCalculater::math
