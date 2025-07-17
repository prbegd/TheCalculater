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
#include "spdlog/spdlog.h"
#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTranslator>

namespace TheCalculater::translator {
    static QTranslator* s_translator = nullptr;
    void switchLanguage(const QString& language)
    {
        if (s_translator != nullptr) {
            qApp->removeTranslator(s_translator);
            delete s_translator;
            s_translator = nullptr;
        }

        s_translator = new QTranslator;
        if (s_translator->load(getLanguageQmPath(language))) {
            qApp->installTranslator(s_translator);
            SPDLOG_INFO("Switched language to {}", language.toStdString());
        } else
            SPDLOG_ERROR("Unable to load language file for {}", language.toStdString());
    }
    QString getLanguageQmPath(const QString& language)
    {
        QFile supportedLanguagesFile(":/resources/data/supportedLanguages.json");
        if (!supportedLanguagesFile.open(QIODevice::ReadOnly)) {
            SPDLOG_ERROR("Failed to open supportedLanguages.json file.");
            return "";
        }
        QJsonObject doc = QJsonDocument::fromJson(supportedLanguagesFile.readAll()).object();
        if (doc.contains(language)) {
            return doc[language].toObject()["file"].toString();
        }
        return doc["en_US"].toObject()["file"].toString();
    }
} // namespace TheCalculater::translator
