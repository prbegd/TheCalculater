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

namespace TheCalculater::dbgutil {
    void customTerminateHandler();
    std::string formatStacktrace(const boost::stacktrace::stacktrace& stk);
    void init();
    std::string currentISO8601TimeUTC();
}