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

export module TheCalculater.util.exceptions;
import tpmm.boost;
import std;

namespace TheCalculater::util {
    export THECALCULATER_DEFINE_EXCEPTION(UnexpectedException, std::logic_error);
    export THECALCULATER_DEFINE_EXCEPTION(IOException, std::runtime_error);
    export THECALCULATER_DEFINE_EXCEPTION(FileNotFoundException, IOException);
    export THECALCULATER_DEFINE_EXCEPTION(WeakPointerExpiredException, std::runtime_error);

    /**
     * @brief Format a stacktrace into a string.
     *
     * @param stk The stacktrace to format.
     * @return std::string A string representation of the stacktrace.
     */
    export std::string formatStacktrace(const boost::stacktrace::stacktrace& stk = boost::stacktrace::stacktrace(1, -1));

    /**
     * @brief Format an exception into a string.
     *
     * @param e The exception to format.
     * @return std::string A string representation of the exception.
     */
    export THECALC_API std::string formatException(const std::exception& e);
} // namespace TheCalculater::util