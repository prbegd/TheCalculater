/**
 * @file resources.cppm
 * @author prbegd
 * @date 2026-03-29
 * 
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to 
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include "TheCalculater/macros.hpp"
#include <QByteArray>

export module TheCalculaterQtBridge.resources;

namespace TheCalculaterQtBridge {
    /**
     * @brief Read data from a resources file.
     *
     * Use std::string_view to read content (instead of toStdString()) avoid unnecessary copying.
     *
     * @param fileName The name of the resource file to read.
     * @return QByteArray containing the uncompressed data of the resource file.
     */
    export THECALC_API QByteArray readResourcesFile(const std::string_view& fileName);
} // namespace TheCalculaterQtBridge