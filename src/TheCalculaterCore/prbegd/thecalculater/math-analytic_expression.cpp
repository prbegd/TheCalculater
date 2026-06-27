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
    AnalyticExpression::AnalyticExpression(util::observer_ptr<std::pmr::memory_resource> memoryResource)
        : memoryResource_(memoryResource)
    { }
    AnalyticExpression::AnalyticExpression(const INode& node, util::observer_ptr<std::pmr::memory_resource> memoryResource)
        : memoryResource_(memoryResource), base(node.clone(memoryResource_))
    { }
    AnalyticExpression::AnalyticExpression(const util::unique_pmr_ptr<INode>& node)
        : memoryResource_(util::ownerOf(base)), base(node->clone(memoryResource_))
    { }
    AnalyticExpression::AnalyticExpression(const util::unique_pmr_ptr<INode>& node, util::observer_ptr<std::pmr::memory_resource> memoryResource)
        : memoryResource_(memoryResource), base(node->clone(memoryResource_))
    { }
    AnalyticExpression::AnalyticExpression(util::unique_pmr_ptr<INode>&& node)
        : memoryResource_(util::ownerOf(base)), base(std::move(node))
    { }
    AnalyticExpression::AnalyticExpression(util::unique_pmr_ptr<INode>&& node, util::observer_ptr<std::pmr::memory_resource> memoryResource)
        : memoryResource_(memoryResource), base(std::move(node))
    { }

    AnalyticExpression::AnalyticExpression(const AnalyticExpression& other)
        : memoryResource_(other.memoryResource_), base(other.base->clone(memoryResource_))
    { }
    AnalyticExpression& AnalyticExpression::operator=(const AnalyticExpression& other)
    {
        if (this != &other) {
            base = other.base->clone(memoryResource_);
            memoryResource_ = other.memoryResource_;
        }
        return *this;
    }

    util::unique_pmr_ptr<AnalyticExpression::INode> AnalyticExpression::INode::clone() const
    {
        return clone(std::pmr::get_default_resource());
    }
    util::unique_pmr_ptr<AnalyticExpression::INode> AnalyticExpression::INode::clone(util::unique_pmr_ptr<INode> node) const
    {
        return clone(util::ownerOf(node));
    }

    // NOLINTNEXTLINE
#define _HASH_FUNC0(_class, _hash)                       \
    std::size_t AnalyticExpression::_class::hash() const \
    {                                                    \
        return _hash;                                    \
    }
    // NOLINTNEXTLINE
#define _HASH_FUNC1(_class, _hash, _member)              \
    std::size_t AnalyticExpression::_class::hash() const \
    {                                                    \
        std::size_t seed = _hash;                        \
        boost::hash_combine(seed, _member->hash());      \
        return seed;                                     \
    }
    // NOLINTNEXTLINE
#define _HASH_FUNC10(_class, _hash, _member)             \
    std::size_t AnalyticExpression::_class::hash() const \
    {                                                    \
        std::size_t seed = _hash;                        \
        boost::hash_combine(seed, _member);              \
        return seed;                                     \
    }
    // NOLINTNEXTLINE
