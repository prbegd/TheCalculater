/**
 * @file util.hpp
 * @author prbegd
 * @brief Core utility functions and types.
 * @date 2025-06-21
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#pragma once
#include "boost/stacktrace/stacktrace.hpp"
#include "core.hpp"
#include <boost/exception/all.hpp>
#include <boost/stacktrace.hpp>
#include <exception>
#include <string_view>
#include <type_traits>
#include <utility>

class QByteArray;
namespace Json {
    class Value;
}

namespace TheCalculater {
    namespace util {
        struct ThrowExData {
            boost::stacktrace::stacktrace trace;
            std::exception_ptr cause;

            ThrowExData(boost::stacktrace::stacktrace trace, std::exception_ptr cause)
                : trace(std::move(trace)), cause(std::move(cause))
            { }
            ThrowExData(boost::stacktrace::stacktrace trace)
                : trace(std::move(trace)), cause(nullptr)
            { }
        };
        using ThrowExDataErrorInfo = boost::error_info<struct tag_throw_ex_data, ThrowExData>;

        /**
         * @brief Parse JSON5 string into a Json::Value object.
         *
         * @param json5String The JSON5 string to parse.
         * @param error (output) The error message if parsing fails.
         * @return Json::Value Parsed JSON5 string as a Json::Value object. If parsing fails, an empty value is returned and the error message is stored in 'error'.
         */
        THECALC_API Json::Value parse(std::string_view json5String, std::string& error);
        /**
         * @brief Parse JSON5 string into a Json::Value object.
         *
         * @param json5String The JSON5 string to parse.
         * @param errorHandleType How to handle errors during parsing.
         * @throw std::invalid_argument If the JSON5 string is invalid and errorHandleType is set to ThrowException.
         * @return Json::Value Parsed JSON5 string as a Json::Value object. If parsing fails, an empty value is returned.
         */
        THECALC_API Json::Value parse(std::string_view json5String, core::ErrorHandleType errorHandleType = core::ErrorHandleType::Ignore);

        /**
         * @brief Serialize a Json::Value object into a JSON string.
         *
         * @param value The Json::Value object to serialize.
         * @return std::string The serialized JSON string.
         */
        THECALC_API std::string serialize(const Json::Value& value);
        /**
         * @brief Serialize a Json::Value object into a JSON5 string.
         *
         * @param value The Json::Value object to serialize.
         * @return std::string The serialized JSON5 string.
         */
        THECALC_API std::string serialize5(const Json::Value& value);

        /**
         * @brief Read all text from a resources file.
         *
         * @param fileName Qt resource file path. e.g., ":/resources/icon.ico"
         * @throw TheCalculater::core::IOException If the file cannot be opened or read.
         * @throw TheCalculater::core::FileNotFoundException If the file does not exist.
         * @deprecated Use readResourcesFile() instead.
         * @return std::string The content of the file as a string.
         */
        [[deprecated("Use readResourcesFile() instead")]] THECALC_API std::string readResourcesFileAllText(const std::string_view& fileName);

        /**
         * @brief Read data from a resources file.
         *
         * Use std::string_view to read content (instead of toStdString()) avoid unnecessary copying.
         *
         * @param fileName
         * @return THECALC_API
         */
        THECALC_API QByteArray readResourcesFile(const std::string_view& fileName);

        /**
         * @brief Format a stacktrace into a string.
         *
         * @tparam SkipFirstFrame Whether to skip the first frame in the stacktrace.
         * @param stk The stacktrace to format.
         * @return std::string A string representation of the stacktrace.
         */
        template <bool SkipFirstFrame = true>
        std::string formatStacktrace(const boost::stacktrace::stacktrace& stk = boost::stacktrace::stacktrace {})
        {
            std::ostringstream oss;
            for (size_t i = SkipFirstFrame ? 1 : 0; i < stk.size(); i++) {
                if (stk[i].empty())
                    continue;
                oss << "  #" << i << ' ' << stk[i].name();
                if (stk[i].source_line() != 0) {
                    oss << " at " << stk[i].source_file() << ':' << stk[i].source_line();
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
        THECALC_API std::string formatException(const std::exception& e);

        /**
         * @brief Unary predicate to check if a value is equal to an expected value.
         *
         * @tparam T The type of the value to check.
         */
        template <std::equality_comparable T>
        struct Expect {
            T expected;

            constexpr bool operator()(const T& other) const noexcept(noexcept(expected == other)) { return expected == other; }
        };
    } // namespace util
    template <typename E>
    void throwEx(const E& e)
        requires(std::is_base_of_v<std::exception, E>)
    {
        throw boost::enable_error_info(e)
            << util::ThrowExDataErrorInfo(util::ThrowExData(boost::stacktrace::stacktrace()));
    }
    template <typename E>
    void throwEx(const E& e, const std::exception_ptr& cause)
        requires(std::is_base_of_v<std::exception, E>)
    {
        if (!cause)
            throw boost::enable_error_info(e)
                << util::ThrowExDataErrorInfo(util::ThrowExData(boost::stacktrace::stacktrace(), std::current_exception()));
        throw boost::enable_error_info(e)
            << util::ThrowExDataErrorInfo(util::ThrowExData(boost::stacktrace::stacktrace(), cause));
    }
} // namespace TheCalculater