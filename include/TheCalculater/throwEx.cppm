/**
 * @file throwEx.cppm
 * @author prbegd
 * @date 2026-03-15
 * 
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to 
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
export module TheCalculater.throwEx;
import tpmm.boost;
import std;

namespace TheCalculater {
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
}

