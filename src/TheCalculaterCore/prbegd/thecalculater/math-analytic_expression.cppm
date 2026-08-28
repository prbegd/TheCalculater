/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version. TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details. You should have received a copy of the GNU General Public License along
 * with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 *
 */
module;
#include "thecalculater/macros.hpp"
#include <cassert>

export module prbegd.thecalculater.math:analytic_expression;
import std;
import thirdparty.core;
import prbegd.thecalculater.util;
import :rational;

namespace thecalculater::math {
export class TCAPI AnalyticExpression {
public:
#pragma region // AnalyticExpression nested classes definitions
    class Node;

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

    template <template <typename> typename TModifier>
    class BasicNodeVisitor {
    public:
        std::function<void(TModifier<Node>)> defaultCallback;
        std::flat_map<std::type_index, std::function<void(TModifier<Node>)>> callbacks;

        void operator()(TModifier<Node> node) const
        {
            auto it = this->callbacks.find(std::type_index(typeid(node)));
            if (it != this->callbacks.end()) {
                it->second(node);
            } else if (this->defaultCallback) {
                this->defaultCallback(node);
            }
        }

        template <typename... TCallbacks>
        explicit BasicNodeVisitor(TCallbacks&&... callbacks)
        {
            (
                [this](TCallbacks&& callback) -> void {
                    using CallbackArgs = boost::callable_traits::args_t<decltype(callback)>;
                    static_assert(std::tuple_size_v<CallbackArgs> == 1,
                                  "\nCallback type must have exactly one argument.");
                    using CallbackOriginalArgs = std::tuple_element_t<0, CallbackArgs>;
                    using CallbackArg = std::decay_t<CallbackOriginalArgs>;
                    static_assert(std::derived_from<CallbackArg, Node>,
                                  "\nCallback type must be derived from AnalyticExpression::Node.");
                    static_assert(std::convertible_to<TModifier<CallbackArg>, CallbackOriginalArgs>,
                                  "\nCallback type must be convertible from TModifier<callback_t>.");
                    if constexpr (std::invocable<decltype(callback), TModifier<Node>>) {
                        this->defaultCallback = std::move(callback);
                    } else {
                        auto wrapper = [callback = std::move(callback)](TModifier<Node> node) -> void {
                            assert((std::is_same_v<TModifier<Node>, TModifier<CallbackArg>>));
                            callback(static_cast<TModifier<CallbackArg>>(node));
                        };
                        this->callbacks[std::type_index(typeid(CallbackArg))] = wrapper;
                    }
                }(std::forward<TCallbacks>(callbacks)),
                ...);
        }
    };
    using NodeVisitor = BasicNodeVisitor<std::add_lvalue_reference_t>;
    using NodeVisitorConst =
        BasicNodeVisitor<boost::mp11::mp_compose<std::add_const_t, std::add_lvalue_reference_t>::fn>;

    /**
     * @brief The abstract class of the expression tree node.
     *
     * TODO(P0): Make all nodes orphans.
     */
    class Node {
    public:
        explicit Node();
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

        void accept(const NodeVisitor& visitor) override { visitor(static_cast<T&>(*this)); }
        void accept(const NodeVisitorConst& visitor) const override { visitor(static_cast<const T&>(*this)); }

    protected:
        VisitableNode() = default; // NOLINT(bugprone-crtp-constructor-accessibility)
    };
    struct Wildcard {
    public:
        using Id = char8_t;
        class UsedInCalculationException : public std::logic_error, public boost::exception {
        public:
            explicit UsedInCalculationException(
                const std::string& message =
                    "Wild card nodes is only for rule matching and is not for calculation.");
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

            explicit Any(char8_t id);

            [[nodiscard]]
            util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
        };
        class Variadic : public WildNode<Variadic> {
        public:
            char8_t id { };

            explicit Variadic(char8_t id);

