/**
 * @file core.cpp
 * @author prbegd
 * @brief Core functionality for TheCalculater.
 * @date 2025-07-31
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to 
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#include "TheCalculater/core.hpp"

namespace TheCalculater::core {
    void registerLogger(const std::shared_ptr<spdlog::logger>& logger)
    {
        spdlog::register_logger(logger);
        spdlog::set_default_logger(logger);
    }
}