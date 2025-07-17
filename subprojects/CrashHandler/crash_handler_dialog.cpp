/**
 * @file crash_handler_dialog.cpp
 * @author prbegd
 * @brief Crash Handler Dialog UI.
 * @date 2025-07-17
 * 
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 * 
 */
#include "crash_handler_dialog.hpp"
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

        ui->messageText->setText(ui->messageText->text().arg(crashReportFile_));
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