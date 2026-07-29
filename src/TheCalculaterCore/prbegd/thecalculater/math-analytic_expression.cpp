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
namespace { namespace _ {
    bool structuralEqual(util::observer_ptr<const AnalyticExpression::Node> a, util::observer_ptr<const AnalyticExpression::Node> b)
    {
        if (a == b) {
            return true;
        }
        if (!a || !b) {
            return false;
        }
        enum NodeType : std::uint8_t { // NOLINT(cppcoreguidelines-use-enum-class)
            Constant = 0,
            Variable,
            Infinity,
            Pi,
            Euler,
            ImaginaryUnit,
            Addition,
            Multiplication,
            Power,
            AbsoluteValue,
            Ceiling,
            Floor,
            Modulus,
            Logarithm,
            NaturalLogarithm,
            Sine,
            Cosine,
            Tangent,
            Arcsine,
            Arccosine,
            Arctangent,
            WildcardAny,
            WildcardVariadic,
        };
        std::vector<util::observer_ptr<const AnalyticExpression::Node>> aChildren;
        std::vector<util::observer_ptr<const AnalyticExpression::Node>> bChildren;
        std::optional<std::uint8_t> aType;
        std::optional<std::uint8_t> bType;
        auto* visitingChildren = &aChildren;
        auto* visitingType = &aType;
        const AnalyticExpression::NodeVisitorConst visitor(
            [visitingType](const AnalyticExpression::Constant& node) {
                *visitingType = Constant;
            },
            [visitingType](const AnalyticExpression::Variable& node) {
                *visitingType = Variable;
            },
            [visitingType](const AnalyticExpression::Infinity& node) {
                *visitingType = Infinity;
            },
            [visitingType](const AnalyticExpression::Pi& node) {
                *visitingType = Pi;
            },
            [visitingType](const AnalyticExpression::Euler& node) {
                *visitingType = Euler;
            },
            [visitingType](const AnalyticExpression::ImaginaryUnit& node) {
                *visitingType = ImaginaryUnit;
            },
            [visitingChildren, visitingType](const AnalyticExpression::Addition& node) {
                visitingChildren->append_range(node.terms | std::views::transform([](auto& term) { return term.get(); }));
                *visitingType = Addition;
            },
            [visitingChildren, visitingType](const AnalyticExpression::Multiplication& node) {
                visitingChildren->append_range(node.factors | std::views::transform([](auto& factor) { return factor.get(); }));
                *visitingType = Multiplication;
            },
            [visitingChildren, visitingType](const AnalyticExpression::Power& node) {
                visitingChildren->push_back(node.base.get());
                visitingChildren->push_back(node.exponent.get());
                *visitingType = Power;
            },
            [visitingChildren, visitingType](const AnalyticExpression::AbsoluteValue& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = AbsoluteValue;
            },
            [visitingChildren, visitingType](const AnalyticExpression::Ceiling& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Ceiling;
            },
            [visitingChildren, visitingType](const AnalyticExpression::Floor& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Floor;
            },
            [visitingChildren, visitingType](const AnalyticExpression::Modulus& node) {
                visitingChildren->push_back(node.dividend.get());
                visitingChildren->push_back(node.divisor.get());
                *visitingType = Modulus;
            },
            [visitingChildren, visitingType](const AnalyticExpression::Logarithm& node) {
                visitingChildren->push_back(node.base.get());
                visitingChildren->push_back(node.operand.get());
                *visitingType = Logarithm;
            },
            [visitingChildren, visitingType](const AnalyticExpression::NaturalLogarithm& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = NaturalLogarithm;
            },
            [visitingChildren, visitingType](const AnalyticExpression::Sine& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Sine;
            },
            [visitingChildren, visitingType](const AnalyticExpression::Cosine& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Cosine;
            },
            [visitingChildren, visitingType](const AnalyticExpression::Tangent& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Tangent;
            },
            [visitingChildren, visitingType](const AnalyticExpression::Arcsine& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Arcsine;
            },
            [visitingChildren, visitingType](const AnalyticExpression::Arccosine& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Arccosine;
            },
            [visitingChildren, visitingType](const AnalyticExpression::Arctangent& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Arctangent;
            },
            [&visitingType](const AnalyticExpression::Wildcard::Any&) {
                *visitingType = WildcardAny;
            },
            [&visitingType](const AnalyticExpression::Wildcard::Variadic&) {
                *visitingType = WildcardVariadic;
            });
        a->accept(visitor);
        visitingChildren = &bChildren;
        visitingType = &bType;
        b->accept(visitor);

        if ((!aType.has_value() || !bType.has_value()) || *aType != *bType || aChildren.size() != bChildren.size()) {
            return false;
        }
        if (aType == Constant) {
            return static_cast<const AnalyticExpression::Constant*>(a)->value == static_cast<const AnalyticExpression::Constant*>(b)->value; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        }
        if (aType == Variable) {
            return static_cast<const AnalyticExpression::Variable*>(a)->name == static_cast<const AnalyticExpression::Variable*>(b)->name; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        }
        for (std::size_t i = 0; i < aChildren.size(); ++i) {
            if (!structuralEqual(aChildren[i], bChildren[i])) {
                return false;
            }
        }
        return true;
    }
    struct NodeStrictEqual {
        bool operator()(const util::unique_pmr_ptr<AnalyticExpression::Node>& a, const util::unique_pmr_ptr<AnalyticExpression::Node>& b) const
        {
            if (a == b) {
                return true;
            }
            return _::structuralEqual(a.get(), b.get());
        }
        bool operator()(util::unique_pmr_ptr<AnalyticExpression::Node>& a, util::unique_pmr_ptr<AnalyticExpression::Node>& b) const
        {
            if (a == b) {
                return true;
            }
            return _::structuralEqual(a.get(), b.get());
        }
    };
    struct NodeHash {
        std::size_t operator()(const util::unique_pmr_ptr<AnalyticExpression::Node>& a) const
        {
            if (!a) {
                return 0;
            }
            return a->hash();
        }
        std::size_t operator()(util::unique_pmr_ptr<AnalyticExpression::Node>& a) const
        {
            if (!a) {
                return 0;
            }
            return a->hash();
        }
    };
    using CandidateNodes = std::pmr::unordered_set<util::unique_pmr_ptr<AnalyticExpression::Node>, NodeHash, NodeStrictEqual>;
}} // namespace ::_
AnalyticExpression::Wildcard::UsedInCalculationException::UsedInCalculationException(const std::string& message)
    : std::logic_error(message)
{ }

