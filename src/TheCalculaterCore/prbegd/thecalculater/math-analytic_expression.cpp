/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version. TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details. You should have received a copy of the GNU General Public
 * License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;
#include <cassert>

module prbegd.thecalculater.math;
import thirdparty.core;
import std;
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
            [&children](const AnalyticExpression::AbsoluteValue& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Ceiling& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Floor& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Modulus& node) {
                children.push_back(node.dividend.get());
                children.push_back(node.divisor.get());
            },
            [&children](const AnalyticExpression::Logarithm& node) {
                children.push_back(node.argument.get());
                children.push_back(node.base.get());
            },
            [&children](const AnalyticExpression::NaturalLogarithm& node) { children.push_back(node.argument.get()); },
            [&children](const AnalyticExpression::Sine& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Cosine& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Tangent& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Arcsine& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Arccosine& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Arctangent& node) { children.push_back(node.operand.get()); });
        node.accept(visitor);
        return children;
    }
    std::vector<AnalyticExpression::Node*> retrieveChildren(AnalyticExpression::Node& node)
    {
        std::vector<AnalyticExpression::Node*> children;
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
            [&children](const AnalyticExpression::AbsoluteValue& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Ceiling& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Floor& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Modulus& node) {
                children.push_back(node.dividend.get());
                children.push_back(node.divisor.get());
            },
            [&children](const AnalyticExpression::Logarithm& node) {
                children.push_back(node.argument.get());
                children.push_back(node.base.get());
            },
            [&children](const AnalyticExpression::NaturalLogarithm& node) { children.push_back(node.argument.get()); },
            [&children](const AnalyticExpression::Sine& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Cosine& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Tangent& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Arcsine& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Arccosine& node) { children.push_back(node.operand.get()); },
            [&children](const AnalyticExpression::Arctangent& node) { children.push_back(node.operand.get()); });
        node.accept(visitor);
        return children;
    }
    bool isLeafNode(const AnalyticExpression::Node& target)
    {
        const std::type_info& type = typeid(target);
        return type == typeid(AnalyticExpression::Constant) || type == typeid(AnalyticExpression::Variable)
            || type == typeid(AnalyticExpression::Infinity) || type == typeid(AnalyticExpression::Pi)
            || type == typeid(AnalyticExpression::Euler) || type == typeid(AnalyticExpression::ImaginaryUnit);
    }
    template <typename TAdditionalComparator = decltype([](const AnalyticExpression::Node&,
                                                           const AnalyticExpression::Node&) { return true; })>
    bool structuralEqual(const AnalyticExpression::Node& a,
                         const AnalyticExpression::Node& b,
                         const TAdditionalComparator& additionalComparator = { })
        requires std::is_invocable_r_v<bool,
                                       decltype(additionalComparator),
                                       const AnalyticExpression::Node&,
                                       const AnalyticExpression::Node&>
    {
        const std::type_info& aType = typeid(a);
        const std::type_info& bType = typeid(b);
        std::vector<const AnalyticExpression::Node*> aChildren = impl::retrieveChildren(a);
        std::vector<const AnalyticExpression::Node*> bChildren = impl::retrieveChildren(b);

        if (aType != bType || aChildren.size() != bChildren.size()) {
            return false;
        }
        if (aType == typeid(AnalyticExpression::Constant)) {
            return static_cast<const AnalyticExpression::Constant&>(a).value
                == static_cast<const AnalyticExpression::Constant&>(b).value;
        }
        if (aType == typeid(AnalyticExpression::Variable)) {
            return static_cast<const AnalyticExpression::Variable&>(a).name
                == static_cast<const AnalyticExpression::Variable&>(b).name;
        }
        for (std::size_t i = 0; i < aChildren.size(); ++i) {
            if (!structuralEqual(*aChildren[i], *bChildren[i], additionalComparator)) {
                return false;
            }
        }
        return additionalComparator(a, b);
    }
    enum class NormalizationMode : std::uint8_t {
        Once,
        Full,
    };
    template <NormalizationMode mode>
    void normalize(AnalyticExpression::Node& node)
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
                    [&it, &node](AnalyticExpression::Constant& child) {
                        if (child.value == 0) {
                            it = node.terms.erase(it);
                        } else {
                            it++;
                        }
                    },
                    [&it](AnalyticExpression::Node&) { it++; });
                for (it = node.terms.begin(); it != node.terms.end();) {
                    if constexpr (mode == NormalizationMode::Full) {
                        (*it)->accept(visitor);
                    }
                    (*it)->accept(childrenVisitor);
                }
                std::ranges::sort(node.terms, [](const auto& a, const auto& b) { return a->hash() < b->hash(); });
            },
            [&visitor](AnalyticExpression::Multiplication& node) {
                decltype(node.factors.begin()) it;
                const AnalyticExpression::NodeVisitor childrenVisitor(
                    [&node, &it](AnalyticExpression::Multiplication& child) {
                        const std::size_t childChildrenCount = child.factors.size();
                        it = node.factors.insert_range(it, child.factors | std::views::as_rvalue);
                        it += static_cast<std::int64_t>(childChildrenCount);
                        it = node.factors.erase(it);
                    },
                    [&it, &node](AnalyticExpression::Constant& child) {
                        if (child.value == 1) {
                            it = node.factors.erase(it);
                        } else {
                            it++;
                        }
                    },
                    [&it](AnalyticExpression::Node&) { it++; });
                for (it = node.factors.begin(); it != node.factors.end();) {
                    if constexpr (mode == NormalizationMode::Full) {
                        (*it)->accept(visitor);
                    }
                    (*it)->accept(childrenVisitor);
                }
                std::ranges::sort(node.factors, [](const auto& a, const auto& b) { return a->hash() < b->hash(); });
            },
            [&visitor](AnalyticExpression::Power& node) {
                if constexpr (mode == NormalizationMode::Full) {
                    node.base->accept(visitor);
                    node.exponent->accept(visitor);
                }
            },
            [&visitor](AnalyticExpression::AbsoluteValue& node) {
                if constexpr (mode == NormalizationMode::Full) {
                    node.operand->accept(visitor);
                }
            },
            [&visitor](AnalyticExpression::Ceiling& node) {
                if constexpr (mode == NormalizationMode::Full) {
                    node.operand->accept(visitor);
                }
            },
            [&visitor](AnalyticExpression::Floor& node) {
                if constexpr (mode == NormalizationMode::Full) {
                    node.operand->accept(visitor);
                }
            },
            [&visitor](AnalyticExpression::Modulus& node) {
                if constexpr (mode == NormalizationMode::Full) {
                    node.dividend->accept(visitor);
                    node.divisor->accept(visitor);
                }
            },
            [&visitor](AnalyticExpression::Logarithm& node) {
                if constexpr (mode == NormalizationMode::Full) {
                    node.argument->accept(visitor);
                    node.base->accept(visitor);
                }
            },
            [&visitor](AnalyticExpression::NaturalLogarithm& node) {
                if constexpr (mode == NormalizationMode::Full) {
                    node.argument->accept(visitor);
                }
            },
            [&visitor](AnalyticExpression::Sine& node) {
                if constexpr (mode == NormalizationMode::Full) {
                    node.operand->accept(visitor);
                }
            },
            [&visitor](AnalyticExpression::Cosine& node) {
                if constexpr (mode == NormalizationMode::Full) {
                    node.operand->accept(visitor);
                }
            },
            [&visitor](AnalyticExpression::Tangent& node) {
                if constexpr (mode == NormalizationMode::Full) {
                    node.operand->accept(visitor);
                }
            },
            [&visitor](AnalyticExpression::Arcsine& node) {
                if constexpr (mode == NormalizationMode::Full) {
                    node.operand->accept(visitor);
                }
            },
            [&visitor](AnalyticExpression::Arccosine& node) {
                if constexpr (mode == NormalizationMode::Full) {
                    node.operand->accept(visitor);
                }
            },
            [&visitor](AnalyticExpression::Arctangent& node) {
                if constexpr (mode == NormalizationMode::Full) {
                    node.operand->accept(visitor);
                }
            });
        node.accept(visitor);
    }
}} // namespace ::impl
AnalyticExpression::Wildcard::UsedInCalculationException::UsedInCalculationException(const std::string& message)
    : std::logic_error(message)
{ }

