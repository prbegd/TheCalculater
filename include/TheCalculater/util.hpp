/**
 * @file util.hpp
 * @author prbegd
 * @brief Core utility functions and types.
 * @date 2025-06-21
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#pragma once
#include <boost/exception/all.hpp>
#include <boost/stacktrace.hpp>

namespace TheCalculater {
    namespace util {

        typedef boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace> traced;
    }
    template <class E>
    void throw_with_trace(const E& e) {
        throw boost::enable_error_info(e)
            << util::traced(boost::stacktrace::stacktrace());
    }
}