AnalyticExpression::Node::Node(util::observer_ptr<Node> parent)
    : parent(parent)
{ }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_CONSTRUCTOR0_(_class_) \
    AnalyticExpression::_class_::_class_(util::observer_ptr<Node> parent) \
        : VisitableNode(parent) \
    { }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_CONSTRUCTOR1_(_class_, _member_) \
    AnalyticExpression::_class_::_class_(util::observer_ptr<Node> parent, const util::unique_pmr_ptr<Node>&(_member_), util::observer_ptr<std::pmr::memory_resource> memoryResource) \
        : VisitableNode(parent), \
          _member_((_member_)->clone(memoryResource)) \
    { \
        this->_member_->parent = this; \
    } \
    AnalyticExpression::_class_::_class_(util::observer_ptr<Node> parent, util::unique_pmr_ptr<Node> && (_member_)) \
        : VisitableNode(parent), \
          _member_(std::move(_member_)) \
    { \
        this->_member_->parent = this; \
    }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_CONSTRUCTOR2_(_class_, _member1_, _member2_) \
    AnalyticExpression::_class_::_class_(util::observer_ptr<Node> parent, const util::unique_pmr_ptr<Node>&(_member1_), const util::unique_pmr_ptr<Node>&(_member2_), util::observer_ptr<std::pmr::memory_resource> memoryResource) \
        : VisitableNode(parent), \
          _member1_((_member1_)->clone(memoryResource)), \
          _member2_((_member2_)->clone(memoryResource)) \
    { \
        this->_member1_->parent = this; \
        this->_member2_->parent = this; \
    } \
    AnalyticExpression::_class_::_class_(util::observer_ptr<Node> parent, util::unique_pmr_ptr<Node> && (_member1_), util::unique_pmr_ptr<Node> && (_member2_)) \
        : VisitableNode(parent), \
          _member1_(std::move(_member1_)), \
          _member2_(std::move(_member2_)) \
    { \
        this->_member1_->parent = this; \
        this->_member2_->parent = this; \
    }