AnalyticExpression::Node::Node() { }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_CONSTRUCTOR0_(_class_) \
    AnalyticExpression::_class_::_class_() { }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_CONSTRUCTOR1_(_class_, _member_) \
    AnalyticExpression::_class_::_class_(const util::unique_pmr_ptr<Node>&(_member_), \
                                         std::pmr::memory_resource* memoryResource) \
        : _member_((_member_)->clone(memoryResource)) \
    { } \
    AnalyticExpression::_class_::_class_(util::unique_pmr_ptr<Node> && (_member_)) \
        : _member_(std::move(_member_)) \
    { }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_CONSTRUCTOR2_(_class_, _member1_, _member2_) \
    AnalyticExpression::_class_::_class_(const util::unique_pmr_ptr<Node>&(_member1_), \
                                         const util::unique_pmr_ptr<Node>&(_member2_), \
                                         std::pmr::memory_resource* memoryResource) \
        : _member1_((_member1_)->clone(memoryResource)), \
          _member2_((_member2_)->clone(memoryResource)) \
    { } \
    AnalyticExpression::_class_::_class_(util::unique_pmr_ptr<Node> && (_member1_), \
                                         util::unique_pmr_ptr<Node> && (_member2_)) \
        : _member1_(std::move(_member1_)), \
          _member2_(std::move(_member2_)) \
    { }
AnalyticExpression::Wildcard::Any::Any(Wildcard::Id id)
    : id(id)
{ }
AnalyticExpression::Wildcard::Variadic::Variadic(Wildcard::Id id)
    : id(id)
{ }
AnalyticExpression::Constant::Constant(Rational value)
    : value(std::move(value))
{ }
AnalyticExpression::Variable::Variable(std::string_view name, std::pmr::memory_resource* memoryResource)
    : name(name, memoryResource)
{ }
AnalyticExpression::Variable::Variable(std::pmr::string&& name)
    : name(std::move(name))
{ }
NODE_CONSTRUCTOR0_(Infinity)
NODE_CONSTRUCTOR0_(Pi)
NODE_CONSTRUCTOR0_(Euler)
NODE_CONSTRUCTOR0_(ImaginaryUnit)
AnalyticExpression::Addition::Addition(std::pmr::vector<util::unique_pmr_ptr<Node>>&& terms)
    : terms(std::move(terms))
{ }
AnalyticExpression::Multiplication::Multiplication(std::pmr::vector<util::unique_pmr_ptr<Node>>&& factors)
    : factors(std::move(factors))
{ }
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
    std::size_t AnalyticExpression::_class_::hash() const { return _hash_; }
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
    util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::_class_::clone( \
        std::pmr::memory_resource* memoryResource) const \
    { \
        return util::makeUniquePmr<_class_>(memoryResource); \
    }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_METHOD_CLONE1_(_class_, _parameter_) \
    util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::_class_::clone( \
        std::pmr::memory_resource* memoryResource) const \
    { \
        return util::makeUniquePmr<_class_>(memoryResource, _parameter_); \
    }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_METHOD_CLONE2_(_class_, _parameter1_, _parameter2_) \
    util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::_class_::clone( \
        std::pmr::memory_resource* memoryResource) const \
    { \
        return util::makeUniquePmr<_class_>(memoryResource, _parameter1_, _parameter2_); \
    }
