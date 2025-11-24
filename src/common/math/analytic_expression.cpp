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
#include "boost/container_hash/hash.hpp"
#include <memory>

namespace TheCalculater::math {
    [[nodiscard]] size_t AnalyticExpression::Constant::hash() const
    {
        size_t seed = 0x3361e811604a8be7; // Hash of 'TheCalculater::math::AnalyticExpression::Constant'
        boost::hash_combine(seed, value);
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Variable::hash() const
    {
        size_t seed = 0xe60cbdcfe41d881a; // Hash of 'TheCalculater::math::AnalyticExpression::Variable
        boost::hash_combine(seed, name);
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Addition::hash() const
    {
        size_t seed = 0x26b57e0cad6d1c3; // Hash of 'TheCalculater::math::AnalyticExpression::Addition'
        boost::hash_combine(seed, left->hash());
        boost::hash_combine(seed, right->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Subtraction::hash() const
    {
        size_t seed = 0xf1563e6038e40c35; // Hash of 'TheCalculater::math::AnalyticExpression::Subtraction'
        boost::hash_combine(seed, left->hash());
        boost::hash_combine(seed, right->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Multiplication::hash() const
    {
        size_t seed = 0x95d1ec6364d57dc8; // Hash of 'TheCalculater::math::AnalyticExpression::Multiplication'
        boost::hash_combine(seed, left->hash());
        boost::hash_combine(seed, right->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Division::hash() const
    {
        size_t seed = 0x2d76e0229be33792; // Hash of 'TheCalculater::math::AnalyticExpression::Division'
        boost::hash_combine(seed, numerator->hash());
        boost::hash_combine(seed, denominator->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Negation::hash() const
    {
        size_t seed = 0xe09ccb459549b2d; // Hash of 'TheCalculater::math::AnalyticExpression::Negation'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }

    namespace {
    namespace _d_normalize {
        template <typename T>
        std::vector<std::unique_ptr<AnalyticExpression::AbstractNode>> collectTermsFor(const T& node)
            requires(std::is_same_v<T, AnalyticExpression::Addition> || std::is_same_v<T, AnalyticExpression::Multiplication>)
        {
            static std::function<void(const AnalyticExpression::AbstractNode& node, std::vector<std::unique_ptr<AnalyticExpression::AbstractNode>>& result)> collect = [&](const AnalyticExpression::AbstractNode& node, std::vector<std::unique_ptr<AnalyticExpression::AbstractNode>>& result) {
                if (node.type() == T::staticType()) {
                    const auto& binOp = static_cast<const T&>(node);
                    collect(binOp.firstOperand(), result);
                    collect(binOp.secondOperand(), result);
                } else {
                    result.push_back(node.clone());
                }
            };

            std::vector<std::unique_ptr<AnalyticExpression::AbstractNode>> result;
            collect(*node, result);
            return result;
        }
        template <typename T>
        std::unique_ptr<AnalyticExpression::AbstractNode> rebuildTree(std::vector<std::unique_ptr<AnalyticExpression::AbstractNode>>&& terms)
            requires(std::is_same_v<T, AnalyticExpression::Addition> || std::is_same_v<T, AnalyticExpression::Multiplication>)
        {
            if (terms.size() == 1) {
                return std::move(terms[0]);
            }

            // No sure how to rebuild the tree yet.
        }
    }
    } // namespace ::_d_normalize

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Addition::normalize() const
    {
        auto leftNorm = left->normalize();
        auto rightNorm = right->normalize();
    }

    std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Variable::simplifyImpl(const VariableContext& context, const SimplifyConfig&) const
    {
        auto it = context.find(name);
        if (it == context.end()) {
            return clone();
        }
        return it->second->clone();
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Pi::simplifyImpl(const VariableContext&, const SimplifyConfig& config) const
    {
        if (config.irrationalUseApproximation) {
            return std::make_unique<Constant>(pi());
        }
        return clone();
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Euler::simplifyImpl(const VariableContext&, const SimplifyConfig& config) const
    {
        if (config.irrationalUseApproximation) {
            return std::make_unique<Constant>(e());
        }
        return clone();
    }

} // namespace TheCalculater::math
