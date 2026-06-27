/**
 * @file json.cpp
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

module prbegd.thecalculater.util.json;
import prbegd.thecalculater.util.io;
import prbegd.thecalculater.throwEx;
import thirdparty.core;
import std;

namespace thecalculater::util {
    Json::Value parse(std::string_view json5String)
    {
        IStringViewStream iss(json5String);
        Json::Value result;
        std::string error;
        Json5::parse(iss, result, &error);
        if (!error.empty()) {
            throwEx(InvalidJsonException(std::format("Failed to parse JSON5 string: {}", error)));
        }
        return result;
    }

    std::string serializeJson(const Json::Value& value)
    {
        std::ostringstream oss;
        Json5::serialize(oss, value, { false, false, "    " });
        return oss.str();
    }
    std::string serializeJson5(const Json::Value& value)
    {
        std::ostringstream oss;
        Json5::serialize(oss, value, { true, true, "    " });
        return oss.str();
    }
} // namespace thecalculater::util