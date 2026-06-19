/**
 * @file throwEx.cppm
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
export module thecalculater.throwEx;
import tpmm.boost;
import std;

namespace thecalculater {
    export struct ThrowExData {
        boost::stacktrace::stacktrace trace;
        std::exception_ptr cause;

        ThrowExData(boost::stacktrace::stacktrace trace, std::exception_ptr cause);
        ThrowExData(boost::stacktrace::stacktrace trace);
    };
    export using ThrowExDataErrorInfo = boost::error_info<struct tag_throw_ex_data, ThrowExData>;

    export template <std::derived_from<std::exception> E>
    [[noreturn]] void throwEx(const E& e)
    {
        throw boost::enable_error_info(e)
            << ThrowExDataErrorInfo(ThrowExData(boost::stacktrace::stacktrace(1, -1)));
    }
    export template <std::derived_from<std::exception> E>
    [[noreturn]] void throwEx(const E& e, const std::exception_ptr& cause)
    {
        if (!cause)
            throw boost::enable_error_info(e)
                << ThrowExDataErrorInfo(ThrowExData(boost::stacktrace::stacktrace(1, -1), std::current_exception()));
        throw boost::enable_error_info(e)
            << ThrowExDataErrorInfo(ThrowExData(boost::stacktrace::stacktrace(1, -1), cause));
    }
} // namespace thecalculater
