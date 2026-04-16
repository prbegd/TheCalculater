/**
 * @file throwEx.cpp
 * @author prbegd
 * @date 2026-03-28
 * 
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to 
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module TheCalculater.throwEx;
import tpmm.boost;

namespace TheCalculater {
    ThrowExData::ThrowExData(boost::stacktrace::stacktrace trace, std::exception_ptr cause)
        : trace(std::move(trace)), cause(std::move(cause))
    { }
    ThrowExData::ThrowExData(boost::stacktrace::stacktrace trace)
        : trace(std::move(trace)), cause(nullptr)
    { }
} // namespace TheCalculater::core