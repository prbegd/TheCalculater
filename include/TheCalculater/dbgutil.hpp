/**
 * @file dbgutil.hpp
 * @author prbegd
 * @brief Some debug utilities.
 * @date 2025-05-25
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#pragma once
#include <boost/stacktrace/stacktrace.hpp>
#include <string_view>

namespace TheCalculater::dbgutil {
    std::string formatStacktrace(const boost::stacktrace::stacktrace& stk = boost::stacktrace::stacktrace());
    void init();
    inline std::string getCurrentStackTrace()
    {
        return formatStacktrace(boost::stacktrace::stacktrace());
    }
    /**
     * @brief Starts a detached process.
     * 
     * @param programPath The path to the program. Absolute or relative.
     * @param args The arguments to pass to the program.
     * @return true If the process was started successfully.
     * @return false If the process was not started. Will also log the error.
                   (In unix, it will not log the error if the new process failed to run execvp())
     */
    bool startDetachedProcess(std::string_view programPath, const std::vector<std::string_view> &arg);
}