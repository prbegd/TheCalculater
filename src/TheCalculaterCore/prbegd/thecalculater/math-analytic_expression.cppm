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
        Degree,
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

    class NodeVisitor {
    public:
        std::function<void(Node& node)> defaultVisitor;

        NodeVisitor() = default;
        explicit NodeVisitor(std::function<void(Node&)> defaultVisitor);
        virtual ~NodeVisitor() = default;

        virtual void visit(Constant& node);
        virtual void visit(Variable& node);
        virtual void visit(Infinity& node);
        virtual void visit(Pi& node);
        virtual void visit(Euler& node);
        virtual void visit(ImaginaryUnit& node);
        virtual void visit(Addition& node);
        virtual void visit(Multiplication& node);
        virtual void visit(Power& node);
        virtual void visit(AbsoluteValue& node);
        virtual void visit(Ceiling& node);
        virtual void visit(Floor& node);
        virtual void visit(Modulus& node);
        virtual void visit(Logarithm& node);
        virtual void visit(NaturalLogarithm& node);
        virtual void visit(Sine& node);
        virtual void visit(Cosine& node);
        virtual void visit(Tangent& node);
        virtual void visit(Arcsine& node);
        virtual void visit(Arccosine& node);
        virtual void visit(Arctangent& node);
    };

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
        virtual void accept(NodeVisitor& visitor) = 0;
    };
    class Constant : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class Variable : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class Infinity : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class Pi : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class Euler : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class ImaginaryUnit : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    // TODO: Make this owns multiple terms
    class Addition : public Node {
    public:
        util::unique_pmr_ptr<Node> left;
        util::unique_pmr_ptr<Node> right;

        explicit Addition(const util::unique_pmr_ptr<Node>& left, const util::unique_pmr_ptr<Node>& right, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit Addition(util::unique_pmr_ptr<Node>&& left, util::unique_pmr_ptr<Node>&& right);

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
        void accept(NodeVisitor& visitor) override;
    };

    class Multiplication : public Node {
    public:
        util::unique_pmr_ptr<Node> left;
        util::unique_pmr_ptr<Node> right;

        explicit Multiplication(const util::unique_pmr_ptr<Node>& left, const util::unique_pmr_ptr<Node>& right, util::observer_ptr<std::pmr::memory_resource> memoryResource);
        explicit Multiplication(util::unique_pmr_ptr<Node>&& left, util::unique_pmr_ptr<Node>&& right);

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
        void accept(NodeVisitor& visitor) override;
    };

    class Power : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class AbsoluteValue : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class Ceiling : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class Floor : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class Modulus : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class Logarithm : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class NaturalLogarithm : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };
    class Sine : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class Cosine : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class Tangent : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class Arcsine : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class Arccosine : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };

    class Arctangent : public Node {
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
        void accept(NodeVisitor& visitor) override;
    };
    template <typename TContext>
    class Rule;
    template <typename TContext>
    class RuleSet;
    struct DifferentiationContext;
    struct SimplifyContext {
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

        // TODO: Uncomment this when we have implemented the condition system.
        // struct Condition {
        //     std::vector<Equation> equations;
        //     std::vector<Inequality> inequalities;
        // } condition;

        explicit SimplifyContext() noexcept;
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
};
/**
 * @brief Format an analytic expression in LaTeX format.
 *
 * @param expr The expression to format.
 * @return std::string The formatted expression in LaTeX format.
 */
export TCAPI std::string format(const AnalyticExpression& expr);

export TCAPI AnalyticExpression normalize(AnalyticExpression expr);
export TCAPI AnalyticExpression simplify(const AnalyticExpression& expr, AnalyticExpression::SimplifyContext context = AnalyticExpression::SimplifyContext());

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
// TODO: add factory to construct expressions in a convenient way
} // namespace thecalculater::math