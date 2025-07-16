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
#include "crash_handler_dialog.hpp"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    if (argc != 2) return 2;
    QString crashReportFile = argv[1];
    // TODO: make this check more elegant
    if (!crashReportFile.startsWith("log/")) return 2;

    TheCalculater::crash_handler::CrashHandlerDialog dialog(crashReportFile);
    dialog.show();
    
    return app.exec();
}