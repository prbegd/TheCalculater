/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module prbegd.thecalculater.util;
import thirdparty.core;
import std;

namespace thecalculater::util {
    void printStacktrace(std::ostream& os, const boost::stacktrace::stacktrace& stk)
    {
        for (std::size_t i = 0; i < stk.size(); i++) {
            if (stk[i].empty()) {
                continue;
            }
            os << "  #" << i + 1 << ' ' << stk[i].name();
            if (stk[i].source_line() != 0) {
                os << " at " << stk[i].source_file() << ':' << stk[i].source_line();
            } else {
                boost::stacktrace::detail::location_from_symbol loc(stk[i].address());
                if (!loc.empty()) {
                    os << " in " << loc.name();
                }
            }
            os << " (" << stk[i].address() << ')';
            if (i < stk.size() - 1) {
                os << '\n';
            }
        }
    }
    void printException(std::ostream& os, const std::exception& e)
    {
        const auto getExceptionTypename = [](const std::exception& e) {
            const char* mangledTypename = nullptr;
            if (const std::type_index* originalType = boost::get_error_info<util::exceptiontype_info>(e); originalType) {
                mangledTypename = originalType->name();
            } else {
                mangledTypename = typeid(e).name();
            }
            return boost::core::demangle(mangledTypename);
        };

        os << getExceptionTypename(e);
        if (std::string_view what(e.what()); !what.empty()) {
            os << ": " << what;
        }
        if (const boost::stacktrace::stacktrace* trace = boost::get_error_info<util::stacktrace_info>(e); trace) {
            os << '\n';
            printStacktrace(os, *trace);
        }
        if (const std::exception_ptr* cause = boost::get_error_info<util::cause_info>(e); cause) {
            try {
                std::rethrow_exception(*cause);
            } catch (const std::exception& causeException) {
                os << "\nCaused By: ";
                printException(os, causeException);
            }
        }
    }
} // namespace thecalculater::util