AnalyticExpression::Wildcard::Any::Any(util::observer_ptr<Node> parent, Wildcard::id_t id)
    : WildNode(parent),
      id(id)
{ }
AnalyticExpression::Wildcard::Variadic::Variadic(util::observer_ptr<Node> parent, Wildcard::id_t id)
    : WildNode(parent),
      id(id)
{ }
AnalyticExpression::Constant::Constant(util::observer_ptr<Node> parent, Rational value)
    : VisitableNode(parent),
      value(std::move(value))
{ }
AnalyticExpression::Variable::Variable(util::observer_ptr<Node> parent, std::string_view name, util::observer_ptr<std::pmr::memory_resource> memoryResource)
    : VisitableNode(parent),
      name(name, memoryResource)
{ }
AnalyticExpression::Variable::Variable(util::observer_ptr<Node> parent, std::pmr::string&& name)
    : VisitableNode(parent),
      name(std::move(name))
{ }
NODE_CONSTRUCTOR0_(Infinity)
NODE_CONSTRUCTOR0_(Pi)
NODE_CONSTRUCTOR0_(Euler)
NODE_CONSTRUCTOR0_(ImaginaryUnit)
AnalyticExpression::Addition::Addition(util::observer_ptr<Node> parent, std::pmr::vector<util::unique_pmr_ptr<Node>>&& terms)
    : VisitableNode(parent),
      terms(std::move(terms))
{
    for (const auto& term : this->terms) {
        term->parent = this;
    }
}
AnalyticExpression::Multiplication::Multiplication(util::observer_ptr<Node> parent, std::pmr::vector<util::unique_pmr_ptr<Node>>&& factors)
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
NODE_CONSTRUCTOR2_(Logarithm, base, operand)
NODE_CONSTRUCTOR1_(NaturalLogarithm, operand)
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
NODE_METHOD_HASH2_(Logarithm, 0xfa76de7ccdb3659d, this->base, this->operand)
NODE_METHOD_HASH1_(NaturalLogarithm, 0xffb7367750971651, this->operand)
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
    util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::_class_::clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const \
    { \
        return util::makeUniquePmr<_class_>(memoryResource, this->parent); \
    }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_METHOD_CLONE1_(_class_, _parameter_) \
    util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::_class_::clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const \
    { \
        return util::makeUniquePmr<_class_>(memoryResource, this->parent, _parameter_); \
    }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_METHOD_CLONE2_(_class_, _parameter1_, _parameter2_) \
    util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::_class_::clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const \
    { \
        return util::makeUniquePmr<_class_>(memoryResource, this->parent, _parameter1_, _parameter2_); \
    }
NODE_METHOD_CLONE1_(Wildcard::Any, this->id)
NODE_METHOD_CLONE1_(Wildcard::Variadic, this->id)
NODE_METHOD_CLONE1_(Constant, this->value)
util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::Addition::clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const
{
    std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>> terms;
    for (const auto& term : this->terms) {
        terms.push_back(term->clone(memoryResource));
    }
    return util::makeUniquePmr<Addition>(memoryResource, this->parent, std::move(terms));
}
util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::Multiplication::clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const
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
NODE_METHOD_CLONE2_(Logarithm, this->base->clone(memoryResource), this->operand->clone(memoryResource))
NODE_METHOD_CLONE1_(NaturalLogarithm, this->operand->clone(memoryResource))
NODE_METHOD_CLONE1_(Sine, this->operand->clone(memoryResource))
NODE_METHOD_CLONE1_(Cosine, this->operand->clone(memoryResource))
NODE_METHOD_CLONE1_(Tangent, this->operand->clone(memoryResource))
NODE_METHOD_CLONE1_(Arcsine, this->operand->clone(memoryResource))
NODE_METHOD_CLONE1_(Arccosine, this->operand->clone(memoryResource))
NODE_METHOD_CLONE1_(Arctangent, this->operand->clone(memoryResource))
#undef NODE_METHOD_CLONE0_
#undef NODE_METHOD_CLONE1_
#undef NODE_METHOD_CLONE2_

