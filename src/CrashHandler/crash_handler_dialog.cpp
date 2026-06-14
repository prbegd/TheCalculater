/**
 * @file crash_handler_dialog.cpp
 * @author prbegd
 * @brief Crash Handler Dialog UI.
 * @date 2025-07-17
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

import TheCalculater.translator;
import TheCalculaterQtBridge.qtmock;

namespace TheCalculater::crash_handler {
    CrashHandlerDialog::CrashHandlerDialog(const QString& crashReportFile, QStringList originArgs, QWidget* parent)
        : QDialog(parent), ui(std::make_unique<Ui::CrashHandlerDialog>()), crashReportFile_(QCoreApplication::applicationDirPath() + "/" + crashReportFile), originArgs_(std::move(originArgs))
    {

        ui->setupUi(this);
        ui->restartBtn->setFocus();
        setTabOrder({ ui->restartBtn, ui->reportBtn, ui->logBtn, ui->closeBtn });

        setWindowTitle(QString::fromStdString(TheCalculater::tr(windowTitle().toStdString())));
        ui->messageText->setText(QString::fromStdString(TheCalculater::tr(ui->messageText->text().toStdString())).arg(crashReportFile));
        ui->reportBtn->setText(QString::fromStdString(TheCalculater::tr(ui->reportBtn->text().toStdString())));
        ui->logBtn->setText(QString::fromStdString(TheCalculater::tr(ui->logBtn->text().toStdString())));
        ui->closeBtn->setText(QString::fromStdString(TheCalculater::tr(ui->closeBtn->text().toStdString())));
        ui->restartBtn->setText(QString::fromStdString(TheCalculater::tr(ui->restartBtn->text().toStdString())));
        setWindowIcon(QIcon(":/resources/ui/common/iconWarning.svg"));
    }

    void CrashHandlerDialog::on_restartBtn_clicked()
    {
        QProcess::startDetached("./TheCalculater", originArgs_);
        qApp->quit();
    }
    void CrashHandlerDialog::on_closeBtn_clicked()
    {
        close();
    }
    void CrashHandlerDialog::on_logBtn_clicked()
    {
        QDesktopServices::openUrl(QCoreApplication::applicationDirPath() + "/log/log.log");
    }
    void CrashHandlerDialog::on_reportBtn_clicked()
    {
        QDesktopServices::openUrl(QUrl(crashReportFile_));
    }
} // namespace TheCalculater::crash_handler