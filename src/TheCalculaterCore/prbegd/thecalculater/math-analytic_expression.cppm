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
#include <boost/preprocessor/seq/for_each_i.hpp>

export module prbegd.thecalculater.math:analytic_expression;
import std;
import thirdparty.core;
import prbegd.thecalculater.util;
import :rational;

namespace thecalculater::math {
export class TCAPI AnalyticExpression {
public:
#pragma region // AnalyticExpression nested classes definitions
    class NodeVisitor;
    class NodeVisitorConst;

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

    /**
     * @brief The abstract class of the expression tree node.
     */
    class Node {
    public:
        [[deprecated]]
        Node* parent;

        explicit Node(Node* parent);
        virtual ~Node() = default;

        /// @warning The hash value is NOT meant to be used in checking equality of two expressions.
        [[nodiscard]]
        virtual std::size_t hash() const = 0;
        [[nodiscard]]
        virtual util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const = 0;
        virtual void accept(const NodeVisitor& visitor) = 0;
        virtual void accept(const NodeVisitorConst& visitor) const = 0;
    };
    template <typename T>
    class VisitableNode : public Node {
    public:
        using Node::Node;

        void accept(const NodeVisitor& visitor) override
        {
            visitor(static_cast<T&>(*this));
        }
        void accept(const NodeVisitorConst& visitor) const override
        {
            visitor(static_cast<const T&>(*this));
        }

    protected:
        VisitableNode() = default; // NOLINT(bugprone-crtp-constructor-accessibility)
    };
    struct Wildcard {
    public:
        using id_t = char8_t;
        class UsedInCalculationException : public std::logic_error, public boost::exception { // NOLINT(misc-multiple-inheritance)
        public:
            explicit UsedInCalculationException(const std::string& message = "Wild card nodes is only for rule matching and is not for calculation.");
        };
        template <typename T>
        class WildNode : public VisitableNode<T> {
        public:
            using VisitableNode<T>::VisitableNode;

            [[noreturn]]
            std::size_t hash() const override
            {
                throwext(UsedInCalculationException());
            }

        protected:
            WildNode() = default; // NOLINT(bugprone-crtp-constructor-accessibility)
        };
        class Any : public WildNode<Any> {
        public:
            char8_t id { };

            explicit Any(Node* parent, char8_t id);

            [[nodiscard]]
            util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
        };
        class Variadic : public WildNode<Variadic> {
        public:
            char8_t id { };

            explicit Variadic(Node* parent, char8_t id);

            [[nodiscard]]
            util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
        };
    };

    // REFACTOR(P3): Maybe use a unordered_map to store the visitor callbacks.
#define NODE_VISITOR_GENERATE_MEMBER(_r_, _data_, _i_, _nodeType_) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    std::function<void(_nodeType_&)> BOOST_PP_CAT(callback, _i_); /* NOLINT(bugprone-macro-parentheses) */ \
    void operator()(_nodeType_& node) const /* NOLINT(bugprone-macro-parentheses) */ \
    { \
        if (this->BOOST_PP_CAT(callback, _i_)) { \
            this->BOOST_PP_CAT(callback, _i_)(node); \
        } else if (this->defaultCallback) { \
            this->defaultCallback(node); \
        } \
    }
#define NODE_VISITOR_CONSTRUCTOR_ASSIGN_FIELD(_r_, _data_, _i_, _nodeType_) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    if constexpr (std::invocable<decltype(_data_), _nodeType_&> && !std::invocable<decltype(_data_), Node&>) { /* NOLINT */ \
        this->BOOST_PP_CAT(callback, _i_) = std::move(_data_); \
    } else
#define NODE_VISITOR_GENERATE(_nodeTypes_) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    BOOST_PP_SEQ_FOR_EACH_I(NODE_VISITOR_GENERATE_MEMBER, _, _nodeTypes_) \
    template <typename... TCallbacks> \
    explicit NodeVisitor(TCallbacks... callbacks) \
        : defaultCallback([](Node&) { }) \
    { \
        ([this](TCallbacks callback) { \
            BOOST_PP_SEQ_FOR_EACH_I(NODE_VISITOR_CONSTRUCTOR_ASSIGN_FIELD, callback, _nodeTypes_) \
            if constexpr (requires(Node& node) { callback(node); }) { \
                this->defaultCallback = std::move(callback); \
            } else { \
                static_assert(sizeof(TCallbacks) == 0, "\n  Callbacks of NodeVisitor must all be invocable with Node& or (one of its derived types)&."); \
            } \
        }(std::move(callbacks)), \
         ...); \
    }
#define NODE_VISITOR_CONST_GENERATE_MEMBER(_r_, _data_, _i_, _nodeType_) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    std::function<void(const _nodeType_&)> BOOST_PP_CAT(callback, _i_); /* NOLINT(bugprone-macro-parentheses) */ \
    void operator()(const _nodeType_& node) const \
    { \
        if (this->BOOST_PP_CAT(callback, _i_)) { \
            this->BOOST_PP_CAT(callback, _i_)(node); \
        } else if (this->defaultCallback) { \
            this->defaultCallback(node); \
        } \
    }
#define NODE_VISITOR_CONST_CONSTRUCTOR_ASSIGN_FIELD(_r_, _data_, _i_, _nodeType_) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    if constexpr (std::invocable<decltype(_data_), const _nodeType_&> && !std::invocable<decltype(_data_), const Node&>) { \
        this->BOOST_PP_CAT(callback, _i_) = std::move(_data_); \
    } else

