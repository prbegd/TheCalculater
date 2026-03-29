/**
 * @file macros.hpp
 * @author prbegd
 * @date 2026-03-15
 * 
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to 
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
#pragma once

#define THECALC_API __attribute__((visibility("default")))

#define THECALCULATER_DEFINE_EXCEPTION(name, base) \
    class name : public base {                     \
    public:                                        \
        explicit name(const std::string& message)  \
            : base(message)                        \
        { }                                        \
    }
