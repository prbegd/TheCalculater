/**
 * @file algebraic_expression.hpp
 * @author prbegd
 * @brief Algebraic expression class and related functions.
 * @date 2025-11-11
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */

// Sorry, my brain doesn't allow me to extend the domain to functional expressions.
// So no sin(), cos(), log(), etc. Maybe in the future I'll add them. Sorry.

#include "TheCalculater/core.hpp"
#include "fraction.hpp"
#include <memory>

namespace TheCalculater::math {
    class AlgebraicExpression {
    public:
        class AbstractTerm;
        class Monomial;
        class FractionTerm;
        class RadicalTerm;
        enum class TermType : uint8_t { Monomial, FractionTerm, RadicalTerm };

        struct Variable;
        struct Parameter;

        using VariableContext = std::unordered_map<std::string, Fraction>;
        using ParameterContext = std::unordered_map<std::string, Fraction>;

        enum class ToStrringMode : uint8_t { Latex, PlainText };

        AlgebraicExpression() = default;
        AlgebraicExpression(const AlgebraicExpression& other) = default;
        AlgebraicExpression(AlgebraicExpression&& other) = default;
        AlgebraicExpression& operator=(const AlgebraicExpression& other) = default;
        AlgebraicExpression& operator=(AlgebraicExpression&& other) = default;
        ~AlgebraicExpression() = default;

        /// @brief Get the terms of the algebraic expression.
        [[nodiscard]] const std::vector<std::unique_ptr<AbstractTerm>>& terms() const;
        /// @brief Get the terms of the algebraic expression.
        std::vector<std::unique_ptr<AbstractTerm>>& terms();

        /**
         * @brief Evaluate the algebraic expression.
         *
         * @param variables Variables context.
         * @param parameters Parameters context.
         * @return Fraction The result of the evaluation.
         */
        [[nodiscard]] Fraction evaluate(const VariableContext& variables, const ParameterContext& parameters) const;
        /**
         * @brief Derivate the algebraic expression with respect to a variable. (Partial derivative)
         *
         * @param variable The variable to derivate with respect to.
         * @param parameters Parameters context.
         * @return AlgebraicExpression The result of the derivate.
         */
        [[nodiscard]] AlgebraicExpression derivate(const std::string& variable, const ParameterContext& parameters) const;

        /// @brief Convert the algebraic expression to a string.
        [[nodiscard]] std::string toString(ToStrringMode mode = ToStrringMode::PlainText) const;

    private:
        std::vector<std::unique_ptr<AbstractTerm>> terms_;
        bool simplified_ = false;

        void simplify();
    };

    THECALCULATER_DEFINE_EXCEPTION(BadAlgebraicExpressionException, std::logic_error);

    class AlgebraicExpression::AbstractTerm {
    public:
        virtual ~AbstractTerm() = default;

        [[nodiscard]] virtual std::unique_ptr<AbstractTerm> clone() const = 0;
        [[nodiscard]] virtual TermType type() const = 0;

        [[nodiscard]] virtual Fraction evaluate(const AlgebraicExpression::VariableContext& variables, const AlgebraicExpression::ParameterContext& parameters) const = 0;
        [[nodiscard]] virtual AlgebraicExpression derivate(const std::string& variable, const AlgebraicExpression::ParameterContext& parameters) const = 0;
        [[nodiscard]] virtual std::string toString(ToStrringMode mode) const = 0;

    private:
        void simplify();
    };
} // namespace TheCalculater::math