namespace { namespace _simplification_rule_match {
    bool wildcardMatch(util::observer_ptr<const AnalyticExpression::Node> a, util::observer_ptr<const AnalyticExpression::Node> b, AnalyticExpression::Simplification::Rule::wildcard_map_t& wildcardMap, util::observer_ptr<std::pmr::memory_resource> memoryResource)
    {
        enum NodeType : std::uint8_t { // NOLINT(cppcoreguidelines-use-enum-class)
            Constant = 0,
            Variable,
            Infinity,
            Pi,
            Euler,
            ImaginaryUnit,
            Addition,
            Multiplication,
            Power,
            AbsoluteValue,
            Ceiling,
            Floor,
            Modulus,
            Logarithm,
            NaturalLogarithm,
            Sine,
            Cosine,
            Tangent,
            Arcsine,
            Arccosine,
            Arctangent,
            WildcardAny,
            WildcardVariadic,
        };
        std::pmr::vector<util::observer_ptr<const AnalyticExpression::Node>> aChildren(memoryResource);
        std::pmr::vector<util::observer_ptr<const AnalyticExpression::Node>> bChildren(memoryResource);
        std::optional<std::uint8_t> aType;
        std::optional<std::uint8_t> bType;
        auto* visitingChildren = &aChildren;
        auto* visitingType = &aType;
        const AnalyticExpression::NodeVisitorConst visitor(
            [&visitingType](const AnalyticExpression::Constant&) {
                *visitingType = Constant;
            },
            [&visitingType](const AnalyticExpression::Variable&) {
                *visitingType = Variable;
            },
            [&visitingType](const AnalyticExpression::Infinity&) {
                *visitingType = Infinity;
            },
            [&visitingType](const AnalyticExpression::Pi&) {
                *visitingType = Pi;
            },
            [&visitingType](const AnalyticExpression::Euler&) {
                *visitingType = Euler;
            },
            [&visitingType](const AnalyticExpression::ImaginaryUnit&) {
                *visitingType = ImaginaryUnit;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::Addition& node) {
                visitingChildren->append_range(node.terms | std::views::transform([](auto& term) { return term.get(); }));
                *visitingType = Addition;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::Multiplication& node) {
                visitingChildren->append_range(node.factors | std::views::transform([](auto& factor) { return factor.get(); }));
                *visitingType = Multiplication;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::Power& node) {
                visitingChildren->push_back(node.base.get());
                visitingChildren->push_back(node.exponent.get());
                *visitingType = Power;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::AbsoluteValue& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = AbsoluteValue;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::Ceiling& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Ceiling;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::Floor& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Floor;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::Modulus& node) {
                visitingChildren->push_back(node.dividend.get());
                visitingChildren->push_back(node.divisor.get());
                *visitingType = Modulus;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::Logarithm& node) {
                visitingChildren->push_back(node.base.get());
                visitingChildren->push_back(node.operand.get());
                *visitingType = Logarithm;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::NaturalLogarithm& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = NaturalLogarithm;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::Sine& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Sine;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::Cosine& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Cosine;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::Tangent& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Tangent;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::Arcsine& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Arcsine;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::Arccosine& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Arccosine;
            },
            [&visitingChildren, &visitingType](const AnalyticExpression::Arctangent& node) {
                visitingChildren->push_back(node.operand.get());
                *visitingType = Arctangent;
            },
            [&visitingType](const AnalyticExpression::Wildcard::Any&) {
                *visitingType = WildcardAny;
            },
            [&visitingType](const AnalyticExpression::Wildcard::Variadic&) {
                *visitingType = WildcardVariadic;
            });
        a->accept(visitor);
        visitingChildren = &bChildren;
        visitingType = &bType;
        b->accept(visitor);

        if (!aType.has_value() || !bType.has_value()) {
            return false;
        }
        if (aType == WildcardAny || bType == WildcardAny) {
            assert((aType == WildcardAny) != (bType == WildcardAny) && "Wildcard::Any could only present on either left hand side or right hand side.");
            if (bType == WildcardAny) {
                std::swap(a, b);
            }
            const AnalyticExpression::Wildcard::id_t id = static_cast<const AnalyticExpression::Wildcard::Any*>(a)->id; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            if (wildcardMap.contains(id)) {
                return _simplification_rule_match::wildcardMatch(wildcardMap[id].get(), b, wildcardMap, memoryResource);
            }
            util::unique_pmr_ptr<AnalyticExpression::Node> anyHolderNode = b->clone(memoryResource);
            anyHolderNode->parent = nullptr;
            wildcardMap[id] = std::move(anyHolderNode);
            return true;
        }
        if (*aType != *bType) {
            return false;
        }
        do { // NOLINT(cppcoreguidelines-avoid-do-while)
            if (aType != Addition && aType != Multiplication) {
                break;
            }
            auto isVariadicNode = [](util::observer_ptr<const AnalyticExpression::Node> child) -> bool {
                bool res = false;
                child->accept(AnalyticExpression::NodeVisitorConst(
                    [&res](const AnalyticExpression::Wildcard::Variadic&) {
                        res = true;
                    }));
                return res;
            };
            const auto aVariadicIt = std::ranges::find_if(aChildren, isVariadicNode);
            const auto bVariadicIt = std::ranges::find_if(bChildren, isVariadicNode);
            const bool aHasWildcardVariadic = aVariadicIt != aChildren.end();
            const bool bHasWildcardVariadic = bVariadicIt != bChildren.end();
            if (!aHasWildcardVariadic && !bHasWildcardVariadic) {
                break;
            }
            assert(aHasWildcardVariadic != bHasWildcardVariadic && "Wildcard::Variadic could only present on either left hand side or right hand side.");
            if (bHasWildcardVariadic) {
                std::swap(a, b);
                std::swap(aChildren, bChildren);
                std::swap(aType, bType);
            }
            assert(aVariadicIt == aChildren.end() - 1 && "Variadic node must be the last one");
            if (aChildren.size() > bChildren.size() + 1) {
                return false;
            }

            for (std::size_t i = 0; i < aChildren.size() - 1; ++i) {
                if (!_simplification_rule_match::wildcardMatch(aChildren[i], bChildren[i], wildcardMap, memoryResource)) {
                    return false;
                }
            }

            const std::size_t variadicSize = bChildren.size() - (aChildren.size() - 1);
            util::unique_pmr_ptr<AnalyticExpression::Node> variadicHolderNode;
            if (variadicSize == 0) {
                variadicHolderNode = util::makeUniquePmr<AnalyticExpression::Constant>(memoryResource, nullptr, *aType == Addition ? 0 : 1);
            } else if (variadicSize == 1) {
                variadicHolderNode = bChildren.back()->clone(memoryResource);
                variadicHolderNode->parent = nullptr;
            } else {
                auto variadicNodes = bChildren
                    | std::views::drop(bChildren.size() - variadicSize)
                    | std::views::transform([memoryResource](util::observer_ptr<const AnalyticExpression::Node> variadicNode) { return variadicNode->clone(memoryResource); })
                    | std::ranges::to<std::pmr::vector<util::unique_pmr_ptr<AnalyticExpression::Node>>>(memoryResource);

                if (*aType == Addition) {
                    variadicHolderNode = util::makeUniquePmr<AnalyticExpression::Addition>(memoryResource, nullptr, std::move(variadicNodes));
                } else {
                    variadicHolderNode = util::makeUniquePmr<AnalyticExpression::Multiplication>(memoryResource, nullptr, std::move(variadicNodes));
                }
            }
            const AnalyticExpression::Wildcard::id_t id = static_cast<const AnalyticExpression::Wildcard::Variadic*>(a)->id; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            if (wildcardMap.contains(id)) {
                return _simplification_rule_match::wildcardMatch(wildcardMap[id].get(), variadicHolderNode.get(), wildcardMap, memoryResource);
            }
            wildcardMap[id] = std::move(variadicHolderNode);
            return true;
        } while (false);
        if (aChildren.size() != bChildren.size()) {
            return false;
        }
        if (aType == Constant) {
            return static_cast<const AnalyticExpression::Constant*>(a)->value == static_cast<const AnalyticExpression::Constant*>(b)->value; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        }
        if (aType == Variable) {
            return static_cast<const AnalyticExpression::Variable*>(a)->name == static_cast<const AnalyticExpression::Variable*>(b)->name; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        }
        for (std::size_t i = 0; i < aChildren.size(); ++i) {
            if (!_simplification_rule_match::wildcardMatch(aChildren[i], bChildren[i], wildcardMap, memoryResource)) {
                return false;
            }
        }
        return true;
    }
}
} // namespace ::_simplification_rule_match

