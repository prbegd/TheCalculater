/**
 * @file main.cpp
 * @author prbegd
 * @brief Crash Handler Program Entry Point.
 * @date 2025-07-11
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#include "TheCalculater/translator.hpp"
#include "TheCalculater/util.hpp"
#include "crash_handler_dialog.hpp"
#include "json/value.h"
#include <QApplication>
#include <QResource>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    if (argc < 2)
        return 2;
    QString crashReportFile = argv[1];
    if (!crashReportFile.startsWith("log/crash_"))
        return 2;

    QStringList originArgs;
    if (argc > 2)
        originArgs = QStringList(argv + 2, argv + argc);

    QResource::registerResource("./resources.rcc");

    TheCalculater::translator::loadTranslations(
            TheCalculater::util::parse(std::string_view(
                TheCalculater::util::readResourcesFile(":/resources/data/translations.json5").constData()),
                TheCalculater::core::ErrorHandleType::LogError));
    TheCalculater::translator::switchLanguage();

    TheCalculater::crash_handler::CrashHandlerDialog dialog(crashReportFile, originArgs);
    dialog.show();
    dialog.raise();
    dialog.activateWindow();

    return app.exec();
}