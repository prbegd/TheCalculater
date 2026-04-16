/**
 * @file crash_handler_dialog.hpp
 * @author prbegd
 * @brief Crash Handler Dialog Header File
 * @date 2025-07-11
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to 
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#pragma once

#include "ui_crash_handler_dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class CrashHandlerDialog;
}
QT_END_NAMESPACE

namespace TheCalculater::crash_handler {
    class CrashHandlerDialog : public QDialog {
        Q_OBJECT
    public:
        CrashHandlerDialog(const QString& crashReportFile, QStringList originArgs, QWidget* parent = nullptr);

    public slots:
        void on_restartBtn_clicked();
        void on_closeBtn_clicked();
        void on_logBtn_clicked();
        void on_reportBtn_clicked();

    private:
        std::unique_ptr<Ui::CrashHandlerDialog> ui;
        QString crashReportFile_;
        QStringList originArgs_;
    };
} // namespace TheCalculater::crash_handler