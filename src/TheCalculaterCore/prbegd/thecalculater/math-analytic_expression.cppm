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
    class INode;

    explicit AnalyticExpression(util::observer_ptr<std::pmr::memory_resource> memoryResource = std::pmr::get_default_resource());
    explicit AnalyticExpression(const INode& node, util::observer_ptr<std::pmr::memory_resource> memoryResource = std::pmr::get_default_resource());
    AnalyticExpression(const util::unique_pmr_ptr<INode>& node);
    explicit AnalyticExpression(const util::unique_pmr_ptr<INode>& node, util::observer_ptr<std::pmr::memory_resource> memoryResource);
    AnalyticExpression(util::unique_pmr_ptr<INode>&& node);
    explicit AnalyticExpression(util::unique_pmr_ptr<INode>&& node, util::observer_ptr<std::pmr::memory_resource> memoryResource);

    AnalyticExpression(const AnalyticExpression& other);
    AnalyticExpression(AnalyticExpression&& other) noexcept = default;
    AnalyticExpression& operator=(const AnalyticExpression& other);
    AnalyticExpression& operator=(AnalyticExpression&& other) noexcept = default;
    ~AnalyticExpression() = default;

    enum class NodeType : std::int8_t;

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

    template <typename TContext>
    class Rule;
    template <typename TContext>
    class RuleSubset;
    template <typename TContext>
    class RuleSet;
    struct DifferentiationContext;
    struct SimplifyContext;

private:
    /// The memory resource used for allocating nodes in the expression tree. It is not owned by the expression and should be managed by the caller.
    util::observer_ptr<std::pmr::memory_resource> memoryResource_;

public:
    /// The root node of the expression tree.
    util::unique_pmr_ptr<INode> base;
};
/**
 * @brief Format an analytic expression in LaTeX format.
 *
 * @param expr The expression to format.
 * @return std::string The formatted expression in LaTeX format.
 */
export TCAPI std::string format(const AnalyticExpression& expr);

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

/**
 * @brief The abstract class of the expression tree node.
 */
class AnalyticExpression::INode {
public:
    virtual ~INode() = default;

    /// @warning The hash value is NOT meant to be used in checking equality of two expressions.
    [[nodiscard]]
    virtual std::size_t hash() const = 0;

    [[nodiscard]]
    virtual util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const = 0;
    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone() const;
    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::unique_pmr_ptr<INode>) const;
    [[nodiscard]]
    virtual NodeType type() const = 0;
};

enum class AnalyticExpression::NodeType : std::int8_t {
    Constant,
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
};

