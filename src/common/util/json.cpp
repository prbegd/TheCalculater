/**
 * @file json.cpp
 * @author prbegd
 * @date 2025-12-29
 * 
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to 
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
#include "TheCalculater/util/json.hpp"
#include "json5cpp/json5cpp.h"
#include "TheCalculater/util.hpp"

namespace TheCalculater::util {
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
}