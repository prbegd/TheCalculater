/**
 * @file -.cppm
 * @author prbegd
 * @date 2026-03-15
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include "TheCalculater/macros.hpp"

export module TheCalculater.translator;
import tpmm.jsoncpp;
import std;

namespace TheCalculater::translator {
    /**
     * @brief Translates a given Translation Key to its corresponding text in the current language.
     * @note This function is thread-safe.
     * @note If the key is not found in the current language, it falls back to English. If not found in English, it returns the key itself.
     *
     * @param key The Translation Key.
     * @return std::string The translated text.
     */
    export TCAPI std::string tr(std::string_view key);

    /**
     * @brief Switches the language of the application.
     * @note This function is thread-safe.
     * @param language The language to switch to. e.g. en_US.
     */
    export TCAPI void switchLanguage(std::string_view language);

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
    export TCAPI bool loadTranslations(const Json::Value& translations);
}

namespace TheCalculater {
    /**
     * @brief An alias for TheCalculater::translator::tr function.
     *
     * @see TheCalculater::translator::tr(std::string_view)
     */
    export inline std::string tr(std::string_view key) { return translator::tr(key); }
}
