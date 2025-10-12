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
                throwEx(std::invalid_argument(std::format("Error parsing JSON5: {}", error)));
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
            throwEx(core::FileNotFoundException(std::format("File not found: {}", fileName)));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            throwEx(core::IOException(std::format("Failed to open file: {}", fileName)));

        return file.readAll().toStdString();
    }

    QByteArray readResourcesFile(const std::string_view& fileName)
    {
        return QResource(fileName.data()).uncompressedData();
    }

    std::string formatException(const std::exception& e)
    {
        std::ostringstream oss;
        std::string type = boost::core::demangle(typeid(e).name());
        const ThrowExData* exData = boost::get_error_info<util::ThrowExDataErrorInfo>(e);
        if (exData) {
            // the template parmenter of e is the actual (unpacked) type
            size_t templateStart = type.find_first_of('<');
            size_t templateEnd = type.find_last_of('>');
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