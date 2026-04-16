/**
 * @file value.cpp
 * @author prbegd
 * @date 2026-03-28
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;

module TheCalculater.settings.value;
import std;

namespace TheCalculater::settings {
    std::string Value::type() const noexcept
    {
        switch (index()) {
        case 0:
            return "BooleanValue";
        case 1:
            return "ListValue";
        case 2:
            return "ObjectValue";
        case 3:
            return "StringValue";
        case 4:
            return "IntegerValue";
        case 5:
            return "DecimalValue";
        default:
            return "Nothing";
        }
    }
} // namespace TheCalculater::settings