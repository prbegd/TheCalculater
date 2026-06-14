/**
 * @file resources.cppm
 * @author prbegd
 * @date 2026-03-29
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;
#include "TheCalculater/macros.hpp"

export module TheCalculaterQtBridge.resources;
import TheCalculaterQtBridge.qtmock;
import std;

namespace TheCalculaterQtBridge {
    /**
     * @brief Read data from a resources file.
     *
     * Use std::string_view to read content (instead of toStdString()) avoid unnecessary copying.
     *
     * @param fileName The name of the resource file to read.
     * @return QByteArray containing the uncompressed data of the resource file.
     */
    export TCQTAPI QByteArray readResourcesFile(const std::string_view& fileName);
} // namespace TheCalculaterQtBridge