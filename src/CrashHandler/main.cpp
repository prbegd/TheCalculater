/**
 * @file main.cpp
 * @author prbegd
 * @brief Crash Handler Program Entry Point.
 * @date 2025-07-11
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include "crash_handler_dialog.hpp"

import prbegd.thecalculater.util;
import prbegd.thecalculater.translator;
import thirdparty.core;
import thirdparty.extra;

QByteArray readResourcesFile(const std::string_view& fileName)
{
    return QResource(fileName.data()).uncompressedData();
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    if (argc < 2) {
        QMessageBox::question(nullptr, "CrashHandler", "This is a program that simply handles crash, just like its name.", QMessageBox::StandardButtons(QMessageBox::NoButton));
        return 2;
    }
    QString crashReportFile = argv[1];

    QStringList originArgs;
    if (argc > 2)
        originArgs = QStringList(argv + 2, argv + argc);

    QResource::registerResource("./resources.rcc");

    thecalculater::translator::loadTranslations(
        thecalculater::util::parse(readResourcesFile(":/resources/data/translations.json5").constData()));
    thecalculater::translator::switchLanguage(QLocale::system().name().toStdString());

    thecalculater::crash_handler::CrashHandlerDialog dialog(crashReportFile, originArgs);
    dialog.show();
    dialog.raise();
    dialog.activateWindow();

    return app.exec();
}