/**
 * @file analytic_expression.hpp
 * @author prbegd
 * @brief Declaration of algebraic expression class and related functions.
 * @date 2025-11-12
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#pragma once

namespace TheCalculater::math {
    class AnalyticExpression {
    public:
        class AbstractNode;

        class Constant;
        class Variable;

        class Addition;
        class Subtraction;
        class Multiplication;
        class Division;
        class Negation;
        class Affirmation;
        class Power;
        class Root;
        class Factorial;
        class AbsoluteValue;
        class Modulus;

        class Logarithm;
        class NaturalLogarithm;
        class Sine;
        class Cosine;
        class Tangent;
        class Arcsine;
        class Arccosine;
        class Arctangent;

    private:
        std::unique_ptr<AbstractNode> expr_;
    };
}