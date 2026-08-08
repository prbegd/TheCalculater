/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;
#include <cassert>

module prbegd.thecalculater.math;
import std;
import thirdparty.core;
import prbegd.thecalculater.util;

namespace thecalculater::math {
namespace { namespace impl {
    std::vector<const AnalyticExpression::Node*> retrieveChildren(const AnalyticExpression::Node& node)
    {
        std::vector<const AnalyticExpression::Node*> children;
        const AnalyticExpression::NodeVisitorConst visitor(
            [&children](const AnalyticExpression::Addition& node) {
                children.append_range(node.terms | std::views::transform([](auto& term) { return term.get(); }));
            },
            [&children](const AnalyticExpression::Multiplication& node) {
                children.append_range(node.factors | std::views::transform([](auto& factor) { return factor.get(); }));
            },
            [&children](const AnalyticExpression::Power& node) {
                children.push_back(node.base.get());
                children.push_back(node.exponent.get());
            },
            [&children](const AnalyticExpression::AbsoluteValue& node) {
                children.push_back(node.operand.get());
            },
            [&children](const AnalyticExpression::Ceiling& node) {
                children.push_back(node.operand.get());
            },
            [&children](const AnalyticExpression::Floor& node) {
                children.push_back(node.operand.get());
            },
            [&children](const AnalyticExpression::Modulus& node) {
                children.push_back(node.dividend.get());
                children.push_back(node.divisor.get());
            },
            [&children](const AnalyticExpression::Logarithm& node) {
                children.push_back(node.argument.get());
                children.push_back(node.base.get());
            },
            [&children](const AnalyticExpression::NaturalLogarithm& node) {
                children.push_back(node.argument.get());
            },
            [&children](const AnalyticExpression::Sine& node) {
                children.push_back(node.operand.get());
            },
            [&children](const AnalyticExpression::Cosine& node) {
                children.push_back(node.operand.get());
            },
            [&children](const AnalyticExpression::Tangent& node) {
                children.push_back(node.operand.get());
            },
            [&children](const AnalyticExpression::Arcsine& node) {
                children.push_back(node.operand.get());
            },
            [&children](const AnalyticExpression::Arccosine& node) {
                children.push_back(node.operand.get());
            },
            [&children](const AnalyticExpression::Arctangent& node) {
                children.push_back(node.operand.get());
            });
        node.accept(visitor);
        return children;
    }
    bool isLeafNode(const AnalyticExpression::Node& target)
    {
        const std::type_info& type = typeid(target);
        return type == typeid(AnalyticExpression::Constant) || type == typeid(AnalyticExpression::Variable) || type == typeid(AnalyticExpression::Infinity) || type == typeid(AnalyticExpression::Pi) || type == typeid(AnalyticExpression::Euler) || type == typeid(AnalyticExpression::ImaginaryUnit);
    }
    void normalizeOnce(AnalyticExpression::Node& node)
    {
        const AnalyticExpression::NodeVisitor visitor(
            [](AnalyticExpression::Addition& node) {
                decltype(node.terms.begin()) it;
                const AnalyticExpression::NodeVisitor childrenVisitor(
                    [&node, &it](AnalyticExpression::Addition& child) {
                        const std::size_t childChildrenCount = child.terms.size();
                        it = node.terms.insert_range(it, child.terms | std::views::as_rvalue);
                        it += static_cast<std::int64_t>(childChildrenCount);
                        it = node.terms.erase(it);
                    },
                    [&it](AnalyticExpression::Node&) {
                        it++;
                    });
                for (it = node.terms.begin(); it != node.terms.end();) {
                    (*it)->accept(childrenVisitor);
                }
                std::ranges::sort(node.terms, [](const auto& a, const auto& b) { return a->hash() < b->hash(); });
            },
            [](AnalyticExpression::Multiplication& node) {
                decltype(node.factors.begin()) it;
                const AnalyticExpression::NodeVisitor childrenVisitor(
                    // cppcheck-suppress constParameterReference
                    [&node, &it](AnalyticExpression::Multiplication& child) {
                        const std::size_t childChildrenCount = child.factors.size();
                        it = node.factors.insert_range(it, child.factors | std::views::as_rvalue);
                        it += static_cast<std::int64_t>(childChildrenCount);
                        it = node.factors.erase(it);
                    },
                    [&it](AnalyticExpression::Node&) {
                        it++;
                    });
                for (it = node.factors.begin(); it != node.factors.end();) {
                    (*it)->accept(childrenVisitor);
                }
                std::ranges::sort(node.factors, [](const auto& a, const auto& b) { return a->hash() < b->hash(); });
            });
        node.accept(visitor);
    }
    void normalizeFull(AnalyticExpression::Node& node)
    {
        const AnalyticExpression::NodeVisitor visitor(
            [&visitor](AnalyticExpression::Addition& node) {
                decltype(node.terms.begin()) it;
                const AnalyticExpression::NodeVisitor childrenVisitor(
                    [&node, &it](AnalyticExpression::Addition& child) {
                        const std::size_t childChildrenCount = child.terms.size();
                        it = node.terms.insert_range(it, child.terms | std::views::as_rvalue);
                        it += static_cast<std::int64_t>(childChildrenCount);
                        it = node.terms.erase(it);
                    },
                    [&it](AnalyticExpression::Node&) {
                        it++;
                    });
                for (it = node.terms.begin(); it != node.terms.end();) {
                    (*it)->accept(visitor);
                    (*it)->accept(childrenVisitor);
                }
                std::ranges::sort(node.terms, [](const auto& a, const auto& b) { return a->hash() < b->hash(); });
            },
            [&visitor](AnalyticExpression::Multiplication& node) {
                decltype(node.factors.begin()) it;
                const AnalyticExpression::NodeVisitor childrenVisitor(
                    // cppcheck-suppress constParameterReference
                    [&node, &it](AnalyticExpression::Multiplication& child) {
                        const std::size_t childChildrenCount = child.factors.size();
                        it = node.factors.insert_range(it, child.factors | std::views::as_rvalue);
                        it += static_cast<std::int64_t>(childChildrenCount);
                        it = node.factors.erase(it);
                    },
                    [&it](AnalyticExpression::Node&) {
                        it++;
                    });
                for (it = node.factors.begin(); it != node.factors.end();) {
                    (*it)->accept(visitor);
                    (*it)->accept(childrenVisitor);
                }
                std::ranges::sort(node.factors, [](const auto& a, const auto& b) { return a->hash() < b->hash(); });
            },
            [&visitor](AnalyticExpression::Power& node) {
                node.base->accept(visitor);
                node.exponent->accept(visitor);
            },
            [&visitor](AnalyticExpression::AbsoluteValue& node) {
                node.operand->accept(visitor);
            },
            [&visitor](AnalyticExpression::Ceiling& node) {
                node.operand->accept(visitor);
            },
            [&visitor](AnalyticExpression::Floor& node) {
                node.operand->accept(visitor);
            },
            [&visitor](AnalyticExpression::Modulus& node) {
                node.dividend->accept(visitor);
                node.divisor->accept(visitor);
            },
            [&visitor](AnalyticExpression::Logarithm& node) {
                node.argument->accept(visitor);
                node.base->accept(visitor);
            },
            [&visitor](AnalyticExpression::NaturalLogarithm& node) {
                node.argument->accept(visitor);
            },
            [&visitor](AnalyticExpression::Sine& node) {
                node.operand->accept(visitor);
            },
            [&visitor](AnalyticExpression::Cosine& node) {
                node.operand->accept(visitor);
            },
            [&visitor](AnalyticExpression::Tangent& node) {
                node.operand->accept(visitor);
            },
            [&visitor](AnalyticExpression::Arcsine& node) {
                node.operand->accept(visitor);
            },
            [&visitor](AnalyticExpression::Arccosine& node) {
                node.operand->accept(visitor);
            },
            [&visitor](AnalyticExpression::Arctangent& node) {
                node.operand->accept(visitor);
            });
        node.accept(visitor);
    }
}} // namespace ::impl
AnalyticExpression::Wildcard::UsedInCalculationException::UsedInCalculationException(const std::string& message)
    : std::logic_error(message)
{ }

AnalyticExpression::Node::Node(Node* parent)
    : parent(parent)
{ }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_CONSTRUCTOR0_(_class_) \
    AnalyticExpression::_class_::_class_(Node* parent) \
        : VisitableNode(parent) \
    { }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_CONSTRUCTOR1_(_class_, _member_) \
    AnalyticExpression::_class_::_class_(Node* parent, const util::unique_pmr_ptr<Node>&(_member_), std::pmr::memory_resource* memoryResource) \
        : VisitableNode(parent), \
          _member_((_member_)->clone(memoryResource)) \
    { \
        this->_member_->parent = this; \
    } \
    AnalyticExpression::_class_::_class_(Node* parent, util::unique_pmr_ptr<Node> && (_member_)) \
        : VisitableNode(parent), \
          _member_(std::move(_member_)) \
    { \
        this->_member_->parent = this; \
    }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_CONSTRUCTOR2_(_class_, _member1_, _member2_) \
    AnalyticExpression::_class_::_class_(Node* parent, const util::unique_pmr_ptr<Node>&(_member1_), const util::unique_pmr_ptr<Node>&(_member2_), std::pmr::memory_resource* memoryResource) \
        : VisitableNode(parent), \
          _member1_((_member1_)->clone(memoryResource)), \
          _member2_((_member2_)->clone(memoryResource)) \
    { \
        this->_member1_->parent = this; \
        this->_member2_->parent = this; \
    } \
    AnalyticExpression::_class_::_class_(Node* parent, util::unique_pmr_ptr<Node> && (_member1_), util::unique_pmr_ptr<Node> && (_member2_)) \
        : VisitableNode(parent), \
          _member1_(std::move(_member1_)), \
          _member2_(std::move(_member2_)) \
    { \
        this->_member1_->parent = this; \
        this->_member2_->parent = this; \
    }
AnalyticExpression::Wildcard::Any::Any(Node* parent, Wildcard::Id id)
    : WildNode(parent),
      id(id)
{ }
AnalyticExpression::Wildcard::Variadic::Variadic(Node* parent, Wildcard::Id id)
    : WildNode(parent),
      id(id)
{ }
AnalyticExpression::Constant::Constant(Node* parent, Rational value)
    : VisitableNode(parent),
      value(std::move(value))
{ }
AnalyticExpression::Variable::Variable(Node* parent, std::string_view name, std::pmr::memory_resource* memoryResource)
    : VisitableNode(parent),
      name(name, memoryResource)
{ }
AnalyticExpression::Variable::Variable(Node* parent, std::pmr::string&& name)
    : VisitableNode(parent),
      name(std::move(name))
{ }
NODE_CONSTRUCTOR0_(Infinity)
NODE_CONSTRUCTOR0_(Pi)
NODE_CONSTRUCTOR0_(Euler)
NODE_CONSTRUCTOR0_(ImaginaryUnit)
AnalyticExpression::Addition::Addition(Node* parent, std::pmr::vector<util::unique_pmr_ptr<Node>>&& terms)
    : VisitableNode(parent),
      terms(std::move(terms))
{
    for (const auto& term : this->terms) {
        term->parent = this;
    }
}
AnalyticExpression::Multiplication::Multiplication(Node* parent, std::pmr::vector<util::unique_pmr_ptr<Node>>&& factors)
    : VisitableNode(parent),
      factors(std::move(factors))
{
    for (const auto& factor : this->factors) {
        factor->parent = this;
    }
}
NODE_CONSTRUCTOR2_(Power, base, exponent)
NODE_CONSTRUCTOR1_(AbsoluteValue, operand)
NODE_CONSTRUCTOR1_(Ceiling, operand)
NODE_CONSTRUCTOR1_(Floor, operand)
NODE_CONSTRUCTOR2_(Modulus, dividend, divisor)
NODE_CONSTRUCTOR2_(Logarithm, argument, base)
NODE_CONSTRUCTOR1_(NaturalLogarithm, argument)
NODE_CONSTRUCTOR1_(Sine, operand)
NODE_CONSTRUCTOR1_(Cosine, operand)
NODE_CONSTRUCTOR1_(Tangent, operand)
NODE_CONSTRUCTOR1_(Arcsine, operand)
NODE_CONSTRUCTOR1_(Arccosine, operand)
NODE_CONSTRUCTOR1_(Arctangent, operand)
#undef NODE_CONSTRUCTOR1_
#undef NODE_CONSTRUCTOR2_

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_METHOD_HASH0_(_class_, _hash_) \
    std::size_t AnalyticExpression::_class_::hash() const \
    { \
        return _hash_; \
    }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_METHOD_HASH1_(_class_, _hash_, _member_) \
    std::size_t AnalyticExpression::_class_::hash() const \
    { \
        std::size_t seed = _hash_; \
        boost::hash_combine(seed, (_member_)->hash()); \
        return seed; \
    }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_METHOD_HASH10_(_class_, _hash_, _member_) \
    std::size_t AnalyticExpression::_class_::hash() const \
    { \
        std::size_t seed = _hash_; \
        boost::hash_combine(seed, _member_); \
        return seed; \
    }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_METHOD_HASH2_(_class_, _hash_, _member1_, _member2_) \
    std::size_t AnalyticExpression::_class_::hash() const \
    { \
        std::size_t seed = _hash_; \
        boost::hash_combine(seed, (_member1_)->hash()); \
        boost::hash_combine(seed, (_member2_)->hash()); \
        return seed; \
    }
std::size_t AnalyticExpression::Addition::hash() const
{
    std::size_t seed = 0x26b57e0cad6d1c3;
    for (const auto& term : terms) {
        boost::hash_combine(seed, term->hash());
    }
    return seed;
}
std::size_t AnalyticExpression::Multiplication::hash() const
{
    std::size_t seed = 0x95d1ec6364d57dc8;
    for (const auto& factor : factors) {
        boost::hash_combine(seed, factor->hash());
    }
    return seed;
}
NODE_METHOD_HASH10_(Constant, 0x3361e811604a8be7, this->value)
NODE_METHOD_HASH10_(Variable, 0xe60cbdcfe41d881a, this->name)
NODE_METHOD_HASH0_(Infinity, 0xc3dc0c723e73cbc3)
NODE_METHOD_HASH0_(Pi, 0x8c18f600b6867066)
NODE_METHOD_HASH0_(Euler, 0x573ab0792d7b9fca)
NODE_METHOD_HASH0_(ImaginaryUnit, 0x99506ad9db02af43)
NODE_METHOD_HASH2_(Power, 0xf709b05f78a07dcb, this->base, this->exponent)
NODE_METHOD_HASH1_(AbsoluteValue, 0xb6520fc18810bef7, this->operand)
NODE_METHOD_HASH1_(Ceiling, 0x9794942fdb2ced17, this->operand)
NODE_METHOD_HASH1_(Floor, 0xd581148f9f049570, this->operand)
NODE_METHOD_HASH2_(Modulus, 0x46e0bc3ac0eb3723, this->dividend, this->divisor)
NODE_METHOD_HASH2_(Logarithm, 0xfa76de7ccdb3659d, this->argument, this->base)
NODE_METHOD_HASH1_(NaturalLogarithm, 0xffb7367750971651, this->argument)
NODE_METHOD_HASH1_(Sine, 0x682422b47671e928, this->operand)
NODE_METHOD_HASH1_(Cosine, 0x3bfcfb15956054ad, this->operand)
NODE_METHOD_HASH1_(Tangent, 0x6da758eca579e7ae, this->operand)
NODE_METHOD_HASH1_(Arcsine, 0x9c05a7b5a7b29fca, this->operand)
NODE_METHOD_HASH1_(Arccosine, 0x27e3b1fde1e5166e, this->operand)
NODE_METHOD_HASH1_(Arctangent, 0xe467b7f655c81cc8, this->operand)
#undef NODE_METHOD_HASH0_
#undef NODE_METHOD_HASH1_
#undef NODE_METHOD_HASH10_
#undef NODE_METHOD_HASH2_

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_METHOD_CLONE0_(_class_) \
    util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::_class_::clone(std::pmr::memory_resource* memoryResource) const \
    { \
        return util::makeUniquePmr<_class_>(memoryResource, this->parent); \
    }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_METHOD_CLONE1_(_class_, _parameter_) \
    util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::_class_::clone(std::pmr::memory_resource* memoryResource) const \
    { \
        return util::makeUniquePmr<_class_>(memoryResource, this->parent, _parameter_); \
    }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_METHOD_CLONE2_(_class_, _parameter1_, _parameter2_) \
    util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::_class_::clone(std::pmr::memory_resource* memoryResource) const \
    { \
        return util::makeUniquePmr<_class_>(memoryResource, this->parent, _parameter1_, _parameter2_); \
    }
NODE_METHOD_CLONE1_(Wildcard::Any, this->id)
NODE_METHOD_CLONE1_(Wildcard::Variadic, this->id)
NODE_METHOD_CLONE1_(Constant, this->value)
util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::Addition::clone(std::pmr::memory_resource* memoryResource) const
{
    std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>> terms;
    for (const auto& term : this->terms) {
        terms.push_back(term->clone(memoryResource));
    }
    return util::makeUniquePmr<Addition>(memoryResource, this->parent, std::move(terms));
}
util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::Multiplication::clone(std::pmr::memory_resource* memoryResource) const
{
    std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>> factors;
    for (const auto& factor : this->factors) {
        factors.push_back(factor->clone(memoryResource));
    }
    return util::makeUniquePmr<Multiplication>(memoryResource, this->parent, std::move(factors));
}
NODE_METHOD_CLONE2_(Variable, this->name, memoryResource)
NODE_METHOD_CLONE0_(Infinity)
NODE_METHOD_CLONE0_(Pi)
NODE_METHOD_CLONE0_(Euler)
NODE_METHOD_CLONE0_(ImaginaryUnit)
NODE_METHOD_CLONE2_(Power, this->base->clone(memoryResource), this->exponent->clone(memoryResource))
NODE_METHOD_CLONE1_(AbsoluteValue, this->operand->clone(memoryResource))
NODE_METHOD_CLONE1_(Ceiling, this->operand->clone(memoryResource))
NODE_METHOD_CLONE1_(Floor, this->operand->clone(memoryResource))
NODE_METHOD_CLONE2_(Modulus, this->dividend->clone(memoryResource), this->divisor->clone(memoryResource))
NODE_METHOD_CLONE2_(Logarithm, this->argument->clone(memoryResource), this->base->clone(memoryResource))
NODE_METHOD_CLONE1_(NaturalLogarithm, this->argument->clone(memoryResource))
NODE_METHOD_CLONE1_(Sine, this->operand->clone(memoryResource))
NODE_METHOD_CLONE1_(Cosine, this->operand->clone(memoryResource))
NODE_METHOD_CLONE1_(Tangent, this->operand->clone(memoryResource))
NODE_METHOD_CLONE1_(Arcsine, this->operand->clone(memoryResource))
NODE_METHOD_CLONE1_(Arccosine, this->operand->clone(memoryResource))
NODE_METHOD_CLONE1_(Arctangent, this->operand->clone(memoryResource))
#undef NODE_METHOD_CLONE0_
#undef NODE_METHOD_CLONE1_
#undef NODE_METHOD_CLONE2_

namespace { namespace impl::simplification::rule::match {
    bool isVariadicNode(const AnalyticExpression::Node* node)
    {
        return typeid(*node) == typeid(AnalyticExpression::Wildcard::Variadic);
    }
    bool wildcardMatch(const AnalyticExpression::Node& pattern, const AnalyticExpression::Node& target, AnalyticExpression::Simplification::Rule::WildcardMap& wildcardMap, std::pmr::memory_resource* memoryResource);
    bool matchWildcardVariant(const std::type_info& patternType,
                              std::vector<const AnalyticExpression::Node*>& patternChildren,
                              std::vector<const AnalyticExpression::Node*>& targetChildren,
                              AnalyticExpression::Simplification::Rule::WildcardMap& wildcardMap,
                              std::pmr::memory_resource* memoryResource)
    {
        struct VariadicMatch {
            decltype(patternChildren.begin()) patternIt;
            std::ranges::subrange<decltype(targetChildren.begin())> targetRange;
        };
        std::pmr::vector<VariadicMatch> variadicMatches(memoryResource);

        for (auto patternIt = patternChildren.begin(), targetIt = targetChildren.begin();
             patternIt != patternChildren.end();
             patternIt++, targetIt++) {
            if (isVariadicNode(*patternIt)) {
                const std::ranges::subrange targetRange(targetIt, targetChildren.end());
                variadicMatches.emplace_back(patternIt, targetRange);
                if (!targetRange.empty()) {
                    targetIt = targetRange.end() - 1;
                }
            } else if (targetIt == targetChildren.end() || !impl::simplification::rule::match::wildcardMatch(**patternIt, **targetIt, wildcardMap, memoryResource)) {
                auto lastAvailableVariadicMatchIt = std::ranges::find_last_if_not(variadicMatches, [](const VariadicMatch& match) { return match.targetRange.empty(); }).begin();
                if (lastAvailableVariadicMatchIt == variadicMatches.end()) {
                    return false;
                }
                variadicMatches.erase(lastAvailableVariadicMatchIt + 1, variadicMatches.end());

                const std::ranges::subrange newTargetRange(lastAvailableVariadicMatchIt->targetRange.begin(), lastAvailableVariadicMatchIt->targetRange.end() - 1);
                lastAvailableVariadicMatchIt->targetRange = newTargetRange;
                patternIt = lastAvailableVariadicMatchIt->patternIt;
                targetIt = newTargetRange.end() - 1;
            }
        }

        for (const auto& match : variadicMatches) {
            const std::size_t variadicSize = match.targetRange.size();
            util::unique_pmr_ptr<AnalyticExpression::Node> matchResult;
            if (variadicSize == 0) {
                matchResult = util::makeUniquePmr<AnalyticExpression::Constant>(memoryResource, nullptr, patternType == typeid(AnalyticExpression::Addition) ? 0 : 1);
            } else if (variadicSize == 1) {
                matchResult = match.targetRange.front()->clone(memoryResource);
                matchResult->parent = nullptr;
            } else {
                auto variadicNodes = match.targetRange
                    | std::views::transform([memoryResource](const AnalyticExpression::Node* variadicNode) { return variadicNode->clone(memoryResource); })
                    | std::ranges::to<std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>>>(memoryResource);
                if (patternType == typeid(AnalyticExpression::Addition)) {
                    matchResult = util::makeUniquePmr<AnalyticExpression::Addition>(memoryResource, nullptr, std::move(variadicNodes));
                } else {
                    matchResult = util::makeUniquePmr<AnalyticExpression::Multiplication>(memoryResource, nullptr, std::move(variadicNodes));
                }
            }
            const AnalyticExpression::Wildcard::Id id = static_cast<const AnalyticExpression::Wildcard::Variadic*>(*match.patternIt)->id; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            if (wildcardMap.contains(id)) {
                if (!impl::simplification::rule::match::wildcardMatch(*wildcardMap[id], *matchResult, wildcardMap, memoryResource)) {
                    return false;
                }
            } else {
                wildcardMap[id] = std::move(matchResult);
            }
        }
        return true;
    }
    bool wildcardMatch(const AnalyticExpression::Node& pattern, const AnalyticExpression::Node& target, AnalyticExpression::Simplification::Rule::WildcardMap& wildcardMap, std::pmr::memory_resource* memoryResource)
    {
        const std::type_info& patternType = typeid(pattern);
        const std::type_info& targetType = typeid(target);
        std::vector<const AnalyticExpression::Node*> patternChildren = impl::retrieveChildren(pattern);
        std::vector<const AnalyticExpression::Node*> targetChildren = impl::retrieveChildren(target);

        assert(targetType != typeid(AnalyticExpression::Wildcard::Any) && "Wildcard::Any must only be present on left hand side.");
        if (patternType == typeid(AnalyticExpression::Wildcard::Any)) {
            const AnalyticExpression::Wildcard::Id wildcardId = static_cast<const AnalyticExpression::Wildcard::Any&>(pattern).id; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            if (wildcardMap.contains(wildcardId)) {
                return impl::simplification::rule::match::wildcardMatch(*wildcardMap[wildcardId], target, wildcardMap, memoryResource);
            }
            util::unique_pmr_ptr<AnalyticExpression::Node> matchResult = target.clone(memoryResource);
            matchResult->parent = nullptr;
            wildcardMap[wildcardId] = std::move(matchResult);
            return true;
        }
        if (patternType != targetType) {
            return false;
        }
        do { // NOLINT(cppcoreguidelines-avoid-do-while)
            if (patternType != typeid(AnalyticExpression::Addition) && patternType != typeid(AnalyticExpression::Multiplication)) {
                break;
            }
            assert(std::ranges::none_of(targetChildren, isVariadicNode) && "Wildcard::Variadic node must only be present on left hand side.");
            if (std::ranges::none_of(patternChildren, isVariadicNode)) {
                break;
            }
            return matchWildcardVariant(patternType, patternChildren, targetChildren, wildcardMap, memoryResource);
        } while (false);
        if (patternChildren.size() != targetChildren.size()) {
            return false;
        }
        if (patternType == typeid(AnalyticExpression::Constant)) {
            return static_cast<const AnalyticExpression::Constant&>(pattern).value == static_cast<const AnalyticExpression::Constant&>(target).value; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        }
        if (patternType == typeid(AnalyticExpression::Variable)) {
            return static_cast<const AnalyticExpression::Variable&>(pattern).name == static_cast<const AnalyticExpression::Variable&>(target).name; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        }
        for (std::size_t i = 0; i < patternChildren.size(); ++i) {
            if (!impl::simplification::rule::match::wildcardMatch(*patternChildren[i], *targetChildren[i], wildcardMap, memoryResource)) {
                return false;
            }
        }
        return true;
    }
}
} // namespace ::impl::simplification::rule::match

std::optional<AnalyticExpression::Simplification::Rule::WildcardMap> AnalyticExpression::Simplification::Rule::match(const Node& target, std::pmr::memory_resource* memoryResource) const
{
    AnalyticExpression::Simplification::Rule::WildcardMap wildcardMap(memoryResource);
    if (!impl::simplification::rule::match::wildcardMatch(*this->pattern, target, wildcardMap, memoryResource)) {
        return std::nullopt;
    }
    if (!this->condition(target, wildcardMap)) {
        return std::nullopt;
    }
    return wildcardMap;
}
util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::Simplification::Rule::apply(WildcardMap map, std::pmr::memory_resource* memoryResource) const
{
    return this->replacer(std::move(map), memoryResource);
}

namespace { namespace impl::simplification::hill_climbing_algorithm {
    std::optional<util::unique_pmr_ptr<AnalyticExpression::Node>> apply(const AnalyticExpression::Simplification::Context& context, const AnalyticExpression::Node& target)
    {
        const Integer originalComplexity = AnalyticExpression::Simplification::complexityOf(target);
        auto candidateNodes = context.rules
            | std::views::transform([&context, &target](const AnalyticExpression::Simplification::Rule& rule) { return std::make_pair(std::cref(rule), rule.match(target, context.memoryResource)); })
            | std::views::cache_latest
            | std::views::filter([](const std::pair<const AnalyticExpression::Simplification::Rule&, std::optional<AnalyticExpression::Simplification::Rule::WildcardMap>>& rulePair) { return rulePair.second.has_value(); })
            | std::views::transform([&context](std::pair<const AnalyticExpression::Simplification::Rule&, std::optional<AnalyticExpression::Simplification::Rule::WildcardMap>>& rulePair) { return rulePair.first.apply(std::move(*rulePair.second), context.memoryResource); })
            | std::ranges::to<std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>>>(context.memoryResource);
        const auto candidateNodesComplexities = candidateNodes
            | std::views::transform([](const util::unique_pmr_ptr<AnalyticExpression::Node>& node) { return AnalyticExpression::Simplification::complexityOf(*node); });
        const auto minComplexityCanididate = std::ranges::min_element(candidateNodesComplexities);
        if (*minComplexityCanididate >= originalComplexity) {
            return std::nullopt;
        }
        return std::move(*minComplexityCanididate.base());
    }
    util::unique_pmr_ptr<AnalyticExpression::Node> applyUntilFixed(const AnalyticExpression::Simplification::Context& context, const AnalyticExpression::Node& target)
    {
        const AnalyticExpression::Node* previous = &target;
        util::unique_pmr_ptr<AnalyticExpression::Node> next;
        while (true) {
            std::optional<util::unique_pmr_ptr<AnalyticExpression::Node>> current = apply(context, *previous);
            if (!current.has_value()) {
                break;
            }
            next = std::move(*current);
            previous = next.get();
        }
        return next;
    }
}} // namespace ::impl::simplification::hill_climbing_algorithm

util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::Simplification::HillClimbingAlgorithm::operator()(const AnalyticExpression::Simplification::Context& context, const AnalyticExpression::Node& target) const
{
    if (impl::isLeafNode(target)) {
        return target.clone(context.memoryResource);
    }
    util::unique_pmr_ptr<AnalyticExpression::Node> result = target.clone(context.memoryResource);
    result->accept(NodeVisitor(
        [&context](Addition& node) {
            for (auto& term : node.terms) {
                term = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *term);
            }
        },
        [&context](Multiplication& node) {
            for (auto& factor : node.factors) {
                factor = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *factor);
            }
        },
        [&context](Power& node) {
            node.base = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.base);
            node.exponent = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.exponent);
        },
        [&context](AbsoluteValue& node) {
            node.operand = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.operand);
        },
        [&context](Ceiling& node) {
            node.operand = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.operand);
        },
        [&context](Floor& node) {
            node.operand = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.operand);
        },
        [&context](Modulus& node) {
            node.dividend = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.dividend);
            node.divisor = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.divisor);
        },
        [&context](Logarithm& node) {
            node.argument = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.argument);
            node.base = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.base);
        },
        [&context](NaturalLogarithm& node) {
            node.argument = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.argument);
        },
        [&context](Sine& node) {
            node.operand = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.operand);
        },
        [&context](Cosine& node) {
            node.operand = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.operand);
        },
        [&context](Tangent& node) {
            node.operand = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.operand);
        },
        [&context](Arcsine& node) {
            node.operand = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.operand);
        },
        [&context](Arccosine& node) {
            node.operand = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.operand);
        },
        [&context](Arctangent& node) {
            node.operand = impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *node.operand);
        }));
    impl::normalizeOnce(*result);
    return impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *result);
}
namespace { namespace impl::simplification::e_graph_algorithm {
    struct ENode {
        util::unique_pmr_ptr<AnalyticExpression::Node> node;
    };
    struct EClass {
        std::pmr::vector<ENode> members;
    };
    using EClassReference = std::uint64_t;
    class EClassReferenceNode : public AnalyticExpression::VisitableNode<EClassReferenceNode> {
    public:
        EClassReference reference;

