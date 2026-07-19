/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;
#include "thecalculater/macros.hpp"

export module prbegd.thecalculater.util:exceptions;
import thirdparty.core;
import std;

namespace thecalculater::util {
export using exceptiontype_info = boost::error_info<struct tag_exceptiontype_info, std::type_index>;
export using stacktrace_info = boost::error_info<struct tag_stacktrace_info, boost::stacktrace::stacktrace>;
export using cause_info = boost::error_info<struct tag_cause_info, std::exception_ptr>;

export THECALCULATER_DEFINE_EXCEPTION(UnexpectedException, std::logic_error);
export THECALCULATER_DEFINE_EXCEPTION(IOException, std::runtime_error);
export THECALCULATER_DEFINE_EXCEPTION(FileNotFoundException, IOException);
export THECALCULATER_DEFINE_EXCEPTION(WeakPointerExpiredException, std::runtime_error);

export template <std::derived_from<boost::exception> E>
E enableErrorInfo(const E& exception)
{
    return exception;
}
export template <typename E>
    requires(!std::derived_from<E, boost::exception>)
auto enableErrorInfo(const E& exception) -> decltype(boost::enable_error_info(exception))
{
    return boost::enable_error_info(exception)
        << exceptiontype_info(typeid(exception));
}

export TCAPI void printStacktrace(std::ostream& os, const boost::stacktrace::stacktrace& stk = boost::stacktrace::stacktrace(1, -1));

export TCAPI void printException(std::ostream& os, const std::exception& e);

} // namespace thecalculater::util
namespace thecalculater {
export template <typename T>
[[noreturn]]
void throwext(const T& exception, const std::optional<boost::stacktrace::stacktrace>& trace = boost::stacktrace::stacktrace(1, -1))
{
    auto exceptionExtended = util::enableErrorInfo(exception);
    if (trace) {
        exceptionExtended << util::stacktrace_info(*trace);
    }
    throw std::move(exceptionExtended);
}
export template <typename T>
[[noreturn]]
void throwext_caused(const T& exception, const std::optional<boost::stacktrace::stacktrace>& trace = boost::stacktrace::stacktrace(1, -1), std::exception_ptr cause = std::current_exception())
{
    auto exceptionExtended = util::enableErrorInfo(exception);
    if (trace) {
        exceptionExtended << util::stacktrace_info(*trace);
    }
    if (cause) {
        exceptionExtended << util::cause_info(cause);
    }
    throw std::move(exceptionExtended);
}
} // namespace thecalculater
namespace thecalculater::util {
export class TCAPI UninitializedUsageException : public std::runtime_error, public boost::exception {
public:
    explicit UninitializedUsageException(const std::string&);
    explicit UninitializedUsageException(const char*);
};
}