#define NODE_VISITOR_CONST_GENERATE(_nodeTypes_) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    BOOST_PP_SEQ_FOR_EACH_I(NODE_VISITOR_CONST_GENERATE_MEMBER, _, _nodeTypes_) \
    template <typename... TCallbacks> \
    explicit NodeVisitorConst(TCallbacks... callbacks) \
        : defaultCallback([](const Node&) { }) \
    { \
        ([this](TCallbacks callback) { \
            BOOST_PP_SEQ_FOR_EACH_I(NODE_VISITOR_CONST_CONSTRUCTOR_ASSIGN_FIELD, callback, _nodeTypes_) \
            if constexpr (requires(const Node& node) { callback(node); }) { \
                this->defaultCallback = std::move(callback); \
            } else { \
                static_assert(sizeof(TCallbacks) == 0, "\n  Callbacks of NodeVisitorConst must all be invocable with const Node& or const (one of its derived types)&."); \
            } \
        }(std::move(callbacks)), \
         ...); \
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
    CREATE_CLASS_NODE_VISITOR_AND_NODE_VISITOR_CONST((Constant)(Variable)(Infinity)(Pi)(Euler)(ImaginaryUnit)(Addition)(Multiplication)(Power)(AbsoluteValue)(Ceiling)(Floor)(Modulus)(Logarithm)(NaturalLogarithm)(Sine)(Cosine)(Tangent)(Arcsine)(Arccosine)(Arctangent)(Wildcard::Any)(Wildcard::Variadic))
    THECALCULATER_DIAGNOSTIC(pop)

#undef NODE_VISITOR_GENERATE_MEMBER
#undef NODE_VISITOR_CONSTRUCTOR_ASSIGN_FIELD
#undef NODE_VISITOR_GENERATE
#undef NODE_VISITOR_CONST_GENERATE_MEMBER
#undef NODE_VISITOR_CONST_CONSTRUCTOR_ASSIGN_FIELD
#undef NODE_VISITOR_CONST_GENERATE
#undef CREATE_CLASS_NODE_VISITOR_AND_NODE_VISITOR_CONST
    class Constant : public VisitableNode<Constant> {
    public:
        Rational value;

        explicit Constant(Node* parent, Rational value);

        Constant(const AnalyticExpression::Constant& other) = delete;
        Constant(AnalyticExpression::Constant&& other) = default;
        Constant& operator=(const AnalyticExpression::Constant& other) = delete;
        Constant& operator=(AnalyticExpression::Constant&& other) = default;
        ~Constant() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Variable : public VisitableNode<Variable> {
    public:
        std::pmr::string name;

        explicit Variable(Node* parent, std::string_view name, std::pmr::memory_resource* memoryResource);
        explicit Variable(Node* parent, std::pmr::string&& name);

        [[nodiscard]]
        std::size_t hash() const override;

        Variable(const AnalyticExpression::Variable& other) = delete;
        Variable(AnalyticExpression::Variable&& other) = default;
        Variable& operator=(const AnalyticExpression::Variable& other) = delete;
        Variable& operator=(AnalyticExpression::Variable&& other) = default;
        ~Variable() override = default;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Infinity : public VisitableNode<Infinity> {
    public:
        explicit Infinity(Node* parent);

        Infinity(const Infinity& other) = delete;
        Infinity(Infinity&& other) = default;
        Infinity& operator=(const Infinity& other) = delete;
        Infinity& operator=(Infinity&& other) = default;
        ~Infinity() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Pi : public VisitableNode<Pi> {
    public:
        explicit Pi(Node* parent);

        Pi(const Pi& other) = delete;
        Pi(Pi&& other) = default;
        Pi& operator=(const Pi& other) = delete;
        Pi& operator=(Pi&& other) = default;
        ~Pi() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Euler : public VisitableNode<Euler> {
    public:
        explicit Euler(Node* parent);

        Euler(const Euler& other) = delete;
        Euler(Euler&& other) = default;
        Euler& operator=(const Euler& other) = delete;
        Euler& operator=(Euler&& other) = default;
        ~Euler() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class ImaginaryUnit : public VisitableNode<ImaginaryUnit> {
    public:
        explicit ImaginaryUnit(Node* parent);

        ImaginaryUnit(const ImaginaryUnit& other) = delete;
        ImaginaryUnit(ImaginaryUnit&& other) = default;
        ImaginaryUnit& operator=(const ImaginaryUnit& other) = delete;
        ImaginaryUnit& operator=(ImaginaryUnit&& other) = default;
        ~ImaginaryUnit() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Addition : public VisitableNode<Addition> {
    public:
        std::pmr::vector<util::unique_pmr_ptr<Node>> terms;

        template <std::same_as<util::unique_pmr_ptr<Node>>... TTerms>
        explicit Addition(Node* parent, std::pmr::memory_resource* memoryResource, const TTerms&... terms)
            : VisitableNode(parent),
              terms { (terms->clone(memoryResource))... }
        {
            for (auto& term : this->terms) {
                term->parent = this;
            }
        }
        template <std::same_as<util::unique_pmr_ptr<Node>>... TTerms>
        explicit Addition(Node* parent, TTerms&&... terms)
            : VisitableNode(parent),
              terms { (std::forward(terms))... }
        {
            for (auto& term : this->terms) {
                term->parent = this;
            }
        }
        explicit Addition(Node* parent, std::pmr::vector<util::unique_pmr_ptr<Node>>&& terms);

        Addition(const AnalyticExpression::Addition& other) = delete;
        Addition(AnalyticExpression::Addition&& other) = default;
        Addition& operator=(const AnalyticExpression::Addition& other) = delete;
        Addition& operator=(AnalyticExpression::Addition&& other) = default;
        ~Addition() override = default;

        [[nodiscard]]
        std::size_t hash() const override;
        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Multiplication : public VisitableNode<Multiplication> {
    public:
        std::pmr::vector<util::unique_pmr_ptr<Node>> factors;

        template <std::same_as<util::unique_pmr_ptr<Node>>... TFactors>
        explicit Multiplication(Node* parent, std::pmr::memory_resource* memoryResource, const TFactors&... factors)
            : VisitableNode(parent),
              factors { (factors->clone(memoryResource))... }
        {
            for (auto& factor : this->factors) {
                factor->parent = this;
            }
        }
        template <std::same_as<util::unique_pmr_ptr<Node>>... TFactors>
        explicit Multiplication(Node* parent, TFactors&&... factors)
            : VisitableNode(parent),
              factors { (std::forward(factors))... }
        {
            for (auto& factor : this->factors) {
                factor->parent = this;
            }
        }
        explicit Multiplication(Node* parent, std::pmr::vector<util::unique_pmr_ptr<Node>>&& factors);

        Multiplication(const AnalyticExpression::Multiplication& other) = delete;
        Multiplication(AnalyticExpression::Multiplication&& other) = default;
        Multiplication& operator=(const AnalyticExpression::Multiplication& other) = delete;
        Multiplication& operator=(AnalyticExpression::Multiplication&& other) = default;
        ~Multiplication() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Power : public VisitableNode<Power> {
    public:
        util::unique_pmr_ptr<Node> base;
        util::unique_pmr_ptr<Node> exponent;

        explicit Power(Node* parent, const util::unique_pmr_ptr<Node>& base, const util::unique_pmr_ptr<Node>& exponent, std::pmr::memory_resource* memoryResource);
        explicit Power(Node* parent, util::unique_pmr_ptr<Node>&& base, util::unique_pmr_ptr<Node>&& exponent);

        Power(const AnalyticExpression::Power& other) = delete;
        Power(AnalyticExpression::Power&& other) = default;
        Power& operator=(const AnalyticExpression::Power& other) = delete;
        Power& operator=(AnalyticExpression::Power&& other) = default;
        ~Power() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class AbsoluteValue : public VisitableNode<AbsoluteValue> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit AbsoluteValue(Node* parent, const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit AbsoluteValue(Node* parent, util::unique_pmr_ptr<Node>&& operand);

        AbsoluteValue(const AnalyticExpression::AbsoluteValue& other) = delete;
        AbsoluteValue(AnalyticExpression::AbsoluteValue&& other) = default;
        AbsoluteValue& operator=(const AnalyticExpression::AbsoluteValue& other) = delete;
        AbsoluteValue& operator=(AnalyticExpression::AbsoluteValue&& other) = default;
        ~AbsoluteValue() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Ceiling : public VisitableNode<Ceiling> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Ceiling(Node* parent, const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Ceiling(Node* parent, util::unique_pmr_ptr<Node>&& operand);

        Ceiling(const AnalyticExpression::Ceiling& other) = delete;
        Ceiling(AnalyticExpression::Ceiling&& other) = default;
        Ceiling& operator=(const AnalyticExpression::Ceiling& other) = delete;
        Ceiling& operator=(AnalyticExpression::Ceiling&& other) = default;
        ~Ceiling() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Floor : public VisitableNode<Floor> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Floor(Node* parent, const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Floor(Node* parent, util::unique_pmr_ptr<Node>&& operand);

        Floor(const AnalyticExpression::Floor& other) = delete;
        Floor(AnalyticExpression::Floor&& other) = default;
        Floor& operator=(const AnalyticExpression::Floor& other) = delete;
        Floor& operator=(AnalyticExpression::Floor&& other) = default;
        ~Floor() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Modulus : public VisitableNode<Modulus> {
    public:
        util::unique_pmr_ptr<Node> dividend;
        util::unique_pmr_ptr<Node> divisor;

        explicit Modulus(Node* parent, const util::unique_pmr_ptr<Node>& dividend, const util::unique_pmr_ptr<Node>& divisor, std::pmr::memory_resource* memoryResource);
        explicit Modulus(Node* parent, util::unique_pmr_ptr<Node>&& dividend, util::unique_pmr_ptr<Node>&& divisor);

        Modulus(const AnalyticExpression::Modulus& other) = delete;
        Modulus(AnalyticExpression::Modulus&& other) = default;
        Modulus& operator=(const AnalyticExpression::Modulus& other) = delete;
        Modulus& operator=(AnalyticExpression::Modulus&& other) = default;
        ~Modulus() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Logarithm : public VisitableNode<Logarithm> {
    public:
        // TODO(P2): swap these two member fields and rename `operand` to `argument`, same as natural logarithm
        util::unique_pmr_ptr<Node> base;
        util::unique_pmr_ptr<Node> operand;

        explicit Logarithm(Node* parent, const util::unique_pmr_ptr<Node>& base, const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Logarithm(Node* parent, util::unique_pmr_ptr<Node>&& base, util::unique_pmr_ptr<Node>&& operand);

        Logarithm(const AnalyticExpression::Logarithm& other) = delete;
        Logarithm(AnalyticExpression::Logarithm&& other) = default;
        Logarithm& operator=(const AnalyticExpression::Logarithm& other) = delete;
        Logarithm& operator=(AnalyticExpression::Logarithm&& other) = default;
        ~Logarithm() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class NaturalLogarithm : public VisitableNode<NaturalLogarithm> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit NaturalLogarithm(Node* parent, const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit NaturalLogarithm(Node* parent, util::unique_pmr_ptr<Node>&& operand);

        NaturalLogarithm(const AnalyticExpression::NaturalLogarithm& other) = delete;
        NaturalLogarithm(AnalyticExpression::NaturalLogarithm&& other) = default;
        NaturalLogarithm& operator=(const AnalyticExpression::NaturalLogarithm& other) = delete;
        NaturalLogarithm& operator=(AnalyticExpression::NaturalLogarithm&& other) = default;
        ~NaturalLogarithm() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };
    class Sine : public VisitableNode<Sine> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Sine(Node* parent, const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Sine(Node* parent, util::unique_pmr_ptr<Node>&& operand);

        Sine(const AnalyticExpression::Sine& other) = delete;
        Sine(AnalyticExpression::Sine&& other) = default;
        Sine& operator=(const AnalyticExpression::Sine& other) = delete;
        Sine& operator=(AnalyticExpression::Sine&& other) = default;
        ~Sine() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Cosine : public VisitableNode<Cosine> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Cosine(Node* parent, const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Cosine(Node* parent, util::unique_pmr_ptr<Node>&& operand);

        Cosine(const AnalyticExpression::Cosine& other) = delete;
        Cosine(AnalyticExpression::Cosine&& other) = default;
        Cosine& operator=(const AnalyticExpression::Cosine& other) = delete;
        Cosine& operator=(AnalyticExpression::Cosine&& other) = default;
        ~Cosine() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Tangent : public VisitableNode<Tangent> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Tangent(Node* parent, const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Tangent(Node* parent, util::unique_pmr_ptr<Node>&& operand);

        Tangent(const AnalyticExpression::Tangent& other) = delete;
        Tangent(AnalyticExpression::Tangent&& other) = default;
        Tangent& operator=(const AnalyticExpression::Tangent& other) = delete;
        Tangent& operator=(AnalyticExpression::Tangent&& other) = default;
        ~Tangent() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Arcsine : public VisitableNode<Arcsine> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Arcsine(Node* parent, const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Arcsine(Node* parent, util::unique_pmr_ptr<Node>&& operand);

        Arcsine(const AnalyticExpression::Arcsine& other) = delete;
        Arcsine(AnalyticExpression::Arcsine&& other) = default;
        Arcsine& operator=(const AnalyticExpression::Arcsine& other) = delete;
        Arcsine& operator=(AnalyticExpression::Arcsine&& other) = default;
        ~Arcsine() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Arccosine : public VisitableNode<Arccosine> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Arccosine(Node* parent, const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Arccosine(Node* parent, util::unique_pmr_ptr<Node>&& operand);

        Arccosine(const AnalyticExpression::Arccosine& other) = delete;
        Arccosine(AnalyticExpression::Arccosine&& other) = default;
        Arccosine& operator=(const AnalyticExpression::Arccosine& other) = delete;
        Arccosine& operator=(AnalyticExpression::Arccosine&& other) = default;
        ~Arccosine() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Arctangent : public VisitableNode<Arctangent> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Arctangent(Node* parent, const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Arctangent(Node* parent, util::unique_pmr_ptr<Node>&& operand);

        Arctangent(const AnalyticExpression::Arctangent& other) = delete;
        Arctangent(AnalyticExpression::Arctangent&& other) = default;
        Arctangent& operator=(const AnalyticExpression::Arctangent& other) = delete;
        Arctangent& operator=(AnalyticExpression::Arctangent&& other) = default;
        ~Arctangent() override = default;

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    struct DifferentiationContext;
    struct Simplification {
        struct Context;
        class Rule {
        public:
            using wildcard_map_t = std::pmr::unordered_map<Wildcard::id_t, util::unique_pmr_ptr<Node>>;

            util::unique_pmr_ptr<Node> pattern;
            std::function<bool(const Node& matched, const wildcard_map_t& map)> condition;
            std::function<util::unique_pmr_ptr<Node>(wildcard_map_t map, std::pmr::memory_resource* memoryResource)> replacer;

            std::optional<wildcard_map_t> match(const Node& target, std::pmr::memory_resource* memoryResource) const;
            util::unique_pmr_ptr<Node> apply(wildcard_map_t map, std::pmr::memory_resource* memoryResource) const;

            bool operator==(const Rule& other) const;
        };
        using RuleSet = std::pmr::vector<Rule>;

        class Algorithm {
        public:
            virtual ~Algorithm() = default;

            virtual util::unique_pmr_ptr<Node> operator()(const Context& context, const Node& target) const = 0;
        };
        template <std::derived_from<Algorithm>... TAlgorithms>
        class SequenceAlgorithm : public Algorithm {
            static_assert(sizeof...(TAlgorithms) > 0, "SequenceAlgorithm requires at least one Algorithm");

        public:
            std::tuple<TAlgorithms...> algorithms;

            explicit SequenceAlgorithm(TAlgorithms&&... algorithms)
                : algorithms(std::forward<TAlgorithms>(algorithms)...)
            { }

            util::unique_pmr_ptr<Node> operator()(const Context& context, const Node& target) const override
            {
                return [&]<std::size_t... TIndexes>(std::index_sequence<TIndexes...>) -> util::unique_pmr_ptr<Node> {
                    util::unique_pmr_ptr<Node> result;
                    const Node* nextTarget = &target;

                    ([&]<std::size_t TIndex> -> void {
                        result = std::get<TIndex>(algorithms)(context, *nextTarget);
                        nextTarget = result.get();
                    }.template operator()<TIndexes>(),
                     ...);

                    return result;
                }(std::index_sequence_for<TAlgorithms...> { });
            }
        };
        class HillClimbingAlgorithm : public Algorithm {
        public:
            util::unique_pmr_ptr<Node> operator()(const Context& context, const Node& target) const override;
        };
        class EGraphAlgorithm : public Algorithm {
        public:
            util::unique_pmr_ptr<Node> operator()(const Context& context, const Node& target) const override;
        };
        // class ImplFixpointAlgorithm {
        //     util::unique_pmr_ptr<Node> operator()(const Algorithm& algorithm, const Context& context, const Node* target) const;
        //     template <std::derived_from<Algorithm> TAlgorithm>
        //     friend class FixpointAlgorithm;
        // };
        // template <std::derived_from<Algorithm> TAlgorithm>
        // class FixpointAlgorithm : public Algorithm {
        // public:
        //     TAlgorithm algorithm;

        //     explicit FixpointAlgorithm(TAlgorithm&& algorithm)
        //         : algorithm(std::forward<TAlgorithm>(algorithm))
        //     { }

        //     util::unique_pmr_ptr<Node> operator()(const Context& context, const Node* target) const override
        //     {
        //         return ImplFixpointAlgorithm()(algorithm, context, target);
        //     }
        //     // {
        //     //     const Node* previous = target;
        //     //     util::unique_pmr_ptr<Node> next;
        //     //     while (true) {
        //     //         util::unique_pmr_ptr<Node> current = algorithm(context, previous);
        //     //         if (structuralEqual_(current.get(), previous)) {
        //     //             break;
        //     //         }
        //     //         next = std::move(current);
        //     //         previous = next.get();
        //     //     }
        //     //     return next;
        //     // }
        // };
        // class ImplTreeWalkerAlgorithm {
        //     util::unique_pmr_ptr<Node> operator()(const Algorithm& algorithm, const Context& context, const Node* target) const;
        //     template <std::derived_from<Algorithm> TNodeAlgorithm>
        //     friend class TreeWalkerAlgorithm;
        // };
        // template <std::derived_from<Algorithm> TNodeAlgorithm>
        // class TreeWalkerAlgorithm : public Algorithm {
        // public:
        //     TNodeAlgorithm nodeAlgorithm;

        //     explicit TreeWalkerAlgorithm(TNodeAlgorithm&& nodeAlgorithm)
        //         : nodeAlgorithm(std::forward<TNodeAlgorithm>(nodeAlgorithm))
        //     { }

        //     util::unique_pmr_ptr<Node> operator()(const Context& context, const Node* target) const override
        //     {
        //         return ImplTreeWalkerAlgorithm()(nodeAlgorithm, context, target);
        //     }
        // };

        struct Context {
            RuleSet rules;
            util::unique_pmr_ptr<Algorithm> algorithm;
            ApproximationOptions<Rational> approximation;
            std::pmr::memory_resource* memoryResource;

            explicit Context(const AnalyticExpression& expr);
            explicit Context(std::pmr::memory_resource* memoryResource);
        };

        [[nodiscard]]
        static RuleSet generateDefaultRules(std::pmr::memory_resource* memoryResource);

        [[nodiscard]]
        static bool structuralEqual(const AnalyticExpression::Node& a, const AnalyticExpression::Node& b, std::pmr::memory_resource* memoryResource);
        [[nodiscard]]
        static Integer complexityOf(const Node& node);
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

    [[nodiscard]]
    std::pmr::memory_resource* memoryResource() const;

private:
    /// The memory resource used for allocation in the expression tree.
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
export TCAPI AnalyticExpression simplify(const AnalyticExpression& expr, AnalyticExpression::Simplification::Context context);

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