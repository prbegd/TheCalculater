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
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

namespace TheCalculater::math {
    const AnalyticExpression::Constant AnalyticExpression::Constant::ZERO(0);
    namespace {
        [[nodiscard]] int sortingCompare(const AnalyticExpression::AbstractNode& a, const AnalyticExpression::AbstractNode& b)
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
    } // namespace

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

    bool AnalyticExpression::Constant::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Constant) {
            return false;
        }
        const auto& o = static_cast<const Constant&>(other);
        return value == o.value;
    }
    bool AnalyticExpression::Variable::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Variable) {
            return false;
        }
        const auto& o = static_cast<const Variable&>(other);
        return name == o.name;
    }
    bool AnalyticExpression::Addition::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Addition) {
            return false;
        }
        const auto& o = static_cast<const Addition&>(other);
        return left->rawEqualTo(*o.left) && right->rawEqualTo(*o.right);
    }
    bool AnalyticExpression::Subtraction::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Subtraction) {
            return false;
        }
        const auto& o = static_cast<const Subtraction&>(other);
        return left->rawEqualTo(*o.left) && right->rawEqualTo(*o.right);
    }
    bool AnalyticExpression::Multiplication::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Multiplication) {
            return false;
        }
        const auto& o = static_cast<const Multiplication&>(other);
        return left->rawEqualTo(*o.left) && right->rawEqualTo(*o.right);
    }
    bool AnalyticExpression::Division::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Division) {
            return false;
        }
        const auto& o = static_cast<const Division&>(other);
        return numerator->rawEqualTo(*o.numerator) && denominator->rawEqualTo(*o.denominator);
    }
    bool AnalyticExpression::Negation::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Negation) {
            return false;
        }
        const auto& o = static_cast<const Negation&>(other);
        return operand->rawEqualTo(*o.operand);
    }

    std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Variable::simplify(const SimplifyContext& context) const
    {
        auto it = context.vars.find(name);
        if (it == context.vars.end()) {
            return clone();
        }
        context.logger("Replacing variable {} with its value.", name);
        return it->second->clone();
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Pi::simplify(const SimplifyContext& context) const
    {
        if (context.config.irrationalUseApproximation) {
            return std::make_unique<Constant>(settings::readDecimal("calculating.pi"));
        }
        return clone();
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Euler::simplify(const SimplifyContext& context) const
    {
        if (context.config.irrationalUseApproximation) {
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
                std::sort(terms.begin(), terms.end(), [](const auto& a, const auto& b) { return sortingCompare(*a, *b) < 0; });
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

        } // namespace _d_simplify
        namespace _d_simplify::addition {
            std::unique_ptr<AnalyticExpression::AbstractNode> tryCombine(const AnalyticExpression::AbstractNode& a, const AnalyticExpression::AbstractNode& b, const AnalyticExpression::SimplifyContext& context);
            namespace try_combine {
                std::unique_ptr<AnalyticExpression::AbstractNode> bothAreNegations(const AnalyticExpression::AbstractNode& a, const AnalyticExpression::AbstractNode& b, const AnalyticExpression::SimplifyContext& context)
                {
                    const auto& aNeg = static_cast<const AnalyticExpression::Negation&>(a);
                    const auto& bNeg = static_cast<const AnalyticExpression::Negation&>(b);
                    auto combined = tryCombine(aNeg.firstOperand(), bNeg.firstOperand(), context);
                    if (combined) {
                        return std::make_unique<AnalyticExpression::Negation>(std::move(combined));
                    }
                    return nullptr;
                }
                std::unique_ptr<AnalyticExpression::AbstractNode> oneIsNegation(const AnalyticExpression::AbstractNode& a, const AnalyticExpression::AbstractNode& b)
                {
                    const AnalyticExpression::AbstractNode* posNode = nullptr;
                    const AnalyticExpression::Negation* negNode = nullptr;
                    if (a.type() == AnalyticExpression::NodeType::Negation) {
                        negNode = reinterpret_cast<const AnalyticExpression::Negation*>(&a);
                        posNode = &b;
                    } else {
                        negNode = reinterpret_cast<const AnalyticExpression::Negation*>(&b);
                        posNode = &a;
                    }
                    if (negNode->firstOperand().rawEqualTo(*posNode)) {
                        return std::make_unique<AnalyticExpression::Constant>(0);
                    }
                    return nullptr;
                }
                std::unique_ptr<AnalyticExpression::AbstractNode> bothAreMultiplications(const AnalyticExpression::AbstractNode& a, const AnalyticExpression::AbstractNode& b, const AnalyticExpression::SimplifyContext& context)
                {
                    const auto& aMul = static_cast<const AnalyticExpression::Multiplication&>(a);
                    const auto& bMul = static_cast<const AnalyticExpression::Multiplication&>(b);
                    if (aMul.firstOperand().rawEqualTo(bMul.firstOperand())) {
                        return AnalyticExpression::Multiplication(
                            aMul.firstOperand().clone(),
                            AnalyticExpression::Addition(
                                aMul.secondOperand().clone(),
                                bMul.secondOperand().clone())
                                .simplify(context))
                            .simplify(context);
                    } else if (aMul.secondOperand().rawEqualTo(bMul.secondOperand())) {
                        return AnalyticExpression::Multiplication(
                            aMul.secondOperand().clone(),
                            AnalyticExpression::Addition(
                                aMul.firstOperand().clone(),
                                bMul.firstOperand().clone())
                                .simplify(context))
                            .simplify(context);
                    } else if (aMul.firstOperand().rawEqualTo(bMul.secondOperand())) {
                        return AnalyticExpression::Multiplication(
                            aMul.firstOperand().clone(),
                            AnalyticExpression::Addition(
                                aMul.secondOperand().clone(),
                                bMul.firstOperand().clone())
                                .simplify(context))
                            .simplify(context);
                    } else if (aMul.secondOperand().rawEqualTo(bMul.firstOperand())) {
                        return AnalyticExpression::Multiplication(
                            aMul.secondOperand().clone(),
                            AnalyticExpression::Addition(
                                aMul.firstOperand().clone(),
                                bMul.secondOperand().clone())
                                .simplify(context))
                            .simplify(context);
                    }
                    return nullptr;
                }
                std::unique_ptr<AnalyticExpression::AbstractNode> oneIsMultiplication(const AnalyticExpression::AbstractNode& a, const AnalyticExpression::AbstractNode& b, const AnalyticExpression::SimplifyContext& context)
                {
                    const AnalyticExpression::Multiplication* mulNode = nullptr;
                    const AnalyticExpression::AbstractNode* otherNode = nullptr;
                    if (a.type() == AnalyticExpression::NodeType::Multiplication) {
                        mulNode = reinterpret_cast<const AnalyticExpression::Multiplication*>(&a);
                        otherNode = &b;
                    } else {
                        mulNode = reinterpret_cast<const AnalyticExpression::Multiplication*>(&b);
                        otherNode = &a;
                    }
                    if (mulNode->firstOperand().rawEqualTo(*otherNode)) {
                        return AnalyticExpression::Multiplication(
                            mulNode->firstOperand().clone(),
                            AnalyticExpression::Addition(
                                mulNode->secondOperand().clone(),
                                std::make_unique<AnalyticExpression::Constant>(1))
                                .simplify(context))
                            .simplify(context);
                    } else if (mulNode->secondOperand().rawEqualTo(*otherNode)) {
                        return AnalyticExpression::Multiplication(
                            mulNode->secondOperand().clone(),
                            AnalyticExpression::Addition(
                                mulNode->firstOperand().clone(),
                                std::make_unique<AnalyticExpression::Constant>(1))
                                .simplify(context))
                            .simplify(context);
                    }
                    return nullptr;
                }
                std::unique_ptr<AnalyticExpression::AbstractNode> bothAreDivisions(const AnalyticExpression::AbstractNode& a, const AnalyticExpression::AbstractNode& b, const AnalyticExpression::SimplifyContext& context)
                {
                    const auto& aDiv = static_cast<const AnalyticExpression::Division&>(a);
                    const auto& bDiv = static_cast<const AnalyticExpression::Division&>(b);
                    if (aDiv.denominator->rawEqualTo(*bDiv.denominator)) {
                        return AnalyticExpression::Division(
                            AnalyticExpression::Addition(
                                aDiv.numerator->clone(),
                                bDiv.numerator->clone())
                                .simplify(context),
                            aDiv.denominator->clone())
                            .simplify(context);
                    }
                    return nullptr;
                }

            } // namespace try_combine
            // Assume both side is already simplified. IF FAILED, return nullptr.
            std::unique_ptr<AnalyticExpression::AbstractNode> tryCombine(const AnalyticExpression::AbstractNode& a, const AnalyticExpression::AbstractNode& b, const AnalyticExpression::SimplifyContext& context)
            {
                // Handle common cases here.
                if (a.rawEqualTo(b)) {
                    return std::make_unique<AnalyticExpression::Multiplication>(
                        std::make_unique<AnalyticExpression::Constant>(2),
                        a.clone());
                } else if (a.rawEqualTo(AnalyticExpression::Constant::ZERO)) {
                    return b.clone();
                } else if (b.rawEqualTo(AnalyticExpression::Constant::ZERO)) {
                    return a.clone();
                }
                if (a.type() == AnalyticExpression::NodeType::Constant && b.type() == AnalyticExpression::NodeType::Constant) {
                    const auto& aConst = static_cast<const AnalyticExpression::Constant&>(a);
                    const auto& bConst = static_cast<const AnalyticExpression::Constant&>(b);
                    return std::make_unique<AnalyticExpression::Constant>(aConst.value + bConst.value);
                } else if (a.type() == AnalyticExpression::NodeType::Negation && b.type() == AnalyticExpression::NodeType::Negation) {
                    // -x + -y = -(x + y)
                    return try_combine::bothAreNegations(a, b, context);
                } else if (a.type() == AnalyticExpression::NodeType::Negation || b.type() == AnalyticExpression::NodeType::Negation) {
                    // -x + x = 0
                    return try_combine::oneIsNegation(a, b);
                } else if (a.type() == AnalyticExpression::NodeType::Multiplication && b.type() == AnalyticExpression::NodeType::Multiplication) {
                    // x * y + x * z = x * (y + z)
                    return try_combine::bothAreMultiplications(a, b, context);
                } else if (a.type() == AnalyticExpression::NodeType::Multiplication || b.type() == AnalyticExpression::NodeType::Multiplication) {
                    // x * y + x = x * (y + 1)
                    return try_combine::oneIsMultiplication(a, b, context);
                } else if (a.type() == AnalyticExpression::NodeType::Division && b.type() == AnalyticExpression::NodeType::Division) {
                    // x / z + y / z = (x + y) / z
                    return try_combine::bothAreDivisions(a, b, context);
                }

                return nullptr;
            }
        } // namespace _d_simplify::addition
    } // namespace

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Addition::simplify(const SimplifyContext& context) const
    {
        auto leftSimplified = firstOperand().simplify(context);
        auto rightSimplified = secondOperand().simplify(context);

        context.logger("Flattening addition expression.");
        std::vector<std::unique_ptr<AbstractNode>> terms = _d_simplify::flatten<Addition>(*this);

        // If there's a undefined term, it'll be the first one after sorting. Same for infinity.
        if (terms[0]->type() == NodeType::Undefined)
            return std::make_unique<Undefined>();
        if (terms[0]->type() == NodeType::Infinity)
            return std::make_unique<Infinity>();

        for (size_t i = 0; i < terms.size(); ++i) {
            for (size_t j = i + 1; j < terms.size();) {
                context.logger("Combining terms at index {} and {}.", i, j);
                auto combined = _d_simplify::addition::tryCombine(*terms[i], *terms[j], context);
                if (!combined) {
                    ++j;
                    continue;
                }
                assert(combined->type() != NodeType::Subtraction && "Combination should not produce Subtraction node.");
                if (combined->type() == NodeType::Addition) {
                    std::vector<std::unique_ptr<AbstractNode>> combinedTerms = _d_simplify::flatten<Addition>(static_cast<const Addition&>(*combined));
                    terms[i] = std::move(combinedTerms[0]);
                    terms.insert(terms.begin() + static_cast<int64_t>(i) + 1,
                        std::make_move_iterator(combinedTerms.begin() + 1),
                        std::make_move_iterator(combinedTerms.end()));
                } else {
                    terms[i] = std::move(combined);
                }
                terms.erase(terms.begin() + static_cast<int64_t>(j));
            }
        }

        return _d_simplify::rebuildTree<Addition>(std::move(terms));
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Subtraction::simplify(const SimplifyContext& context) const
    { }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Multiplication::simplify(const SimplifyContext& context) const
    { }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Division::simplify(const SimplifyContext& context) const
    { }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Negation::simplify(const SimplifyContext& context) const
    { }
} // namespace TheCalculater::math