std::optional<AnalyticExpression::Simplification::Rule::wildcard_map_t> AnalyticExpression::Simplification::Rule::match(util::observer_ptr<const Node> target, util::observer_ptr<std::pmr::memory_resource> memoryResource) const
{
    AnalyticExpression::Simplification::Rule::wildcard_map_t wildcardMap(memoryResource);
    if (!_simplification_rule_match::wildcardMatch(this->pattern.get(), target, wildcardMap, memoryResource)) {
        return std::nullopt;
    }
    if (!this->condition(target)) {
        return std::nullopt;
    }
    return wildcardMap;
}
util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::Simplification::Rule::apply(wildcard_map_t map, util::observer_ptr<std::pmr::memory_resource> memoryResource) const
{
    return this->replacer(std::move(map), memoryResource);
}
AnalyticExpression::Simplification::RuleSet AnalyticExpression::Simplification::generateDefaultRules(util::observer_ptr<std::pmr::memory_resource> memoryResource)
{
    // TODO(P0): fill this up
    return { };
}

std::optional<util::unique_pmr_ptr<AnalyticExpression::Node>> AnalyticExpression::Simplification::HillClimbingAlgorithm::operator()(AnalyticExpression::Simplification::CandidateRules rules, util::observer_ptr<const AnalyticExpression::Node> target, util::observer_ptr<std::pmr::memory_resource> memoryResource)
{
    const Integer originalComplexity = complexityOf_(target);
    auto candidateNodes = rules
        | std::views::transform([memoryResource](std::pair<const Rule, Rule::wildcard_map_t>& rulePair) -> util::unique_pmr_ptr<AnalyticExpression::Node> { return rulePair.first.apply(std::move(rulePair.second), memoryResource); })
        | std::ranges::to<_::CandidateNodes>(memoryResource);
    const auto candidateNodesComplexities = candidateNodes
        | std::views::transform([this](const util::unique_pmr_ptr<Node>& node) -> Integer { return complexityOf_(node.get()); });
    const auto minComplexityCanididate = std::ranges::min_element(candidateNodesComplexities);
    if (*minComplexityCanididate >= originalComplexity) {
        return std::nullopt;
    }
    return std::move(candidateNodes.extract(minComplexityCanididate.base()).value());
}
Integer AnalyticExpression::Simplification::HillClimbingAlgorithm::complexityOf_(util::observer_ptr<const AnalyticExpression::Node> node) const
{
    const auto childAccumulator = [this](const Integer& accumulation, const util::unique_pmr_ptr<Node>& child) -> Integer { return accumulation + complexityOf_(child.get()); };
    Integer complexity;
    node->accept(NodeVisitorConst(
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
        [this, &complexity](const AnalyticExpression::Power& node) {
            complexity = 16 * (complexityOf_(node.base.get()) + complexityOf_(node.exponent.get()));
        },
        [this, &complexity](const AnalyticExpression::AbsoluteValue& node) {
            complexity = 8 * complexityOf_(node.operand.get());
        },
        [this, &complexity](const AnalyticExpression::Ceiling& node) {
            complexity = 8 * complexityOf_(node.operand.get());
        },
        [this, &complexity](const AnalyticExpression::Floor& node) {
            complexity = 8 * complexityOf_(node.operand.get());
        },
        [this, &complexity](const AnalyticExpression::Modulus& node) {
            complexity = 8 * (complexityOf_(node.dividend.get()) + complexityOf_(node.divisor.get()));
        },
        [this, &complexity](const AnalyticExpression::Logarithm& node) {
            complexity = 32 * (complexityOf_(node.base.get()) + complexityOf_(node.operand.get()));
        },
        [this, &complexity](const AnalyticExpression::NaturalLogarithm& node) {
            complexity = 32 * (1 + complexityOf_(node.operand.get()));
        },
        [this, &complexity](const AnalyticExpression::Sine& node) {
            complexity = 64 * complexityOf_(node.operand.get());
        },
        [this, &complexity](const AnalyticExpression::Cosine& node) {
            complexity = 64 * complexityOf_(node.operand.get());
        },
        [this, &complexity](const AnalyticExpression::Tangent& node) {
            complexity = 64 * complexityOf_(node.operand.get());
        },
        [this, &complexity](const AnalyticExpression::Arcsine& node) {
            complexity = 64 * complexityOf_(node.operand.get());
        },
        [this, &complexity](const AnalyticExpression::Arccosine& node) {
            complexity = 64 * complexityOf_(node.operand.get());
        },
        [this, &complexity](const AnalyticExpression::Arctangent& node) {
            complexity = 64 * complexityOf_(node.operand.get());
        }));
    return complexity;
}
std::optional<util::unique_pmr_ptr<AnalyticExpression::Node>> AnalyticExpression::Simplification::LateAcceptanceHillClimbingAlgorithm::operator()(AnalyticExpression::Simplification::CandidateRules rules, util::observer_ptr<const AnalyticExpression::Node> target, util::observer_ptr<std::pmr::memory_resource> memoryResource)
{
    const Integer originalComplexity = complexityOf_(target);
}

