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
#include "TheCalculater/settings.hpp"
#include "boost/container_hash/hash.hpp"
#include <algorithm>
#include <memory>
#include <sec_api/string_s.h>

namespace TheCalculater::math {
    [[nodiscard]] int AnalyticExpression::AbstractNode::sortCompare(const AbstractNode& a, const AbstractNode& b)
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
            return std::make_unique<Constant>(settings::readDecimal("calculating.pi"));
        }
        return clone();
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Euler::simplify(const VariableContext&, const SimplifyConfig& config) const
    {
        if (config.irrationalUseApproximation) {
            return std::make_unique<Constant>(settings::readDecimal("calculating.e"));
        }
        return clone();
    }

    namespace {
    namespace _d_simplify {
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
            collect(node, result);
            return result;
        }
        template <typename T>
        std::vector<std::unique_ptr<AnalyticExpression::AbstractNode>> flatten(const T& node)
            requires(std::is_same_v<T, AnalyticExpression::Addition> || std::is_same_v<T, AnalyticExpression::Multiplication>)
        {
            auto terms = collectTermsFor<T>(node);
            std::sort(terms.begin(), terms.end(), [](const auto& a, const auto& b) { return AnalyticExpression::AbstractNode::sortCompare(*a, *b) < 0; });
            return terms;
        }
        template <typename T>
        std::unique_ptr<AnalyticExpression::AbstractNode> rebuildTree(std::vector<std::unique_ptr<AnalyticExpression::AbstractNode>>&& terms)
            requires(std::is_same_v<T, AnalyticExpression::Addition> || std::is_same_v<T, AnalyticExpression::Multiplication>)
        {
            if (terms.empty()) {
                return std::make_unique<AnalyticExpression::Constant>(0);
            }

            std::unique_ptr<AnalyticExpression::AbstractNode> result = std::move(terms.back());

            for (size_t i = terms.size() - 1; i-- > 0;) {
                result = std::make_unique<T>(std::move(terms[i]), std::move(result));
            }

            return result;
        }
    }
    } // namespace ::_d_simplify

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Addition::simplify(const VariableContext& context, const SimplifyConfig& config) const
    {
        using namespace _d_simplify;

        auto leftSimplified = firstOperand().simplify(context, config);
        auto rightSimplified = secondOperand().simplify(context, config);

        std::vector<std::unique_ptr<AnalyticExpression::AbstractNode>> terms = flatten<Addition>(*this);

        // If there's a undefined term, it'll be the first one after sorting. Same for infinity.
        if (terms[0]->type() == NodeType::Undefined)
            return std::make_unique<Undefined>();
        if (terms[0]->type() == NodeType::Infinity)
            return std::make_unique<Infinity>();

        // If there're less than 2 constant terms, we can't combine them.
        if (terms[1]->type() == NodeType::Constant) simplify_combineConstantTerms(terms);

        return rebuildTree<Addition>(std::move(terms));
    }
    void AnalyticExpression::Addition::simplify_combineConstantTerms(std::vector<std::unique_ptr<AnalyticExpression::AbstractNode>>& terms)
    {
        auto constantsEnd = std::find_if_not(terms.begin() + 1, terms.end(), [](const std::unique_ptr<AbstractNode>& elem) {
            return elem->type() == NodeType::Constant;
        });

        Fraction sum = 0;
        for (auto it = terms.begin() + 1; it != constantsEnd; ++it) {
            const auto& constant = static_cast<const Constant&>(**it);
            sum += constant.value;
        }
        terms.erase(terms.begin() + 1, constantsEnd);
        terms[0] = std::make_unique<Constant>(sum);
    }
} // namespace TheCalculater::math
