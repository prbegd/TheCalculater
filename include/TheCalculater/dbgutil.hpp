/**
 * @file dbgutil.hpp
 * @author prbegd
 * @brief Some debug utilities.
 * @date 2025-05-25
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#pragma once
#include <boost/stacktrace/stacktrace.hpp>
#include <memory>
#include <string_view>
#include <vector>

namespace TheCalculater::dbgutil {
    extern std::unique_ptr<std::vector<std::string_view>> g_programArgs;

    void init(int argc, char* argv[]);
    /**
     * @brief Starts a detached process.
     *
     * @param programPath The path to the program. Absolute or relative.
     * @param args The arguments to pass to the program.
     * @return true If the process was started successfully.
     * @return false If the process was not started. Will also log the error.
     */
    bool startDetachedProcess(std::string_view programPath, const std::vector<std::string_view>& arg);
}