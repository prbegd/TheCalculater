/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;

module prbegd.thecalculater.translator;
import prbegd.thecalculater.util;
import thirdparty.core;
import std;

namespace thecalculater::translator {
namespace {
    util::atomic_shared_ptr<std::string> currentLanguagePtr(nullptr);

    using LanguageDataType = std::unordered_map<std::string, std::string, util::TransparentHash<std::string_view>, std::equal_to<>>;

    using TranslationDataType = std::unordered_map<std::string, LanguageDataType, util::TransparentHash<std::string_view>, std::equal_to<>>;

    TranslationDataType translationData;
    std::mutex translationDataMutex;

    /// Get translation in a specific language.
    std::optional<std::string> trLanguage(std::string_view key, std::string_view language)
    {
        std::lock_guard<std::mutex> lock(translationDataMutex);
        auto languageIt = translationData.find(language);
        if (languageIt == translationData.end()) {
            return { };
        }
        auto keyIt = languageIt->second.find(key);
        if (keyIt == languageIt->second.end()) {
            return { };
        }
        return keyIt->second;
    }
} // namespace

std::string tr(std::string_view key)
{
    return trLanguage(key, *currentLanguagePtr.load(std::memory_order_acquire))
        .value_or(trLanguage(key, "en")
                      .value_or(std::string(key)));
}

void switchLanguage(std::string_view language)
{
    auto newLanguage = std::make_shared<std::string>(language);
    currentLanguagePtr.store(newLanguage, std::memory_order_release);
    spdlog::info("Switched language to {}({}).", tr("TheCalculater.language"), language);
}
bool loadTranslations(const Json::Value& translations)
{
    if (!translations.isObject()) {
        spdlog::warn("Invalid translations data: Not a JSON object.");
        return false;
    }
    std::size_t loadedTranslations = 0;
    TranslationDataType newTranslationData;
    for (const auto& languageName : translations.getMemberNames()) {
        const auto& language = translations[languageName];
        if (!language.isObject()) {
            spdlog::warn("Invalid translations data: Language '{}' is not a JSON object.", languageName);
            continue;
        }
        LanguageDataType newLanguageData;
        for (const auto& key : language.getMemberNames()) {
            const auto& value = language[key];
            if (!value.isString()) {
                spdlog::warn("Invalid translations data: Translation for key '{}' in language '{}' is not a string.", key, languageName);
                continue;
            }
            newLanguageData.emplace(key, value.asString());
            loadedTranslations++;
        }
        if (!newLanguageData.empty()) {
            newTranslationData.emplace(languageName, std::move(newLanguageData));
        }
    }
    if (newTranslationData.empty()) {
        return false;
    }
    std::size_t loadedLanguages = newTranslationData.size();
    {
        std::lock_guard<std::mutex> lock(translationDataMutex);
        for (auto& [lang, data] : newTranslationData) {
            auto& target = translationData[lang];
            for (auto& [key, value] : data) {
                target[key] = std::move(value);
            }
        }
    }
    spdlog::info("Loaded {} translation(s) for {} language(s).", loadedTranslations, loadedLanguages);
    return true;
}
} // namespace thecalculater::translator
