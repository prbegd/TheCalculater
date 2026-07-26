/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module prbegd.thecalculater.math;
import std;
import thirdparty.core;
import prbegd.thecalculater.util;

namespace thecalculater::math {
namespace {
    bool _structuralEqual(util::observer_ptr<const AnalyticExpression::Node> a, util::observer_ptr<const AnalyticExpression::Node> b)
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
            });
        a->accept(visitor);
        visitingChildren = &bChildren;
        visitingType = &bType;
        b->accept(visitor);

        if ((!aType.has_value() || !bType.has_value()) || *aType != *bType || aChildren.size() != bChildren.size()) {
            return false;
        }
        if (aType == Constant) {
            return reinterpret_cast<const AnalyticExpression::Constant*>(a)->value == reinterpret_cast<const AnalyticExpression::Constant*>(b)->value; // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        }
        if (aType == Variable) {
            return reinterpret_cast<const AnalyticExpression::Variable*>(a)->name == reinterpret_cast<const AnalyticExpression::Variable*>(b)->name; // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        }
        for (std::size_t i = 0; i < aChildren.size(); ++i) {
            if (!_structuralEqual(aChildren[i], bChildren[i])) {
                return false;
            }
        }
        return true;
    }
} // namespace

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_CONSTRUCTOR1_(_class_, _member_) \
    AnalyticExpression::_class_::_class_(const util::unique_pmr_ptr<Node>&(_member_), util::observer_ptr<std::pmr::memory_resource> memoryResource) \
        : _member_((_member_)->clone(memoryResource)) \
    { } \
    AnalyticExpression::_class_::_class_(util::unique_pmr_ptr<Node> && (_member_)) \
        : _member_(std::move(_member_)) \
    { }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_CONSTRUCTOR2_(_class_, _member1_, _member2_) \
    AnalyticExpression::_class_::_class_(const util::unique_pmr_ptr<Node>&(_member1_), const util::unique_pmr_ptr<Node>&(_member2_), util::observer_ptr<std::pmr::memory_resource> memoryResource) \
        : _member1_((_member1_)->clone(memoryResource)), \
          _member2_((_member2_)->clone(memoryResource)) \
    { } \
    AnalyticExpression::_class_::_class_(util::unique_pmr_ptr<Node> && (_member1_), util::unique_pmr_ptr<Node> && (_member2_)) \
        : _member1_(std::move(_member1_)), \
          _member2_(std::move(_member2_)) \
    { }
AnalyticExpression::Wildcard::Any::Any(Wildcard::id_t id)
    : id(id)
{ }
AnalyticExpression::Wildcard::Variadic::Variadic(Wildcard::id_t id)
    : id(id)
{ }
AnalyticExpression::Constant::Constant(Rational value)
    : value(std::move(value))
{ }
AnalyticExpression::Variable::Variable(std::string_view name, util::observer_ptr<std::pmr::memory_resource> memoryResource)
    : name(name, memoryResource)
{ }
AnalyticExpression::Variable::Variable(std::pmr::string&& name)
    : name(std::move(name))
{ }
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
        return util::makeUniquePmr<_class_>(memoryResource); \
    }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_METHOD_CLONE1_(_class_, _parameter_) \
    util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::_class_::clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const \
    { \
        return util::makeUniquePmr<_class_>(memoryResource, _parameter_); \
    }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define NODE_METHOD_CLONE2_(_class_, _parameter1_, _parameter2_) \
    util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::_class_::clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const \
    { \
        return util::makeUniquePmr<_class_>(memoryResource, _parameter1_, _parameter2_); \
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
    return util::makeUniquePmr<Addition>(memoryResource, std::move(terms));
}
util::unique_pmr_ptr<AnalyticExpression::Node> AnalyticExpression::Multiplication::clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const
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

AnalyticExpression::Wildcard::UsedInCalculationException::UsedInCalculationException() { }
const char* AnalyticExpression::Wildcard::UsedInCalculationException::what() const noexcept
{
    return "Wild card nodes is only for rule matching and is not for calculation.";
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