NODE_METHOD_CLONE1_(Wildcard::Any, this->id)
NODE_METHOD_CLONE1_(Wildcard::Variadic, this->id)
NODE_METHOD_CLONE1_(Constant, this->value)
util::unique_pmr_ptr<AnalyticExpression::Node>
AnalyticExpression::Addition::clone(std::pmr::memory_resource* memoryResource) const
{
    std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>> terms;
    for (const auto& term : this->terms) {
        terms.push_back(term->clone(memoryResource));
    }
    return util::makeUniquePmr<Addition>(memoryResource, std::move(terms));
}
util::unique_pmr_ptr<AnalyticExpression::Node>
AnalyticExpression::Multiplication::clone(std::pmr::memory_resource* memoryResource) const
{
    std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>> factors;
    for (const auto& factor : this->factors) {
        factors.push_back(factor->clone(memoryResource));
    }
    return util::makeUniquePmr<Multiplication>(memoryResource, std::move(factors));
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

AnalyticExpression::Simplification::InvalidRuleException::InvalidRuleException(const std::string& message)
    : std::logic_error(message)
{ }
namespace { namespace impl::simplification::rule::match {
    bool isVariadicNode(const AnalyticExpression::Node* node)
    {
        return typeid(*node) == typeid(AnalyticExpression::Wildcard::Variadic);
    }
    bool wildcardMatch(const AnalyticExpression::Node& pattern,
                       const AnalyticExpression::Node& target,
                       AnalyticExpression::Simplification::Rule::WildcardMap& wildcardMap,
                       std::pmr::memory_resource* memoryResource);
    bool unorderedChildrenMatch(std::vector<const AnalyticExpression::Node*>& patternChildren,
                                std::vector<const AnalyticExpression::Node*>& targetChildren,
                                AnalyticExpression::Simplification::Rule::WildcardMap& wildcardMap,
                                std::pmr::memory_resource* memoryResource)
    {
        assert(std::ranges::none_of(targetChildren, isVariadicNode));
        auto variadicChild = patternChildren.end();

        for (auto patternChild = patternChildren.begin(); patternChild != patternChildren.end(); patternChild++) {
            if (isVariadicNode(*patternChild)) {
                if (variadicChild != patternChildren.end()) {
                    throwext(AnalyticExpression::Simplification::InvalidRuleException(
                        "Variadic wildcard can only appear once in a pattern"));
                }
                variadicChild = patternChild;
            }
            auto matchedTargetChild = std::ranges::find_if(
                targetChildren, [&patternChild, &wildcardMap, memoryResource](const auto* targetChild) {
                    return impl::simplification::rule::match::wildcardMatch(
                        **patternChild, *targetChild, wildcardMap, memoryResource);
                });
            if (matchedTargetChild == targetChildren.end()) {
                return false;
            }
            targetChildren.erase(matchedTargetChild);
        }
        if (variadicChild == patternChildren.end()) {
            return targetChildren.empty();
        }
        const AnalyticExpression::Wildcard::Id wildcardId =
            static_cast<const AnalyticExpression::Wildcard::Variadic*>(*variadicChild)->id;
        if (wildcardMap.contains(wildcardId)) {
            auto previousMatchResult = wildcardMap.at(wildcardId)
                | std::views::transform([](const auto& node) { return node.get(); })
                | std::ranges::to<std::vector<const AnalyticExpression::Node*>>();
            return unorderedChildrenMatch(targetChildren, previousMatchResult, wildcardMap, memoryResource);
        }
        auto matchResult = targetChildren
            | std::views::transform([memoryResource](const auto* node) { return node->clone(memoryResource); })
            | std::ranges::to<std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>>>(memoryResource);
        wildcardMap[wildcardId] = std::move(matchResult);
        return true;
    }
    bool wildcardMatch(const AnalyticExpression::Node& pattern,
                       const AnalyticExpression::Node& target,
                       AnalyticExpression::Simplification::Rule::WildcardMap& wildcardMap,
                       std::pmr::memory_resource* memoryResource)
    {
        const std::type_info& patternType = typeid(pattern);
        const std::type_info& targetType = typeid(target);
        std::vector<const AnalyticExpression::Node*> patternChildren = impl::retrieveChildren(pattern);
        std::vector<const AnalyticExpression::Node*> targetChildren = impl::retrieveChildren(target);

        assert(targetType != typeid(AnalyticExpression::Wildcard::Any));
        if (patternType == typeid(AnalyticExpression::Wildcard::Any)) {
            const AnalyticExpression::Wildcard::Id wildcardId =
                static_cast<const AnalyticExpression::Wildcard::Any&>(pattern).id;
            if (wildcardMap.contains(wildcardId)) {
                assert(wildcardMap.at(wildcardId).size() == 1);
                return impl::simplification::rule::match::wildcardMatch(
                    *wildcardMap.at(wildcardId).front(), target, wildcardMap, memoryResource);
            }
            std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>> matchResult(memoryResource);
            matchResult.push_back(target.clone(memoryResource));
            wildcardMap[wildcardId] = std::move(matchResult);
            return true;
        }
        if (patternType != targetType) {
            return false;
        }
        do { // NOLINT(cppcoreguidelines-avoid-do-while)
            if (patternType != typeid(AnalyticExpression::Addition)
                && patternType != typeid(AnalyticExpression::Multiplication)) {
                break;
            }
            return unorderedChildrenMatch(patternChildren, targetChildren, wildcardMap, memoryResource);
        } while (false);
        if (patternChildren.size() != targetChildren.size()) {
            return false;
        }
        if (patternType == typeid(AnalyticExpression::Constant)) {
            return static_cast<const AnalyticExpression::Constant&>(pattern).value
                == static_cast<const AnalyticExpression::Constant&>(target).value;
        }
        if (patternType == typeid(AnalyticExpression::Variable)) {
            return static_cast<const AnalyticExpression::Variable&>(pattern).name
                == static_cast<const AnalyticExpression::Variable&>(target).name;
        }
        for (std::size_t i = 0; i < patternChildren.size(); ++i) {
            if (!impl::simplification::rule::match::wildcardMatch(
                    *patternChildren[i], *targetChildren[i], wildcardMap, memoryResource)) {
                return false;
            }
        }
        return true;
    }
}} // namespace ::impl::simplification::rule::match

std::optional<AnalyticExpression::Simplification::Rule::WildcardMap>
AnalyticExpression::Simplification::Rule::match(const Node& target, const Context& context) const
{
    AnalyticExpression::Simplification::Rule::WildcardMap wildcardMap(context.memoryResource);
    if (!impl::simplification::rule::match::wildcardMatch(
            *this->pattern, target, wildcardMap, context.memoryResource)) {
        return std::nullopt;
    }
    if (condition && !this->condition(target, wildcardMap, context)) {
        return std::nullopt;
    }
    return wildcardMap;
}
util::unique_pmr_ptr<AnalyticExpression::Node>
AnalyticExpression::Simplification::Rule::apply(WildcardMap map, const Context& context) const
{
    return this->replacer(std::move(map), context);
}

// REFACTOR(P2): Replace with range-based for loop.
namespace { namespace impl::simplification::hill_climbing_algorithm {
    std::optional<util::unique_pmr_ptr<AnalyticExpression::Node>>
    apply(const AnalyticExpression::Simplification::Context& context, const AnalyticExpression::Node& target)
    {
        const Integer originalComplexity = AnalyticExpression::Simplification::complexityOf(target);
        auto candidateNodes = context.rules | std::views::transform([&context, &target](const auto& rule) {
                                  return std::make_pair(std::cref(rule), rule.match(target, context));
                              })
            | std::views::cache_latest
            | std::views::filter([](const auto& rulePair) { return rulePair.second.has_value(); })
            | std::views::transform([&context](auto& rulePair) {
                                  return rulePair.first.apply(std::move(*rulePair.second), context);
                              })
            | std::ranges::to<std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>>>(context.memoryResource);
        const auto candidateNodesComplexities = candidateNodes | std::views::transform([](const auto& node) {
                                                    return AnalyticExpression::Simplification::complexityOf(*node);
                                                });
        const auto minComplexityCanididate = std::ranges::min_element(candidateNodesComplexities);
        if (*minComplexityCanididate >= originalComplexity) {
            return std::nullopt;
        }
        return std::move(*minComplexityCanididate.base());
    }
    util::unique_pmr_ptr<AnalyticExpression::Node>
    applyUntilFixed(const AnalyticExpression::Simplification::Context& context, const AnalyticExpression::Node& target)
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

util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::Simplification::HillClimbingAlgorithm::operator()(
    const AnalyticExpression::Simplification::Context& context, const AnalyticExpression::Node& target) const
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
    impl::normalize<impl::NormalizationMode::Once>(*result);
    return impl::simplification::hill_climbing_algorithm::applyUntilFixed(context, *result);
}
namespace { namespace impl::simplification::e_graph_algorithm {
    using EClassReference = std::uint64_t;
    class EClassReferenceNode : public AnalyticExpression::VisitableNode<EClassReferenceNode> {
    public:
        EClassReference reference;

        explicit EClassReferenceNode(EClassReference reference)
            : reference(reference)
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
            return util::makeUniquePmr<EClassReferenceNode>(memoryResource, reference);
        }
    };
    class EGraph;
    struct ENode {
        util::unique_pmr_ptr<AnalyticExpression::Node> node;

        explicit ENode(EGraph* graph, util::unique_pmr_ptr<AnalyticExpression::Node>&& node)
            : node(std::move(node)),
              graph_(graph)
        { }

        ENode clone(std::pmr::memory_resource* memoryResource) const
        {
            return ENode(graph_, node->clone(memoryResource));
        }

        bool operator==(const ENode& other) const;
        struct Hash {
            [[nodiscard]]
            std::size_t operator()(const ENode& node) const
            {
                return node.node->hash();
            }
        };

    private:
        EGraph* graph_;
    };

    struct EClass {
        std::pmr::unordered_set<ENode, ENode::Hash> members;
    };
    // OPTIMIZE(P2): When new member e-nodes (the Changed) are added to one e-class (the Target), the Changed set
    // their status to Pending; the Target's parent e-class (the Parent) set all their members' status (except
    // the Target) to Pending recursively. This way, neighbor members of the Target marked as Processed will
    // be reused, remember to set them back to Pending again after it's done.                This requires
    // e-classes to be independent, but it doesn't seems like to be the case. what the heck?! ASK THE
    // SHALLOWSLEEP FOR MORE CLUES. goal: find a more elegant solution or just give up on status thing (since
    // just optimization)
    // OPTIMIZE(P2): implement wildcardMatch separately. instead of returning a boolean, it should generate
    // multiple tree matches and corresponding wildcard map. E-nodes marked as Processed should be ignored.
    using WorkList = std::pmr::deque<EClassReference>;
    class EGraph {
    public:
        std::pmr::map<EClassReference, EClass> graph;
        EClassReference entry;
        WorkList workList;

        explicit EGraph(const AnalyticExpression::Node& target,
                        const AnalyticExpression::Simplification::Context& context)
            : graph(context.memoryResource),
              workList(context.memoryResource),
              equivalentClassManager_(this)
        {
            this->entry = findOrCreateClass_(buildNode_(target, context.memoryResource));
            appendFamilyToWorkList_(this->entry, context.memoryResource);
        }

        util::unique_pmr_ptr<AnalyticExpression::Node>
        saturate(const AnalyticExpression::Simplification::Context& context, std::uint64_t maxIterations)
        {
            for (std::uint64_t i = 0; i < maxIterations; i++) {
                if (this->workList.empty()) {
                    break;
                }
                const EClassReference target = this->workList.front();
                this->workList.pop_front();
                saturateClass_(target, context);
            }
            return extractBestSolution_(context.memoryResource);
        }

    private:
        class EquivalentClassManager {
        public:
            std::pmr::unordered_map<EClassReference, EClassReference> mergedIndex;

            EClassReference representativeOf(EClassReference target)
            {
                const auto find = [this](this auto&& find, EClassReference target) -> EClassReference {
                    if (!this->mergedIndex.contains(target)) {
                        return target;
                    }
                    return find(this->mergedIndex[target]);
                };
                const EClassReference representative = find(target);
                if (representative != target) {
                    this->mergedIndex[target] = representative;
                }
                return representative;
            }

            std::size_t
            mergeClass(EClassReference target, EClassReference from, std::pmr::memory_resource* memoryResource)
            {
                target = representativeOf(target);
                from = representativeOf(from);
                if (target == from) {
                    return 0;
                }
                EClass& targetClass = eGraph_->graph.at(target);
                EClass& fromClass = eGraph_->graph.at(from);
                const size_t oldSize = targetClass.members.size();
                targetClass.members.merge(fromClass.members);
                fromClass.members.clear();

                rebuildParents_(target, from, memoryResource);
                this->mergedIndex[from] = target;

                return targetClass.members.size() - oldSize;
            }

        private:
            EGraph* eGraph_;

            explicit EquivalentClassManager(EGraph* eGraph)
                : eGraph_(eGraph)
            { }

            void
            rebuildParents_(EClassReference target, EClassReference from, std::pmr::memory_resource* memoryResource)
            {
                for (const auto parent : eGraph_->findParents_(from, memoryResource)) {
                    EClass& parentClass = eGraph_->graph.at(parent);
                    std::pmr::vector<ENode> members(memoryResource);
                    util::extractSetElement(parentClass.members, members);
                    for (auto& member : members) {
                        const std::vector<AnalyticExpression::Node*> children = impl::retrieveChildren(*member.node);
                        for (auto* child : children) {
                            assert(typeid(*child) == typeid(EClassReferenceNode));
                            EClassReference& childReference = static_cast<EClassReferenceNode*>(child)->reference;
                            if (childReference == from) {
                                childReference = target;
                            }
                        }
                    }
                    parentClass.members.insert_range(members | std::views::as_rvalue);
                }
            }

            friend class EGraph;
        };

        EquivalentClassManager equivalentClassManager_;

        EClassReference findOrCreateClass_(ENode&& node)
        {
            for (auto& [reference, eClass] : this->graph) {
                if (std::ranges::any_of(eClass.members, [&node](const auto& member) { return member == node; })) {
                    return equivalentClassManager_.representativeOf(reference);
                }
            }
            const EClassReference newReference = this->graph.size();
            EClass eClass;
            eClass.members.emplace(std::move(node));
            this->graph[newReference] = std::move(eClass);
            return newReference;
        }
        ENode buildNode_(const AnalyticExpression::Node& node, std::pmr::memory_resource* memoryResource)
        {
            const auto nodeToEClassNode = [this,
                                           memoryResource](const util::unique_pmr_ptr<AnalyticExpression::Node>& node) {
                return util::makeUniquePmr<EClassReferenceNode>(memoryResource,
                                                                findOrCreateClass_(buildNode_(*node, memoryResource)));
            };
            ENode result(this, node.clone(memoryResource));
            if (impl::isLeafNode(node)) {
                return result;
            }
            result.node->accept(AnalyticExpression::NodeVisitor(
                [&nodeToEClassNode](AnalyticExpression::Addition& node) {
                    for (auto& term : node.terms) {
                        term = nodeToEClassNode(term);
                    }
                },
                [&nodeToEClassNode](AnalyticExpression::Multiplication& node) {
                    for (auto& factor : node.factors) {
                        factor = nodeToEClassNode(factor);
                    }
                },
                [&nodeToEClassNode](AnalyticExpression::Power& node) {
                    node.base = nodeToEClassNode(node.base);
                    node.exponent = nodeToEClassNode(node.exponent);
                },
                [&nodeToEClassNode](AnalyticExpression::AbsoluteValue& node) {
                    node.operand = nodeToEClassNode(node.operand);
                },
                [&nodeToEClassNode](AnalyticExpression::Ceiling& node) {
                    node.operand = nodeToEClassNode(node.operand);
                },
                [&nodeToEClassNode](AnalyticExpression::Floor& node) { node.operand = nodeToEClassNode(node.operand); },
                [&nodeToEClassNode](AnalyticExpression::Modulus& node) {
                    node.dividend = nodeToEClassNode(node.dividend);
                    node.divisor = nodeToEClassNode(node.divisor);
                },
                [&nodeToEClassNode](AnalyticExpression::Logarithm& node) {
                    node.argument = nodeToEClassNode(node.argument);
                    node.base = nodeToEClassNode(node.base);
                },
                [&nodeToEClassNode](AnalyticExpression::NaturalLogarithm& node) {
                    node.argument = nodeToEClassNode(node.argument);
                },
                [&nodeToEClassNode](AnalyticExpression::Sine& node) { node.operand = nodeToEClassNode(node.operand); },
                [&nodeToEClassNode](AnalyticExpression::Cosine& node) {
                    node.operand = nodeToEClassNode(node.operand);
                },
                [&nodeToEClassNode](AnalyticExpression::Tangent& node) {
                    node.operand = nodeToEClassNode(node.operand);
                },
                [&nodeToEClassNode](AnalyticExpression::Arcsine& node) {
                    node.operand = nodeToEClassNode(node.operand);
                },
                [&nodeToEClassNode](AnalyticExpression::Arccosine& node) {
                    node.operand = nodeToEClassNode(node.operand);
                },
                [&nodeToEClassNode](AnalyticExpression::Arctangent& node) {
                    node.operand = nodeToEClassNode(node.operand);
                }));
            return result;
        }
        void appendFamilyToWorkList_(EClassReference target, std::pmr::memory_resource* memoryResource)
        {
            std::pmr::unordered_set<EClassReference> processing(memoryResource);
            const auto append = [this](this auto&& append,
                                       EClassReference target,
                                       std::pmr::unordered_set<EClassReference>& processing) -> void {
                if (processing.contains(target)) {
                    return;
                }
                processing.emplace(target);

                const EClass& targetClass = this->graph.at(target);
                for (const auto& member : targetClass.members) {
                    const std::vector<AnalyticExpression::Node*> children = impl::retrieveChildren(*member.node);
                    for (const auto* child : children) {
                        if (typeid(*child) == typeid(EClassReferenceNode)) {
                            append(equivalentClassManager_.representativeOf(
                                       static_cast<const EClassReferenceNode*>(child)->reference),
                                   processing);
                        }
                    }
                }
                this->workList.push_back(target);

                processing.erase(target);
            };
            append(target, processing);
        }
        // OPTIMIZE(P2): Use better matching strategy. When you find out hard to refactor, you'll know it's time.
        [[nodiscard]]
        std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>>
        expandAllPossibleSolutions_(EClassReference target, std::pmr::memory_resource* memoryResource)
        {
            class Solutions : public std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>> {
            public:
                using std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>>::vector;

                Solutions clone(std::pmr::memory_resource* memoryResource) const
                {
                    Solutions result(memoryResource);
                    result.reserve(this->size());
                    std::ranges::transform(*this, std::back_inserter(result), [memoryResource](const auto& node) {
                        return node->clone(memoryResource);
                    });
                    return result;
                }
            };
            const auto expand =
                [this, memoryResource](this auto&& expand,
                                       EClassReference target,
                                       std::pmr::unordered_set<EClassReference>& processing,
                                       std::pmr::unordered_map<EClassReference, Solutions>& cache) -> Solutions {
                target = equivalentClassManager_.representativeOf(target);
                if (auto cached = cache.find(target); cached != cache.end()) {
                    return cached->second.clone(memoryResource);
                }
                if (processing.contains(target)) {
                    return { };
                }
                processing.insert(target);

                const EClass& targetClass = this->graph.at(target);
                Solutions solutions(memoryResource);
                for (const auto& member : targetClass.members) {
                    if (impl::isLeafNode(*member.node)) {
                        solutions.push_back(member.node->clone(memoryResource));
                        continue;
                    }
                    member.node->accept(AnalyticExpression::NodeVisitorConst(
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::Addition& node) {
                            std::pmr::vector<std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>>>
                                termsSolutions(memoryResource);
                            for (const auto& term : node.terms) {
                                assert(typeid(*term) == typeid(EClassReferenceNode));
                                termsSolutions.push_back(
                                    expand(equivalentClassManager_.representativeOf(
                                               static_cast<const EClassReferenceNode&>(*term).reference),
                                           processing,
                                           cache));
                            }
                            std::ranges::transform(
                                util::cartesianProduct(std::move(termsSolutions)),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& solution) {
                                    return util::makeUniquePmr<AnalyticExpression::Addition>(
                                        memoryResource,
                                        solution | std::views::transform([memoryResource](const auto& term) {
                                            return term->clone(memoryResource);
                                        })
                                            | std::ranges::to<
                                                std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>>>(
                                                memoryResource));
                                });
                        },
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::Multiplication& node) {
                            std::pmr::vector<std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>>>
                                factorsSolutions(memoryResource);
                            for (const auto& factor : node.factors) {
                                assert(typeid(*factor) == typeid(EClassReferenceNode));
                                factorsSolutions.push_back(
                                    expand(equivalentClassManager_.representativeOf(
                                               static_cast<const EClassReferenceNode&>(*factor).reference),
                                           processing,
                                           cache));
                            }
                            std::ranges::transform(
                                util::cartesianProduct(std::move(factorsSolutions)),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& solution) {
                                    return util::makeUniquePmr<AnalyticExpression::Multiplication>(
                                        memoryResource,
                                        solution | std::views::transform([memoryResource](const auto& term) {
                                            return term->clone(memoryResource);
                                        })
                                            | std::ranges::to<
                                                std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>>>(
                                                memoryResource));
                                });
                        },
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::Power& node) {
                            std::ranges::transform(
                                std::views::cartesian_product(
                                    expand(equivalentClassManager_.representativeOf(
                                               static_cast<const EClassReferenceNode&>(*node.base).reference),
                                           processing,
                                           cache),
                                    expand(equivalentClassManager_.representativeOf(
                                               static_cast<const EClassReferenceNode&>(*node.exponent).reference),
                                           processing,
                                           cache)),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& solution) {
                                    const auto& [baseSolution, exponentSolution] = solution;
                                    return util::makeUniquePmr<AnalyticExpression::Power>(
                                        memoryResource,
                                        baseSolution->clone(memoryResource),
                                        exponentSolution->clone(memoryResource));
                                });
                        },
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::AbsoluteValue& node) {
                            std::ranges::transform(
                                expand(equivalentClassManager_.representativeOf(
                                           static_cast<const EClassReferenceNode&>(*node.operand).reference),
                                       processing,
                                       cache),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& operandSolution) {
                                    return util::makeUniquePmr<AnalyticExpression::AbsoluteValue>(
                                        memoryResource, operandSolution->clone(memoryResource));
                                });
                        },
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::Ceiling& node) {
                            std::ranges::transform(
                                expand(equivalentClassManager_.representativeOf(
                                           static_cast<const EClassReferenceNode&>(*node.operand).reference),
                                       processing,
                                       cache),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& operandSolution) {
                                    return util::makeUniquePmr<AnalyticExpression::Ceiling>(
                                        memoryResource, operandSolution->clone(memoryResource));
                                });
                        },
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::Floor& node) {
                            std::ranges::transform(
                                expand(equivalentClassManager_.representativeOf(
                                           static_cast<const EClassReferenceNode&>(*node.operand).reference),
                                       processing,
                                       cache),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& operandSolution) {
                                    return util::makeUniquePmr<AnalyticExpression::Floor>(
                                        memoryResource, operandSolution->clone(memoryResource));
                                });
                        },
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::Modulus& node) {
                            std::ranges::transform(
                                std::views::cartesian_product(
                                    expand(equivalentClassManager_.representativeOf(
                                               static_cast<const EClassReferenceNode&>(*node.dividend).reference),
                                           processing,
                                           cache),
                                    expand(equivalentClassManager_.representativeOf(
                                               static_cast<const EClassReferenceNode&>(*node.divisor).reference),
                                           processing,
                                           cache)),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& solution) {
                                    const auto& [dividendSolution, divisorSolution] = solution;
                                    return util::makeUniquePmr<AnalyticExpression::Modulus>(
                                        memoryResource,
                                        dividendSolution->clone(memoryResource),
                                        divisorSolution->clone(memoryResource));
                                });
                        },
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::Logarithm& node) {
                            std::ranges::transform(
                                std::views::cartesian_product(
                                    expand(equivalentClassManager_.representativeOf(
                                               static_cast<const EClassReferenceNode&>(*node.argument).reference),
                                           processing,
                                           cache),
                                    expand(equivalentClassManager_.representativeOf(
                                               static_cast<const EClassReferenceNode&>(*node.base).reference),
                                           processing,
                                           cache)),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& solution) {
                                    const auto& [argumentSolution, baseSolution] = solution;
                                    return util::makeUniquePmr<AnalyticExpression::Logarithm>(
                                        memoryResource,
                                        argumentSolution->clone(memoryResource),
                                        baseSolution->clone(memoryResource));
                                });
                        },
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::NaturalLogarithm& node) {
                            std::ranges::transform(
                                expand(equivalentClassManager_.representativeOf(
                                           static_cast<const EClassReferenceNode&>(*node.argument).reference),
                                       processing,
                                       cache),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& argumentSolution) {
                                    return util::makeUniquePmr<AnalyticExpression::NaturalLogarithm>(
                                        memoryResource, argumentSolution->clone(memoryResource));
                                });
                        },
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::Sine& node) {
                            std::ranges::transform(
                                expand(equivalentClassManager_.representativeOf(
                                           static_cast<const EClassReferenceNode&>(*node.operand).reference),
                                       processing,
                                       cache),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& operandSolution) {
                                    return util::makeUniquePmr<AnalyticExpression::Sine>(
                                        memoryResource, operandSolution->clone(memoryResource));
                                });
                        },
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::Cosine& node) {
                            std::ranges::transform(
                                expand(equivalentClassManager_.representativeOf(
                                           static_cast<const EClassReferenceNode&>(*node.operand).reference),
                                       processing,
                                       cache),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& operandSolution) {
                                    return util::makeUniquePmr<AnalyticExpression::Cosine>(
                                        memoryResource, operandSolution->clone(memoryResource));
                                });
                        },
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::Tangent& node) {
                            std::ranges::transform(
                                expand(equivalentClassManager_.representativeOf(
                                           static_cast<const EClassReferenceNode&>(*node.operand).reference),
                                       processing,
                                       cache),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& operandSolution) {
                                    return util::makeUniquePmr<AnalyticExpression::Tangent>(
                                        memoryResource, operandSolution->clone(memoryResource));
                                });
                        },
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::Arcsine& node) {
                            std::ranges::transform(
                                expand(equivalentClassManager_.representativeOf(
                                           static_cast<const EClassReferenceNode&>(*node.operand).reference),
                                       processing,
                                       cache),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& operandSolution) {
                                    return util::makeUniquePmr<AnalyticExpression::Arcsine>(
                                        memoryResource, operandSolution->clone(memoryResource));
                                });
                        },
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::Arccosine& node) {
                            std::ranges::transform(
                                expand(equivalentClassManager_.representativeOf(
                                           static_cast<const EClassReferenceNode&>(*node.operand).reference),
                                       processing,
                                       cache),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& operandSolution) {
                                    return util::makeUniquePmr<AnalyticExpression::Arccosine>(
                                        memoryResource, operandSolution->clone(memoryResource));
                                });
                        },
                        [this, &expand, &solutions, &processing, &cache, memoryResource](
                            const AnalyticExpression::Arctangent& node) {
                            std::ranges::transform(
                                expand(equivalentClassManager_.representativeOf(
                                           static_cast<const EClassReferenceNode&>(*node.operand).reference),
                                       processing,
                                       cache),
                                std::back_inserter(solutions),
                                [memoryResource](const auto& operandSolution) {
                                    return util::makeUniquePmr<AnalyticExpression::Arctangent>(
                                        memoryResource, operandSolution->clone(memoryResource));
                                });
                        }));
                }
                for (const auto& solution : solutions) {
                    impl::normalize<impl::NormalizationMode::Once>(*solution);
                }
                processing.erase(target);
                cache.emplace(target, solutions.clone(memoryResource));
                return solutions;
            };
            std::pmr::unordered_set<EClassReference> processing(memoryResource);
            std::pmr::unordered_map<EClassReference, Solutions> cache(memoryResource);
            return expand(target, processing, cache);
        }
        [[nodiscard]]
        std::pmr::vector<EClassReference> findParents_(EClassReference target,
                                                       std::pmr::memory_resource* memoryResource)
        {
            return this->graph | std::views::filter([this, target](const auto& parent) {
                       if (equivalentClassManager_.representativeOf(parent.first) != parent.first) {
                           return false;
                       }
                       for (const auto& member : parent.second.members) {
                           const std::vector<AnalyticExpression::Node*> children = impl::retrieveChildren(*member.node);
                           assert(std::ranges::all_of(children, [](const auto* child) {
                               return typeid(*child) == typeid(EClassReferenceNode);
                           }));
                           if (std::ranges::any_of(children, [this, target](const auto* child) {
                                   return equivalentClassManager_.representativeOf(
                                              static_cast<const EClassReferenceNode*>(child)->reference)
                                       == target;
                               })) {
                               return true;
                           }
                       }
                       return false;
                   })
                | std::views::transform([](const auto& parent) { return parent.first; })
                | std::ranges::to<std::pmr::vector<EClassReference>>(memoryResource);
        }

        void saturateClass_(EClassReference target, const AnalyticExpression::Simplification::Context& context)
        {
            target = equivalentClassManager_.representativeOf(target);
            std::size_t totalMembersAdded = 0;
            std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>> solutions =
                expandAllPossibleSolutions_(target, context.memoryResource);
            for (auto& solution : solutions) {
                auto availableRules = context.rules | std::views::transform([&context, &solution](const auto& rule) {
                                          return std::make_pair(std::cref(rule), rule.match(*solution, context));
                                      })
                    | std::views::cache_latest
                    | std::views::filter([](const auto& rulePair) { return rulePair.second.has_value(); });
                for (auto& rulePair : availableRules) {
                    util::unique_pmr_ptr<AnalyticExpression::Node> applied =
                        rulePair.first.apply(std::move(*rulePair.second), context);
                    impl::normalize<impl::NormalizationMode::Full>(*applied);
                    const EClassReference newMember = findOrCreateClass_(buildNode_(*applied, context.memoryResource));
                    if (const std::size_t membersAdded =
                            equivalentClassManager_.mergeClass(target, newMember, context.memoryResource);
                        membersAdded > 0) {
                        totalMembersAdded += membersAdded;
                    }
                }
            }
            if (totalMembersAdded == 0) {
                return;
            }
            appendFamilyToWorkList_(target, context.memoryResource);
            for (const auto parent : findParents_(target, context.memoryResource)) {
                this->workList.push_back(parent);
            }
        }

        [[nodiscard]]
        util::unique_pmr_ptr<AnalyticExpression::Node> extractBestSolution_(std::pmr::memory_resource* memoryResource)
        {
            std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>> solutions =
                expandAllPossibleSolutions_(equivalentClassManager_.representativeOf(this->entry), memoryResource);
            assert(!solutions.empty());
            auto bestSolution = std::ranges::min_element(solutions, [](const auto& a, const auto& b) {
                return AnalyticExpression::Simplification::complexityOf(*a)
                    < AnalyticExpression::Simplification::complexityOf(*b);
            });
            return std::move(*bestSolution);
        }

        friend class EquivalentClassManager;
        friend struct ENode;
    };

    bool ENode::operator==(const ENode& other) const
    {
        return structuralEqual(
            *node, *other.node, [this](const AnalyticExpression::Node& a, const AnalyticExpression::Node& b) -> bool {
                if (typeid(a) == typeid(EClassReferenceNode)) {
                    return graph_->equivalentClassManager_.representativeOf(
                               static_cast<const EClassReferenceNode&>(a).reference)
                        == graph_->equivalentClassManager_.representativeOf(
                            static_cast<const EClassReferenceNode&>(b).reference);
                }
                return true;
            });
    }
}} // namespace ::impl::simplification::e_graph_algorithm
AnalyticExpression::Simplification::EGraphAlgorithm::EGraphAlgorithm(std::size_t maxNodes)
    : maxNodes(maxNodes)
{ }
util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::Simplification::EGraphAlgorithm::operator()(
    const AnalyticExpression::Simplification::Context& context, const AnalyticExpression::Node& target) const
{
    return impl::simplification::e_graph_algorithm::EGraph(target, context).saturate(context, this->maxNodes);
}

