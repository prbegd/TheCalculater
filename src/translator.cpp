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
#include <json/json.h>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>

namespace TheCalculater::translator {
    namespace {
        std::string currentLanguage;
        std::mutex currentLanguageMutex;

        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> translationData;
        std::mutex translationDataMutex;

        std::string trLanguage(std::string_view key, std::string_view language)
        {
            std::lock_guard<std::mutex> lock(translationDataMutex);
            auto languageIt = translationData.find(std::string(language));
            if (languageIt == translationData.end()) 
                return {};
            auto keyIt = languageIt->second.find(std::string(key));
            if (keyIt == languageIt->second.end())
                return {};
            return keyIt->second;
        }
    }

    std::string tr(std::string_view key)
    {
        std::lock_guard<std::mutex> lock(currentLanguageMutex);
        std::string translation = trLanguage(key, currentLanguage);
        if (translation.empty()) {
            translation = trLanguage(key, "en_US");
            if (translation.empty()) 
                translation = key;
        }
        return translation;
    }

    void switchLanguage(std::string_view language)
    {
        SPDLOG_DEBUG("Locking mutex...");
        std::lock_guard<std::mutex> lock(currentLanguageMutex);
        SPDLOG_DEBUG("Mutex locked.");
        currentLanguage = language;
    }
    bool loadTranslations(const Json::Value& translations)
    {
        if (!translations.isObject()) {
            SPDLOG_WARN("Invalid translations data: Not a JSON object.");
            return false;
        }
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> currentTranslationData;
        for (const auto& languageName : translations.getMemberNames()) {
            const auto& language = translations[languageName];
            if (!language.isObject()) {
                SPDLOG_WARN("Invalid translations data: Language '{}' is not a JSON object.", languageName);
                continue;
            }
            std::unordered_map<std::string, std::string> currentLanguageData;
            for (const auto& key : language.getMemberNames()) {
                const auto& value = language[key];
                if (!value.isString()) {
                    SPDLOG_WARN("Invalid translations data: Translation for key '{}' in language '{}' is not a string.", key, languageName);
                    continue;
                }
                currentLanguageData[key] = value.asString();
            }
            if (!currentLanguageData.empty())
                currentTranslationData[languageName] = currentLanguageData;
        }
        if (currentTranslationData.empty())
            return false;
        SPDLOG_DEBUG("Locking mutex...");
        std::lock_guard<std::mutex> lock(translationDataMutex);
        SPDLOG_DEBUG("Mutex locked.");
        translationData.insert(currentTranslationData.begin(), currentTranslationData.end());
        return true;
    }
} // namespace TheCalculater::translator
