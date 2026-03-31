/**
 * @file formatter.cppm
 * @author prbegd
 * @date 2025-12-20
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;

export module TheCalculater.math.formatter;
import std.compat;

namespace TheCalculater::math {
    export enum class FormatType : std::int8_t {
        LaTeX,
        PlainText
    };
} // namespace TheCalculater::math