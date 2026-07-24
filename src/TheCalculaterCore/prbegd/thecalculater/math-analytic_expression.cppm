/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 *
 */
module;
#include "thecalculater/macros.hpp"
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>


export module prbegd.thecalculater.math:analytic_expression;
import :rational;
import prbegd.thecalculater.util;
import thirdparty.core;
import std;

namespace thecalculater::math {
export class TCAPI AnalyticExpression {
public:
#pragma region // AnalyticExpression nested classes definitions
    class Node;
    enum class NodeType : std::uint8_t {
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

        NodeType_COUNT_,
    };
    class Constant;
    class Variable;
    class Infinity;
    class Pi;
    class Euler;
    class ImaginaryUnit;

    class Addition;
    class Multiplication;
    class Power;

    class AbsoluteValue;
    class Ceiling;
    class Floor;
    class Modulus;

    class Logarithm;
    class NaturalLogarithm;
    class Sine;
    class Cosine;
    class Tangent;
    class Arcsine;
    class Arccosine;
    class Arctangent;

    // REFACTOR(P3): Maybe use a unordered_map to store the visitor callbacks.
#define NODE_VISITOR_GENERATE_MEMBER(_r_, _data_, _nodeType_) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    std::function<void(_nodeType_&)> BOOST_PP_CAT(_nodeType_, Callback); /* NOLINT(bugprone-macro-parentheses) */ \
    void operator()(_nodeType_& node) const /* NOLINT(bugprone-macro-parentheses) */ \
    { \
        if (this->BOOST_PP_CAT(_nodeType_, Callback)) { \
            this->BOOST_PP_CAT(_nodeType_, Callback)(node); \
        } else if (this->defaultCallback) { \
            this->defaultCallback(node); \
        } \
    }
#define NODE_VISITOR_CONSTRUCTOR_ASSIGN_FIELD(_r_, _data_, _nodeType_) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    if constexpr (std::invocable<decltype(_data_), _nodeType_&> && !std::invocable<decltype(_data_), Node&>) { /* NOLINT */ \
        this->BOOST_PP_CAT(_nodeType_, Callback) = std::move(_data_); \
    } else

#define NODE_VISITOR_GENERATE(_nodeTypes_) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    BOOST_PP_SEQ_FOR_EACH(NODE_VISITOR_GENERATE_MEMBER, _, _nodeTypes_) \
    template <typename... TCallbacks> \
    explicit NodeVisitor(TCallbacks... callbacks) \
        : defaultCallback([](Node&){}) \
    { \
        ([this](TCallbacks callback){ \
            BOOST_PP_SEQ_FOR_EACH(NODE_VISITOR_CONSTRUCTOR_ASSIGN_FIELD, callback, _nodeTypes_) \
            if constexpr (requires (Node& node) { callback(node); }) { \
                this->defaultCallback = std::move(callback); \
            } else { static_assert(sizeof(TCallbacks) == 0, "\n  Callbacks of NodeVisitor must all be invocable with Node& or (one of its derived types)&."); } \
        }(std::move(callbacks)), ...); \
    }
#define NODE_VISITOR_CONST_GENERATE_MEMBER(_r_, _data_, _nodeType_) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    std::function<void(const _nodeType_&)> BOOST_PP_CAT(_nodeType_, Callback); /* NOLINT(bugprone-macro-parentheses) */ \
    void operator()(const _nodeType_& node) const \
    { \
        if (this->BOOST_PP_CAT(_nodeType_, Callback)) { \
            this->BOOST_PP_CAT(_nodeType_, Callback)(node); \
        } else if (this->defaultCallback) { \
            this->defaultCallback(node); \
        } \
    }
#define NODE_VISITOR_CONST_CONSTRUCTOR_ASSIGN_FIELD(_r_, _data_, _nodeType_) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    if constexpr (std::invocable<decltype(_data_), const _nodeType_&> && !std::invocable<decltype(_data_), const Node&>) { \
        this->BOOST_PP_CAT(_nodeType_, Callback) = std::move(_data_); \
    } else

