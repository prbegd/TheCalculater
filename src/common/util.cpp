/**
 * @file util.cpp
 * @author prbegd
 * @brief Core utility functions and types.
 * @date 2025-07-28
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to 
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#include "TheCalculater/util.hpp"
#include "TheCalculater/core.hpp"
#include "json5cpp/json5cpp.h"
#include <QFile>
#include <qresource.h>
#include <sstream>
#include <stdexcept>

namespace TheCalculater::util {
    THECALC_API Json::Value parse(std::string_view json5String, std::string& error)
    {
        core::IStringViewStream iss(json5String);
        Json::Value result;
        Json5::parse(iss, result, &error);
        return result;
    }
    Json::Value parse(std::string_view json5String, core::ErrorHandleType errorHandleType)
    {
        std::string error;
        Json::Value result = parse(json5String, error);

        if (!error.empty()) {
            if (errorHandleType == core::ErrorHandleType::Ignore) {
            } else if (errorHandleType == core::ErrorHandleType::ThrowException) {
                throw_with_trace(std::invalid_argument(std::format("Error parsing JSON5: {}", error)));
            } else {
                std::string jsonPart = std::move(json5String.size() <= 50 ? std::string(json5String) : std::string(json5String.substr(0, 50)) + "...");
                if (errorHandleType == core::ErrorHandleType::LogError) {
                    SPDLOG_ERROR("Error parsing JSON5: {}\nFirst 50 chars: {}", error, jsonPart);
                } else {
                    SPDLOG_WARN("Error parsing JSON5: {}\nFirst 50 chars: {}", error, jsonPart);
                }
            }
        }
        return result;
    }

    std::string serialize(const Json::Value& value)
    {
        std::ostringstream oss;
        Json5::serialize(oss, value, { false, false, "    " });
        return oss.str();
    }
    std::string serialize5(const Json::Value& value)
    {
        std::ostringstream oss;
        Json5::serialize(oss, value, { true, true, "    " });
        return oss.str();
    }

    std::string readResourcesFileAllText(const std::string_view& fileName)
    {
        QFile file(fileName.data());
        if (!file.exists())
            throw_with_trace(core::FileNotFoundException(std::format("File not found: {}", fileName)));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            throw_with_trace(core::IOException(std::format("Failed to open file: {}", fileName)));

        return file.readAll().toStdString();
    }

    QByteArray readResourcesFile(const std::string_view& fileName)
    {
        return QResource(fileName.data()).uncompressedData();
    }
} // namespace TheCalculater::util