/**
 * @file TRASH_QT_BRIDGE.cppm
 * @author prbegd
 * @date 2026-03-15
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 * * This file is a piece of trash.
 * It's only for temporary and is going to be removed soon.
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
     * @param fileName
     * @return THECALC_API
     */
    export THECALC_API QByteArray readResourcesFile(const std::string_view& fileName);
} // namespace TheCalculater::qtbridge