AnalyticExpression::Simplification::Context::Context(const AnalyticExpression& expr)
    : Context(expr.memoryResource())
{ }
AnalyticExpression::Simplification::Context::Context(std::pmr::memory_resource* memoryResource)
    : rules(generateDefaultRules(memoryResource)),
      algorithm(util::makeUniquePmr<SequenceAlgorithm<HillClimbingAlgorithm, EGraphAlgorithm>>(
          memoryResource, HillClimbingAlgorithm(), EGraphAlgorithm(1'000'000))),
      approximation(RationalCalculationOptions().approximation),
      memoryResource(memoryResource)
{ }

AnalyticExpression::Simplification::RuleSet
AnalyticExpression::Simplification::generateDefaultRules(std::pmr::memory_resource* memoryResource)
{
    RuleSet rules(memoryResource);
    
    return rules;
}
bool AnalyticExpression::Simplification::structuralEqual(const AnalyticExpression::Node& a,
                                                         const AnalyticExpression::Node& b)
{
    return impl::structuralEqual(a, b);
}
Integer AnalyticExpression::Simplification::complexityOf(const AnalyticExpression::Node& node)
{
    const auto childAccumulator = [](const Integer& accumulation, const util::unique_pmr_ptr<Node>& child) {
        return accumulation + complexityOf(*child);
    };
    Integer complexity;
    node.accept(NodeVisitorConst(
        [&complexity](const AnalyticExpression::Constant&) { complexity = 1; },
        [&complexity](const AnalyticExpression::Variable&) { complexity = 1; },
        [&complexity](const AnalyticExpression::Infinity&) { complexity = 1; },
        [&complexity](const AnalyticExpression::Pi&) { complexity = 1; },
        [&complexity](const AnalyticExpression::Euler&) { complexity = 1; },
        [&complexity](const AnalyticExpression::ImaginaryUnit&) { complexity = 1; },
        [&childAccumulator, &complexity](const AnalyticExpression::Addition& node) {
            complexity = 2 * std::ranges::fold_left(node.terms, Integer(), childAccumulator);
        },
        [&childAccumulator, &complexity](const AnalyticExpression::Multiplication& node) {
            complexity = 4 * std::ranges::fold_left(node.factors, Integer(), childAccumulator);
        },
        [&complexity](const AnalyticExpression::Power& node) {
            complexity = 16 * (complexityOf(*node.base) + complexityOf(*node.exponent));
        },
        [&complexity](const AnalyticExpression::AbsoluteValue& node) { complexity = 8 * complexityOf(*node.operand); },
        [&complexity](const AnalyticExpression::Ceiling& node) { complexity = 8 * complexityOf(*node.operand); },
        [&complexity](const AnalyticExpression::Floor& node) { complexity = 8 * complexityOf(*node.operand); },
        [&complexity](const AnalyticExpression::Modulus& node) {
            complexity = 8 * (complexityOf(*node.dividend) + complexityOf(*node.divisor));
        },
        [&complexity](const AnalyticExpression::Logarithm& node) {
            complexity = 32 * (complexityOf(*node.argument) + complexityOf(*node.base));
        },
        [&complexity](const AnalyticExpression::NaturalLogarithm& node) {
            complexity = 32 * (1 + complexityOf(*node.argument));
        },
        [&complexity](const AnalyticExpression::Sine& node) { complexity = 64 * complexityOf(*node.operand); },
        [&complexity](const AnalyticExpression::Cosine& node) { complexity = 64 * complexityOf(*node.operand); },
        [&complexity](const AnalyticExpression::Tangent& node) { complexity = 64 * complexityOf(*node.operand); },
        [&complexity](const AnalyticExpression::Arcsine& node) { complexity = 64 * complexityOf(*node.operand); },
        [&complexity](const AnalyticExpression::Arccosine& node) { complexity = 64 * complexityOf(*node.operand); },
        [&complexity](const AnalyticExpression::Arctangent& node) { complexity = 64 * complexityOf(*node.operand); }));
    return complexity;
}

