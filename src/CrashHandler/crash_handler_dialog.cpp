/**
 * @file crash_handler_dialog.cpp
 * @author prbegd
 * @brief Crash Handler Dialog UI.
 * @date 2025-07-17
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to 
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#include "crash_handler_dialog.hpp"
#include "TheCalculater/translator.hpp"
#include <QCoreApplication>
#include <QDesktopServices>
#include <QIcon>
#include <QProcess>
#include <QUrl>

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