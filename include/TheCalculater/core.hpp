/**
 * @file core.hpp
 * @author prbegd
 * @brief Core header file for TheCalculater.
 * @date 2025-07-28
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#pragma once
#include <stdexcept>

#define THECALCULATER_DEFINE_EXCEPTION(name, base) \
    class name : public base {                     \
    public:                                        \
        explicit name(const std::string& message)  \
            : base(message)                        \
        { }                                        \
    }

namespace TheCalculater::core {
    enum class ErrorHandleType {
        Ignore,
        ThrowException,
        LogError,
        LogWarn
    };

    THECALCULATER_DEFINE_EXCEPTION(IOException, std::runtime_error);
    THECALCULATER_DEFINE_EXCEPTION(FileNotFoundException, IOException);
} // namespace TheCalculater::core