AnalyticExpression::Factory::Factory(std::shared_ptr<std::pmr::memory_resource> memoryResource)
    : memoryResource_(memoryResource)
{ }
AnalyticExpression::Factory::Factory(const AnalyticExpression& expr)
    : memoryResource_(expr.memoryResource_)
{ }

AnalyticExpression AnalyticExpression::Factory::constant(Rational value)
{
    return AnalyticExpression(util::makeUniquePmr<Constant>(memoryResource_.get(), std::move(value)), memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::variable(std::string_view name)
{
    return AnalyticExpression(
        util::makeUniquePmr<Variable>(memoryResource_.get(), std::pmr::string(name, memoryResource_.get())),
        memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::infinity()
{
    return AnalyticExpression(util::makeUniquePmr<Infinity>(memoryResource_.get()), memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::pi()
{
    return AnalyticExpression(util::makeUniquePmr<Pi>(memoryResource_.get()), memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::euler()
{
    return AnalyticExpression(util::makeUniquePmr<Euler>(memoryResource_.get()), memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::imaginary()
{
    return AnalyticExpression(util::makeUniquePmr<ImaginaryUnit>(memoryResource_.get()), memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::power(AnalyticExpression base, AnalyticExpression exponent)
{
    return AnalyticExpression(
        util::makeUniquePmr<Power>(memoryResource_.get(), std::move(base.base), std::move(exponent.base)),
        memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::absoluteValue(AnalyticExpression operand)
{
    return AnalyticExpression(util::makeUniquePmr<AbsoluteValue>(memoryResource_.get(), std::move(operand.base)),
                              memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::ceiling(AnalyticExpression operand)
{
    return AnalyticExpression(util::makeUniquePmr<Ceiling>(memoryResource_.get(), std::move(operand.base)),
                              memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::floor(AnalyticExpression operand)
{
    return AnalyticExpression(util::makeUniquePmr<Floor>(memoryResource_.get(), std::move(operand.base)),
                              memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::modulus(AnalyticExpression dividend, AnalyticExpression divisor)
{
    return AnalyticExpression(
        util::makeUniquePmr<Modulus>(memoryResource_.get(), std::move(dividend.base), std::move(divisor.base)),
        memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::logarithm(AnalyticExpression argument, AnalyticExpression base)
{
    return AnalyticExpression(
        util::makeUniquePmr<Logarithm>(memoryResource_.get(), std::move(argument.base), std::move(base.base)),
        memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::naturalLogarithm(AnalyticExpression argument)
{
    return AnalyticExpression(util::makeUniquePmr<NaturalLogarithm>(memoryResource_.get(), std::move(argument.base)),
                              memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::sine(AnalyticExpression operand)
{
    return AnalyticExpression(util::makeUniquePmr<Sine>(memoryResource_.get(), std::move(operand.base)),
                              memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::cosine(AnalyticExpression operand)
{
    return AnalyticExpression(util::makeUniquePmr<Cosine>(memoryResource_.get(), std::move(operand.base)),
                              memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::tangent(AnalyticExpression operand)
{
    return AnalyticExpression(util::makeUniquePmr<Tangent>(memoryResource_.get(), std::move(operand.base)),
                              memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::arcsine(AnalyticExpression operand)
{
    return AnalyticExpression(util::makeUniquePmr<Arcsine>(memoryResource_.get(), std::move(operand.base)),
                              memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::arccosine(AnalyticExpression operand)
{
    return AnalyticExpression(util::makeUniquePmr<Arccosine>(memoryResource_.get(), std::move(operand.base)),
                              memoryResource_);
}
AnalyticExpression AnalyticExpression::Factory::arctangent(AnalyticExpression operand)
{
    return AnalyticExpression(util::makeUniquePmr<Arctangent>(memoryResource_.get(), std::move(operand.base)),
                              memoryResource_);
}

AnalyticExpression::AnalyticExpression(std::shared_ptr<std::pmr::memory_resource> memoryResource)
    : memoryResource_(memoryResource)
{ }
AnalyticExpression::AnalyticExpression(const Node& node, std::shared_ptr<std::pmr::memory_resource> memoryResource)
    : base(node.clone(memoryResource.get())),
      memoryResource_(memoryResource)
{ }
AnalyticExpression::AnalyticExpression(const util::unique_pmr_ptr<Node>& node,
                                       std::shared_ptr<std::pmr::memory_resource> memoryResource)
    : base(node->clone(memoryResource.get())),
      memoryResource_(memoryResource)
{ }
AnalyticExpression::AnalyticExpression(util::unique_pmr_ptr<Node>&& node,
                                       std::shared_ptr<std::pmr::memory_resource> memoryResource)
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
void AnalyticExpression::normalize() { impl::normalize<impl::NormalizationMode::Full>(*this->base); }
void AnalyticExpression::simplify(const Simplification::Context& context)
{
    this->normalize();
    this->base = (*context.algorithm)(context, *this->base);
}
std::pmr::memory_resource* AnalyticExpression::memoryResource() const { return memoryResource_.get(); }

AnalyticExpression normalize(AnalyticExpression expr)
{
    impl::normalize<impl::NormalizationMode::Full>(*expr.base);
    return expr;
}
AnalyticExpression simplify(AnalyticExpression expr, const AnalyticExpression::Simplification::Context& context)
{
    impl::normalize<impl::NormalizationMode::Full>(*expr.base);
    expr.base = (*context.algorithm)(context, *expr.base);
    return expr;
}
} // namespace thecalculater::math