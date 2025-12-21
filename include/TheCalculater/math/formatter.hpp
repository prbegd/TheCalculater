/**
 * @file formatter.hpp
 * @author prbegd
 * @date 2025-12-20
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
#pragma once
#include "TheCalculater/math/fraction.hpp"
namespace TheCalculater::math {
    class AnalyticExpression;
}

namespace TheCalculater::math {

    enum class FormatType : int8_t {
        LaTeX,
        PlainText
    };

    struct FractionFormatOptions {
        enum class Style : int8_t {
            /// Always format as fraction.
            AlwaysFraction,
            /// If the fraction have decimal part, format as fraction. Otherwise, format as integer.
            FractionWhenDecimal,
            /// If the decimal part of the fraction is repeated, format as fraction. Otherwise, format as decimal.
            FractionWhenRepeatedDecimal,
            /// Always format as decimal. So when the fraction is a repeating decimal, it will mark the repeated part.
            AlwaysDecimal
        };
        FormatType type = FormatType::LaTeX;
        /// The style of fraction formatting.
        // TODO: make this read from settings
        Style style = Style::FractionWhenRepeatedDecimal;
    };

    /**
     * @brief Format a fraction in given format.
     *
     * @param frac The fraction to format.
     * @return std::string The formatted fraction in given format.
     */
    std::string format(const Fraction& frac, const FractionFormatOptions& options = {});
    /**
     * @brief Format an analytic expression in LaTeX format.
     *
     * @param expr The expression to format.
     * @return std::string The formatted expression in LaTeX format.
     */
    std::string format(const AnalyticExpression& expr);

} // namespace TheCalculater::math