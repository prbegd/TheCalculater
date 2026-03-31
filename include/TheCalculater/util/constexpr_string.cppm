/**
 * @file constexpr_string.cppm
 * @author prbegd
 * @date 2026-03-15
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;

export module TheCalculater.util.constexpr_string;
import std.compat;

namespace TheCalculater::util {
    export template <std::size_t N>
    struct ConstexprString {
        constexpr ConstexprString(const char (&s)[N])
        {
            std::copy_n(s, N, v);
        }
        char v[N] { };
    };
}