        explicit EClassReferenceNode(Node* parent, EClassReference reference)
            : VisitableNode(parent),
              reference(reference)
        { }

        EClassReferenceNode(const EClassReferenceNode& other) = delete;
        EClassReferenceNode(EClassReferenceNode&& other) = default;
        EClassReferenceNode& operator=(const EClassReferenceNode& other) = delete;
        EClassReferenceNode& operator=(EClassReferenceNode&& other) = default;
        ~EClassReferenceNode() override = default;

        [[nodiscard]]
        std::size_t hash() const override
        {
            std::size_t seed = 0xe749fde1f1c7652a;
            boost::hash_combine(seed, reference);
            return seed;
        }

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override
        {
            return util::makeUniquePmr<EClassReferenceNode>(memoryResource, this->parent, reference);
        }
    };
    // TODO(P2): When new member e-nodes (the Changed) are added to one e-class (the Target), the Changed set their status to Pending; the Target's parent e-class (the Parent) set all their members' status (except the Target) to Pending recursively. This way, neighbor members of the Target marked as Processed will be reused, remember to set them back to Pending again after it's done.                This requires e-classes to be independent, but it doesn't seems like to be the case. what the heck?! ASK THE SHALLOWSLEEP FOR MORE CLUES. goal: find a more elegant solution or just give up on status thing (since just optimization)
    // TODO(P2): implement wildcardMatch separately. instead of returning a boolean, it should generate multiple tree matches and corresponding wildcard map. E-nodes marked as Processed should be ignored.
    using WorkList = std::pmr::deque<EClassReference>;
    class EGraph {
    public:
        std::pmr::unordered_map<EClassReference, EClass> graph;
        EClassReference entry;
        WorkList workList;

