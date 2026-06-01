/**
 * @file -.cpp
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

module TheCalculater.translator;
import TheCalculater.util;
import tpmm.jsoncpp;
import tpmm.spdlog;
import std;

namespace TheCalculater::translator {
    namespace {
        util::atomic_shared_ptr<std::string> currentLanguagePtr(nullptr);

        using LanguageDataType = std::unordered_map<std::string, std::string,
                                                    util::TransparentHash<std::string_view>, std::equal_to<>>;

        using TranslationDataType = std::unordered_map<std::string, LanguageDataType,
                                                       util::TransparentHash<std::string_view>, std::equal_to<>>;

        TranslationDataType translationData;
        std::mutex translationDataMutex;

        /// Get translation in a specific language.
        std::optional<std::string> trLanguage(std::string_view key, std::string_view language)
        {
            std::lock_guard<std::mutex> lock(translationDataMutex);
            auto languageIt = translationData.find(language);
            if (languageIt == translationData.end())
                return { };
            auto keyIt = languageIt->second.find(key);
            if (keyIt == languageIt->second.end())
                return { };
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
            if (!newLanguageData.empty())
                newTranslationData.emplace(languageName, std::move(newLanguageData));
        }
        if (newTranslationData.empty())
            return false;
        std::size_t loadedLanguages = newTranslationData.size();
        {
            std::lock_guard<std::mutex> lock(translationDataMutex);
            for (auto& [lang, data] : newTranslationData) {
                auto& target = translationData[lang];
                for (auto& [key, value] : data)
                    target[key] = std::move(value);
            }
        }
        spdlog::info("Loaded {} translation(s) for {} language(s).", loadedTranslations, loadedLanguages);
        return true;
    }
} // namespace TheCalculater::translator
