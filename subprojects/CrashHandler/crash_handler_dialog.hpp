/**
 * @file crash_handler_dialog.hpp
 * @author prbegd
 * @brief Crash Handler Dialog Header File
 * @date 2025-07-11
 * 
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 * 
 */
#pragma once

#include "ui_crash_handler_dialog.h"
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
    class CrashHandlerDialog;
}
QT_END_NAMESPACE

namespace TheCalculater::crash_handler {
    class CrashHandlerDialog : public QDialog {
        Q_OBJECT
    public:
        CrashHandlerDialog(const QString &crashReportFile, QWidget* parent = nullptr);

    private:
        std::unique_ptr<Ui::CrashHandlerDialog> ui;
    };
}