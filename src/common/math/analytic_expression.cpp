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


    std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Variable::simplify(const VariableContext& context, const SimplifyConfig& ) const
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
            auto* leftConst = static_cast<AnalyticExpression::Constant*>(leftSimplified.get());
            auto* rightConst = static_cast<AnalyticExpression::Constant*>(rightSimplified.get());
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
            auto* leftConst = static_cast<AnalyticExpression::Constant*>(leftSimplified.get());
            auto* rightConst = static_cast<AnalyticExpression::Constant*>(rightSimplified.get());
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
            const auto* leftConst = static_cast<AnalyticExpression::Constant*>(leftSimplified.get());
            const auto* rightConst = static_cast<AnalyticExpression::Constant*>(rightSimplified.get());
            return std::make_unique<Constant>(leftConst->value * rightConst->value);
        } else if (leftSimplified->type() == NodeType::Constant || rightSimplified->type() == NodeType::Constant) {
            const auto* constSide = static_cast<Constant*>((leftSimplified->type() == NodeType::Constant) ? leftSimplified.get() : rightSimplified.get());
            const auto* otherSide = (leftSimplified->type() == NodeType::Constant) ? rightSimplified.get() : leftSimplified.get();

            // Process law of distributivity (a * (b + c) = a*b + a*c or a * (b - c) = a*b - a*c)
            if (otherSide->type() == NodeType::Addition || otherSide->type() == NodeType::Subtraction) {
                const BinaryOperatorInterface* addSubSide = nullptr;
                if (otherSide->type() == NodeType::Addition) {
                    addSubSide = static_cast<const AnalyticExpression::Addition*>(otherSide);
                } else {
                    addSubSide = static_cast<const AnalyticExpression::Subtraction*>(otherSide);
                }
                std::unique_ptr<AbstractNode> resultLeft = std::make_unique<Multiplication>(constSide->clone(), addSubSide->firstOperand()->clone());
                std::unique_ptr<AbstractNode> resultRight = std::make_unique<Multiplication>(constSide->clone(), addSubSide->secondOperand()->clone());
                if (otherSide->type() == NodeType::Addition) {
                    return std::make_unique<Addition>(std::move(resultLeft), std::move(resultRight));
                } else {
                    return std::make_unique<Subtraction>(std::move(resultLeft), std::move(resultRight));
                }
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
            auto* numerConst = static_cast<AnalyticExpression::Constant*>(numerSimplified.get());
            auto* denoConst = static_cast<AnalyticExpression::Constant*>(denoSimplified.get());
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
}