        explicit EGraph(const AnalyticExpression::Node& target, const AnalyticExpression::Simplification::Context& context)
            : graph(context.memoryResource), workList(context.memoryResource)
        {
            this->entry = findOrCreateClass_(buildNode_(target));
            appendToWorkList_(this->entry);
        }

        util::unique_pmr_ptr<AnalyticExpression::Node> saturate(const AnalyticExpression::Simplification::Context& context, std::uint64_t maxIterations)
        {
            for (std::uint64_t i = 0; i < maxIterations; i++) {
                if (this->workList.empty()) {
                    break;
                }
                const EClassReference target = this->workList.front();
                this->workList.pop_front();
                saturateClass_(target, context);
            }
            return extractBestSolution_();
        }

    private:
        EClassReference findOrCreateClass_(const ENode& node)
        {

        }
        ENode buildNode_(const AnalyticExpression::Node& node)
        {
        }
        void appendToWorkList_(EClassReference target)
        {
        }
        void replaceObsoleteClass_(EClassReference obsoleted, EClassReference replacement)
        {
        }
        // TODO(P2): Use better matching strategy.
        std::vector<util::unique_pmr_ptr<AnalyticExpression::Node>> expandAllPossibleSolutions_(EClassReference target) const
        {
        }
        void saturateClass_(EClassReference target, const AnalyticExpression::Simplification::Context& context)
        {
        }
        util::unique_pmr_ptr<AnalyticExpression::Node> extractBestSolution_() const
        {
        }
    };
}} // namespace ::impl::simplification::e_graph_algorithm
util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::Simplification::EGraphAlgorithm::operator()(const AnalyticExpression::Simplification::Context& context, const AnalyticExpression::Node& target) const
{
    // const Integer originalComplexity = complexityOf(target);
    // TODO(P0): Some normalization should take place here.
}

