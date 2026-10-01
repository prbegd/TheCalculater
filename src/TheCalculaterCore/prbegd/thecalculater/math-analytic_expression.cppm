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

    // REFACTOR(P3): rename this to NodeVisitor
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
    // REFACTOR(P3): rename this to NodeVisitorMutable
    using NodeVisitor = BasicNodeVisitor<std::add_lvalue_reference_t>;
    using NodeVisitorConst =
        BasicNodeVisitor<boost::mp11::mp_compose<std::add_const_t, std::add_lvalue_reference_t>::fn>;

    /**
     * @brief The abstract class of the expression tree node.
     */
    class Node {
    public:
        explicit Node();
        virtual ~Node() = default;

        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;
        Node(Node&&) = default;
        Node& operator=(Node&&) = default;

        // REFACTOR(P3) extract this to a individual function since the Node is not only used in calculations.
        // REFACTOR(P3) after above is done, we can also extract the clone function to a middle crtp class without the need of macro magic.
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
                const std::string& message = "Wild card nodes is only meant to be used in rule's patterns.");
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

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Variable : public VisitableNode<Variable> {
    public:
        std::pmr::string name;

        explicit Variable(std::pmr::memory_resource* memoryResource, std::string_view name);
        explicit Variable(std::pmr::string&& name);

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Infinity : public VisitableNode<Infinity> {
    public:
        explicit Infinity();

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Pi : public VisitableNode<Pi> {
    public:
        explicit Pi();

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Euler : public VisitableNode<Euler> {
    public:
        explicit Euler();

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class ImaginaryUnit : public VisitableNode<ImaginaryUnit> {
    public:
        explicit ImaginaryUnit();

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Addition : public VisitableNode<Addition> {
    public:
        std::pmr::vector<util::unique_pmr_ptr<Node>> terms;

        template <std::convertible_to<util::unique_pmr_ptr<Node>>... TTerms>
        explicit Addition(std::pmr::memory_resource* memoryResource, TTerms&&... terms)
            : terms(memoryResource)
        {
            this->terms.reserve(sizeof...(TTerms));
            (this->terms.push_back(std::forward<TTerms>(terms)), ...);
        }
        explicit Addition(std::pmr::vector<util::unique_pmr_ptr<Node>>&& terms);

        [[nodiscard]]
        std::size_t hash() const override;
        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Multiplication : public VisitableNode<Multiplication> {
    public:
        std::pmr::vector<util::unique_pmr_ptr<Node>> factors;

        template <std::convertible_to<util::unique_pmr_ptr<Node>>... TFactors>
        explicit Multiplication(std::pmr::memory_resource* memoryResource, TFactors&&... factors)
            : factors(memoryResource)
        {
            this->factors.reserve(sizeof...(TFactors));
            (this->factors.push_back(std::forward<TFactors>(factors)), ...);
        }
        explicit Multiplication(std::pmr::vector<util::unique_pmr_ptr<Node>>&& factors);

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Power : public VisitableNode<Power> {
    public:
        util::unique_pmr_ptr<Node> base;
        util::unique_pmr_ptr<Node> exponent;

        explicit Power(util::unique_pmr_ptr<Node>&& base, util::unique_pmr_ptr<Node>&& exponent);

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class AbsoluteValue : public VisitableNode<AbsoluteValue> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit AbsoluteValue(util::unique_pmr_ptr<Node>&& operand);

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Ceiling : public VisitableNode<Ceiling> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Ceiling(util::unique_pmr_ptr<Node>&& operand);

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Floor : public VisitableNode<Floor> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Floor(util::unique_pmr_ptr<Node>&& operand);

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Modulus : public VisitableNode<Modulus> {
    public:
        util::unique_pmr_ptr<Node> dividend;
        util::unique_pmr_ptr<Node> divisor;

        explicit Modulus(util::unique_pmr_ptr<Node>&& dividend, util::unique_pmr_ptr<Node>&& divisor);

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Logarithm : public VisitableNode<Logarithm> {
    public:
        util::unique_pmr_ptr<Node> argument;
        util::unique_pmr_ptr<Node> base;

        explicit Logarithm(util::unique_pmr_ptr<Node>&& argument, util::unique_pmr_ptr<Node>&& base);

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class NaturalLogarithm : public VisitableNode<NaturalLogarithm> {
    public:
        util::unique_pmr_ptr<Node> argument;

        explicit NaturalLogarithm(util::unique_pmr_ptr<Node>&& argument);

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };
    class Sine : public VisitableNode<Sine> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Sine(util::unique_pmr_ptr<Node>&& operand);

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Cosine : public VisitableNode<Cosine> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Cosine(util::unique_pmr_ptr<Node>&& operand);

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Tangent : public VisitableNode<Tangent> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Tangent(util::unique_pmr_ptr<Node>&& operand);

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Arcsine : public VisitableNode<Arcsine> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Arcsine(util::unique_pmr_ptr<Node>&& operand);

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Arccosine : public VisitableNode<Arccosine> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Arccosine(util::unique_pmr_ptr<Node>&& operand);

        [[nodiscard]]
        std::size_t hash() const override;

        [[nodiscard]]
        util::unique_pmr_ptr<Node> clone(std::pmr::memory_resource* memoryResource) const override;
    };

    class Arctangent : public VisitableNode<Arctangent> {
    public:
        util::unique_pmr_ptr<Node> operand;

        explicit Arctangent(util::unique_pmr_ptr<Node>&& operand);

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
            explicit UnexpectedInternalException(
                const std::string& message = "An unexpected exception was thrown during simplification.");
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

            // OPTIMIZE(P3): Condition should be able to store something cache-y and pass it into Replacer.
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
                return [&]<std::size_t... TIndexes>(std::index_sequence<TIndexes...>) -> util::unique_pmr_ptr<Node> {
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
        explicit Factory(const AnalyticExpression& from);

        template <std::derived_from<Node> TNodeType, typename... TArgs>
        [[nodiscard]]
        AnalyticExpression make(TArgs&&... args) const
        {
            return AnalyticExpression(raw<TNodeType>(std::forward<TArgs>(args)...), memoryResource_);
        }
        template <std::derived_from<Node> TNodeType, typename... TArgs>
        [[nodiscard]]
        auto raw(TArgs&&... args) const
        {
            constexpr bool needMemoryResourceForFirstArgumentToConstruct = !requires {
                TNodeType(unwrapExpression_(std::forward<TArgs>(args))...);
            } && requires { TNodeType(memoryResource_.get(), unwrapExpression_(std::forward<TArgs>(args))...); };
            if constexpr (needMemoryResourceForFirstArgumentToConstruct) {
                return util::makeUniquePmr<TNodeType>(
                    memoryResource_.get(), memoryResource_.get(), unwrapExpression_(std::forward<TArgs>(args))...);
            } else {
                return util::makeUniquePmr<TNodeType>(memoryResource_.get(),
                                                      unwrapExpression_(std::forward<TArgs>(args))...);
            }
        }

    private:
        std::shared_ptr<std::pmr::memory_resource> memoryResource_;

        template <typename T>
        decltype(auto) unwrapExpression_(T&& expr) const
        {
            if constexpr (std::same_as<std::remove_cvref_t<T>, AnalyticExpression>) {
                if constexpr (std::is_lvalue_reference_v<T>) {
                    return std::forward<T>(expr).base->clone(memoryResource_.get());
                }
                return std::forward<T>(expr).base;
            } else if constexpr (std::convertible_to<std::remove_cvref_t<T>, util::unique_pmr_ptr<Node>>
                                 && std::is_lvalue_reference_v<T>) {
                return std::forward<T>(expr)->clone(memoryResource_.get());
            } else {
                return std::forward<T>(expr);
            }
        }
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