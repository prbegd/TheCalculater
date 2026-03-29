/**
 * @file libTheCalculaterCommon.cppm
 * @author prbegd
 * @brief Core header file for TheCalculater.
 * @date 2025-07-28
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
module;
#include "spdlog/spdlog.h"
#include "TheCalculater/macros.hpp"

export module TheCalculater.libTheCalculaterCommon;

namespace TheCalculater {
    export THECALC_API void registerLogger(const std::shared_ptr<spdlog::logger>& logger);
} // namespace TheCalculater::core