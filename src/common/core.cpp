/**
 * @file core.cpp
 * @author prbegd
 * @brief Core functionality for TheCalculater.
 * @date 2025-07-31
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#include "TheCalculater/core.hpp"
#include <boost/stacktrace.hpp>

namespace TheCalculater::core {
    ThrowExData::ThrowExData(boost::stacktrace::stacktrace trace, std::exception_ptr cause)
        : trace(std::move(trace)), cause(std::move(cause))
    { }
    ThrowExData::ThrowExData(boost::stacktrace::stacktrace trace)
        : trace(std::move(trace)), cause(nullptr)
    { }
    void registerLogger(const std::shared_ptr<spdlog::logger>& logger)
    {
        // logger registered in main program weirdly synced to here (in linux ci build)
        // Just for safe, we check if the logger is already registered.
        if (spdlog::get(logger->name()))
            return;
        spdlog::register_logger(logger);
        spdlog::set_default_logger(logger);
    }
} // namespace TheCalculater::core