#define NODE_VISITOR_CONST_GENERATE(_nodeTypes_) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    BOOST_PP_SEQ_FOR_EACH(NODE_VISITOR_CONST_GENERATE_MEMBER, _, _nodeTypes_) \
    template <typename... TCallbacks> \
    explicit NodeVisitorConst(TCallbacks... callbacks) \
        : defaultCallback([](const Node&){}) \
    { \
        ([this](TCallbacks callback){ \
            BOOST_PP_SEQ_FOR_EACH(NODE_VISITOR_CONST_CONSTRUCTOR_ASSIGN_FIELD, callback, _nodeTypes_) \
            if constexpr (requires (const Node& node) { callback(node); }) { \
                this->defaultCallback = std::move(callback); \
            } else { static_assert(sizeof(TCallbacks) == 0, "\n  Callbacks of NodeVisitorConst must all be invocable with const Node& or const (one of its derived types)&."); } \
        }(std::move(callbacks)), ...); \
    }
#define CREATE_CLASS_NODE_VISITOR_AND_NODE_VISITOR_CONST(_nodeTypes_) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    class NodeVisitor { \
    public: \
        std::function<void(Node&)> defaultCallback; \
        NODE_VISITOR_GENERATE(_nodeTypes_) \
    }; \
    class NodeVisitorConst { \
    public: \
        std::function<void(const Node&)> defaultCallback; \
        NODE_VISITOR_CONST_GENERATE(_nodeTypes_) \
    };

THECALCULATER_DIAGNOSTIC(push)
THECALCULATER_DIAGNOSTIC(ignored "-Wunused-lambda-capture")
CREATE_CLASS_NODE_VISITOR_AND_NODE_VISITOR_CONST((Constant)(Variable)(Infinity)(Pi)(Euler)(ImaginaryUnit)(Addition)(Multiplication)(Power)(AbsoluteValue)(Ceiling)(Floor)(Modulus)(Logarithm)(NaturalLogarithm)(Sine)(Cosine)(Tangent)(Arcsine)(Arccosine)(Arctangent))
THECALCULATER_DIAGNOSTIC(pop)    

