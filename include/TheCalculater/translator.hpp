/**
 * @file translator.hpp
 * @author prbegd
 * @brief Provides language switching functionality.
 * @date 2025-07-17
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#pragma once
#include <QLocale>

namespace TheCalculater::translator {
    /**
     * @brief Switches the language of Qt application. If no language is specified,
     *        it defaults to the system's locale.
     * @note If the language is not found, it uses English as a fallback.
     * @details This function is thread-safe.
     * @param language The language to switch to. Example: en_US.
     */
    void switchLanguage(std::string_view language = QLocale().name().toStdString());

    /**
     * @brief Checks if the specified language is valid.
     * 
     * @param language The language to check. Example: en_US.
     * @return true If the language is valid.
     * @return false If the language is not valid.
     */
    bool validLanguage(std::string_view language);
}