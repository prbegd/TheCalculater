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

namespace TheCalculater::core {
    enum class ErrorHandleType {
        Ignore,
        ThrowException,
        LogError,
        LogWarn
    };
}