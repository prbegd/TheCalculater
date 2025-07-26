/**
 * @file translator.cpp
 * @author prbegd
 * @brief Provides language switching functionality.
 * @date 2025-07-17
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#include "TheCalculater/translator.hpp"
#include <mutex>

namespace TheCalculater::translator {
    namespace {
        std::string currentLanguage;
        std::mutex currentLanguageMutex;
    }
    void switchLanguage(std::string_view language)
    {
        // dumb fallback (But at least it won't copy the string)
        bool useEnglish = false;
        if (!validLanguage(language)) 
            useEnglish = true;
            
        SPDLOG_DEBUG("Locking mutex for language switch.");
        std::lock_guard<std::mutex> lock(currentLanguageMutex);
        if (useEnglish) 
            currentLanguage = "en_US";
        else
            currentLanguage = language;
    }
} // namespace TheCalculater::translator
