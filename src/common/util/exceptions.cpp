/**
 * @file exceptions.cpp
 * @author prbegd
 * @date 2026-03-28
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module TheCalculater.util.exceptions;
import TheCalculater.throwEx;
import tpmm.boost;
import std;

namespace TheCalculater::util {
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
} // namespace TheCalculater::util