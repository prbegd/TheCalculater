/**
 * @file main.cpp
 * @author prbegd
 * @brief Crash Handler Program Entry Point.
 * @date 2025-07-11
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#include "crash_handler_dialog.hpp"
#include "json/value.h"
#include <QApplication>
#include <QResource>
#include <qmessagebox.h>
import TheCalculater.util;
import TheCalculater.translator;
import TheCalculaterQtBridge.resources;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    if (argc < 2) {
        QMessageBox::information(nullptr, "CrashHandler", "Sorry, I'm on vacation.");
        return 2;
    }
    QString crashReportFile = argv[1];

    QStringList originArgs;
    if (argc > 2)
        originArgs = QStringList(argv + 2, argv + argc);

    QResource::registerResource("./resources.rcc");

    TheCalculater::translator::loadTranslations(
        TheCalculater::util::parse(
            TheCalculaterQtBridge::readResourcesFile(":/resources/data/translations.json5").constData()));
    TheCalculater::translator::switchLanguage(QLocale::system().name().toStdString());

    TheCalculater::crash_handler::CrashHandlerDialog dialog(crashReportFile, originArgs);
    dialog.show();
    dialog.raise();
    dialog.activateWindow();

    return app.exec();
}