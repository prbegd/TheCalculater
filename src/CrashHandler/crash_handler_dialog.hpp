/**
 * @file crash_handler_dialog.hpp
 * @author prbegd
 * @brief Crash Handler Dialog Header File
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
#pragma once

#include "ui_crash_handler_dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class CrashHandlerDialog;
}
QT_END_NAMESPACE

namespace thecalculater::crash_handler {
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
} // namespace thecalculater::crash_handler