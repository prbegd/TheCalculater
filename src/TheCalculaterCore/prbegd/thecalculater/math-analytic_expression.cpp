/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module prbegd.thecalculater.math;
import prbegd.thecalculater.util;
import thirdparty.core;
import std;

namespace thecalculater::math {
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
#define NODE_METHOD_TYPE_(_class_) \
    AnalyticExpression::NodeType AnalyticExpression::_class_::type() const \
    { \
        return NodeType::_class_; \
    }
NODE_METHOD_TYPE_(Constant)
NODE_METHOD_TYPE_(Variable)
NODE_METHOD_TYPE_(Infinity)
NODE_METHOD_TYPE_(Pi)
NODE_METHOD_TYPE_(Euler)
NODE_METHOD_TYPE_(ImaginaryUnit)
NODE_METHOD_TYPE_(Addition)
NODE_METHOD_TYPE_(Multiplication)
NODE_METHOD_TYPE_(Power)
NODE_METHOD_TYPE_(AbsoluteValue)
NODE_METHOD_TYPE_(Ceiling)
NODE_METHOD_TYPE_(Floor)
NODE_METHOD_TYPE_(Modulus)
NODE_METHOD_TYPE_(Logarithm)
NODE_METHOD_TYPE_(NaturalLogarithm)
NODE_METHOD_TYPE_(Sine)
NODE_METHOD_TYPE_(Cosine)
NODE_METHOD_TYPE_(Tangent)
NODE_METHOD_TYPE_(Arcsine)
NODE_METHOD_TYPE_(Arccosine)
NODE_METHOD_TYPE_(Arctangent)
#undef NODE_METHOD_TYPE_

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

AnalyticExpression::Simplification::Context::Context() noexcept
    : approximation { }
{
    actions.set(Action::Normalize);
    actions.set(Action::AlgebraicSimplification);
    actions.set(Action::TrigonometricSimplification);
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
util::observer_ptr<std::pmr::memory_resource> AnalyticExpression::memoryResource() const
{
    return memoryResource_.get();
}

namespace { namespace _normalize {
    // class NormalizeVisitor : public AnalyticExpression::NodeVisitor {
    // public:
    //     void visit(AnalyticExpression::Addition& node) override
    //     {
    //         class AdditionFlattenVisitor : public AnalyticExpression::NodeVisitor {
    //         public:
    //             util::observer_ptr<AnalyticExpression::Addition> parent;
    //             explicit AdditionFlattenVisitor(util::observer_ptr<AnalyticExpression::Addition> parent)
    //                 : parent(parent)
    //             { }
    //             void visit(AnalyticExpression::Addition& node) override
    //             {
    //                 parent->terms.append_range(node.terms | std::views::as_rvalue);
    //             }
    //         } flattenVisitor(&node);
    //         for (auto& term : node.terms) {
    //             term->accept(*this);
    //             term->accept(flattenVisitor);
    //         }
    //         std::ranges::sort(node.terms, [](const auto& a, const auto& b) { return a->hash() < b->hash(); });
    //     }
    //     void visit(AnalyticExpression::Multiplication& node) override
    //     {
    //         class MultiplicationFlattenVisitor : public AnalyticExpression::NodeVisitor {
    //         public:
    //             util::observer_ptr<AnalyticExpression::Multiplication> parent;
    //             explicit MultiplicationFlattenVisitor(util::observer_ptr<AnalyticExpression::Multiplication> parent)
    //                 : parent(parent)
    //             { }
    //             void visit(AnalyticExpression::Multiplication& node) override
    //             {
    //                 parent->factors.append_range(node.factors | std::views::as_rvalue);
    //             }
    //         } flattenVisitor(&node);
    //         for (auto& term : node.factors) {
    //             term->accept(*this);
    //             term->accept(flattenVisitor);
    //         }
    //         std::ranges::sort(node.factors, [](const auto& a, const auto& b) { return a->hash() < b->hash(); });
    //     }
    //     void visit(AnalyticExpression::Power& node) override
    //     {
    //         node.base->accept(*this);
    //         node.exponent->accept(*this);
    //     }
    //     void visit(AnalyticExpression::AbsoluteValue& node) override
    //     {
    //         node.operand->accept(*this);
    //     }
    //     void visit(AnalyticExpression::Ceiling& node) override
    //     {
    //         node.operand->accept(*this);
    //     }
    //     void visit(AnalyticExpression::Floor& node) override
    //     {
    //         node.operand->accept(*this);
    //     }
    //     void visit(AnalyticExpression::Modulus& node) override
    //     {
    //         node.dividend->accept(*this);
    //         node.divisor->accept(*this);
    //     }
    //     void visit(AnalyticExpression::Logarithm& node) override
    //     {
    //         node.base->accept(*this);
    //         node.operand->accept(*this);
    //     }
    //     void visit(AnalyticExpression::NaturalLogarithm& node) override
    //     {
    //         node.operand->accept(*this);
    //     }
    //     void visit(AnalyticExpression::Sine& node) override
    //     {
    //         node.operand->accept(*this);
    //     }
    //     void visit(AnalyticExpression::Cosine& node) override
    //     {
    //         node.operand->accept(*this);
    //     }
    //     void visit(AnalyticExpression::Tangent& node) override
    //     {
    //         node.operand->accept(*this);
    //     }
    //     void visit(AnalyticExpression::Arcsine& node) override
    //     {
    //         node.operand->accept(*this);
    //     }
    //     void visit(AnalyticExpression::Arccosine& node) override
    //     {
    //         node.operand->accept(*this);
    //     }
    //     void visit(AnalyticExpression::Arctangent& node) override
    //     {
    //         node.operand->accept(*this);
    //     }
    // };
}} // namespace ::_normalize
AnalyticExpression normalize(AnalyticExpression expr)
{
    expr.base->accept(AnalyticExpression::NodeVisitorConst(
        [](const AnalyticExpression::Addition& node) {

        },
        [](const AnalyticExpression::Multiplication& node) {

        }
    ));
    return expr;
}
} // namespace thecalculater::math