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
#include <QString>


namespace TheCalculater::translator {
    /**
     * @brief Switches the language of Qt application. If no language is specified,
     *        it defaults to the system's locale.
     * @note If the language is not found, it uses English as a fallback.
     * @warning Only call this function in the main thread.
     *
     * @param language The language to switch to. Example: en_US.
     */
    void switchLanguage(const QString& language = QLocale::system().name());

    /**
     * @brief Get the qm file path for the specified language.
     * @note If the language is not found, it returns English as a fallback.
     *
     * @param language The language to get the qm file path for. Example: en_US.
     * @return QString The path to the qm file for the specified language.
     */
    QString getLanguageQmPath(const QString& language);
}