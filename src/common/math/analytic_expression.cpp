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
#include <cstddef>
#include <iterator>
#include <list>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <utility>

namespace TheCalculater::math {
    AnalyticExpression::AnalyticExpression(const AbstractNode& node)
        : root(node.clone())
    { }
    AnalyticExpression::AnalyticExpression(const std::unique_ptr<AbstractNode>& node)
        : root(node->clone())
    { }
    AnalyticExpression::AnalyticExpression(std::unique_ptr<AbstractNode>&& node)
        : root(std::move(node))
    { }

    const AnalyticExpression::Constant AnalyticExpression::Constant::ZERO(0);
    const AnalyticExpression::Constant AnalyticExpression::Constant::ONE(1);
    namespace {
        [[nodiscard]] int sortingCompare(const AnalyticExpression::AbstractNode& a, const AnalyticExpression::AbstractNode& b) // NOLINT
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
    [[nodiscard]] size_t AnalyticExpression::Affirmation::hash() const
    {
        size_t seed = 0xcbf06d47db3ef7bd; // Hash of 'TheCalculater::math::AnalyticExpression::Affirmation'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Power::hash() const
    {
        size_t seed = 0xf709b05f78a07dcb; // Hash of 'TheCalculater::math::AnalyticExpression::Power'
        boost::hash_combine(seed, base->hash());
        boost::hash_combine(seed, exponent->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Root::hash() const
    {
        size_t seed = 0xf0070dea6c9ea427; // Hash of 'TheCalculater::math::AnalyticExpression::Root'
        boost::hash_combine(seed, radicand->hash());
        boost::hash_combine(seed, index->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Factorial::hash() const
    {
        size_t seed = 0x724892ddfc402102; // Hash of 'TheCalculater::math::AnalyticExpression::Factorial'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::AbsoluteValue::hash() const
    {
        size_t seed = 0xb6520fc18810bef7; // Hash of 'TheCalculater::math::AnalyticExpression::AbsoluteValue'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Modulus::hash() const
    {
        size_t seed = 0x46e0bc3ac0eb3723; // Hash of 'TheCalculater::math::AnalyticExpression::Modulus'
        boost::hash_combine(seed, dividend->hash());
        boost::hash_combine(seed, divisor->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Logarithm::hash() const
    {
        size_t seed = 0xfa76de7ccdb3659d; // Hash of 'TheCalculater::math::AnalyticExpression::Logarithm'
        boost::hash_combine(seed, base->hash());
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::NaturalLogarithm::hash() const
    {
        size_t seed = 0xffb7367750971651; // Hash of 'TheCalculater::math::AnalyticExpression::NaturalLogarithm'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Sine::hash() const
    {
        size_t seed = 0x682422b47671e928; // Hash of 'TheCalculater::math::AnalyticExpression::Sine'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Cosine::hash() const
    {
        size_t seed = 0x3bfcfb15956054ad; // Hash of 'TheCalculater::math::AnalyticExpression::Cosine'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Tangent::hash() const
    {
        size_t seed = 0x6da758eca579e7ae; // Hash of 'TheCalculater::math::AnalyticExpression::Tangent'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Cotangent::hash() const
    {
        size_t seed = 0x3078937dde9c0317; // Hash of 'TheCalculater::math::AnalyticExpression::Cotangent'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Secant::hash() const
    {
        size_t seed = 0xa91022ca62cbcccb; // Hash of 'TheCalculater::math::AnalyticExpression::Secant'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Cosecant::hash() const
    {
        size_t seed = 0x79c10674bec54b9d; // Hash of 'TheCalculater::math::AnalyticExpression::Cosecant'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Arcsine::hash() const
    {
        size_t seed = 0x9c05a7b5a7b29fca; // Hash of 'TheCalculater::math::AnalyticExpression::Arcsine'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Arccosine::hash() const
    {
        size_t seed = 0x27e3b1fde1e5166e; // Hash of 'TheCalculater::math::AnalyticExpression::Arccosine'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Arctangent::hash() const
    {
        size_t seed = 0xe467b7f655c81cc8; // Hash of 'TheCalculater::math::AnalyticExpression::Arctangent'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Arccotangent::hash() const
    {
        size_t seed = 0x15c2996b91a0adfd; // Hash of 'TheCalculater::math::AnalyticExpression::Arccotangent'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Arcsecant::hash() const
    {
        size_t seed = 0x8865bb38e15d8b94; // Hash of 'TheCalculater::math::AnalyticExpression::Arcsecant'
        boost::hash_combine(seed, operand->hash());
        return seed;
    }
    [[nodiscard]] size_t AnalyticExpression::Arccosecant::hash() const
    {
        size_t seed = 0xc43b3d0796a9c04c; // Hash of 'TheCalculater::math::AnalyticExpression::Arccosecant'
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
    bool AnalyticExpression::Affirmation::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Affirmation) {
            return false;
        }
        const auto& o = static_cast<const Affirmation&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Power::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Power) {
            return false;
        }
        const auto& o = static_cast<const Power&>(other);
        return base->rawEqualTo(*o.base) && exponent->rawEqualTo(*o.exponent);
    }
    bool AnalyticExpression::Root::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Root) {
            return false;
        }
        const auto& o = static_cast<const Root&>(other);
        return radicand->rawEqualTo(*o.radicand) && index->rawEqualTo(*o.index);
    }
    bool AnalyticExpression::Factorial::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Factorial) {
            return false;
        }
        const auto& o = static_cast<const Factorial&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::AbsoluteValue::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::AbsoluteValue) {
            return false;
        }
        const auto& o = static_cast<const AbsoluteValue&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Modulus::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Modulus) {
            return false;
        }
        const auto& o = static_cast<const Modulus&>(other);
        return dividend->rawEqualTo(*o.dividend) && divisor->rawEqualTo(*o.divisor);
    }
    bool AnalyticExpression::Logarithm::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Logarithm) {
            return false;
        }
        const auto& o = static_cast<const Logarithm&>(other);
        return base->rawEqualTo(*o.base) && operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::NaturalLogarithm::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::NaturalLogarithm) {
            return false;
        }
        const auto& o = static_cast<const NaturalLogarithm&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Sine::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Sine) {
            return false;
        }
        const auto& o = static_cast<const Sine&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Cosine::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Cosine) {
            return false;
        }
        const auto& o = static_cast<const Cosine&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Tangent::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Tangent) {
            return false;
        }
        const auto& o = static_cast<const Tangent&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Cotangent::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Cotangent) {
            return false;
        }
        const auto& o = static_cast<const Cotangent&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Secant::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Secant) {
            return false;
        }
        const auto& o = static_cast<const Secant&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Cosecant::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Cosecant) {
            return false;
        }
        const auto& o = static_cast<const Cosecant&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Arcsine::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Arcsine) {
            return false;
        }
        const auto& o = static_cast<const Arcsine&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Arccosine::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Arccosine) {
            return false;
        }
        const auto& o = static_cast<const Arccosine&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Arctangent::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Arctangent) {
            return false;
        }
        const auto& o = static_cast<const Arctangent&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Arccotangent::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Arccotangent) {
            return false;
        }
        const auto& o = static_cast<const Arccotangent&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Arcsecant::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Arcsecant) {
            return false;
        }
        const auto& o = static_cast<const Arcsecant&>(other);
        return operand->rawEqualTo(*o.operand);
    }
    bool AnalyticExpression::Arccosecant::rawEqualTo(const AbstractNode& other) const
    {
        if (other.type() != NodeType::Arccosecant) {
            return false;
        }
        const auto& o = static_cast<const Arccosecant&>(other);
        return operand->rawEqualTo(*o.operand);
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Variable::simplify(const SimplifyContext& context) const
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

    namespace { namespace _d_simplify {
        using Terms = std::list<std::unique_ptr<AnalyticExpression::AbstractNode>>;

        template <typename TOperation>
        Terms collectTermsFor(const AnalyticExpression::AbstractNode& node) = delete;
        template <>
        Terms collectTermsFor<AnalyticExpression::Addition>(const AnalyticExpression::AbstractNode& node)
        {
            static std::function<void(const AnalyticExpression::AbstractNode&, Terms&, bool)> collect = [&](const AnalyticExpression::AbstractNode& node, Terms& result, bool inverted) {
                if (node.type() == AnalyticExpression::NodeType::Addition) {
                    const auto& binOp = static_cast<const AnalyticExpression::Addition&>(node);
                    collect(binOp.firstOperand(), result, inverted);
                    collect(binOp.secondOperand(), result, inverted);
                } else if (node.type() == AnalyticExpression::NodeType::Subtraction) {
                    const auto& binOp = static_cast<const AnalyticExpression::Subtraction&>(node);
                    collect(binOp.firstOperand(), result, inverted);
                    collect(binOp.secondOperand(), result, !inverted);
                } else {
                    result.push_back(inverted ? AnalyticExpression::Negation(node.clone()).simplify({}) : node.clone());
                }
            };

            Terms result;
            collect(node, result, false);
            return result;
        }
        template <>
        Terms collectTermsFor<AnalyticExpression::Multiplication>(const AnalyticExpression::AbstractNode& node)
        {
            static std::function<void(const AnalyticExpression::AbstractNode&, Terms&, bool)> collect = [&](const AnalyticExpression::AbstractNode& node, Terms& result, bool inverted) {
                if (node.type() == AnalyticExpression::NodeType::Multiplication) {
                    const auto& binOp = static_cast<const AnalyticExpression::Multiplication&>(node);
                    collect(binOp.firstOperand(), result, inverted);
                    collect(binOp.secondOperand(), result, inverted);
                } else if (node.type() == AnalyticExpression::NodeType::Division) {
                    const auto& binOp = static_cast<const AnalyticExpression::Division&>(node);
                    collect(binOp.firstOperand(), result, inverted);
                    collect(binOp.secondOperand(), result, !inverted);
                } else {
                    result.push_back(inverted ? AnalyticExpression::Division(std::make_unique<AnalyticExpression::Constant>(1), node.clone()).simplify({}) : node.clone());
                }
            };

            Terms result;
            collect(node, result, false);
            return result;
        }

        template <typename TOperation>
        Terms flatten(const AnalyticExpression::AbstractNode& node)
        {
            auto terms = collectTermsFor<TOperation>(node);
            terms.sort([](const auto& a, const auto& b) { return sortingCompare(*a, *b) < 0; });
            return terms;
        }
        template <typename TOperation>
        std::unique_ptr<AnalyticExpression::AbstractNode> rebuildTree(Terms&& terms)
        {
            if (terms.empty()) {
                return std::make_unique<AnalyticExpression::Constant>(0);
            }

            std::unique_ptr<AnalyticExpression::AbstractNode> result = std::move(terms.front());

            for (auto& term : terms | std::ranges::views::drop(1)) {
                result = std::make_unique<TOperation>(std::move(result), std::move(term));
            }

            return result;
        }
        template <>
        std::unique_ptr<AnalyticExpression::AbstractNode> rebuildTree<AnalyticExpression::Addition>(Terms&& terms)
        {
            if (terms.empty()) {
                return std::make_unique<AnalyticExpression::Constant>(0);
            }

            std::unique_ptr<AnalyticExpression::AbstractNode> result = std::move(terms.front());

            for (auto& term : terms | std::ranges::views::drop(1)) {
                if (term->type() == AnalyticExpression::NodeType::Negation) {
                    const auto& neg = static_cast<const AnalyticExpression::Negation&>(*term);
                    result = std::make_unique<AnalyticExpression::Subtraction>(std::move(result), std::move(neg.operand));
                    continue;
                }
                result = std::make_unique<AnalyticExpression::Addition>(std::move(result), std::move(term));
            }

            return result;
        }
    }} // namespace ::_d_simplify
    namespace { namespace _d_simplify::addition {
        // Assume both side is already simplified. IF FAILED, return nullptr.
        std::unique_ptr<AnalyticExpression::AbstractNode> tryCombine(const AnalyticExpression::AbstractNode& a, const AnalyticExpression::AbstractNode& b)
        {
            // Handle constants first.
            if (a.type() == AnalyticExpression::NodeType::Constant && b.type() == AnalyticExpression::NodeType::Constant) {
                const auto& aConst = static_cast<const AnalyticExpression::Constant&>(a);
                const auto& bConst = static_cast<const AnalyticExpression::Constant&>(b);
                return std::make_unique<AnalyticExpression::Constant>(aConst.value + bConst.value);
            }
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

            return nullptr;
        }
    }} // namespace ::_d_simplify::addition
    namespace { namespace _d_simplify::multiplication {
        // Assume both side is already simplified. IF FAILED, return nullptr.
        std::unique_ptr<AnalyticExpression::AbstractNode> tryCombine(const AnalyticExpression::AbstractNode& a, const AnalyticExpression::AbstractNode& b)
        {
            // Handle constants first.
            if (a.type() == AnalyticExpression::NodeType::Constant && b.type() == AnalyticExpression::NodeType::Constant) {
                const auto& aConst = static_cast<const AnalyticExpression::Constant&>(a);
                const auto& bConst = static_cast<const AnalyticExpression::Constant&>(b);
                return std::make_unique<AnalyticExpression::Constant>(aConst.value * bConst.value);
            }
            // Handle common cases here.
            if (a.rawEqualTo(b)) {
                return std::make_unique<AnalyticExpression::Power>(a.clone(), std::make_unique<AnalyticExpression::Constant>(2));
            } else if (a.rawEqualTo(AnalyticExpression::Constant::ONE)) {
                return b.clone();
            } else if (b.rawEqualTo(AnalyticExpression::Constant::ONE)) {
                return a.clone();
            } else if (a.rawEqualTo(AnalyticExpression::Constant::ZERO) || b.rawEqualTo(AnalyticExpression::Constant::ZERO)) {
                return std::make_unique<AnalyticExpression::Constant>(0);
            }

            return nullptr;
        }
    }} // namespace ::_d_simplify::multiplication
    namespace { namespace _d_simplify {
        template <decltype(_d_simplify::addition::tryCombine) TCombiner>
        void combineLikeTermsIn(Terms& terms)
        {
            for (auto i = terms.begin(); i != terms.end(); ++i) {
                for (auto j = std::next(i); j != terms.end();) {
                    auto combined = TCombiner(**i, **j);
                    if (!combined) {
                        ++j;
                        continue;
                    }
                    *i = std::move(combined);
                    j = terms.erase(j);
                }
            }
        }
    }} // namespace ::_d_simplify

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Addition::simplify(const SimplifyContext& context) const
    {
        auto leftSimplified = firstOperand().simplify(context);
        auto rightSimplified = secondOperand().simplify(context);

        context.logger("Flattening addition expression.");
        _d_simplify::Terms terms = _d_simplify::flatten<Addition>(Addition(std::move(leftSimplified), std::move(rightSimplified)));

        // If there's a undefined term, it'll be the first one after sorting. Same for infinity.
        if (terms.front()->type() == NodeType::Undefined)
            return std::make_unique<Undefined>();
        if (terms.front()->type() == NodeType::Infinity)
            return std::make_unique<Infinity>();

        context.logger("Combining terms for addition.");
        _d_simplify::combineLikeTermsIn<_d_simplify::addition::tryCombine>(terms);

        context.logger("Rebuilding addition expression.");
        return _d_simplify::rebuildTree<Addition>(std::move(terms));
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Subtraction::simplify(const SimplifyContext& context) const
    {
        // Simple reuse of addition simplification.
        auto leftSimplified = firstOperand().simplify(context);
        auto rightSimplified = secondOperand().simplify(context);

        context.logger("Flattening subtraction expression.");
        _d_simplify::Terms terms = _d_simplify::flatten<Addition>(Subtraction(std::move(leftSimplified), std::move(rightSimplified)));

        if (terms.front()->type() == NodeType::Undefined)
            return std::make_unique<Undefined>();
        if (terms.front()->type() == NodeType::Infinity)
            return std::make_unique<Infinity>();

        context.logger("Combining terms for subtraction.");
        _d_simplify::combineLikeTermsIn<_d_simplify::addition::tryCombine>(terms);

        context.logger("Rebuilding subtraction expression.");
        return _d_simplify::rebuildTree<Addition>(std::move(terms));
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Multiplication::simplify(const SimplifyContext& context) const
    {
        // Similar to addition
        auto leftSimplified = firstOperand().simplify(context);
        auto rightSimplified = secondOperand().simplify(context);

        context.logger("Flattening multiplication expression.");
        _d_simplify::Terms terms = _d_simplify::flatten<Multiplication>(Multiplication(std::move(leftSimplified), std::move(rightSimplified)));

        if (terms.front()->type() == NodeType::Undefined)
            return std::make_unique<Undefined>();
        if (terms.front()->type() == NodeType::Infinity)
            return std::make_unique<Infinity>();

        context.logger("Combining factors for multiplication.");
        _d_simplify::combineLikeTermsIn<_d_simplify::multiplication::tryCombine>(terms);

        context.logger("Rebuilding multiplication expression.");
        return _d_simplify::rebuildTree<Multiplication>(std::move(terms));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Division::simplify(const SimplifyContext& context) const
    {
        auto numerSimplified = firstOperand().simplify(context);
        auto denoSimplified = secondOperand().simplify(context);

        if (numerSimplified->type() == NodeType::Constant && denoSimplified->type() == NodeType::Constant) {
            context.logger("Turning division into constant.");
            const auto& numerConst = static_cast<const AnalyticExpression::Constant&>(*numerSimplified);
            const auto& denoConst = static_cast<const AnalyticExpression::Constant&>(*denoSimplified);
            if (denoConst.value == 0) {
                if (numerConst.value == 0)
                    return std::make_unique<AnalyticExpression::Undefined>();
                return std::make_unique<AnalyticExpression::Infinity>();
            }
            return std::make_unique<AnalyticExpression::Constant>(numerConst.value / denoConst.value);
        }

        context.logger("Flattening division expression.");
        _d_simplify::Terms terms = _d_simplify::flatten<Multiplication>(Division(std::move(numerSimplified), std::move(denoSimplified)));

        if (terms.front()->type() == NodeType::Undefined)
            return std::make_unique<Undefined>();
        if (terms.front()->type() == NodeType::Infinity)
            return std::make_unique<Infinity>();

        context.logger("Combining factors for division.");
        _d_simplify::combineLikeTermsIn<_d_simplify::multiplication::tryCombine>(terms);

        context.logger("Rebuilding division expression.");
        return _d_simplify::rebuildTree<Multiplication>(std::move(terms));
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Negation::simplify(const SimplifyContext& context) const
    {
        auto simplified = firstOperand().simplify(context);

        if (simplified->type() == NodeType::Constant) {
            context.logger("Turning negation into constant.");
            const auto& constant = static_cast<const AnalyticExpression::Constant&>(*simplified);
            return std::make_unique<AnalyticExpression::Constant>(-constant.value);
        } else if (simplified->type() == NodeType::Negation) {
            context.logger("Removing nested negation.");
            const auto& neg = static_cast<const AnalyticExpression::Negation&>(*simplified);
            return neg.firstOperand().simplify(context);
        } else if (simplified->type() == NodeType::Addition || simplified->type() == NodeType::Subtraction) {
            context.logger("Flattening negation expression.");
            _d_simplify::Terms terms = _d_simplify::flatten<Addition>(Negation(std::move(simplified)));

            context.logger("Negating terms for negation.");
            for (auto& term : terms) {
                if (term->type() == NodeType::Negation)
                    term = static_cast<AnalyticExpression::Negation&>(*term).firstOperand().simplify(context);
                else if (term->type() == NodeType::Constant) {
                    auto& constant = static_cast<AnalyticExpression::Constant&>(*term);
                    constant.value = -constant.value;
                } else if (term->type() == NodeType::Undefined) {
                    return std::make_unique<AnalyticExpression::Undefined>();
                } else if (term->type() == NodeType::Infinity) {
                    return std::make_unique<AnalyticExpression::Infinity>();
                } else {
                    term = std::make_unique<AnalyticExpression::Negation>(std::move(term));
                }
            }

            if (terms.size() == 1) {
                return std::move(terms.front());
            }
            context.logger("Rebuilding negation expression.");
            return _d_simplify::rebuildTree<Addition>(std::move(terms));
        }

        return std::make_unique<AnalyticExpression::Negation>(std::move(simplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Affirmation::simplify(const SimplifyContext& context) const
    {
        return operand->simplify(context);
    }

    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Power::simplify(const SimplifyContext& context) const
    {
        auto baseSimplified = firstOperand().simplify(context);
        auto exponentSimplified = secondOperand().simplify(context);

        if (baseSimplified->type() == NodeType::Constant && exponentSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant power.");
            const auto& baseConst = static_cast<const AnalyticExpression::Constant&>(*baseSimplified);
            const auto& expConst = static_cast<const AnalyticExpression::Constant&>(*exponentSimplified);
            if (expConst.value == 0) {
                if (baseConst.value == 0)
                    return std::make_unique<AnalyticExpression::Undefined>();
                return std::make_unique<AnalyticExpression::Constant>(1);
            }
            return std::make_unique<AnalyticExpression::Constant>(pow(baseConst.value, expConst.value));
        } else if (exponentSimplified->type() == NodeType::Constant) {
            context.logger("Handling constant exponent.");
            const auto& expConst = static_cast<const AnalyticExpression::Constant&>(*exponentSimplified);
            if (expConst.value == 0) {
                if (baseSimplified->type() == NodeType::Constant && static_cast<const Constant&>(*baseSimplified).value == 0)
                    return std::make_unique<AnalyticExpression::Undefined>();
                return std::make_unique<AnalyticExpression::Constant>(1);
            } else if (expConst.value == 1) {
                return baseSimplified;
            } else if (expConst.value == -1) {
                return std::make_unique<AnalyticExpression::Division>(std::make_unique<Constant>(1), std::move(baseSimplified));
            }
        }

        return std::make_unique<AnalyticExpression::Power>(std::move(baseSimplified), std::move(exponentSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Root::simplify(const SimplifyContext& context) const
    {
        auto radSimplified = firstOperand().simplify(context);
        auto indSimplified = secondOperand().simplify(context);
        if (radSimplified->type() == NodeType::Constant && indSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant root.");
            const auto& radConst = static_cast<const AnalyticExpression::Constant&>(*radSimplified);
            const auto& indConst = static_cast<const AnalyticExpression::Constant&>(*indSimplified);
            if (indConst.value == 0) {
                if (radConst.value == 0)
                    return std::make_unique<AnalyticExpression::Undefined>();
                return std::make_unique<AnalyticExpression::Undefined>();
            }
            try {
                return std::make_unique<AnalyticExpression::Constant>(pow(radConst.value, 1 / indConst.value));
            } catch (const std::domain_error& e) {
                // TODO: Add processing logic for computing even root of negative number.
                // It's too complex for now to handle more than 2 roots. (has n different roots)
            }
        } else if (indSimplified->type() == NodeType::Constant) {
            context.logger("Handling constant index for root.");
            const auto& indConst = static_cast<const AnalyticExpression::Constant&>(*indSimplified);
            if (indConst.value == 0) {
                return std::make_unique<AnalyticExpression::Undefined>();
            } else if (indConst.value == 1) {
                return radSimplified;
            } else if (indConst.value == -1) {
                return std::make_unique<AnalyticExpression::Division>(std::make_unique<Constant>(1), std::move(radSimplified));
            }
        }

        return std::make_unique<AnalyticExpression::Root>(std::move(radSimplified), std::move(indSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Factorial::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant factorial.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            // Make sure that the operand is a positive integer.
            if (operandConst.value.denominator() == 1 && operandConst.value.numerator() >= 0) {
                return std::make_unique<AnalyticExpression::Constant>(factorial(operandConst.value));
            } // TODO: Use gamma function to calculate factorial.
        }

        return std::make_unique<AnalyticExpression::Factorial>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::AbsoluteValue::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant absolute value.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(abs(operandConst.value));
        }

        return std::make_unique<AnalyticExpression::AbsoluteValue>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Modulus::simplify(const SimplifyContext& context) const
    {
        auto dividendSimplified = firstOperand().simplify(context);
        auto divisorSimplified = secondOperand().simplify(context);

        if (dividendSimplified->type() == NodeType::Constant && divisorSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant modulus.");
            const auto& dividendConst = static_cast<const AnalyticExpression::Constant&>(*dividendSimplified);
            const auto& divisorConst = static_cast<const AnalyticExpression::Constant&>(*divisorSimplified);
            return std::make_unique<AnalyticExpression::Constant>(dividendConst.value % divisorConst.value);
        }

        return std::make_unique<AnalyticExpression::Modulus>(std::move(dividendSimplified), std::move(divisorSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Logarithm::simplify(const SimplifyContext& context) const
    {
        auto baseSimplified = firstOperand().simplify(context);
        auto operandSimplified = secondOperand().simplify(context);

        if (baseSimplified->type() == NodeType::Constant && operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant logarithm.");
            const auto& baseConst = static_cast<const AnalyticExpression::Constant&>(*baseSimplified);
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            if (baseConst.value != 0 && baseConst.value != 1 && baseConst.value > 0 && operandConst.value > 0) {
                return std::make_unique<AnalyticExpression::Constant>(log(baseConst.value, operandConst.value));
            }
        }

        return std::make_unique<AnalyticExpression::Logarithm>(std::move(baseSimplified), std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::NaturalLogarithm::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant natural logarithm.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            if (operandConst.value > 0) {
                return std::make_unique<AnalyticExpression::Constant>(ln(operandConst.value));
            }
        }

        return std::make_unique<AnalyticExpression::NaturalLogarithm>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Sine::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant sine.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(sin(operandConst.value));
        }

        return std::make_unique<AnalyticExpression::Sine>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Cosine::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant cosine.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(cos(operandConst.value));
        }

        return std::make_unique<AnalyticExpression::Cosine>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Tangent::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant tangent.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(tan(operandConst.value));
        }

        return std::make_unique<AnalyticExpression::Tangent>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Cotangent::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant cotangent.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(cot(operandConst.value));
        }

        return std::make_unique<AnalyticExpression::Cotangent>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Secant::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant secant.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(sec(operandConst.value));
        }

        return std::make_unique<AnalyticExpression::Secant>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Cosecant::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant cosecant.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(csc(operandConst.value));
        }

        return std::make_unique<AnalyticExpression::Cosecant>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Arcsine::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant arcsine.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(arcsin(operandConst.value));
        }

        return std::make_unique<AnalyticExpression::Arcsine>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Arccosine::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant arccosine.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(arccos(operandConst.value));
        }

        return std::make_unique<AnalyticExpression::Arccosine>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Arctangent::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant arctangent.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(arctan(operandConst.value));
        }

        return std::make_unique<AnalyticExpression::Arctangent>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Arccotangent::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant arccotangent.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(arccot(operandConst.value));
        }

        return std::make_unique<AnalyticExpression::Arccotangent>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Arcsecant::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant arcsecant.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(arcsec(operandConst.value));
        }

        return std::make_unique<AnalyticExpression::Arcsecant>(std::move(operandSimplified));
    }
    [[nodiscard]] std::unique_ptr<AnalyticExpression::AbstractNode> AnalyticExpression::Arccosecant::simplify(const SimplifyContext& context) const
    {
        auto operandSimplified = firstOperand().simplify(context);

        if (operandSimplified->type() == NodeType::Constant) {
            context.logger("Calculating constant arccosecant.");
            const auto& operandConst = static_cast<const AnalyticExpression::Constant&>(*operandSimplified);
            return std::make_unique<AnalyticExpression::Constant>(arccsc(operandConst.value));
        }

        return std::make_unique<AnalyticExpression::Arccosecant>(std::move(operandSimplified));
    }
} // namespace TheCalculater::math