#define _HASH_FUNC2(_class, _hash, _member1, _member2)   \
    std::size_t AnalyticExpression::_class::hash() const \
    {                                                    \
        std::size_t seed = _hash;                        \
        boost::hash_combine(seed, _member1->hash());     \
        boost::hash_combine(seed, _member2->hash());     \
        return seed;                                     \
    }
    _HASH_FUNC10(Constant, 0x3361e811604a8be7, value)
    _HASH_FUNC10(Variable, 0xe60cbdcfe41d881a, name)
    _HASH_FUNC0(Infinity, 0xc3dc0c723e73cbc3)
    _HASH_FUNC0(Pi, 0x8c18f600b6867066)
    _HASH_FUNC0(Euler, 0x573ab0792d7b9fca)
    _HASH_FUNC0(ImaginaryUnit, 0x99506ad9db02af43)
    _HASH_FUNC2(Addition, 0x26b57e0cad6d1c3, left, right)
    _HASH_FUNC2(Multiplication, 0x95d1ec6364d57dc8, left, right)
    _HASH_FUNC2(Power, 0xf709b05f78a07dcb, base, exponent)
    _HASH_FUNC1(AbsoluteValue, 0xb6520fc18810bef7, operand)
    _HASH_FUNC1(Ceiling, 0x9794942fdb2ced17, operand)
    _HASH_FUNC1(Floor, 0xd581148f9f049570, operand)
    _HASH_FUNC2(Modulus, 0x46e0bc3ac0eb3723, dividend, divisor)
    _HASH_FUNC2(Logarithm, 0xfa76de7ccdb3659d, base, operand)
    _HASH_FUNC1(NaturalLogarithm, 0xffb7367750971651, operand)
    _HASH_FUNC1(Sine, 0x682422b47671e928, operand)
    _HASH_FUNC1(Cosine, 0x3bfcfb15956054ad, operand)
    _HASH_FUNC1(Tangent, 0x6da758eca579e7ae, operand)
    _HASH_FUNC1(Arcsine, 0x9c05a7b5a7b29fca, operand)
    _HASH_FUNC1(Arccosine, 0x27e3b1fde1e5166e, operand)
    _HASH_FUNC1(Arctangent, 0xe467b7f655c81cc8, operand)
#undef _HASH_FUNC0
#undef _HASH_FUNC1
#undef _HASH_FUNC10
#undef _HASH_FUNC2

    // NOLINTNEXTLINE
#define _TYPE_FUNC(_class)                                                \
    AnalyticExpression::NodeType AnalyticExpression::_class::type() const \
    {                                                                     \
        return NodeType::_class;                                          \
    }
    _TYPE_FUNC(Constant)
    _TYPE_FUNC(Variable)
    _TYPE_FUNC(Infinity)
    _TYPE_FUNC(Pi)
    _TYPE_FUNC(Euler)
    _TYPE_FUNC(ImaginaryUnit)
    _TYPE_FUNC(Addition)
    _TYPE_FUNC(Multiplication)
    _TYPE_FUNC(Power)
    _TYPE_FUNC(AbsoluteValue)
    _TYPE_FUNC(Ceiling)
    _TYPE_FUNC(Floor)
    _TYPE_FUNC(Modulus)
    _TYPE_FUNC(Logarithm)
    _TYPE_FUNC(NaturalLogarithm)
    _TYPE_FUNC(Sine)
    _TYPE_FUNC(Cosine)
    _TYPE_FUNC(Tangent)
    _TYPE_FUNC(Arcsine)
    _TYPE_FUNC(Arccosine)
    _TYPE_FUNC(Arctangent)
#undef _TYPE_FUNC

    // NOLINTNEXTLINE
#define _CLONE_FUNC0(_class)                                                                                                                              \
    util::unique_pmr_ptr<AnalyticExpression::INode> AnalyticExpression::_class::clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const \
    {                                                                                                                                                     \
        return util::make_unique_pmr<_class>(memoryResource);                                                                                             \
    }
    // NOLINTNEXTLINE
#define _CLONE_FUNC1(_class, _parameter)                                                                                                                  \
    util::unique_pmr_ptr<AnalyticExpression::INode> AnalyticExpression::_class::clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const \
    {                                                                                                                                                     \
        return util::make_unique_pmr<_class>(memoryResource, _parameter);                                                                                 \
    }
    // NOLINTNEXTLINE
