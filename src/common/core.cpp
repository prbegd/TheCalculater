/**
 * @file core.cpp
 * @author prbegd
 * @brief Core functionality for TheCalculater.
 * @date 2025-07-31
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#include "TheCalculater/core.hpp"
#include "spdlog/spdlog.h"

namespace TheCalculater::core {
    void registerLogger(const std::shared_ptr<spdlog::logger>& logger)
    {
        spdlog::register_logger(logger);
        spdlog::set_default_logger(logger);
    }
}