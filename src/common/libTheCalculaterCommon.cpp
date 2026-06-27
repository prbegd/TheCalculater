/**
 * @file libTheCalculaterCommon.cpp
 * @author prbegd
 * @date 2026-03-28
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module thecalculater.libTheCalculaterCommon;
import thirdparty.core;
import std;

namespace thecalculater {
    void registerLogger(const std::shared_ptr<spdlog::logger>& logger)
    {
        // logger registered in main program weirdly synced to here (in linux ci build)
        // Just for safe, we check if the logger is already registered.
        if (spdlog::get(logger->name()))
            return;
        spdlog::register_logger(logger);
        spdlog::set_default_logger(logger);
    }
} // namespace thecalculater