#define _CLONE_FUNC2(_class, _parameter1, _parameter2)                                                                                                    \
    util::unique_pmr_ptr<AnalyticExpression::INode> AnalyticExpression::_class::clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const \
    {                                                                                                                                                     \
        return util::make_unique_pmr<_class>(memoryResource, _parameter1, _parameter2);                                                                   \
    }
    _CLONE_FUNC1(Constant, value)
    _CLONE_FUNC1(Variable, name)
    _CLONE_FUNC0(Infinity)
    _CLONE_FUNC0(Pi)
    _CLONE_FUNC0(Euler)
    _CLONE_FUNC0(ImaginaryUnit)
    _CLONE_FUNC2(Addition, left->clone(), right->clone())
    _CLONE_FUNC2(Multiplication, left->clone(), right->clone())
    _CLONE_FUNC2(Power, base->clone(), exponent->clone())
    _CLONE_FUNC1(AbsoluteValue, operand->clone())
    _CLONE_FUNC1(Ceiling, operand->clone())
    _CLONE_FUNC1(Floor, operand->clone())
    _CLONE_FUNC2(Modulus, dividend->clone(), divisor->clone())
    _CLONE_FUNC2(Logarithm, base->clone(), operand->clone())
    _CLONE_FUNC1(NaturalLogarithm, operand->clone())
    _CLONE_FUNC1(Sine, operand->clone())
    _CLONE_FUNC1(Cosine, operand->clone())
    _CLONE_FUNC1(Tangent, operand->clone())
    _CLONE_FUNC1(Arcsine, operand->clone())
    _CLONE_FUNC1(Arccosine, operand->clone())
    _CLONE_FUNC1(Arctangent, operand->clone())
#undef _CLONE_FUNC0
#undef _CLONE_FUNC1
#undef _CLONE_FUNC2

    // NOLINTNEXTLINE
#define _CONSTRUCTOR1(_class, _member)                                             \
    AnalyticExpression::_class::_class(const util::unique_pmr_ptr<INode>& _member) \
        : _member(_member->clone())                                                \
    { }                                                                            \
    AnalyticExpression::_class::_class(util::unique_pmr_ptr<INode>&& _member)      \
        : _member(std::move(_member))                                              \
    { }
    // NOLINTNEXTLINE
#define _CONSTRUCTOR2(_class, _member1, _member2)                                                                                \
    AnalyticExpression::_class::_class(const util::unique_pmr_ptr<INode>& _member1, const util::unique_pmr_ptr<INode>& _member2) \
        : _member1(_member1->clone()), _member2(_member2->clone())                                                               \
    { }                                                                                                                          \
    AnalyticExpression::_class::_class(util::unique_pmr_ptr<INode>&& _member1, util::unique_pmr_ptr<INode>&& _member2)           \
        : _member1(std::move(_member1)), _member2(std::move(_member2))                                                           \
    { }
    AnalyticExpression::Constant::Constant(Fraction value)
        : value(std::move(value))
    { }
    AnalyticExpression::Variable::Variable(std::string_view name)
        : name(name)
    { }
    AnalyticExpression::Variable::Variable(std::string&& name)
        : name(std::move(name))
    { }
    _CONSTRUCTOR2(Addition, left, right)
    _CONSTRUCTOR2(Multiplication, left, right)
    _CONSTRUCTOR2(Power, base, exponent)
    _CONSTRUCTOR1(AbsoluteValue, operand)
    _CONSTRUCTOR1(Ceiling, operand)
    _CONSTRUCTOR1(Floor, operand)
    _CONSTRUCTOR2(Modulus, dividend, divisor)
    _CONSTRUCTOR2(Logarithm, base, operand)
    _CONSTRUCTOR1(NaturalLogarithm, operand)
    _CONSTRUCTOR1(Sine, operand)
    _CONSTRUCTOR1(Cosine, operand)
    _CONSTRUCTOR1(Tangent, operand)
    _CONSTRUCTOR1(Arcsine, operand)
    _CONSTRUCTOR1(Arccosine, operand)
    _CONSTRUCTOR1(Arctangent, operand)
#undef _CONSTRUCTOR1
#undef _CONSTRUCTOR2
    AnalyticExpression::SimplifyContext::SimplifyContext() noexcept
        : approximation { }
    {
        actions.set(Normalize);
        actions.set(AlgebraicSimplification);
        actions.set(TrigonometricSimplification);
    }
} // namespace thecalculater::math