AnalyticExpression::Simplification::Context::Context(const AnalyticExpression& expr)
    : Context(expr.memoryResource())
{ }
AnalyticExpression::Simplification::Context::Context(util::observer_ptr<std::pmr::memory_resource> memoryResource)
    : rules(generateDefaultRules(memoryResource)),
      algorithms([&]() {
          TreeApplierAlgorithms treeAlgorithms(memoryResource);
          NodeApplierAlgorithms nodeAlgorithms(memoryResource);
          nodeAlgorithms.emplace_back(util::makeUniquePmr<HillClimbingAlgorithm>(memoryResource));
          nodeAlgorithms.emplace_back(util::makeUniquePmr<LateAcceptanceHillClimbingAlgorithm>(memoryResource));
          treeAlgorithms.push_back(std::move(nodeAlgorithms));
          return treeAlgorithms;
      }()),
      approximation(RationalCalculationOptions().approximation)
{ }

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
util::observer_ptr<std::pmr::memory_resource> AnalyticExpression::memoryResource() const
{
    return memoryResource_.get();
}

AnalyticExpression normalize(AnalyticExpression expr)
{
    const AnalyticExpression::NodeVisitor visitor(
        [&visitor](AnalyticExpression::Addition& node) {
            decltype(node.terms.begin()) it;
            const AnalyticExpression::NodeVisitor childrenVisitor(
                [&node, &it](AnalyticExpression::Addition& child) {
                    std::size_t childChildrenCount = child.terms.size();
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
                    std::size_t childChildrenCount = child.factors.size();
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
            node.base->accept(visitor);
            node.operand->accept(visitor);
        },
        [&visitor](AnalyticExpression::NaturalLogarithm& node) {
            node.operand->accept(visitor);
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
    expr.base->accept(visitor);
    return expr;
}
} // namespace thecalculater::math