AnalyticExpression::Simplification::Context::Context(const AnalyticExpression& expr)
    : Context(expr.memoryResource())
{ }
AnalyticExpression::Simplification::Context::Context(std::pmr::memory_resource* memoryResource)
    : rules(generateDefaultRules(memoryResource)),
      algorithm(util::makeUniquePmr<SequenceAlgorithm<HillClimbingAlgorithm, EGraphAlgorithm>>(memoryResource, HillClimbingAlgorithm(), EGraphAlgorithm())),
      approximation(RationalCalculationOptions().approximation),
      memoryResource(memoryResource)
{ }

AnalyticExpression::Simplification::RuleSet AnalyticExpression::Simplification::generateDefaultRules(std::pmr::memory_resource* memoryResource)
{
    // TODO(P0): fill this up
    return { };
}
bool AnalyticExpression::Simplification::structuralEqual(const AnalyticExpression::Node& a, const AnalyticExpression::Node& b)
{
    const std::type_info& aType = typeid(a);
    const std::type_info& bType = typeid(b);
    std::vector<const AnalyticExpression::Node*> aChildren = impl::retrieveChildren(a);
    std::vector<const AnalyticExpression::Node*> bChildren = impl::retrieveChildren(b);

    if (aType != bType || aChildren.size() != bChildren.size()) {
        return false;
    }
    if (aType == typeid(AnalyticExpression::Constant)) {
        return static_cast<const AnalyticExpression::Constant&>(a).value == static_cast<const AnalyticExpression::Constant&>(b).value; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
    }
    if (aType == typeid(AnalyticExpression::Variable)) {
        return static_cast<const AnalyticExpression::Variable&>(a).name == static_cast<const AnalyticExpression::Variable&>(b).name; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
    }
    for (std::size_t i = 0; i < aChildren.size(); ++i) {
        if (!structuralEqual(*aChildren[i], *bChildren[i])) {
            return false;
        }
    }
    return true;
}
Integer AnalyticExpression::Simplification::complexityOf(const AnalyticExpression::Node& node)
{
    const auto childAccumulator = [](const Integer& accumulation, const util::unique_pmr_ptr<Node>& child) { return accumulation + complexityOf(*child); };
    Integer complexity;
    node.accept(NodeVisitorConst(
        [&complexity](const AnalyticExpression::Constant&) {
            complexity = 1;
        },
        [&complexity](const AnalyticExpression::Variable&) {
            complexity = 1;
        },
        [&complexity](const AnalyticExpression::Infinity&) {
            complexity = 1;
        },
        [&complexity](const AnalyticExpression::Pi&) {
            complexity = 1;
        },
        [&complexity](const AnalyticExpression::Euler&) {
            complexity = 1;
        },
        [&complexity](const AnalyticExpression::ImaginaryUnit&) {
            complexity = 1;
        },
        [&childAccumulator, &complexity](const AnalyticExpression::Addition& node) {
            complexity = 2 * std::ranges::fold_left(node.terms, Integer(), childAccumulator);
        },
        [&childAccumulator, &complexity](const AnalyticExpression::Multiplication& node) {
            complexity = 4 * std::ranges::fold_left(node.factors, Integer(), childAccumulator);
        },
        [&complexity](const AnalyticExpression::Power& node) {
            complexity = 16 * (complexityOf(*node.base) + complexityOf(*node.exponent));
        },
        [&complexity](const AnalyticExpression::AbsoluteValue& node) {
            complexity = 8 * complexityOf(*node.operand);
        },
        [&complexity](const AnalyticExpression::Ceiling& node) {
            complexity = 8 * complexityOf(*node.operand);
        },
        [&complexity](const AnalyticExpression::Floor& node) {
            complexity = 8 * complexityOf(*node.operand);
        },
        [&complexity](const AnalyticExpression::Modulus& node) {
            complexity = 8 * (complexityOf(*node.dividend) + complexityOf(*node.divisor));
        },
        [&complexity](const AnalyticExpression::Logarithm& node) {
            complexity = 32 * (complexityOf(*node.argument) + complexityOf(*node.base));
        },
        [&complexity](const AnalyticExpression::NaturalLogarithm& node) {
            complexity = 32 * (1 + complexityOf(*node.argument));
        },
        [&complexity](const AnalyticExpression::Sine& node) {
            complexity = 64 * complexityOf(*node.operand);
        },
        [&complexity](const AnalyticExpression::Cosine& node) {
            complexity = 64 * complexityOf(*node.operand);
        },
        [&complexity](const AnalyticExpression::Tangent& node) {
            complexity = 64 * complexityOf(*node.operand);
        },
        [&complexity](const AnalyticExpression::Arcsine& node) {
            complexity = 64 * complexityOf(*node.operand);
        },
        [&complexity](const AnalyticExpression::Arccosine& node) {
            complexity = 64 * complexityOf(*node.operand);
        },
        [&complexity](const AnalyticExpression::Arctangent& node) {
            complexity = 64 * complexityOf(*node.operand);
        }));
    return complexity;
}