            [[nodiscard]]
            util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
        };
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
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Variable : public VisitableNode<Variable> {
    public:
        std::pmr::string name;

        explicit Variable(std::string_view name, std::pmr::memory_resource* memoryResource);
        explicit Variable(std::pmr::string&& name);

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
        explicit Infinity();

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
        explicit Pi();

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
        explicit Euler();

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
        explicit ImaginaryUnit();

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

        template <std::convertible_to<util::unique_pmr_ptr<Node>>... TTerms>
            requires(requires(std::pmr::memory_resource* memoryResource, const TTerms& t) {
                t->clone(memoryResource);
            } && ...)
        explicit Addition(std::pmr::memory_resource* memoryResource, const TTerms&... terms)
        {
            this->terms.reserve(sizeof...(TTerms));
            (this->terms.push_back(terms->clone(memoryResource)), ...);
        }
        template <std::convertible_to<util::unique_pmr_ptr<Node>>... TTerms>
        explicit Addition(TTerms&&... terms)
        {
            this->terms.reserve(sizeof...(TTerms));
            (this->terms.push_back(std::forward<TTerms>(terms)), ...);
        }
        explicit Addition(std::pmr::vector<util::unique_pmr_ptr<Node>>&& terms);

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

        template <std::convertible_to<util::unique_pmr_ptr<Node>>... TFactors>
            requires(requires(std::pmr::memory_resource* memoryResource, const TFactors& t) {
                t->clone(memoryResource);
            } && ...)
        explicit Multiplication(std::pmr::memory_resource* memoryResource, const TFactors&... factors)
        {
            this->factors.reserve(sizeof...(TFactors));
            (this->factors.push_back(factors->clone(memoryResource)), ...);
        }
        template <std::convertible_to<util::unique_pmr_ptr<Node>>... TFactors>
        explicit Multiplication(TFactors&&... factors)
        {
            this->factors.reserve(sizeof...(TFactors));
            (this->factors.push_back(std::forward<TFactors>(factors)), ...);
        }
        explicit Multiplication(std::pmr::vector<util::unique_pmr_ptr<Node>>&& factors);

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

        explicit Power(const util::unique_pmr_ptr<Node>& base,
                       const util::unique_pmr_ptr<Node>& exponent,
                       std::pmr::memory_resource* memoryResource);
        explicit Power(util::unique_pmr_ptr<Node>&& base, util::unique_pmr_ptr<Node>&& exponent);

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

        explicit AbsoluteValue(const util::unique_pmr_ptr<Node>& operand,
                               std::pmr::memory_resource* memoryResource);
        explicit AbsoluteValue(util::unique_pmr_ptr<Node>&& operand);

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

        explicit Ceiling(const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Ceiling(util::unique_pmr_ptr<Node>&& operand);

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

        explicit Floor(const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Floor(util::unique_pmr_ptr<Node>&& operand);

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

        explicit Modulus(const util::unique_pmr_ptr<Node>& dividend,
                         const util::unique_pmr_ptr<Node>& divisor,
                         std::pmr::memory_resource* memoryResource);
        explicit Modulus(util::unique_pmr_ptr<Node>&& dividend, util::unique_pmr_ptr<Node>&& divisor);

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
        util::unique_pmr_ptr<Node> argument;
        util::unique_pmr_ptr<Node> base;

        explicit Logarithm(const util::unique_pmr_ptr<Node>& argument,
                           const util::unique_pmr_ptr<Node>& base,
                           std::pmr::memory_resource* memoryResource);
        explicit Logarithm(util::unique_pmr_ptr<Node>&& argument, util::unique_pmr_ptr<Node>&& base);

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
        util::unique_pmr_ptr<Node> argument;

        explicit NaturalLogarithm(const util::unique_pmr_ptr<Node>& argument,
                                  std::pmr::memory_resource* memoryResource);
        explicit NaturalLogarithm(util::unique_pmr_ptr<Node>&& argument);

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

        explicit Sine(const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Sine(util::unique_pmr_ptr<Node>&& operand);

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

        explicit Cosine(const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Cosine(util::unique_pmr_ptr<Node>&& operand);

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

        explicit Tangent(const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Tangent(util::unique_pmr_ptr<Node>&& operand);

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

        explicit Arcsine(const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Arcsine(util::unique_pmr_ptr<Node>&& operand);

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

        explicit Arccosine(const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Arccosine(util::unique_pmr_ptr<Node>&& operand);

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

        explicit Arctangent(const util::unique_pmr_ptr<Node>& operand, std::pmr::memory_resource* memoryResource);
        explicit Arctangent(util::unique_pmr_ptr<Node>&& operand);

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
        class UnexpectedInternalException : public std::runtime_error, public boost::exception {
        public:
            explicit UnexpectedInternalException(const std::string& message = "An unexpected exception was thrown during simplification.");
        };
        class InvalidRuleException : public std::logic_error, public boost::exception {
        public:
            explicit InvalidRuleException(const std::string& message);
        };
        class Rule {
        public:
            using WildcardMap = std::pmr::unordered_map<Wildcard::Id, std::pmr::vector<util::unique_pmr_ptr<Node>>>;
            using Condition = std::function<bool(const Node& matched, const WildcardMap& map, const Context& context)>;
            using Replacer = std::function<util::unique_pmr_ptr<Node>(WildcardMap map, const Context& context)>;

            // OPTIMIZE(P2): Condition should be able to store something cache-y and pass it into Replacer.
            util::unique_pmr_ptr<Node> pattern;
            Condition condition;
            Replacer replacer;

            explicit Rule(util::unique_pmr_ptr<Node>&& pattern, Condition condition, Replacer replacer);

            [[nodiscard]]
            std::optional<WildcardMap> match(const Node& target, const Context& context) const;
            [[nodiscard]]
            util::unique_pmr_ptr<Node> apply(WildcardMap map, const Context& context) const;

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
                return
                    [&]<std::size_t... TIndexes>(std::index_sequence<TIndexes...>) -> util::unique_pmr_ptr<Node> {
                        util::unique_pmr_ptr<Node> result;
                        const Node* nextTarget = &target;

                        (
                            [&]<std::size_t TIndex> -> void {
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
            std::size_t maxNodes;

            explicit EGraphAlgorithm(std::size_t maxNodes);

            util::unique_pmr_ptr<Node> operator()(const Context& context, const Node& target) const override;
        };
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
        static bool structuralEqual(const AnalyticExpression::Node& a, const AnalyticExpression::Node& b);
        [[nodiscard]]
        static Integer complexityOf(const Node& node);
    };

    class Factory {
    public:
        explicit Factory(std::shared_ptr<std::pmr::memory_resource> memoryResource =
                             util::wrapUnownedAsShared(std::pmr::get_default_resource()));
        explicit Factory(const AnalyticExpression& expr);

        [[nodiscard]]
        AnalyticExpression constant(Rational value);
        [[nodiscard]]
        AnalyticExpression variable(std::string_view name);
        [[nodiscard]]
        AnalyticExpression infinity();
        [[nodiscard]]
        AnalyticExpression pi();
        [[nodiscard]]
        AnalyticExpression euler();
        [[nodiscard]]
        AnalyticExpression imaginary();

        template <std::same_as<AnalyticExpression>... Ts>
            requires(sizeof...(Ts) > 1)
        [[nodiscard]]
        AnalyticExpression addition(Ts... terms)
        {
            return AnalyticExpression(
                util::makeUniquePmr<Addition>(memoryResource_.get(), std::move(terms.base)...), memoryResource_);
        }
        template <std::same_as<AnalyticExpression>... Ts>
            requires(sizeof...(Ts) > 1)
        [[nodiscard]]
        AnalyticExpression multiplication(Ts... factors)
        {
            return AnalyticExpression(
                util::makeUniquePmr<Multiplication>(memoryResource_.get(), std::move(factors.base)...),
                memoryResource_);
        }
        [[nodiscard]]
        AnalyticExpression power(AnalyticExpression base, AnalyticExpression exponent);
        [[nodiscard]]
        AnalyticExpression absoluteValue(AnalyticExpression operand);
        [[nodiscard]]
        AnalyticExpression ceiling(AnalyticExpression operand);
        [[nodiscard]]
        AnalyticExpression floor(AnalyticExpression operand);
        [[nodiscard]]
        AnalyticExpression modulus(AnalyticExpression dividend, AnalyticExpression divisor);
        [[nodiscard]]
        AnalyticExpression logarithm(AnalyticExpression argument, AnalyticExpression base);
        [[nodiscard]]
        AnalyticExpression naturalLogarithm(AnalyticExpression argument);
        [[nodiscard]]
        AnalyticExpression sine(AnalyticExpression operand);
        [[nodiscard]]
        AnalyticExpression cosine(AnalyticExpression operand);
        [[nodiscard]]
        AnalyticExpression tangent(AnalyticExpression operand);
        [[nodiscard]]
        AnalyticExpression arcsine(AnalyticExpression operand);
        [[nodiscard]]
        AnalyticExpression arccosine(AnalyticExpression operand);
        [[nodiscard]]
        AnalyticExpression arctangent(AnalyticExpression operand);

    private:
        std::shared_ptr<std::pmr::memory_resource> memoryResource_;
    };
#pragma endregion
    explicit AnalyticExpression(std::shared_ptr<std::pmr::memory_resource> memoryResource =
                                    util::wrapUnownedAsShared(std::pmr::get_default_resource()));
    explicit AnalyticExpression(const Node& node, std::shared_ptr<std::pmr::memory_resource> memoryResource);
    explicit AnalyticExpression(const util::unique_pmr_ptr<Node>& node,
                                std::shared_ptr<std::pmr::memory_resource> memoryResource);
    explicit AnalyticExpression(util::unique_pmr_ptr<Node>&& node,
                                std::shared_ptr<std::pmr::memory_resource> memoryResource);

    AnalyticExpression(const AnalyticExpression& other);
    AnalyticExpression(AnalyticExpression&& other) noexcept = default;
    AnalyticExpression& operator=(const AnalyticExpression& other);
    AnalyticExpression& operator=(AnalyticExpression&& other) noexcept = default;
    ~AnalyticExpression() = default;

    void normalize();
    void simplify(const Simplification::Context& context);

    [[nodiscard]]
    std::pmr::memory_resource* memoryResource() const;

    /// The root node of the expression tree.
    util::unique_pmr_ptr<Node> base;

private:
    friend class Factory;

    /// The memory resource used for allocation in the expression tree.
    std::shared_ptr<std::pmr::memory_resource> memoryResource_;
}; // namespace thecalculater::math
/**
 * @brief Format an analytic expression in LaTeX format.
 *
 * @param expr The expression to format.
 * @return std::string The formatted expression in LaTeX format.
 * TODO(P0): implement this
 */
export TCAPI std::string format(const AnalyticExpression& expr);

export TCAPI AnalyticExpression normalize(AnalyticExpression expr);
export TCAPI AnalyticExpression simplify(AnalyticExpression expr,
                                         const AnalyticExpression::Simplification::Context& context);
} // namespace thecalculater::math