/**
 * @file libTheCalculaterCommon.cppm
 * @author prbegd
 * @brief Core header file for TheCalculater.
 * @date 2025-07-28
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 *
 */
module;
#include "thecalculater/macros.hpp"

export module thecalculater.libTheCalculaterCommon;
import tpmm.spdlog;
import std;

namespace thecalculater {
    export TCAPI void registerLogger(const std::shared_ptr<spdlog::logger>& logger);
} // namespace thecalculater