/**
 * @file exceptions.cpp
 * @author prbegd
 * @date 2026-03-28
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module thecalculater.util.exceptions;
import thecalculater.throwEx;
import thirdparty.core;
import std;

namespace thecalculater::util {
    std::string formatStacktrace(const boost::stacktrace::stacktrace& stk)
    {
        std::ostringstream oss;
        for (std::size_t i = 0; i < stk.size(); i++) {
            if (stk[i].empty())
                continue;
            oss << "  #" << i + 1 << ' ' << stk[i].name();
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
    std::string formatException(const std::exception& e)
    {
        std::ostringstream oss;
        std::string type = boost::core::demangle(typeid(e).name());
        const ThrowExData* exData = boost::get_error_info<ThrowExDataErrorInfo>(e);
        if (exData) {
            // the template parmenter of e is the actual (unpacked) type
            std::size_t templateStart = type.find_first_of('<');
            std::size_t templateEnd = type.find_last_of('>');
            if (templateStart != std::string::npos && templateEnd != std::string::npos)
                type = type.substr(templateStart + 1, templateEnd - templateStart - 1);
        }
        oss << type << ": " << e.what();
        if (exData) {
            oss << '\n'
                << formatStacktrace(exData->trace);
            if (exData->cause) {
                try {
                    std::rethrow_exception(exData->cause);
                } catch (const std::exception& eCause) {
                    oss << '\n'
                        << "Caused by: "
                        << formatException(eCause);
                } catch (...) {
                    oss << '\n'
                        << "Caused by: UNKNOWN EXCEPTION";
                }
            }
        }
        return oss.str();
    }
} // namespace thecalculater::util