#undef NODE_VISITOR_GENERATE_MEMBER
#undef NODE_VISITOR_CONSTRUCTOR_ASSIGN_FIELD
#undef NODE_VISITOR_GENERATE
#undef NODE_VISITOR_CONST_GENERATE_MEMBER
#undef NODE_VISITOR_CONST_CONSTRUCTOR_ASSIGN_FIELD
#undef NODE_VISITOR_CONST_GENERATE
#undef CREATE_CLASS_NODE_VISITOR_AND_NODE_VISITOR_CONST
    /**
     * @brief The abstract class of the expression tree node.
     */
    class Node {
    public:
        virtual ~Node() = default;

        /// @warning The hash value is NOT meant to be used in checking equality of two expressions.
        [[nodiscard]]
        virtual std::size_t hash() const = 0;
        [[nodiscard]]
        virtual util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const = 0;
        [[nodiscard]] [[deprecated]]
        virtual NodeType type() const = 0;
        virtual void accept(const NodeVisitor& visitor) = 0;
        virtual void accept(const NodeVisitorConst& visitor) const = 0;
    };
    template <typename T>
    class VisitableNode : public Node {
    public:
        void accept(const NodeVisitor& visitor) override
        {
            visitor(static_cast<T&>(*this));
        }
        void accept(const NodeVisitorConst& visitor) const override
        {
            visitor(static_cast<const T&>(*this));
        }
    };
    class Constant : public VisitableNode<Constant> {
    public:
        Rational value;

        explicit Constant(Rational value);

        Constant(const AnalyticExpression::Constant& other) = delete;
        Constant(AnalyticExpression::Constant&& other) = default;
        Constant& operator=(const AnalyticExpression::Constant& other) = delete;
        Constant& operator=(AnalyticExpression::Constant&& other) = default;
        ~Constant() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Variable : public VisitableNode<Variable> {
    public:
        std::pmr::string name;

        explicit Variable(std::string_view name, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit Variable(std::pmr::string&& name);

        [[nodiscard]]
        std::size_t hash() const override;

        Variable(const AnalyticExpression::Variable& other) = delete;
        Variable(AnalyticExpression::Variable&& other) = default;
        Variable& operator=(const AnalyticExpression::Variable& other) = delete;
        Variable& operator=(AnalyticExpression::Variable&& other) = default;
        ~Variable() override = default;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Infinity : public VisitableNode<Infinity> {
    public:
        Infinity() = default;
        Infinity(const Infinity& other) = delete;
        Infinity(Infinity&& other) = default;
        Infinity& operator=(const Infinity& other) = delete;
        Infinity& operator=(Infinity&& other) = default;
        ~Infinity() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Pi : public VisitableNode<Pi> {
    public:
        Pi() = default;
        Pi(const Pi& other) = delete;
        Pi(Pi&& other) = default;
        Pi& operator=(const Pi& other) = delete;
        Pi& operator=(Pi&& other) = default;
        ~Pi() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Euler : public VisitableNode<Euler> {
    public:
        Euler() = default;
        Euler(const Euler& other) = delete;
        Euler(Euler&& other) = default;
        Euler& operator=(const Euler& other) = delete;
        Euler& operator=(Euler&& other) = default;
        ~Euler() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class ImaginaryUnit : public VisitableNode<ImaginaryUnit> {
    public:
        ImaginaryUnit() = default;
        ImaginaryUnit(const ImaginaryUnit& other) = delete;
        ImaginaryUnit(ImaginaryUnit&& other) = default;
        ImaginaryUnit& operator=(const ImaginaryUnit& other) = delete;
        ImaginaryUnit& operator=(ImaginaryUnit&& other) = default;
        ~ImaginaryUnit() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    // ~~~~~TODO: Make this owns multiple terms
    class Addition : public VisitableNode<Addition> {
    public:
        std::pmr::vector<util::unique_pmr_ptr<Node>> terms;

        template <std::same_as<util::unique_pmr_ptr<Node>>... TTerms>
        explicit Addition(util::observer_ptr<std::pmr::memory_resource> memoryResource, const TTerms&... terms)
            : terms { (terms->clone(memoryResource))... }
        { }
        template <std::same_as<util::unique_pmr_ptr<Node>>... TTerms>
        explicit Addition(TTerms&&... terms)
            : terms { (std::forward(terms))... }
        { }
        explicit Addition(std::pmr::vector<util::unique_pmr_ptr<Node>>&& terms);

        Addition(const AnalyticExpression::Addition& other) = delete;
        Addition(AnalyticExpression::Addition&& other) = default;
        Addition& operator=(const AnalyticExpression::Addition& other) = delete;
        Addition& operator=(AnalyticExpression::Addition&& other) = default;
        ~Addition() override = default;

        [[nodiscard]]
        std::size_t hash() const override;
        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Multiplication : public VisitableNode<Multiplication> {
    public:
        std::pmr::vector<util::unique_pmr_ptr<Node>> factors;

        template <std::same_as<util::unique_pmr_ptr<Node>>... TFactors>
        explicit Multiplication(util::observer_ptr<std::pmr::memory_resource> memoryResource, const TFactors&... factors)
            : factors { (factors->clone(memoryResource))... }
        { }
        template <std::same_as<util::unique_pmr_ptr<Node>>... TFactors>
        explicit Multiplication(TFactors&&... factors)
            : factors { (std::forward(factors))... }
        { }
        explicit Multiplication(std::pmr::vector<util::unique_pmr_ptr<Node>>&& factors);

        Multiplication(const AnalyticExpression::Multiplication& other) = delete;
        Multiplication(AnalyticExpression::Multiplication&& other) = default;
        Multiplication& operator=(const AnalyticExpression::Multiplication& other) = delete;
        Multiplication& operator=(AnalyticExpression::Multiplication&& other) = default;
        ~Multiplication() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Power : public VisitableNode<Power> {
    public:
        util::unique_pmr_ptr<Node> base;
        util::unique_pmr_ptr<Node> exponent;

        explicit Power(const util::unique_pmr_ptr<Node>& base, const util::unique_pmr_ptr<Node>& exponent, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit Power(util::unique_pmr_ptr<Node>&& base, util::unique_pmr_ptr<Node>&& exponent);

        Power(const AnalyticExpression::Power& other) = delete;
        Power(AnalyticExpression::Power&& other) = default;
        Power& operator=(const AnalyticExpression::Power& other) = delete;
        Power& operator=(AnalyticExpression::Power&& other) = default;
        ~Power() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class AbsoluteValue : public VisitableNode<AbsoluteValue> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit AbsoluteValue(const util::unique_pmr_ptr<Node>& operand, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit AbsoluteValue(util::unique_pmr_ptr<Node>&& operand);

        AbsoluteValue(const AnalyticExpression::AbsoluteValue& other) = delete;
        AbsoluteValue(AnalyticExpression::AbsoluteValue&& other) = default;
        AbsoluteValue& operator=(const AnalyticExpression::AbsoluteValue& other) = delete;
        AbsoluteValue& operator=(AnalyticExpression::AbsoluteValue&& other) = default;
        ~AbsoluteValue() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Ceiling : public VisitableNode<Ceiling> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Ceiling(const util::unique_pmr_ptr<Node>& operand, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit Ceiling(util::unique_pmr_ptr<Node>&& operand);

        Ceiling(const AnalyticExpression::Ceiling& other) = delete;
        Ceiling(AnalyticExpression::Ceiling&& other) = default;
        Ceiling& operator=(const AnalyticExpression::Ceiling& other) = delete;
        Ceiling& operator=(AnalyticExpression::Ceiling&& other) = default;
        ~Ceiling() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Floor : public VisitableNode<Floor> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Floor(const util::unique_pmr_ptr<Node>& operand, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit Floor(util::unique_pmr_ptr<Node>&& operand);

        Floor(const AnalyticExpression::Floor& other) = delete;
        Floor(AnalyticExpression::Floor&& other) = default;
        Floor& operator=(const AnalyticExpression::Floor& other) = delete;
        Floor& operator=(AnalyticExpression::Floor&& other) = default;
        ~Floor() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Modulus : public VisitableNode<Modulus> {
    public:
        util::unique_pmr_ptr<Node> dividend;
        util::unique_pmr_ptr<Node> divisor;

        explicit Modulus(const util::unique_pmr_ptr<Node>& dividend, const util::unique_pmr_ptr<Node>& divisor, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit Modulus(util::unique_pmr_ptr<Node>&& dividend, util::unique_pmr_ptr<Node>&& divisor);

        Modulus(const AnalyticExpression::Modulus& other) = delete;
        Modulus(AnalyticExpression::Modulus&& other) = default;
        Modulus& operator=(const AnalyticExpression::Modulus& other) = delete;
        Modulus& operator=(AnalyticExpression::Modulus&& other) = default;
        ~Modulus() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Logarithm : public VisitableNode<Logarithm> {
    public:
        util::unique_pmr_ptr<Node> base;
        util::unique_pmr_ptr<Node> operand;

        explicit Logarithm(const util::unique_pmr_ptr<Node>& base, const util::unique_pmr_ptr<Node>& operand, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit Logarithm(util::unique_pmr_ptr<Node>&& base, util::unique_pmr_ptr<Node>&& operand);

        Logarithm(const AnalyticExpression::Logarithm& other) = delete;
        Logarithm(AnalyticExpression::Logarithm&& other) = default;
        Logarithm& operator=(const AnalyticExpression::Logarithm& other) = delete;
        Logarithm& operator=(AnalyticExpression::Logarithm&& other) = default;
        ~Logarithm() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class NaturalLogarithm : public VisitableNode<NaturalLogarithm> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit NaturalLogarithm(const util::unique_pmr_ptr<Node>& operand, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit NaturalLogarithm(util::unique_pmr_ptr<Node>&& operand);

        NaturalLogarithm(const AnalyticExpression::NaturalLogarithm& other) = delete;
        NaturalLogarithm(AnalyticExpression::NaturalLogarithm&& other) = default;
        NaturalLogarithm& operator=(const AnalyticExpression::NaturalLogarithm& other) = delete;
        NaturalLogarithm& operator=(AnalyticExpression::NaturalLogarithm&& other) = default;
        ~NaturalLogarithm() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };
    class Sine : public VisitableNode<Sine> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Sine(const util::unique_pmr_ptr<Node>& operand, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit Sine(util::unique_pmr_ptr<Node>&& operand);

        Sine(const AnalyticExpression::Sine& other) = delete;
        Sine(AnalyticExpression::Sine&& other) = default;
        Sine& operator=(const AnalyticExpression::Sine& other) = delete;
        Sine& operator=(AnalyticExpression::Sine&& other) = default;
        ~Sine() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Cosine : public VisitableNode<Cosine> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Cosine(const util::unique_pmr_ptr<Node>& operand, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit Cosine(util::unique_pmr_ptr<Node>&& operand);

        Cosine(const AnalyticExpression::Cosine& other) = delete;
        Cosine(AnalyticExpression::Cosine&& other) = default;
        Cosine& operator=(const AnalyticExpression::Cosine& other) = delete;
        Cosine& operator=(AnalyticExpression::Cosine&& other) = default;
        ~Cosine() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Tangent : public VisitableNode<Tangent> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Tangent(const util::unique_pmr_ptr<Node>& operand, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit Tangent(util::unique_pmr_ptr<Node>&& operand);

        Tangent(const AnalyticExpression::Tangent& other) = delete;
        Tangent(AnalyticExpression::Tangent&& other) = default;
        Tangent& operator=(const AnalyticExpression::Tangent& other) = delete;
        Tangent& operator=(AnalyticExpression::Tangent&& other) = default;
        ~Tangent() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Arcsine : public VisitableNode<Arcsine> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Arcsine(const util::unique_pmr_ptr<Node>& operand, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit Arcsine(util::unique_pmr_ptr<Node>&& operand);

        Arcsine(const AnalyticExpression::Arcsine& other) = delete;
        Arcsine(AnalyticExpression::Arcsine&& other) = default;
        Arcsine& operator=(const AnalyticExpression::Arcsine& other) = delete;
        Arcsine& operator=(AnalyticExpression::Arcsine&& other) = default;
        ~Arcsine() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Arccosine : public VisitableNode<Arccosine> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Arccosine(const util::unique_pmr_ptr<Node>& operand, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit Arccosine(util::unique_pmr_ptr<Node>&& operand);

        Arccosine(const AnalyticExpression::Arccosine& other) = delete;
        Arccosine(AnalyticExpression::Arccosine&& other) = default;
        Arccosine& operator=(const AnalyticExpression::Arccosine& other) = delete;
        Arccosine& operator=(AnalyticExpression::Arccosine&& other) = default;
        ~Arccosine() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };

    class Arctangent : public VisitableNode<Arctangent> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Arctangent(const util::unique_pmr_ptr<Node>& operand, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit Arctangent(util::unique_pmr_ptr<Node>&& operand);

        Arctangent(const AnalyticExpression::Arctangent& other) = delete;
        Arctangent(AnalyticExpression::Arctangent&& other) = default;
        Arctangent& operator=(const AnalyticExpression::Arctangent& other) = delete;
        Arctangent& operator=(AnalyticExpression::Arctangent&& other) = default;
        ~Arctangent() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
        [[nodiscard]]
        NodeType type() const override;
    };
    struct DifferentiationContext;
    struct Simplification {
        struct Context {
            ApproximationOptions<Rational> approximation;
            enum Action : std::uint8_t {
                /// Normalize the expression, including basic operations. e.g. (x * 2) * (y * 3) -> 6xy
                Normalize,
                /// Perform algebraic simplification.
                AlgebraicSimplification,
                /// Perform trigonometric simplification. e.g. sin^2(x) + cos^2(x) -> 1
                TrigonometricSimplification,
                /// Perform approximation for irrational numbers. e.g. cos(1) -> 0.5403
                /// You have to specify the `approximation` field in the context for this action to work.
                ApproximateCalculation,
                /// Force expand the expression. e.g. (x + 1)^2 -> x^2 + 2x + 1.
                /// Should not be used together with `Factor` action.
                Expand,
                /// Force factor the expression. e.g. x^2 + 2x + 1 -> (x + 1)^2.
                /// Should not be used together with `Expand` action.
                Factor,
                /// Replace the expression with equivalent basic operations. e.g. x mod y -> x - y * floor(x / y)
                Rewrite,

                //
                Action_COUNT_,
            };
            std::bitset<Action_COUNT_> actions;

            // TODO(P2): Uncomment this when we have implemented the condition system.
            // struct Condition {
            //     std::vector<Equation> equations;
            //     std::vector<Inequality> inequalities;
            // } condition;

            explicit Context() noexcept;
        };
    };
#pragma endregion
    /// The root node of the expression tree.
    util::unique_pmr_ptr<Node> base;

    explicit AnalyticExpression(std::shared_ptr<std::pmr::memory_resource> memoryResource = util::wrapUnownedAsShared(std::pmr::get_default_resource()));
    explicit AnalyticExpression(const Node& node, std::shared_ptr<std::pmr::memory_resource> memoryResource);
    explicit AnalyticExpression(const util::unique_pmr_ptr<Node>& node, std::shared_ptr<std::pmr::memory_resource> memoryResource);
    explicit AnalyticExpression(util::unique_pmr_ptr<Node>&& node, std::shared_ptr<std::pmr::memory_resource> memoryResource);

    AnalyticExpression(const AnalyticExpression& other);
    AnalyticExpression(AnalyticExpression&& other) noexcept = default;
    AnalyticExpression& operator=(const AnalyticExpression& other);
    AnalyticExpression& operator=(AnalyticExpression&& other) noexcept = default;
    ~AnalyticExpression() = default;

    util::observer_ptr<std::pmr::memory_resource> memoryResource() const;

private:
    /// The memory resource used for allocating nodes in the expression tree.
    std::shared_ptr<std::pmr::memory_resource> memoryResource_;
}; // namespace thecalculater::math
/**
 * @brief Format an analytic expression in LaTeX format.
 *
 * @param expr The expression to format.
 * @return std::string The formatted expression in LaTeX format.
 */
export TCAPI std::string format(const AnalyticExpression& expr);

export TCAPI AnalyticExpression normalize(AnalyticExpression expr);
export TCAPI AnalyticExpression simplify(const AnalyticExpression& expr, AnalyticExpression::Simplification::Context context = AnalyticExpression::Simplification::Context());

export TCAPI AnalyticExpression operator+(AnalyticExpression left, AnalyticExpression right);
export TCAPI AnalyticExpression operator-(AnalyticExpression left, AnalyticExpression right);
export TCAPI AnalyticExpression operator*(AnalyticExpression left, AnalyticExpression right);
export TCAPI AnalyticExpression operator/(AnalyticExpression left, AnalyticExpression right);
export TCAPI AnalyticExpression operator%(AnalyticExpression left, AnalyticExpression right);
export TCAPI AnalyticExpression operator-(AnalyticExpression operand);
export TCAPI AnalyticExpression operator+(AnalyticExpression operand);
export TCAPI AnalyticExpression& operator+=(AnalyticExpression left, AnalyticExpression right);
export TCAPI AnalyticExpression& operator-=(AnalyticExpression left, AnalyticExpression right);
export TCAPI AnalyticExpression& operator*=(AnalyticExpression left, AnalyticExpression right);
export TCAPI AnalyticExpression& operator/=(AnalyticExpression left, AnalyticExpression right);
export TCAPI AnalyticExpression& operator%=(AnalyticExpression left, AnalyticExpression right);
// TODO(P0): add factory to construct expressions in a convenient way
} // namespace thecalculater::math