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
#include "core.hpp"
#include <boost/exception/all.hpp>
#include <boost/stacktrace.hpp>
#include <string_view>

class QByteArray;
namespace Json {
    class Value;
}

namespace TheCalculater {
    namespace util {
        typedef boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace> traced;

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
    } // namespace util
    template <class E>
    void throw_with_trace(const E& e)
    {
        throw boost::enable_error_info(e)
            << util::traced(boost::stacktrace::stacktrace());
    }
} // namespace TheCalculater