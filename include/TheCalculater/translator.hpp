/**
 * @file translator.hpp
 * @author prbegd
 * @brief Provides internationalization functionality.
 * @date 2025-07-17
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#pragma once
#include "core.hpp"
#include <QLocale>
#include <string_view>

namespace Json {
    class Value;
}

namespace TheCalculater::translator {
    /**
     * @brief Translates a given Translation Key to its corresponding text in the current language.
     * @note This function is thread-safe.
     * @note If the key is not found in the current language, it falls back to English. If not found in English, it returns the key itself.
     * 
     * @param key The Translation Key.
     * @return std::string The translated text.
     */
    THECALC_API std::string tr(std::string_view key);

    /**
     * @brief Switches the language of Qt application. If no language is specified,
     *        it defaults to the system's locale.
     * @note This function is thread-safe.
     * @param language The language to switch to. Example: en_US.
     */
    THECALC_API void switchLanguage(std::string_view language = QLocale().name().toStdString());

    /**
     * @brief Loads translations data from a JSON object.
     * @details The JSON value passed in should be a JSON object, its keys are the languages code, and its values are mappings (objects) of Translation Keys to texts.
     * @note This function should be called when initializing.
     * @note This function is thread-safe.
     * 
     * @param translations The JSON object containing translations data.
     * @return true If at least one language is loaded.
     * @return false If no language is loaded.
     */
    THECALC_API bool loadTranslations(const Json::Value& translations);
}

namespace TheCalculater {
    /**
     * @brief An alias for TheCalculater::translator::tr function.
     *
     * @see TheCalculater::translator::tr(std::string_view)
     */
    inline std::string tr(std::string_view key) { return translator::tr(key); }
}