AnalyticExpression::AnalyticExpression(std::shared_ptr<std::pmr::memory_resource> memoryResource)
    : memoryResource_(memoryResource)
{ }
AnalyticExpression::AnalyticExpression(const Node& node, std::shared_ptr<std::pmr::memory_resource> memoryResource)
    : base(node.clone(memoryResource.get())),
      memoryResource_(memoryResource)
{ }
AnalyticExpression::AnalyticExpression(const util::unique_pmr_ptr<Node>& node, std::shared_ptr<std::pmr::memory_resource> memoryResource)
    : base(node->clone(memoryResource.get())),
      memoryResource_(memoryResource)
{ }
AnalyticExpression::AnalyticExpression(util::unique_pmr_ptr<Node>&& node, std::shared_ptr<std::pmr::memory_resource> memoryResource)
    : base(std::move(node)),
      memoryResource_(memoryResource)
{ }

AnalyticExpression::AnalyticExpression(const AnalyticExpression& other)
    : base(other.base->clone(other.memoryResource_.get())),
      memoryResource_(other.memoryResource_)
{ }
AnalyticExpression& AnalyticExpression::operator=(const AnalyticExpression& other)
{
    if (this != &other) {
        base = other.base->clone(memoryResource_.get());
        memoryResource_ = other.memoryResource_;
    }
    return *this;
}
std::pmr::memory_resource* AnalyticExpression::memoryResource() const
{
    return memoryResource_.get();
}

AnalyticExpression normalize(AnalyticExpression expr)
{
    impl::normalizeFull(*expr.base);
    return expr;
}
// TODO(P0): Some normalization should take place when simplifying
} // namespace thecalculater::math