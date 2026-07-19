/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
export module prbegd.thecalculater.math:basis;
import std;
import :concepts;

namespace thecalculater::math {
export enum class ExpressionOutputFormat : std::uint8_t {
    // e.g. \frac{2}{1}, 0.\overline{3}
    LaTeX,
    // e.g. 2/1, 0.{3}
    PlainText,
};
export enum class RationalNumericFormat : std::uint8_t {
    // e.g. 2/1, 3/2, 1/7
    AlwaysFraction,
    // e.g. 2, 3/2, 1/7
    PreferInteger,
    // e.g. 2, 1.5, 1/7
    PreferDecimal,
    // e.g. 2, 1.5, 0.{142857}
    AlwaysDecimal,
};
export template <typename T>
struct MathematicalConstants {
    T pi;
    T euler;
    T gamma;
    T phi;
};
export template <calculable T>
struct ApproximationOptions {
    bool enabled = true;
    std::uint32_t maxIterations = 1000;
    T tolerance = 0.0000000000000001;
    MathematicalConstants<T> constants;

    [[nodiscard]]
    bool approxEqual(const T& a, const T& b) const
    {
        if (!this->enabled) {
            return a == b;
        }
        return a > b ? a - b <= this->tolerance : b - a <= this->tolerance;
    }
};
} // namespace thecalculater::math