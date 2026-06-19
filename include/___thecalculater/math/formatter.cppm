/**
 * @file formatter.cppm
 * @author prbegd
 * @date 2025-12-20
 *
 * @brief Common types used in formatting
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;

export module thecalculater.math.formatter;
import std;

namespace thecalculater::math {
    export enum class FormatType : std::int8_t {
        LaTeX,
        PlainText
    };
} // namespace thecalculater::math