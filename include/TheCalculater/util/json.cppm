/**
 * @file json.cppm
 * @author prbegd
 * @date 2025-12-29
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include "TheCalculater/macros.hpp"

export module TheCalculater.util.json;
export import tpmm.jsoncpp;
import std;

namespace TheCalculater::util {
    export THECALCULATER_DEFINE_EXCEPTION(InvalidJsonException, std::logic_error);

    /**
     * @brief Parse JSON5 string into a Json::Value object.
     *
     * @param json5String The JSON5 string to parse.
     * @param error (output) The error message if parsing fails.
     * @return Json::Value Parsed JSON5 string as a Json::Value object.
     * @throw TheCalculater::util::InvalidJsonException If the JSON5 string is invalid.
     */
    export THECALC_API Json::Value parse(std::string_view json5String);

    /**
     * @brief Serialize a Json::Value object into a JSON string.
     *
     * @param value The Json::Value object to serialize.
     * @return std::string The serialized JSON string.
     */
    export THECALC_API std::string serializeJson(const Json::Value& value);
    /**
     * @brief Serialize a Json::Value object into a JSON5 string.
     *
     * @param value The Json::Value object to serialize.
     * @return std::string The serialized JSON5 string.
     */
    export THECALC_API std::string serializeJson5(const Json::Value& value);
}