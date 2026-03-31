/**
 * @file exceptions.cppm
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
#include "TheCalculater/macros.hpp"
#include <boost/stacktrace/stacktrace.hpp>

export module TheCalculater.util.exceptions;
import std.compat;

namespace TheCalculater::util {
    export THECALCULATER_DEFINE_EXCEPTION(UnexpectedException, std::logic_error);
    export THECALCULATER_DEFINE_EXCEPTION(IOException, std::runtime_error);
    export THECALCULATER_DEFINE_EXCEPTION(FileNotFoundException, IOException);
    export THECALCULATER_DEFINE_EXCEPTION(WeakPointerExpiredException, std::runtime_error);

    /**
     * @brief Format a stacktrace into a string.
     *
     * @tparam SkipFirstFrame Whether to skip the first frame in the stacktrace.
     * @param stk The stacktrace to format.
     * @return std::string A string representation of the stacktrace.
     */
    export template <bool SkipFirstFrame = true>
    std::string formatStacktrace(const boost::stacktrace::stacktrace& stk = boost::stacktrace::stacktrace { })
    {
        std::ostringstream oss;
        for (std::size_t i = SkipFirstFrame ? 1 : 0; i < stk.size(); i++) {
            if (stk[i].empty())
                continue;
            oss << "  #" << i << ' ' << stk[i].name();
            if (stk[i].source_line() != 0) {
                oss << " at " << stk[i].source_file() << ':' << stk[i].source_line();
            } else {
                boost::stacktrace::detail::location_from_symbol loc(stk[i].address());
                if (!loc.empty())
                    oss << " in " << loc.name();
            }
            oss << " (" << stk[i].address() << ')';
            if (i < stk.size() - 1)
                oss << '\n';
        }
        return oss.str();
    }
    /**
     * @brief Format an exception into a string.
     *
     * @param e The exception to format.
     * @return std::string A string representation of the exception.
     */
    export THECALC_API std::string formatException(const std::exception& e);
} // namespace TheCalculater::util