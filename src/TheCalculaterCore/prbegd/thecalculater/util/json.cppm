/**
 * @file json.cppm
 * @author prbegd
 * @date 2025-12-29
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

export module prbegd.thecalculater.util.json;
import thirdparty.core;
import std;

namespace thecalculater::util {
    export THECALCULATER_DEFINE_EXCEPTION(InvalidJsonException, std::logic_error);

    /**
     * @brief Parse JSON5 string into a Json::Value object.
     *
     * @param json5String The JSON5 string to parse.
     * @param error (output) The error message if parsing fails.
     * @return Json::Value Parsed JSON5 string as a Json::Value object.
     * @throw thecalculater::util::InvalidJsonException If the JSON5 string is invalid.
     */
    export TCAPI Json::Value parse(std::string_view json5String);

    /**
     * @brief Serialize a Json::Value object into a JSON string.
     *
     * @param value The Json::Value object to serialize.
     * @return std::string The serialized JSON string.
     */
    export TCAPI std::string serializeJson(const Json::Value& value);
    /**
     * @brief Serialize a Json::Value object into a JSON5 string.
     *
     * @param value The Json::Value object to serialize.
     * @return std::string The serialized JSON5 string.
     */
    export TCAPI std::string serializeJson5(const Json::Value& value);
}