class AnalyticExpression::Constant : public INode {
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
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Variable : public INode {
public:
    std::string name;

    explicit Variable(std::string_view name);
    explicit Variable(std::string&& name);

    [[nodiscard]]
    std::size_t hash() const override;

    Variable(const AnalyticExpression::Variable& other) = delete;
    Variable(AnalyticExpression::Variable&& other) = default;
    Variable& operator=(const AnalyticExpression::Variable& other) = delete;
    Variable& operator=(AnalyticExpression::Variable&& other) = default;
    ~Variable() override = default;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Infinity : public INode {
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
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Pi : public INode {
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
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Euler : public INode {
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
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::ImaginaryUnit : public INode {
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
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Addition : public INode {
public:
    util::unique_pmr_ptr<INode> left;
    util::unique_pmr_ptr<INode> right;

    explicit Addition(const util::unique_pmr_ptr<INode>& left, const util::unique_pmr_ptr<INode>& right);
    explicit Addition(util::unique_pmr_ptr<INode>&& left, util::unique_pmr_ptr<INode>&& right);

    Addition(const AnalyticExpression::Addition& other) = delete;
    Addition(AnalyticExpression::Addition&& other) = default;
    Addition& operator=(const AnalyticExpression::Addition& other) = delete;
    Addition& operator=(AnalyticExpression::Addition&& other) = default;
    ~Addition() override = default;

    [[nodiscard]]
    std::size_t hash() const override;
    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Multiplication : public INode {
public:
    util::unique_pmr_ptr<INode> left;
    util::unique_pmr_ptr<INode> right;

    explicit Multiplication(const util::unique_pmr_ptr<INode>& left, const util::unique_pmr_ptr<INode>& right);
    explicit Multiplication(util::unique_pmr_ptr<INode>&& left, util::unique_pmr_ptr<INode>&& right);

    Multiplication(const AnalyticExpression::Multiplication& other) = delete;
    Multiplication(AnalyticExpression::Multiplication&& other) = default;
    Multiplication& operator=(const AnalyticExpression::Multiplication& other) = delete;
    Multiplication& operator=(AnalyticExpression::Multiplication&& other) = default;
    ~Multiplication() override = default;

    [[nodiscard]]
    std::size_t hash() const override;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Power : public INode {
public:
    util::unique_pmr_ptr<INode> base;
    util::unique_pmr_ptr<INode> exponent;

    explicit Power(const util::unique_pmr_ptr<INode>& base, const util::unique_pmr_ptr<INode>& exponent);
    explicit Power(util::unique_pmr_ptr<INode>&& base, util::unique_pmr_ptr<INode>&& exponent);

    Power(const AnalyticExpression::Power& other) = delete;
    Power(AnalyticExpression::Power&& other) = default;
    Power& operator=(const AnalyticExpression::Power& other) = delete;
    Power& operator=(AnalyticExpression::Power&& other) = default;
    ~Power() override = default;

    [[nodiscard]]
    std::size_t hash() const override;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::AbsoluteValue : public INode {
public:
    util::unique_pmr_ptr<INode> operand;

    explicit AbsoluteValue(const util::unique_pmr_ptr<INode>& operand);
    explicit AbsoluteValue(util::unique_pmr_ptr<INode>&& operand);

    AbsoluteValue(const AnalyticExpression::AbsoluteValue& other) = delete;
    AbsoluteValue(AnalyticExpression::AbsoluteValue&& other) = default;
    AbsoluteValue& operator=(const AnalyticExpression::AbsoluteValue& other) = delete;
    AbsoluteValue& operator=(AnalyticExpression::AbsoluteValue&& other) = default;
    ~AbsoluteValue() override = default;

    [[nodiscard]]
    std::size_t hash() const override;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Ceiling : public INode {
public:
    util::unique_pmr_ptr<INode> operand;

    explicit Ceiling(const util::unique_pmr_ptr<INode>& operand);
    explicit Ceiling(util::unique_pmr_ptr<INode>&& operand);

    Ceiling(const AnalyticExpression::Ceiling& other) = delete;
    Ceiling(AnalyticExpression::Ceiling&& other) = default;
    Ceiling& operator=(const AnalyticExpression::Ceiling& other) = delete;
    Ceiling& operator=(AnalyticExpression::Ceiling&& other) = default;
    ~Ceiling() override = default;

    [[nodiscard]]
    std::size_t hash() const override;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Floor : public INode {
public:
    util::unique_pmr_ptr<INode> operand;

    explicit Floor(const util::unique_pmr_ptr<INode>& operand);
    explicit Floor(util::unique_pmr_ptr<INode>&& operand);

    Floor(const AnalyticExpression::Floor& other) = delete;
    Floor(AnalyticExpression::Floor&& other) = default;
    Floor& operator=(const AnalyticExpression::Floor& other) = delete;
    Floor& operator=(AnalyticExpression::Floor&& other) = default;
    ~Floor() override = default;

    [[nodiscard]]
    std::size_t hash() const override;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Modulus : public INode {
public:
    util::unique_pmr_ptr<INode> dividend;
    util::unique_pmr_ptr<INode> divisor;

    explicit Modulus(const util::unique_pmr_ptr<INode>& dividend, const util::unique_pmr_ptr<INode>& divisor);
    explicit Modulus(util::unique_pmr_ptr<INode>&& dividend, util::unique_pmr_ptr<INode>&& divisor);

    Modulus(const AnalyticExpression::Modulus& other) = delete;
    Modulus(AnalyticExpression::Modulus&& other) = default;
    Modulus& operator=(const AnalyticExpression::Modulus& other) = delete;
    Modulus& operator=(AnalyticExpression::Modulus&& other) = default;
    ~Modulus() override = default;

    [[nodiscard]]
    std::size_t hash() const override;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Logarithm : public INode {
public:
    util::unique_pmr_ptr<INode> base;
    util::unique_pmr_ptr<INode> operand;

    explicit Logarithm(const util::unique_pmr_ptr<INode>& base, const util::unique_pmr_ptr<INode>& operand);
    explicit Logarithm(util::unique_pmr_ptr<INode>&& base, util::unique_pmr_ptr<INode>&& operand);

    Logarithm(const AnalyticExpression::Logarithm& other) = delete;
    Logarithm(AnalyticExpression::Logarithm&& other) = default;
    Logarithm& operator=(const AnalyticExpression::Logarithm& other) = delete;
    Logarithm& operator=(AnalyticExpression::Logarithm&& other) = default;
    ~Logarithm() override = default;

    [[nodiscard]]
    std::size_t hash() const override;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::NaturalLogarithm : public INode {
public:
    util::unique_pmr_ptr<INode> operand;

    explicit NaturalLogarithm(const util::unique_pmr_ptr<INode>& operand);
    explicit NaturalLogarithm(util::unique_pmr_ptr<INode>&& operand);

    NaturalLogarithm(const AnalyticExpression::NaturalLogarithm& other) = delete;
    NaturalLogarithm(AnalyticExpression::NaturalLogarithm&& other) = default;
    NaturalLogarithm& operator=(const AnalyticExpression::NaturalLogarithm& other) = delete;
    NaturalLogarithm& operator=(AnalyticExpression::NaturalLogarithm&& other) = default;
    ~NaturalLogarithm() override = default;

    [[nodiscard]]
    std::size_t hash() const override;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};
class AnalyticExpression::Sine : public INode {
public:
    util::unique_pmr_ptr<INode> operand;

    explicit Sine(const util::unique_pmr_ptr<INode>& operand);
    explicit Sine(util::unique_pmr_ptr<INode>&& operand);

    Sine(const AnalyticExpression::Sine& other) = delete;
    Sine(AnalyticExpression::Sine&& other) = default;
    Sine& operator=(const AnalyticExpression::Sine& other) = delete;
    Sine& operator=(AnalyticExpression::Sine&& other) = default;
    ~Sine() override = default;

    [[nodiscard]]
    std::size_t hash() const override;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Cosine : public INode {
public:
    util::unique_pmr_ptr<INode> operand;

    explicit Cosine(const util::unique_pmr_ptr<INode>& operand);
    explicit Cosine(util::unique_pmr_ptr<INode>&& operand);

    Cosine(const AnalyticExpression::Cosine& other) = delete;
    Cosine(AnalyticExpression::Cosine&& other) = default;
    Cosine& operator=(const AnalyticExpression::Cosine& other) = delete;
    Cosine& operator=(AnalyticExpression::Cosine&& other) = default;
    ~Cosine() override = default;

    [[nodiscard]]
    std::size_t hash() const override;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Tangent : public INode {
public:
    util::unique_pmr_ptr<INode> operand;

    explicit Tangent(const util::unique_pmr_ptr<INode>& operand);
    explicit Tangent(util::unique_pmr_ptr<INode>&& operand);

    Tangent(const AnalyticExpression::Tangent& other) = delete;
    Tangent(AnalyticExpression::Tangent&& other) = default;
    Tangent& operator=(const AnalyticExpression::Tangent& other) = delete;
    Tangent& operator=(AnalyticExpression::Tangent&& other) = default;
    ~Tangent() override = default;

    [[nodiscard]]
    std::size_t hash() const override;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Arcsine : public INode {
public:
    util::unique_pmr_ptr<INode> operand;

    explicit Arcsine(const util::unique_pmr_ptr<INode>& operand);
    explicit Arcsine(util::unique_pmr_ptr<INode>&& operand);

    Arcsine(const AnalyticExpression::Arcsine& other) = delete;
    Arcsine(AnalyticExpression::Arcsine&& other) = default;
    Arcsine& operator=(const AnalyticExpression::Arcsine& other) = delete;
    Arcsine& operator=(AnalyticExpression::Arcsine&& other) = default;
    ~Arcsine() override = default;

    [[nodiscard]]
    std::size_t hash() const override;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Arccosine : public INode {
public:
    util::unique_pmr_ptr<INode> operand;

    explicit Arccosine(const util::unique_pmr_ptr<INode>& operand);
    explicit Arccosine(util::unique_pmr_ptr<INode>&& operand);

    Arccosine(const AnalyticExpression::Arccosine& other) = delete;
    Arccosine(AnalyticExpression::Arccosine&& other) = default;
    Arccosine& operator=(const AnalyticExpression::Arccosine& other) = delete;
    Arccosine& operator=(AnalyticExpression::Arccosine&& other) = default;
    ~Arccosine() override = default;

    [[nodiscard]]
    std::size_t hash() const override;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

class AnalyticExpression::Arctangent : public INode {
public:
    util::unique_pmr_ptr<INode> operand;

    explicit Arctangent(const util::unique_pmr_ptr<INode>& operand);
    explicit Arctangent(util::unique_pmr_ptr<INode>&& operand);

    Arctangent(const AnalyticExpression::Arctangent& other) = delete;
    Arctangent(AnalyticExpression::Arctangent&& other) = default;
    Arctangent& operator=(const AnalyticExpression::Arctangent& other) = delete;
    Arctangent& operator=(AnalyticExpression::Arctangent&& other) = default;
    ~Arctangent() override = default;

    [[nodiscard]]
    std::size_t hash() const override;

    [[nodiscard]]
    util::unique_pmr_ptr<INode> clone(util::observer_ptr<std::pmr::memory_resource> memoryResource) const override;
    [[nodiscard]]
    NodeType type() const override;
};

struct AnalyticExpression::SimplifyContext {
    struct Approximation {
        // e.g. we want 4 significant digits: 0.0001
        Rational approximationTolerance;
        // If approximation uses iterations, the iteration will stop after iteration count reach this value to prevent infinite loop.
        std::uint32_t approximationMaxIterations = 0;
        /// The pi value used in calculation.
        Rational pi;
        /// The euler value used in calculation.
        Rational e;
    };
    /// The approximation configuration for approximation action. It is ignored if approximation action is not set in `actions`.
    Approximation approximation;
    enum class Action : std::uint8_t {
        /// Do not simplify the expression.
        None = 0,
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
        EnumeratorsCount,
    };
    std::bitset<std::to_underlying(Action::EnumeratorsCount)> actions;

    // TODO: Uncomment this when we have implemented the condition system.
    // struct Condition {
    //     std::vector<Equation> equations;
    //     std::vector<Inequality> inequalities;
    // } condition;

    explicit SimplifyContext() noexcept;
};
} // namespace thecalculater::math