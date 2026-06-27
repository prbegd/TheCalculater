/**
 * @file exceptions.cppm
 * @author prbegd
 * @date 2026-03-15
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;
#include "thecalculater/macros.hpp"

export module prbegd.thecalculater.util.exceptions;
import thirdparty.core;
import std;

namespace thecalculater::util {
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
    export TCAPI std::string formatException(const std::exception& e);
} // namespace thecalculater::util