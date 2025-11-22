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

    
} // namespace TheCalculater::math
