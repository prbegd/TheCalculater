/**
 * @file translator.cpp
 * @author prbegd
 * @brief Provides internationalization functionality.
 * @date 2025-07-17
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#include "TheCalculater/translator.hpp"
#include "TheCalculater/core.hpp"
#include "spdlog/spdlog.h"
#include <atomic>
#include <json/json.h>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace TheCalculater::translator {
    namespace {
        std::atomic<std::shared_ptr<std::string>> currentLanguagePtr = nullptr;

        using LanguageDataType = std::unordered_map<std::string, std::string,
            core::Hash<std::string_view>, core::EqualTo<std::string_view>>;

        using TranslationDataType = std::unordered_map<std::string, LanguageDataType,
            core::Hash<std::string_view>, core::EqualTo<std::string_view>>;

        TranslationDataType translationData;
        std::mutex translationDataMutex;

        std::optional<std::string> trLanguage(std::string_view key, std::string_view language)
        {
            std::lock_guard<std::mutex> lock(translationDataMutex);
            auto languageIt = translationData.find(language);
            if (languageIt == translationData.end())
                return {};
            auto keyIt = languageIt->second.find(key);
            if (keyIt == languageIt->second.end())
                return {};
            return keyIt->second;
        }
    } // namespace

    std::string tr(std::string_view key)
    {
        return trLanguage(key, *currentLanguagePtr.load(std::memory_order_acquire))
            .value_or(trLanguage(key, "en_US")
                    .value_or(std::string(key)));
    }

    void switchLanguage(std::string_view language)
    {
        auto newLanguage = std::make_shared<std::string>(language);
        currentLanguagePtr.store(newLanguage, std::memory_order_release);
    }
    bool loadTranslations(const Json::Value& translations)
    {
        if (!translations.isObject()) {
            SPDLOG_WARN("Invalid translations data: Not a JSON object.");
            return false;
        }
        TranslationDataType newTranslationData;
        for (const auto& languageName : translations.getMemberNames()) {
            const auto& language = translations[languageName];
            if (!language.isObject()) {
                SPDLOG_WARN("Invalid translations data: Language '{}' is not a JSON object.", languageName);
                continue;
            }
            LanguageDataType newLanguageData;
            for (const auto& key : language.getMemberNames()) {
                const auto& value = language[key];
                if (!value.isString()) {
                    SPDLOG_WARN("Invalid translations data: Translation for key '{}' in language '{}' is not a string.", key, languageName);
                    continue;
                }
                newLanguageData.emplace(key, value.asString());
            }
            if (!newLanguageData.empty())
                newTranslationData.emplace(languageName, std::move(newLanguageData));
        }
        if (newTranslationData.empty())
            return false;
        {
            SPDLOG_DEBUG("Locking mutex...");
            std::lock_guard<std::mutex> lock(translationDataMutex);
            SPDLOG_DEBUG("Mutex locked.");
            for (auto& [lang, data] : newTranslationData) {
                auto& target = translationData[lang];
                for (auto& [key, value] : data)
                    target[key] = std::move(value);
            }
        }
        return true;
    }
} // namespace TheCalculater::translator
