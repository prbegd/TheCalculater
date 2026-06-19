/**
 * @file -.cppm
 * @author prbegd
 * @date 2026-03-15
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;
#include "thecalculater/macros.hpp"

export module thecalculater.translator;
import tpmm.jsoncpp;
import std;

namespace thecalculater::translator {
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

namespace thecalculater {
    /**
     * @brief An alias for thecalculater::translator::tr function.
     *
     * @see thecalculater::translator::tr(std::string_view)
     */
    export inline std::string tr(std::string_view key) { return translator::tr(key); }
}
