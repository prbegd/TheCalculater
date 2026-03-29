/**
 * @file TRASH_QT_BRIDGE.cpp
 * @author prbegd
 * @date 2026-03-28
 * 
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to 
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include <QResource>

module TheCalculaterQtBridge.resources;

namespace TheCalculaterQtBridge {
    QByteArray readResourcesFile(const std::string_view& fileName)
    {
        return QResource(fileName.data()).uncompressedData();
    }
} // namespace TheCalculater::util
